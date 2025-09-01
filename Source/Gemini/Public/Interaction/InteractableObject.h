#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Evidence/EvidenceData.h"
#include "InteractableObject.generated.h"

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
    Evidence        UMETA(DisplayName = "証拠収集"),
    Document        UMETA(DisplayName = "文書閲覧"),
    Computer        UMETA(DisplayName = "コンピューター操作"),
    Character       UMETA(DisplayName = "人物との会話"),
    Environmental   UMETA(DisplayName = "環境調査")
};

USTRUCT(BlueprintType)
struct GEMINI_API FInteractionData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    EInteractionType InteractionType = EInteractionType::Evidence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    FString InteractionText = TEXT("調べる");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    bool bCanInteract = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    bool bHasBeenInteracted = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float InteractionRange = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    bool bRequiresLineOfSight = true;
};

UCLASS()
class GEMINI_API AInteractableObject : public AActor
{
    GENERATED_BODY()

public:
    AInteractableObject();

protected:
    // コンポーネント
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* InteractionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UWidgetComponent* InteractionWidget;

    // インタラクション設定
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    FInteractionData InteractionData;

    // 証拠データ（証拠タイプの場合）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evidence")
    TArray<FEvidenceData> ContainedEvidence;

    // 現在インタラクション範囲内のプレイヤー
    UPROPERTY(BlueprintReadOnly, Category = "State")
    class AThirdPersonCharacter* InteractingPlayer;

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // インタラクション球体のイベント
    UFUNCTION()
    void OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
    // インタラクション関数
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    virtual bool CanInteract(class AThirdPersonCharacter* Player);

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    virtual void Interact(class AThirdPersonCharacter* Player);

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    virtual void StartFocus(class AThirdPersonCharacter* Player);

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    virtual void EndFocus(class AThirdPersonCharacter* Player);

    // 証拠関連
    UFUNCTION(BlueprintCallable, Category = "Evidence")
    void AddEvidence(const FEvidenceData& NewEvidence);

    UFUNCTION(BlueprintCallable, Category = "Evidence")
    TArray<FEvidenceData> GetAvailableEvidence() const;

    UFUNCTION(BlueprintCallable, Category = "Evidence")
    bool HasEvidence() const;

    // 状態管理
    UFUNCTION(BlueprintCallable, Category = "State")
    void SetInteractionEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "State")
    bool IsInteractionEnabled() const;

    UFUNCTION(BlueprintCallable, Category = "State")
    void MarkAsInteracted();

    // ビジュアル更新
    UFUNCTION(BlueprintCallable, Category = "Visual")
    virtual void UpdateVisuals();

    UFUNCTION(BlueprintCallable, Category = "Visual")
    void ShowInteractionPrompt(bool bShow);

    // デリゲート
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteracted, AInteractableObject*, InteractedObject, class AThirdPersonCharacter*, InteractingPlayer);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFocusChanged, AInteractableObject*, Object, bool, bInFocus);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEvidenceCollected, AInteractableObject*, Object, const FEvidenceData&, Evidence, class AThirdPersonCharacter*, Collector);

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnInteracted OnInteracted;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnFocusChanged OnFocusChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnEvidenceCollected OnEvidenceCollected;

private:
    void UpdateInteractionWidget();
    bool CheckLineOfSight(class AThirdPersonCharacter* Player);
};
