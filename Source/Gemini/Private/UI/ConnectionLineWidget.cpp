#include "UI/ConnectionLineWidget.h"
#include "UI/EvidenceNodeWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Slate/SlateBrushAsset.h"
#include "Engine/Engine.h"
#include "Framework/Application/SlateApplication.h"

void UConnectionLineWidget::NativeConstruct()
{
    Super::NativeConstruct();
    UpdateLineColor();
}

void UConnectionLineWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    if (bAnimateFlow)
    {
        AnimationTime += InDeltaTime * FlowSpeed;
        if (AnimationTime > 1.0f)
        {
            AnimationTime = 0.0f;
        }
    }
}

int32 UConnectionLineWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, 
    const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, 
    const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    if (!bIsVisible || !StartNode || !EndNode)
    {
        return LayerId;
    }

    // 基本の線を描画
    switch (LineType)
    {
        case EConnectionLineType::Reality:
            DrawStraightLine(AllottedGeometry, OutDrawElements, LayerId);
            break;
        case EConnectionLineType::Dream:
            DrawCurvedLine(AllottedGeometry, OutDrawElements, LayerId);
            break;
        case EConnectionLineType::Corrupted:
            DrawCorruptedLine(AllottedGeometry, OutDrawElements, LayerId);
            break;
    }
    
    // フローエフェクトを描画
    if (bAnimateFlow)
    {
        DrawFlowEffect(AllottedGeometry, OutDrawElements, LayerId + 1);
    }
    
    return LayerId + 2;
}

FReply UConnectionLineWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        OnConnectionLineClicked.Broadcast(this);
        return FReply::Handled();
    }
    
    return FReply::Unhandled();
}

void UConnectionLineWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    bIsHighlighted = true;
    OnConnectionLineHovered.Broadcast(this, true);
}

void UConnectionLineWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    bIsHighlighted = false;
    OnConnectionLineHovered.Broadcast(this, false);
}

void UConnectionLineWidget::SetConnection(UEvidenceNodeWidget* InStartNode, UEvidenceNodeWidget* InEndNode, const FEvidenceConnection& InConnectionData)
{
    StartNode = InStartNode;
    EndNode = InEndNode;
    ConnectionData = InConnectionData;
    
    if (StartNode)
    {
        StartNode->AddConnectionLine(this);
    }
    if (EndNode)
    {
        EndNode->AddConnectionLine(this);
    }
    
    UpdateLineColor();
}

void UConnectionLineWidget::SetLineType(EConnectionLineType NewLineType)
{
    LineType = NewLineType;
    UpdateLineColor();
}

void UConnectionLineWidget::SetSelected(bool bSelected)
{
    bIsSelected = bSelected;
    UpdateLineColor();
}

void UConnectionLineWidget::SetHighlighted(bool bHighlighted)
{
    bIsHighlighted = bHighlighted;
    UpdateLineColor();
}

void UConnectionLineWidget::UpdateLineColor()
{
    // 基本色の設定
    FLinearColor BaseColor = FLinearColor::White;
    
    // 接続の種類に基づく色
    if (ConnectionData.RelationshipType == TEXT("Causal"))
    {
        BaseColor = FLinearColor::Green;
    }
    else if (ConnectionData.RelationshipType == TEXT("Contradictory"))
    {
        BaseColor = FLinearColor::Red;
    }
    else if (ConnectionData.RelationshipType == TEXT("Similar"))
    {
        BaseColor = FLinearColor::Blue;
    }
    else if (ConnectionData.RelationshipType == TEXT("Temporal"))
    {
        BaseColor = FLinearColor::Yellow;
    }
    
    // 状態に基づく調整
    if (bIsSelected)
    {
        BaseColor = FLinearColor::Yellow;
    }
    else if (bIsHighlighted)
    {
        BaseColor = BaseColor * 1.5f; // 明度アップ
    }
    
    // 信頼度に基づく透明度
    BaseColor.A = ConnectionData.ConnectionStrength * LineOpacity;
    
    LineColor = BaseColor;
}

void UConnectionLineWidget::StartFlowAnimation()
{
    bAnimateFlow = true;
    AnimationTime = 0.0f;
}

void UConnectionLineWidget::StopFlowAnimation()
{
    bAnimateFlow = false;
}

void UConnectionLineWidget::ApplyCorruption(float CorruptionLevel)
{
    LineType = EConnectionLineType::Corrupted;
    LineOpacity = 1.0f - (CorruptionLevel * 0.3f);
    UpdateLineColor();
}

bool UConnectionLineWidget::IsPointOnLine(FVector2D Point, float Tolerance) const
{
    if (!StartNode || !EndNode)
        return false;
        
    FVector2D StartPos = GetStartPosition();
    FVector2D EndPos = GetEndPosition();
    
    // 点と線分の距離を計算
    FVector2D LineVec = EndPos - StartPos;
    FVector2D PointVec = Point - StartPos;
    
    float LineLengthSq = LineVec.SizeSquared();
    if (LineLengthSq == 0.0f)
        return false;
    
    float t = FMath::Clamp(FVector2D::DotProduct(PointVec, LineVec) / LineLengthSq, 0.0f, 1.0f);
    FVector2D ClosestPoint = StartPos + t * LineVec;
    
    float Distance = FVector2D::Distance(Point, ClosestPoint);
    return Distance <= Tolerance;
}

FVector2D UConnectionLineWidget::GetLineCenter() const
{
    FVector2D StartPos = GetStartPosition();
    FVector2D EndPos = GetEndPosition();
    return (StartPos + EndPos) * 0.5f;
}

float UConnectionLineWidget::GetLineLength() const
{
    FVector2D StartPos = GetStartPosition();
    FVector2D EndPos = GetEndPosition();
    return FVector2D::Distance(StartPos, EndPos);
}

void UConnectionLineWidget::DrawStraightLine(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32 LayerId) const
{
    FVector2D StartPos = GetStartPosition();
    FVector2D EndPos = GetEndPosition();
    
    TArray<FVector2D> Points;
    Points.Add(StartPos);
    Points.Add(EndPos);
    
    FSlateDrawElement::MakeLines(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(),
        Points,
        ESlateDrawEffect::None,
        LineColor,
        true,
        LineThickness
    );
}

void UConnectionLineWidget::DrawCurvedLine(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32 LayerId) const
{
    TArray<FVector2D> BezierPoints = GenerateBezierPoints(30);
    
    if (BezierPoints.Num() < 2)
        return;
    
    FSlateDrawElement::MakeLines(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(),
        BezierPoints,
        ESlateDrawEffect::None,
        LineColor,
        true,
        LineThickness
    );
}

void UConnectionLineWidget::DrawCorruptedLine(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32 LayerId) const
{
    TArray<FVector2D> CorruptedPoints = GenerateBezierPoints(20);
    
    // ランダムなノイズを追加して腐敗した見た目にする
    for (int32 i = 1; i < CorruptedPoints.Num() - 1; ++i)
    {
        float NoiseStrength = 10.0f;
        FVector2D Noise = FVector2D(
            FMath::RandRange(-NoiseStrength, NoiseStrength),
            FMath::RandRange(-NoiseStrength, NoiseStrength)
        );
        CorruptedPoints[i] += Noise;
    }
    
    FSlateDrawElement::MakeLines(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(),
        CorruptedPoints,
        ESlateDrawEffect::None,
        LineColor,
        true,
        LineThickness
    );
}

void UConnectionLineWidget::DrawFlowEffect(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32 LayerId) const
{
    // フローエフェクト用の小さなドットを描画
    TArray<FVector2D> Points;
    
    if (LineType == EConnectionLineType::Dream)
    {
        Points = GenerateBezierPoints(10);
    }
    else
    {
        FVector2D StartPos = GetStartPosition();
        FVector2D EndPos = GetEndPosition();
        
        for (int32 i = 0; i < 10; ++i)
        {
            float t = static_cast<float>(i) / 9.0f;
            Points.Add(FMath::Lerp(StartPos, EndPos, t));
        }
    }
    
    // アニメーション位置に基づいてドットを表示
    int32 DotIndex = FMath::FloorToInt(AnimationTime * Points.Num());
    if (Points.IsValidIndex(DotIndex))
    {
        FVector2D DotPos = Points[DotIndex];
        FLinearColor DotColor = LineColor;
        DotColor.A = 0.8f;
        
        // 小さな円を描画
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry(FVector2D(6.0f, 6.0f), FSlateLayoutTransform(DotPos - FVector2D(3.0f, 3.0f))),
            nullptr,
            ESlateDrawEffect::None,
            DotColor
        );
    }
}

FVector2D UConnectionLineWidget::GetStartPosition() const
{
    if (!StartNode)
        return FVector2D::ZeroVector;
    
    return StartNode->CanvasPosition;
}

FVector2D UConnectionLineWidget::GetEndPosition() const
{
    if (!EndNode)
        return FVector2D::ZeroVector;
    
    return EndNode->CanvasPosition;
}

FVector2D UConnectionLineWidget::CalculateBezierPoint(float t, FVector2D P0, FVector2D P1, FVector2D P2, FVector2D P3) const
{
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;
    
    FVector2D Point = uuu * P0;
    Point += 3 * uu * t * P1;
    Point += 3 * u * tt * P2;
    Point += ttt * P3;
    
    return Point;
}

TArray<FVector2D> UConnectionLineWidget::GenerateBezierPoints(int32 NumPoints) const
{
    TArray<FVector2D> Points;
    
    FVector2D StartPos = GetStartPosition();
    FVector2D EndPos = GetEndPosition();
    
    if (StartPos.Equals(EndPos, 1.0f))
    {
        Points.Add(StartPos);
        return Points;
    }
    
    // 制御点を計算
    FVector2D Direction = (EndPos - StartPos).GetSafeNormal();
    FVector2D Perpendicular = FVector2D(-Direction.Y, Direction.X);
    
    FVector2D Control1 = StartPos + ControlPoint1Offset.X * Direction + ControlPoint1Offset.Y * Perpendicular;
    FVector2D Control2 = EndPos + ControlPoint2Offset.X * Direction + ControlPoint2Offset.Y * Perpendicular;
    
    // ベジェ曲線のポイントを生成
    for (int32 i = 0; i <= NumPoints; ++i)
    {
        float t = static_cast<float>(i) / static_cast<float>(NumPoints);
        FVector2D Point = CalculateBezierPoint(t, StartPos, Control1, Control2, EndPos);
        Points.Add(Point);
    }
    
    return Points;
}
