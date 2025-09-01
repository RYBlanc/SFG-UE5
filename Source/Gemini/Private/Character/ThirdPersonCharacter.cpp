#include "Character/ThirdPersonCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "UI/CanvasOfThoughtWidget.h"
#include "Interaction/InteractableObject.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AThirdPersonCharacter::AThirdPersonCharacter()
{
    // Set size for collision capsule
    GetCapsuleComponent()->SetCapsuleSize(42.f, 96.0f);
        
    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...    
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

    // Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
    // instead of recompiling to adjust them
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character    
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
    // are set in the derived blueprint asset named ThirdPersonBP (to avoid direct content references in C++)

    // 初期化
    FocusedInteractable = nullptr;
    CanvasWidget = nullptr;
    bIsCanvasOpen = false;
    MaxEvidenceCapacity = 50;
    InteractionRange = 300.0f;
    
    // Tick を有効にする
    PrimaryActorTick.bCanEverTick = true;
}

void AThirdPersonCharacter::BeginPlay()
{
    // Call the base class  
    Super::BeginPlay();

    // Add Input Mapping Context
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    // 思考キャンバスUIを初期化
    InitializeCanvas();
}

void AThirdPersonCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // 最も近いインタラクション可能なオブジェクトを検索
    FindNearestInteractable();
}

void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Set up action bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Move);

        // Looking
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Look);

        // Interacting
        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::Interact);

        // Toggle Canvas
        EnhancedInputComponent->BindAction(ToggleCanvasAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::ToggleCanvas);
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
    }
}

void AThirdPersonCharacter::Move(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    
        // get right vector 
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // add movement 
        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void AThirdPersonCharacter::Look(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // add yaw and pitch input to controller
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void AThirdPersonCharacter::Interact()
{
    if (FocusedInteractable && FocusedInteractable->CanInteract(this))
    {
        FocusedInteractable->Interact(this);
    }
}

void AThirdPersonCharacter::ToggleCanvas()
{
    if (bIsCanvasOpen)
    {
        CloseCanvas();
    }
    else
    {
        OpenCanvas();
    }
}

bool AThirdPersonCharacter::AddEvidence(const FEvidenceData& NewEvidence)
{
    // 容量チェック
    if (CollectedEvidence.Num() >= MaxEvidenceCapacity)
    {
        UE_LOG(LogTemp, Warning, TEXT("Evidence inventory is full"));
        return false;
    }

    // 重複チェック
    bool bAlreadyExists = CollectedEvidence.ContainsByPredicate([&NewEvidence](const FEvidenceData& Existing)
    {
        return Existing.EvidenceID == NewEvidence.EvidenceID;
    });

    if (bAlreadyExists)
    {
        UE_LOG(LogTemp, Warning, TEXT("Evidence already exists: %s"), *NewEvidence.EvidenceID);
        return false;
    }

    // 証拠を追加
    CollectedEvidence.Add(NewEvidence);
    
    // 思考キャンバスのインベントリにも追加
    if (CanvasWidget)
    {
        CanvasWidget->AddEvidenceToInventory(NewEvidence);
    }

    // イベントを発火
    OnEvidenceAdded.Broadcast(NewEvidence);

    UE_LOG(LogTemp, Log, TEXT("Evidence added: %s"), *NewEvidence.Title);
    return true;
}

bool AThirdPersonCharacter::RemoveEvidence(const FString& EvidenceID)
{
    int32 RemovedCount = CollectedEvidence.RemoveAll([&EvidenceID](const FEvidenceData& Evidence)
    {
        return Evidence.EvidenceID == EvidenceID;
    });

    if (RemovedCount > 0)
    {
        OnEvidenceRemoved.Broadcast(EvidenceID);
        UE_LOG(LogTemp, Log, TEXT("Evidence removed: %s"), *EvidenceID);
        return true;
    }

    return false;
}

FEvidenceData AThirdPersonCharacter::GetEvidence(const FString& EvidenceID) const
{
    const FEvidenceData* FoundEvidence = CollectedEvidence.FindByPredicate([&EvidenceID](const FEvidenceData& Evidence)
    {
        return Evidence.EvidenceID == EvidenceID;
    });

    if (FoundEvidence)
    {
        return *FoundEvidence;
    }

    return FEvidenceData(); // デフォルト値を返す
}

TArray<FEvidenceData> AThirdPersonCharacter::GetAllEvidence() const
{
    return CollectedEvidence;
}

bool AThirdPersonCharacter::HasEvidence(const FString& EvidenceID) const
{
    return CollectedEvidence.ContainsByPredicate([&EvidenceID](const FEvidenceData& Evidence)
    {
        return Evidence.EvidenceID == EvidenceID;
    });
}

int32 AThirdPersonCharacter::GetEvidenceCount() const
{
    return CollectedEvidence.Num();
}

void AThirdPersonCharacter::SetFocusedInteractable(AInteractableObject* NewFocusedInteractable)
{
    AInteractableObject* OldFocusedInteractable = FocusedInteractable;
    
    if (OldFocusedInteractable != NewFocusedInteractable)
    {
        FocusedInteractable = NewFocusedInteractable;
        OnInteractableChanged.Broadcast(OldFocusedInteractable, NewFocusedInteractable);
    }
}

AInteractableObject* AThirdPersonCharacter::GetFocusedInteractable() const
{
    return FocusedInteractable;
}

void AThirdPersonCharacter::FindNearestInteractable()
{
    UWorld* World = GetWorld();
    if (!World)
        return;

    FVector PlayerLocation = GetActorLocation();
    AInteractableObject* NearestInteractable = nullptr;
    float NearestDistance = InteractionRange;

    // 範囲内のすべてのインタラクション可能オブジェクトを検索
    for (TActorIterator<AInteractableObject> ActorIterator(World); ActorIterator; ++ActorIterator)
    {
        AInteractableObject* InteractableObject = *ActorIterator;
        
        if (InteractableObject && InteractableObject->CanInteract(this))
        {
            float Distance = FVector::Distance(PlayerLocation, InteractableObject->GetActorLocation());
            
            if (Distance < NearestDistance)
            {
                NearestDistance = Distance;
                NearestInteractable = InteractableObject;
            }
        }
    }

    UpdateFocusedInteractable();
}

void AThirdPersonCharacter::OpenCanvas()
{
    if (!CanvasWidget)
    {
        InitializeCanvas();
    }

    if (CanvasWidget && !bIsCanvasOpen)
    {
        CanvasWidget->AddToViewport();
        bIsCanvasOpen = true;
        
        // マウスカーソルを表示
        if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
        {
            PlayerController->bShowMouseCursor = true;
            PlayerController->SetInputMode(FInputModeGameAndUI());
        }

        OnCanvasToggled.Broadcast(true);
        UE_LOG(LogTemp, Log, TEXT("Canvas opened"));
    }
}

void AThirdPersonCharacter::CloseCanvas()
{
    if (CanvasWidget && bIsCanvasOpen)
    {
        CanvasWidget->RemoveFromViewport();
        bIsCanvasOpen = false;
        
        // マウスカーソルを非表示
        if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
        {
            PlayerController->bShowMouseCursor = false;
            PlayerController->SetInputMode(FInputModeGameOnly());
        }

        OnCanvasToggled.Broadcast(false);
        UE_LOG(LogTemp, Log, TEXT("Canvas closed"));
    }
}

void AThirdPersonCharacter::InitializeCanvas()
{
    if (CanvasWidgetClass && !CanvasWidget)
    {
        CanvasWidget = CreateWidget<UCanvasOfThoughtWidget>(GetWorld(), CanvasWidgetClass);
        
        if (CanvasWidget)
        {
            // 既存の証拠をキャンバスに追加
            UpdateCanvasWithEvidence();
            
            UE_LOG(LogTemp, Log, TEXT("Canvas initialized"));
        }
    }
}

UCanvasOfThoughtWidget* AThirdPersonCharacter::GetCanvasWidget() const
{
    return CanvasWidget;
}

void AThirdPersonCharacter::OnEvidenceCollected(AInteractableObject* Object, const FEvidenceData& Evidence, AThirdPersonCharacter* Collector)
{
    if (Collector == this)
    {
        AddEvidence(Evidence);
    }
}

void AThirdPersonCharacter::UpdateFocusedInteractable()
{
    // プライベート関数として実装
    // 実際のフォーカス対象の更新ロジック
}

void AThirdPersonCharacter::UpdateCanvasWithEvidence()
{
    if (CanvasWidget)
    {
        // 収集済みの証拠をキャンバスのインベントリに追加
        for (const FEvidenceData& Evidence : CollectedEvidence)
        {
            CanvasWidget->AddEvidenceToInventory(Evidence);
        }
    }
}
