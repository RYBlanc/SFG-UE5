#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "Evidence/EvidenceData.h"
#include "ConnectionLineWidget.generated.h"

UENUM(BlueprintType)
enum class EConnectionLineType : uint8
{
    Reality     UMETA(DisplayName = "現実（直線的）"),
    Dream       UMETA(DisplayName = "夢（曲線的）"),
    Corrupted   UMETA(DisplayName = "腐敗（不安定）")
};

UCLASS()
class GEMINI_API UConnectionLineWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 接続する証拠ノード
    UPROPERTY(BlueprintReadWrite, Category = "Connection")
    class UEvidenceNodeWidget* StartNode;

    UPROPERTY(BlueprintReadWrite, Category = "Connection")
    class UEvidenceNodeWidget* EndNode;

    // 接続データ
    UPROPERTY(BlueprintReadWrite, Category = "Connection")
    FEvidenceConnection ConnectionData;

    // 線の種類とスタイル
    UPROPERTY(BlueprintReadWrite, Category = "Visual")
    EConnectionLineType LineType = EConnectionLineType::Reality;

    UPROPERTY(BlueprintReadWrite, Category = "Visual")
    FLinearColor LineColor = FLinearColor::White;

    UPROPERTY(BlueprintReadWrite, Category = "Visual")
    float LineThickness = 2.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Visual")
    float LineOpacity = 1.0f;

    // 曲線制御（夢状態用）
    UPROPERTY(BlueprintReadWrite, Category = "Dream Visual")
    float CurvatureStrength = 0.5f;

    UPROPERTY(BlueprintReadWrite, Category = "Dream Visual")
    FVector2D ControlPoint1Offset = FVector2D(100.0f, 0.0f);

    UPROPERTY(BlueprintReadWrite, Category = "Dream Visual")
    FVector2D ControlPoint2Offset = FVector2D(-100.0f, 0.0f);

    // アニメーション
    UPROPERTY(BlueprintReadWrite, Category = "Animation")
    bool bAnimateFlow = false;

    UPROPERTY(BlueprintReadWrite, Category = "Animation")
    float FlowSpeed = 1.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Animation")
    float AnimationTime = 0.0f;

    // 状態
    UPROPERTY(BlueprintReadWrite, Category = "State")
    bool bIsSelected = false;

    UPROPERTY(BlueprintReadWrite, Category = "State")
    bool bIsHighlighted = false;

    UPROPERTY(BlueprintReadWrite, Category = "State")
    bool bIsVisible = true;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

    // マウスイベント
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

public:
    // パブリック関数
    UFUNCTION(BlueprintCallable, Category = "Connection")
    void SetConnection(UEvidenceNodeWidget* InStartNode, UEvidenceNodeWidget* InEndNode, const FEvidenceConnection& InConnectionData);

    UFUNCTION(BlueprintCallable, Category = "Visual")
    void SetLineType(EConnectionLineType NewLineType);

    UFUNCTION(BlueprintCallable, Category = "Visual")
    void SetSelected(bool bSelected);

    UFUNCTION(BlueprintCallable, Category = "Visual")
    void SetHighlighted(bool bHighlighted);

    UFUNCTION(BlueprintCallable, Category = "Visual")
    void UpdateLineColor();

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void StartFlowAnimation();

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void StopFlowAnimation();

    UFUNCTION(BlueprintCallable, Category = "Corruption")
    void ApplyCorruption(float CorruptionLevel);

    // ユーティリティ関数
    UFUNCTION(BlueprintCallable, Category = "Utility")
    bool IsPointOnLine(FVector2D Point, float Tolerance = 10.0f) const;

    UFUNCTION(BlueprintCallable, Category = "Utility")
    FVector2D GetLineCenter() const;

    UFUNCTION(BlueprintCallable, Category = "Utility")
    float GetLineLength() const;

    // デリゲート
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConnectionLineClicked, UConnectionLineWidget*, ClickedLine);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnConnectionLineHovered, UConnectionLineWidget*, HoveredLine, bool, bIsHovering);

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnConnectionLineClicked OnConnectionLineClicked;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnConnectionLineHovered OnConnectionLineHovered;

private:
    void DrawStraightLine(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32 LayerId) const;
    void DrawCurvedLine(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32 LayerId) const;
    void DrawCorruptedLine(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32 LayerId) const;
    void DrawFlowEffect(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32 LayerId) const;
    
    FVector2D GetStartPosition() const;
    FVector2D GetEndPosition() const;
    
    // 曲線計算用
    FVector2D CalculateBezierPoint(float t, FVector2D P0, FVector2D P1, FVector2D P2, FVector2D P3) const;
    TArray<FVector2D> GenerateBezierPoints(int32 NumPoints = 20) const;
};
