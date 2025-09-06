#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Evidence/EvidenceData.h"
#include "UI/EvidenceNodeWidget.h"
#include "UI/ConnectionLineWidget.h"
#include "CanvasOfThoughtWidget.generated.h"

UENUM(BlueprintType)
enum class ECanvasMode : uint8
{
    Reality     UMETA(DisplayName = "現実（推理ボード）"),
    Dream       UMETA(DisplayName = "夢（ロジックボード）"),
    Corrupted   UMETA(DisplayName = "腐敗（境界崩壊）")
};

UENUM(BlueprintType)
enum class EAnalysisResult : uint8
{
    NoConnection        UMETA(DisplayName = "関連性なし"),
    WeakConnection      UMETA(DisplayName = "弱い関連性"),
    StrongConnection    UMETA(DisplayName = "強い関連性"),
    Contradiction      UMETA(DisplayName = "矛盾"),
    CausalChain        UMETA(DisplayName = "因果関係"),
    NewInsight         UMETA(DisplayName = "新たな洞察")
};

USTRUCT(BlueprintType)
struct GEMINI_API FCanvasState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    ECanvasMode CurrentMode = ECanvasMode::Reality;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    float CorruptionLevel = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    TArray<FString> ActiveEvidenceIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    TArray<FEvidenceConnection> ActiveConnections;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    int32 MaxEvidenceCapacity = 20;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
    bool bIsAnalyzing = false;
};

UCLASS()
class GEMINI_API UCanvasOfThoughtWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // ウィジェットコンポーネント
    UPROPERTY(meta = (BindWidget))
    class UCanvasPanel* MainCanvas;

    UPROPERTY(meta = (BindWidget))
    class UScrollBox* EvidenceInventory;

    UPROPERTY(meta = (BindWidget))
    class UButton* AnalyzeButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* ClearButton;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ModeText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* StatusText;

    // キャンバス状態
    UPROPERTY(BlueprintReadWrite, Category = "Canvas")
    FCanvasState CanvasState;

    // 証拠ノードとライン管理
    UPROPERTY(BlueprintReadWrite, Category = "Nodes")
    TArray<UEvidenceNodeWidget*> ActiveNodes;

    UPROPERTY(BlueprintReadWrite, Category = "Connections")
    TArray<UConnectionLineWidget*> ActiveLines;

    // 選択中のノード
    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    UEvidenceNodeWidget* SelectedNode;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    UEvidenceNodeWidget* ConnectionStartNode;

    // テンプレートクラス
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Templates")
    TSubclassOf<UEvidenceNodeWidget> EvidenceNodeClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Templates")
    TSubclassOf<UConnectionLineWidget> ConnectionLineClass;

    // 証拠データベース
    UPROPERTY(BlueprintReadWrite, Category = "Data")
    TArray<FEvidenceData> AvailableEvidence;

    UPROPERTY(BlueprintReadWrite, Category = "Data")
    TMap<FString, UEvidenceNodeWidget*> NodeMap;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // ドロップ処理
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

public:
    // 証拠管理
    UFUNCTION(BlueprintCallable, Category = "Evidence")
    void AddEvidenceToInventory(const FEvidenceData& EvidenceData);

    UFUNCTION(BlueprintCallable, Category = "Evidence")
    UEvidenceNodeWidget* CreateEvidenceNode(const FEvidenceData& EvidenceData, FVector2D Position);

    UFUNCTION(BlueprintCallable, Category = "Evidence")
    void RemoveEvidenceNode(UEvidenceNodeWidget* NodeToRemove);

    UFUNCTION(BlueprintCallable, Category = "Evidence")
    void MoveEvidenceNode(UEvidenceNodeWidget* NodeToMove, FVector2D NewPosition);

    // 接続管理
    UFUNCTION(BlueprintCallable, Category = "Connections")
    UConnectionLineWidget* CreateConnection(UEvidenceNodeWidget* StartNode, UEvidenceNodeWidget* EndNode, const FString& RelationshipType);

    UFUNCTION(BlueprintCallable, Category = "Connections")
    void RemoveConnection(UConnectionLineWidget* ConnectionToRemove);

    UFUNCTION(BlueprintCallable, Category = "Connections")
    void RemoveAllConnections();

    // 分析システム
    UFUNCTION(BlueprintCallable, Category = "Analysis")
    EAnalysisResult AnalyzeEvidence();

    UFUNCTION(BlueprintCallable, Category = "Analysis")
    TArray<FString> GenerateInsights();

    UFUNCTION(BlueprintCallable, Category = "Analysis")
    bool ValidateTheory();

    // キャンバスモード制御
    UFUNCTION(BlueprintCallable, Category = "Mode")
    void SetCanvasMode(ECanvasMode NewMode);

    UFUNCTION(BlueprintCallable, Category = "Mode")
    void ApplyCorruption(float CorruptionLevel);

    UFUNCTION(BlueprintCallable, Category = "Mode")
    void UpdateVisualStyle();

    // 選択とハイライト
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void SelectNode(UEvidenceNodeWidget* NodeToSelect);

    UFUNCTION(BlueprintCallable, Category = "Selection")
    void DeselectAllNodes();

    UFUNCTION(BlueprintCallable, Category = "Selection")
    void HighlightConnectedNodes(UEvidenceNodeWidget* CenterNode);

    // ユーティリティ
    UFUNCTION(BlueprintCallable, Category = "Utility")
    void ClearCanvas();

    UFUNCTION(BlueprintCallable, Category = "Utility")
    void SaveCanvasState();

    UFUNCTION(BlueprintCallable, Category = "Utility")
    void LoadCanvasState();

    UFUNCTION(BlueprintCallable, Category = "Utility")
    FString ExportCanvasAsText();

    // イベントハンドラー
    UFUNCTION(BlueprintCallable, Category = "Events")
    void OnEvidenceNodeClicked(UEvidenceNodeWidget* ClickedNode, const FEvidenceData& EvidenceData);

    UFUNCTION(BlueprintCallable, Category = "Events")
    void OnEvidenceNodeMoved(UEvidenceNodeWidget* MovedNode, FVector2D NewPosition);

    UFUNCTION(BlueprintCallable, Category = "Events")
    void OnConnectionLineClicked(UConnectionLineWidget* ClickedLine);

    UFUNCTION(BlueprintCallable, Category = "Events")
    void OnAnalyzeButtonClicked();

    UFUNCTION(BlueprintCallable, Category = "Events")
    void OnClearButtonClicked();

    // デリゲート
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCanvasModeChanged, ECanvasMode, NewMode);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnalysisCompleted, EAnalysisResult, Result);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInsightGenerated, const FString&, Insight);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEvidenceAdded, const FEvidenceData&, EvidenceData, UEvidenceNodeWidget*, CreatedNode);

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnCanvasModeChanged OnCanvasModeChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnAnalysisCompleted OnAnalysisCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnInsightGenerated OnInsightGenerated;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnEvidenceAdded OnEvidenceAdded;

private:
    void UpdateCanvasVisuals();
    void UpdateModeText();
    void UpdateStatusText();
    
    // 分析アルゴリズム
    float CalculateConnectionStrength(const FEvidenceData& Evidence1, const FEvidenceData& Evidence2);
    bool CheckForContradictions();
    TArray<FString> FindCausalChains();
    FString GenerateRandomInsight(); // 仮実装
    
    // 腐敗エフェクト
    void ApplyCorruptionToNodes();
    void ApplyCorruptionToLines();
    void StartCorruptionAnimation();
};
