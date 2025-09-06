#include "Interaction/InteractableObject.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

AInteractableObject::AInteractableObject()
{
    PrimaryActorTick.bCanEverTick = true;

    // ルートコンポーネントとしてシーンコンポーネントを設定
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // メッシュコンポーネント
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
    MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

    // インタラクション範囲コンポーネント
    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    InteractionSphere->SetupAttachment(RootComponent);
    InteractionSphere->SetSphereRadius(200.0f);
    InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    InteractionSphere->SetGenerateOverlapEvents(true);

    // インタラクションUIウィジェット
    InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
    InteractionWidget->SetupAttachment(RootComponent);
    InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
    InteractionWidget->SetDrawAtDesiredSize(true);
    InteractionWidget->SetVisibility(false);

    // デフォルト設定
    InteractionData.InteractionType = EInteractionType::Evidence;
    InteractionData.InteractionText = TEXT("調べる");
    InteractionData.bCanInteract = true;
    InteractionData.bHasBeenInteracted = false;
    InteractionData.InteractionRange = 200.0f;
    InteractionData.bRequiresLineOfSight = true;

    InteractingPlayer = nullptr;
}

void AInteractableObject::BeginPlay()
{
    Super::BeginPlay();

    // インタラクション球体のイベントをバインド
    InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AInteractableObject::OnInteractionSphereBeginOverlap);
    InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AInteractableObject::OnInteractionSphereEndOverlap);

    // インタラクション範囲を設定
    InteractionSphere->SetSphereRadius(InteractionData.InteractionRange);

    // 初期ビジュアル設定
    UpdateVisuals();
}

void AInteractableObject::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // インタラクト中のプレイヤーがいる場合、視線チェックを継続的に行う
    if (InteractingPlayer && InteractionData.bRequiresLineOfSight)
    {
        if (!CheckLineOfSight(InteractingPlayer))
        {
            EndFocus(InteractingPlayer);
        }
    }
}

void AInteractableObject::OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ACharacter* Player = Cast<ACharacter>(OtherActor);
    if (!Player)
        return;

    class AThirdPersonCharacter* ThirdPersonPlayer = Cast<class AThirdPersonCharacter>(Player);
    if (ThirdPersonPlayer && CanInteract(ThirdPersonPlayer))
    {
        StartFocus(ThirdPersonPlayer);
    }
}

void AInteractableObject::OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ACharacter* Player = Cast<ACharacter>(OtherActor);
    if (!Player)
        return;

    class AThirdPersonCharacter* ThirdPersonPlayer = Cast<class AThirdPersonCharacter>(Player);
    if (ThirdPersonPlayer && ThirdPersonPlayer == InteractingPlayer)
    {
        EndFocus(ThirdPersonPlayer);
    }
}

bool AInteractableObject::CanInteract(class AThirdPersonCharacter* Player)
{
    if (!Player)
        return false;

    if (!InteractionData.bCanInteract)
        return false;

    // 距離チェック
    float Distance = FVector::Distance(GetActorLocation(), Player->GetActorLocation());
    if (Distance > InteractionData.InteractionRange)
        return false;

    // 視線チェック
    if (InteractionData.bRequiresLineOfSight && !CheckLineOfSight(Player))
        return false;

    return true;
}

void AInteractableObject::Interact(class AThirdPersonCharacter* Player)
{
    if (!CanInteract(Player))
        return;

    // インタラクションタイプ別の処理
    switch (InteractionData.InteractionType)
    {
        case EInteractionType::Evidence:
        {
            // 証拠を収集
            for (const FEvidenceData& Evidence : ContainedEvidence)
            {
                OnEvidenceCollected.Broadcast(this, Evidence, Player);
                
                // プレイヤーの証拠インベントリに追加
                // 実装はプレイヤークラスに依存
                UE_LOG(LogTemp, Log, TEXT("Evidence collected: %s"), *Evidence.Title);
            }
            break;
        }
        case EInteractionType::Document:
        {
            // 文書閲覧UI表示
            UE_LOG(LogTemp, Log, TEXT("Document interaction"));
            break;
        }
        case EInteractionType::Computer:
        {
            // コンピューターUI表示
            UE_LOG(LogTemp, Log, TEXT("Computer interaction"));
            break;
        }
        case EInteractionType::Character:
        {
            // 会話UI表示
            UE_LOG(LogTemp, Log, TEXT("Character interaction"));
            break;
        }
        case EInteractionType::Environmental:
        {
            // 環境調査
            UE_LOG(LogTemp, Log, TEXT("Environmental interaction"));
            break;
        }
    }

    // インタラクション完了をマーク
    MarkAsInteracted();

    // イベントを発火
    OnInteracted.Broadcast(this, Player);

    // ビジュアルを更新
    UpdateVisuals();
}

void AInteractableObject::StartFocus(class AThirdPersonCharacter* Player)
{
    InteractingPlayer = Player;
    ShowInteractionPrompt(true);
    OnFocusChanged.Broadcast(this, true);

    UE_LOG(LogTemp, Log, TEXT("Started focusing on interactable object"));
}

void AInteractableObject::EndFocus(class AThirdPersonCharacter* Player)
{
    if (InteractingPlayer == Player)
    {
        InteractingPlayer = nullptr;
        ShowInteractionPrompt(false);
        OnFocusChanged.Broadcast(this, false);

        UE_LOG(LogTemp, Log, TEXT("Ended focusing on interactable object"));
    }
}

void AInteractableObject::AddEvidence(const FEvidenceData& NewEvidence)
{
    // 重複チェック
    bool bAlreadyExists = ContainedEvidence.ContainsByPredicate([&NewEvidence](const FEvidenceData& Existing)
    {
        return Existing.EvidenceID == NewEvidence.EvidenceID;
    });

    if (!bAlreadyExists)
    {
        ContainedEvidence.Add(NewEvidence);
        UpdateVisuals();
    }
}

TArray<FEvidenceData> AInteractableObject::GetAvailableEvidence() const
{
    return ContainedEvidence;
}

bool AInteractableObject::HasEvidence() const
{
    return ContainedEvidence.Num() > 0;
}

void AInteractableObject::SetInteractionEnabled(bool bEnabled)
{
    InteractionData.bCanInteract = bEnabled;
    UpdateVisuals();
}

bool AInteractableObject::IsInteractionEnabled() const
{
    return InteractionData.bCanInteract;
}

void AInteractableObject::MarkAsInteracted()
{
    InteractionData.bHasBeenInteracted = true;
    
    // 証拠タイプの場合、一度インタラクションしたら無効化
    if (InteractionData.InteractionType == EInteractionType::Evidence)
    {
        SetInteractionEnabled(false);
    }
}

void AInteractableObject::UpdateVisuals()
{
    if (!MeshComponent)
        return;

    // インタラクション状態に応じてマテリアルパラメータを変更
    UMaterialInstanceDynamic* DynamicMaterial = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
    if (DynamicMaterial)
    {
        // 基本色の設定
        FLinearColor BaseColor = FLinearColor::White;
        
        if (!InteractionData.bCanInteract)
        {
            BaseColor = FLinearColor::Gray; // 無効状態
        }
        else if (InteractionData.bHasBeenInteracted)
        {
            BaseColor = FLinearColor::Green; // インタラクション済み
        }
        else if (InteractingPlayer)
        {
            BaseColor = FLinearColor::Yellow; // フォーカス中
        }
        else if (HasEvidence())
        {
            BaseColor = FLinearColor::Blue; // 証拠あり
        }

        DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), BaseColor);
        
        // エミッシブ強度
        float EmissiveStrength = InteractingPlayer ? 2.0f : 1.0f;
        DynamicMaterial->SetScalarParameterValue(TEXT("EmissiveStrength"), EmissiveStrength);
    }
}

void AInteractableObject::ShowInteractionPrompt(bool bShow)
{
    if (InteractionWidget)
    {
        InteractionWidget->SetVisibility(bShow);
        UpdateInteractionWidget();
    }
}

void AInteractableObject::UpdateInteractionWidget()
{
    // インタラクションUIウィジェットのテキストや表示を更新
    // 実装は具体的なUIウィジェットクラスに依存
    
    if (InteractionWidget && InteractionWidget->GetWidget())
    {
        // ウィジェット内のテキストブロックを更新
        // 例：インタラクションテキストを表示
    }
}

bool AInteractableObject::CheckLineOfSight(class AThirdPersonCharacter* Player)
{
    if (!Player)
        return false;

    FVector StartLocation = Player->GetActorLocation() + FVector(0, 0, Player->GetDefaultHalfHeight());
    FVector EndLocation = GetActorLocation();
    
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Player);
    QueryParams.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_Visibility,
        QueryParams
    );

    // デバッグライン表示（開発時のみ）
    #if WITH_EDITOR
    if (GEngine && GEngine->GetGameUserSettings())
    {
        FColor LineColor = bHit ? FColor::Red : FColor::Green;
        DrawDebugLine(GetWorld(), StartLocation, EndLocation, LineColor, false, 0.1f);
    }
    #endif

    return !bHit; // ヒットしなければ視線が通っている
}
