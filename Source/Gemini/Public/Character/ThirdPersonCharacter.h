#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Evidence/EvidenceData.h"
#include "ThirdPersonCharacter.generated.h"

UCLASS(config=Game)
class GEMINI_API AThirdPersonCharacter : public ACharacter
{
    GENERATED_BODY()

    /** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    /** Follow camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;
    
    /** MappingContext */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputMappingContext* DefaultMappingContext;

    /** Jump Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction* JumpAction;

    /** Move Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction* MoveAction;

    /** Look Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction* LookAction;

    /** Interact Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction* InteractAction;

    /** Toggle Canvas Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction* ToggleCanvasAction;

public:
    AThirdPersonCharacter();

    // 思考キャンバスUI
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<class UCanvasOfThoughtWidget> CanvasWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    class UCanvasOfThoughtWidget* CanvasWidget;

    // 証拠インベントリ
    UPROPERTY(BlueprintReadWrite, Category = "Evidence")
    TArray<FEvidenceData> CollectedEvidence;

    UPROPERTY(BlueprintReadWrite, Category = "Evidence")
    int32 MaxEvidenceCapacity = 50;

    // インタラクション
    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
    class AInteractableObject* FocusedInteractable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float InteractionRange = 300.0f;

    // キャンバス状態
    UPROPERTY(BlueprintReadWrite, Category = "Canvas")
    bool bIsCanvasOpen = false;

protected:
    /** Called for movement input */
    void Move(const FInputActionValue& Value);

    /** Called for looking input */
    void Look(const FInputActionValue& Value);

    /** Called for interaction input */
    void Interact();

    /** Called for canvas toggle input */
    void ToggleCanvas();

    // APawn interface
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
    // To add mapping context
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

    // 証拠管理
    UFUNCTION(BlueprintCallable, Category = "Evidence")
    bool AddEvidence(const FEvidenceData& NewEvidence);

    UFUNCTION(BlueprintCallable, Category = "Evidence")
    bool RemoveEvidence(const FString& EvidenceID);

    UFUNCTION(BlueprintCallable, Category = "Evidence")
    FEvidenceData GetEvidence(const FString& EvidenceID) const;

    UFUNCTION(BlueprintCallable, Category = "Evidence")
    TArray<FEvidenceData> GetAllEvidence() const;

    UFUNCTION(BlueprintCallable, Category = "Evidence")
    bool HasEvidence(const FString& EvidenceID) const;

    UFUNCTION(BlueprintCallable, Category = "Evidence")
    int32 GetEvidenceCount() const;

    // インタラクション
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void SetFocusedInteractable(class AInteractableObject* NewFocusedInteractable);

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    class AInteractableObject* GetFocusedInteractable() const;

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void FindNearestInteractable();

    // 思考キャンバス
    UFUNCTION(BlueprintCallable, Category = "Canvas")
    void OpenCanvas();

    UFUNCTION(BlueprintCallable, Category = "Canvas")
    void CloseCanvas();

    UFUNCTION(BlueprintCallable, Category = "Canvas")
    void InitializeCanvas();

    UFUNCTION(BlueprintCallable, Category = "Canvas")
    class UCanvasOfThoughtWidget* GetCanvasWidget() const;

    // イベントハンドラー
    UFUNCTION(BlueprintCallable, Category = "Events")
    void OnEvidenceCollected(class AInteractableObject* Object, const FEvidenceData& Evidence, class AThirdPersonCharacter* Collector);

    // デリゲート
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEvidenceAdded, const FEvidenceData&, Evidence);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEvidenceRemoved, const FString&, EvidenceID);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractableChanged, class AInteractableObject*, OldInteractable, class AInteractableObject*, NewInteractable);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCanvasToggled, bool, bIsOpen);

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnEvidenceAdded OnEvidenceAdded;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnEvidenceRemoved OnEvidenceRemoved;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnInteractableChanged OnInteractableChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnCanvasToggled OnCanvasToggled;

private:
    void UpdateFocusedInteractable();
    void UpdateCanvasWithEvidence();
};
