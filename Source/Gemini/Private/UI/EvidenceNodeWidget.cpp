#include "UI/EvidenceNodeWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/DragDropOperation.h"
#include "UI/ConnectionLineWidget.h"

void UEvidenceNodeWidget::NativeConstruct()
{
    Super::NativeConstruct();
    UpdateVisuals();
}

void UEvidenceNodeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    // 腐敗レベルに基づいた視覚的変化
    if (EvidenceData.CorruptionLevel > 0.0f)
    {
        UpdateCorruptionVisuals();
    }
}

FReply UEvidenceNodeWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        // クリックイベントを発火
        OnEvidenceNodeClicked.Broadcast(this, EvidenceData);
        
        // ドラッグ検出を有効にする
        return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
    }
    
    return FReply::Unhandled();
}

void UEvidenceNodeWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        bIsDragging = true;
        
        // ドラッグ操作を作成
        UEvidenceNodeDragDropOperation* DragDropOp = NewObject<UEvidenceNodeDragDropOperation>();
        DragDropOp->EvidenceData = EvidenceData;
        DragDropOp->SourceWidget = this;
        DragDropOp->DefaultDragVisual = this;
        DragDropOp->Pivot = EDragPivot::MouseDown;
        
        OutOperation = DragDropOp;
    }
}

bool UEvidenceNodeWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    UEvidenceNodeDragDropOperation* EvidenceDragDrop = Cast<UEvidenceNodeDragDropOperation>(InOperation);
    if (EvidenceDragDrop && EvidenceDragDrop->SourceWidget != this)
    {
        // 他の証拠ノードがドロップされた場合、接続を作成
        // この処理は親のキャンバスで行う予定
        return true;
    }
    
    return false;
}

void UEvidenceNodeWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    UEvidenceNodeDragDropOperation* EvidenceDragDrop = Cast<UEvidenceNodeDragDropOperation>(InOperation);
    if (EvidenceDragDrop && EvidenceDragDrop->SourceWidget != this)
    {
        // ドロップ可能な状態を示すビジュアルフィードバック
        SetSelected(true);
    }
}

void UEvidenceNodeWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    // ドロップ候補のハイライトを解除
    SetSelected(false);
}

void UEvidenceNodeWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    bIsHovered = true;
    OnEvidenceNodeHovered.Broadcast(this, true);
    UpdateVisuals();
}

void UEvidenceNodeWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    bIsHovered = false;
    OnEvidenceNodeHovered.Broadcast(this, false);
    UpdateVisuals();
}

void UEvidenceNodeWidget::SetEvidenceData(const FEvidenceData& NewEvidenceData)
{
    EvidenceData = NewEvidenceData;
    UpdateVisuals();
}

void UEvidenceNodeWidget::UpdateVisuals()
{
    if (!TitleText || !TypeText || !NodeBorder)
        return;

    // テキストを更新
    TitleText->SetText(FText::FromString(EvidenceData.Title));
    
    // 証拠タイプに応じたテキスト表示
    FString TypeString;
    switch (EvidenceData.Type)
    {
        case EEvidenceType::Physical:
            TypeString = TEXT("物的証拠");
            break;
        case EEvidenceType::Testimony:
            TypeString = TEXT("証言");
            break;
        case EEvidenceType::Document:
            TypeString = TEXT("書類");
            break;
        case EEvidenceType::Audio:
            TypeString = TEXT("音声記録");
            break;
        case EEvidenceType::Visual:
            TypeString = TEXT("映像記録");
            break;
        case EEvidenceType::Abstract:
            TypeString = TEXT("抽象的概念");
            break;
    }
    TypeText->SetText(FText::FromString(TypeString));

    // 画像を更新
    if (EvidenceImage && EvidenceData.EvidenceImage.IsValid())
    {
        EvidenceImage->SetBrushFromTexture(EvidenceData.EvidenceImage.LoadSynchronous());
    }

    UpdateNodeColor();
}

void UEvidenceNodeWidget::SetSelected(bool bSelected)
{
    bIsSelected = bSelected;
    UpdateVisuals();
}

void UEvidenceNodeWidget::SetCanvasPosition(FVector2D NewPosition)
{
    CanvasPosition = NewPosition;
    EvidenceData.CanvasPosition = NewPosition;
    OnEvidenceNodeMoved.Broadcast(this, NewPosition);
}

void UEvidenceNodeWidget::AddConnectionLine(UConnectionLineWidget* LineWidget)
{
    if (LineWidget && !ConnectedLines.Contains(LineWidget))
    {
        ConnectedLines.Add(LineWidget);
    }
}

void UEvidenceNodeWidget::RemoveConnectionLine(UConnectionLineWidget* LineWidget)
{
    ConnectedLines.Remove(LineWidget);
}

void UEvidenceNodeWidget::SetCorruptionLevel(float CorruptionLevel)
{
    EvidenceData.CorruptionLevel = FMath::Clamp(CorruptionLevel, 0.0f, 1.0f);
    UpdateCorruptionVisuals();
}

void UEvidenceNodeWidget::UpdateNodeColor()
{
    if (!NodeBorder)
        return;

    FLinearColor BorderColor = EvidenceData.NodeColor;
    
    // 状態に応じて色を調整
    if (bIsSelected)
    {
        BorderColor = FLinearColor::Yellow;
    }
    else if (bIsHovered)
    {
        BorderColor = BorderColor * 1.2f; // 少し明るく
    }
    
    // 信頼性に応じた色調整
    switch (EvidenceData.Reliability)
    {
        case EEvidenceReliability::Certain:
            BorderColor = FLinearColor::Green;
            break;
        case EEvidenceReliability::Probable:
            BorderColor = FLinearColor::Blue;
            break;
        case EEvidenceReliability::Uncertain:
            BorderColor = FLinearColor::Yellow;
            break;
        case EEvidenceReliability::Suspicious:
            BorderColor = FLinearColor::Red;
            break;
    }
    
    NodeBorder->SetBrushColor(BorderColor);
}

void UEvidenceNodeWidget::UpdateCorruptionVisuals()
{
    if (!NodeBorder)
        return;

    // 腐敗レベルに応じて透明度や歪みエフェクトを適用
    float Alpha = 1.0f - (EvidenceData.CorruptionLevel * 0.5f);
    FLinearColor CurrentColor = NodeBorder->GetBrushColor();
    CurrentColor.A = Alpha;
    
    NodeBorder->SetBrushColor(CurrentColor);
    
    // さらなる視覚効果（パーティクル、歪み等）は後で追加予定
}
