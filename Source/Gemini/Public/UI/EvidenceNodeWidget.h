#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Evidence/EvidenceData.h"
#include "Blueprint/DragDropOperation.h"
#include "EvidenceNodeWidget.generated.h"

UCLASS()
class GEMINI_API UEvidenceNodeDragDropOperation : public UDragDropOperation
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, Category = "Evidence")
    FEvidenceData EvidenceData;

    UPROPERTY(BlueprintReadWrite, Category = "Evidence") 
    class UEvidenceNodeWidget* SourceWidget;
};

UCLASS()
class GEMINI_API UEvidenceNodeWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // ウィジェットコンポーネント
    UPROPERTY(meta = (BindWidget))
    class UBorder* NodeBorder;

    UPROPERTY(meta = (BindWidget))
    class UImage* EvidenceImage;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TitleText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TypeText;

    // 証拠データ
    UPROPERTY(BlueprintReadWrite, Category = "Evidence")
    FEvidenceData EvidenceData;

    // 接続ライン関連
    UPROPERTY(BlueprintReadWrite, Category = "Connections")
    TArray<class UConnectionLineWidget*> ConnectedLines;

    // キャンバス位置
    UPROPERTY(BlueprintReadWrite, Category = "Position")
    FVector2D CanvasPosition;

    // 状態フラグ
    UPROPERTY(BlueprintReadWrite, Category = "State")
    bool bIsSelected = false;

    UPROPERTY(BlueprintReadWrite, Category = "State")
    bool bIsDragging = false;

    UPROPERTY(BlueprintReadWrite, Category = "State")
    bool bIsHovered = false;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // ドラッグ&ドロップ関連
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    // マウスイベント
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

public:
    // パブリック関数
    UFUNCTION(BlueprintCallable, Category = "Evidence")
    void SetEvidenceData(const FEvidenceData& NewEvidenceData);

    UFUNCTION(BlueprintCallable, Category = "Evidence")
    void UpdateVisuals();

    UFUNCTION(BlueprintCallable, Category = "Evidence")
    void SetSelected(bool bSelected);

    UFUNCTION(BlueprintCallable, Category = "Position")
    void SetCanvasPosition(FVector2D NewPosition);

    UFUNCTION(BlueprintCallable, Category = "Connections")
    void AddConnectionLine(class UConnectionLineWidget* LineWidget);

    UFUNCTION(BlueprintCallable, Category = "Connections")
    void RemoveConnectionLine(class UConnectionLineWidget* LineWidget);

    UFUNCTION(BlueprintCallable, Category = "State")
    void SetCorruptionLevel(float CorruptionLevel);

    // デリゲート
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEvidenceNodeClicked, UEvidenceNodeWidget*, ClickedNode, const FEvidenceData&, EvidenceData);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEvidenceNodeMoved, UEvidenceNodeWidget*, MovedNode, FVector2D, NewPosition);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEvidenceNodeHovered, UEvidenceNodeWidget*, HoveredNode, bool, bIsHovering);

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnEvidenceNodeClicked OnEvidenceNodeClicked;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnEvidenceNodeMoved OnEvidenceNodeMoved;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnEvidenceNodeHovered OnEvidenceNodeHovered;

private:
    void UpdateNodeColor();
    void UpdateCorruptionVisuals();
};
