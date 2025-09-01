#include "UI/CanvasOfThoughtWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UI/EvidenceNodeWidget.h"

void UCanvasOfThoughtWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // ボタンイベントをバインド
    if (AnalyzeButton)
    {
        AnalyzeButton->OnClicked.AddDynamic(this, &UCanvasOfThoughtWidget::OnAnalyzeButtonClicked);
    }
    
    if (ClearButton)
    {
        ClearButton->OnClicked.AddDynamic(this, &UCanvasOfThoughtWidget::OnClearButtonClicked);
    }
    
    // 初期状態を設定
    CanvasState.CurrentMode = ECanvasMode::Reality;
    CanvasState.CorruptionLevel = 0.0f;
    
    UpdateModeText();
    UpdateStatusText();
}

void UCanvasOfThoughtWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    // 腐敗レベルが変化している場合、ビジュアルを更新
    if (CanvasState.CorruptionLevel > 0.0f)
    {
        UpdateCanvasVisuals();
    }
}

bool UCanvasOfThoughtWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    UEvidenceNodeDragDropOperation* EvidenceDragDrop = Cast<UEvidenceNodeDragDropOperation>(InOperation);
    if (!EvidenceDragDrop)
        return false;

    // ドロップ位置を計算
    FVector2D LocalMousePosition = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
    
    // 既存ノードの移動かどうかチェック
    if (EvidenceDragDrop->SourceWidget)
    {
        // 既存ノードの移動
        MoveEvidenceNode(EvidenceDragDrop->SourceWidget, LocalMousePosition);
    }
    else
    {
        // 新しい証拠ノードを作成
        CreateEvidenceNode(EvidenceDragDrop->EvidenceData, LocalMousePosition);
    }
    
    return true;
}

void UCanvasOfThoughtWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    // ドラッグエンター時のビジュアルフィードバック
    if (MainCanvas)
    {
        // キャンバスをハイライト
    }
}

void UCanvasOfThoughtWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    // ドラッグリーブ時のビジュアルリセット
    if (MainCanvas)
    {
        // ハイライトを解除
    }
}

void UCanvasOfThoughtWidget::AddEvidenceToInventory(const FEvidenceData& EvidenceData)
{
    // 重複チェック
    bool bAlreadyExists = AvailableEvidence.ContainsByPredicate([&EvidenceData](const FEvidenceData& Existing)
    {
        return Existing.EvidenceID == EvidenceData.EvidenceID;
    });
    
    if (!bAlreadyExists)
    {
        AvailableEvidence.Add(EvidenceData);
        
        // インベントリUIに追加（ここでは簡単な実装）
        // 実際にはスクロールボックス内にドラッグ可能なアイテムウィジェットを作成
    }
}

UEvidenceNodeWidget* UCanvasOfThoughtWidget::CreateEvidenceNode(const FEvidenceData& EvidenceData, FVector2D Position)
{
    if (!EvidenceNodeClass || !MainCanvas)
        return nullptr;
    
    // 容量チェック
    if (ActiveNodes.Num() >= CanvasState.MaxEvidenceCapacity)
    {
        UE_LOG(LogTemp, Warning, TEXT("Canvas capacity exceeded"));
        return nullptr;
    }
    
    // ノードウィジェットを作成
    UEvidenceNodeWidget* NewNode = CreateWidget<UEvidenceNodeWidget>(this, EvidenceNodeClass);
    if (!NewNode)
        return nullptr;
    
    // 証拠データを設定
    NewNode->SetEvidenceData(EvidenceData);
    NewNode->SetCanvasPosition(Position);
    
    // イベントをバインド
    NewNode->OnEvidenceNodeClicked.AddDynamic(this, &UCanvasOfThoughtWidget::OnEvidenceNodeClicked);
    NewNode->OnEvidenceNodeMoved.AddDynamic(this, &UCanvasOfThoughtWidget::OnEvidenceNodeMoved);
    
    // キャンバスに追加
    UCanvasPanelSlot* CanvasSlot = MainCanvas->AddChildToCanvas(NewNode);
    if (CanvasSlot)
    {
        CanvasSlot->SetPosition(Position);
        CanvasSlot->SetSize(FVector2D(150.0f, 100.0f)); // デフォルトサイズ
    }
    
    // 管理配列に追加
    ActiveNodes.Add(NewNode);
    NodeMap.Add(EvidenceData.EvidenceID, NewNode);
    CanvasState.ActiveEvidenceIDs.Add(EvidenceData.EvidenceID);
    
    // イベントを発火
    OnEvidenceAdded.Broadcast(EvidenceData, NewNode);
    
    UpdateStatusText();
    return NewNode;
}

void UCanvasOfThoughtWidget::RemoveEvidenceNode(UEvidenceNodeWidget* NodeToRemove)
{
    if (!NodeToRemove)
        return;
    
    // 関連する接続ラインを削除
    for (int32 i = ActiveLines.Num() - 1; i >= 0; --i)
    {
        UConnectionLineWidget* Line = ActiveLines[i];
        if (Line && (Line->StartNode == NodeToRemove || Line->EndNode == NodeToRemove))
        {
            RemoveConnection(Line);
        }
    }
    
    // 管理配列から削除
    ActiveNodes.Remove(NodeToRemove);
    CanvasState.ActiveEvidenceIDs.Remove(NodeToRemove->EvidenceData.EvidenceID);
    NodeMap.Remove(NodeToRemove->EvidenceData.EvidenceID);
    
    // ウィジェットを削除
    NodeToRemove->RemoveFromParent();
    
    UpdateStatusText();
}

void UCanvasOfThoughtWidget::MoveEvidenceNode(UEvidenceNodeWidget* NodeToMove, FVector2D NewPosition)
{
    if (!NodeToMove || !MainCanvas)
        return;
    
    // キャンバス上での位置を更新
    if (UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(NodeToMove))
    {
        CanvasSlot->SetPosition(NewPosition);
    }
    
    NodeToMove->SetCanvasPosition(NewPosition);
    
    // 接続ラインの再描画をトリガー
    for (UConnectionLineWidget* Line : NodeToMove->ConnectedLines)
    {
        if (Line)
        {
            // ラインの再描画は自動的に行われる（NativePaintで）
        }
    }
}

UConnectionLineWidget* UCanvasOfThoughtWidget::CreateConnection(UEvidenceNodeWidget* StartNode, UEvidenceNodeWidget* EndNode, const FString& RelationshipType)
{
    if (!StartNode || !EndNode || !ConnectionLineClass || !MainCanvas)
        return nullptr;
    
    // 既存の接続をチェック
    for (UConnectionLineWidget* ExistingLine : ActiveLines)
    {
        if (ExistingLine && 
            ((ExistingLine->StartNode == StartNode && ExistingLine->EndNode == EndNode) ||
             (ExistingLine->StartNode == EndNode && ExistingLine->EndNode == StartNode)))
        {
            return ExistingLine; // 既に接続済み
        }
    }
    
    // 接続ラインを作成
    UConnectionLineWidget* NewLine = CreateWidget<UConnectionLineWidget>(this, ConnectionLineClass);
    if (!NewLine)
        return nullptr;
    
    // 接続データを設定
    FEvidenceConnection ConnectionData;
    ConnectionData.ConnectedEvidenceID = EndNode->EvidenceData.EvidenceID;
    ConnectionData.RelationshipType = RelationshipType;
    ConnectionData.ConnectionStrength = CalculateConnectionStrength(StartNode->EvidenceData, EndNode->EvidenceData);
    ConnectionData.bIsConfirmed = false;
    
    NewLine->SetConnection(StartNode, EndNode, ConnectionData);
    
    // キャンバスモードに応じてラインタイプを設定
    switch (CanvasState.CurrentMode)
    {
        case ECanvasMode::Reality:
            NewLine->SetLineType(EConnectionLineType::Reality);
            break;
        case ECanvasMode::Dream:
            NewLine->SetLineType(EConnectionLineType::Dream);
            break;
        case ECanvasMode::Corrupted:
            NewLine->SetLineType(EConnectionLineType::Corrupted);
            break;
    }
    
    // イベントをバインド
    NewLine->OnConnectionLineClicked.AddDynamic(this, &UCanvasOfThoughtWidget::OnConnectionLineClicked);
    
    // キャンバスに追加
    MainCanvas->AddChildToCanvas(NewLine);
    
    // 管理配列に追加
    ActiveLines.Add(NewLine);
    CanvasState.ActiveConnections.Add(ConnectionData);
    
    return NewLine;
}

void UCanvasOfThoughtWidget::RemoveConnection(UConnectionLineWidget* ConnectionToRemove)
{
    if (!ConnectionToRemove)
        return;
    
    // ノードから接続を削除
    if (ConnectionToRemove->StartNode)
    {
        ConnectionToRemove->StartNode->RemoveConnectionLine(ConnectionToRemove);
    }
    if (ConnectionToRemove->EndNode)
    {
        ConnectionToRemove->EndNode->RemoveConnectionLine(ConnectionToRemove);
    }
    
    // 管理配列から削除
    ActiveLines.Remove(ConnectionToRemove);
    
    // 接続データも削除
    CanvasState.ActiveConnections.RemoveAll([ConnectionToRemove](const FEvidenceConnection& Connection)
    {
        return Connection.ConnectedEvidenceID == ConnectionToRemove->ConnectionData.ConnectedEvidenceID;
    });
    
    // ウィジェットを削除
    ConnectionToRemove->RemoveFromParent();
}

void UCanvasOfThoughtWidget::RemoveAllConnections()
{
    for (int32 i = ActiveLines.Num() - 1; i >= 0; --i)
    {
        RemoveConnection(ActiveLines[i]);
    }
}

EAnalysisResult UCanvasOfThoughtWidget::AnalyzeEvidence()
{
    CanvasState.bIsAnalyzing = true;
    
    // 分析アルゴリズムを実行
    if (ActiveNodes.Num() < 2)
    {
        CanvasState.bIsAnalyzing = false;
        return EAnalysisResult::NoConnection;
    }
    
    // 矛盾をチェック
    if (CheckForContradictions())
    {
        CanvasState.bIsAnalyzing = false;
        return EAnalysisResult::Contradiction;
    }
    
    // 因果関係チェーンを探索
    TArray<FString> CausalChains = FindCausalChains();
    if (CausalChains.Num() > 0)
    {
        CanvasState.bIsAnalyzing = false;
        return EAnalysisResult::CausalChain;
    }
    
    // 新しい洞察を生成
    TArray<FString> Insights = GenerateInsights();
    if (Insights.Num() > 0)
    {
        for (const FString& Insight : Insights)
        {
            OnInsightGenerated.Broadcast(Insight);
        }
        CanvasState.bIsAnalyzing = false;
        return EAnalysisResult::NewInsight;
    }
    
    // 接続の強度を評価
    float TotalConnectionStrength = 0.0f;
    for (const UConnectionLineWidget* Line : ActiveLines)
    {
        if (Line)
        {
            TotalConnectionStrength += Line->ConnectionData.ConnectionStrength;
        }
    }
    
    float AverageStrength = ActiveLines.Num() > 0 ? TotalConnectionStrength / ActiveLines.Num() : 0.0f;
    
    CanvasState.bIsAnalyzing = false;
    
    if (AverageStrength > 0.7f)
    {
        return EAnalysisResult::StrongConnection;
    }
    else if (AverageStrength > 0.3f)
    {
        return EAnalysisResult::WeakConnection;
    }
    
    return EAnalysisResult::NoConnection;
}

TArray<FString> UCanvasOfThoughtWidget::GenerateInsights()
{
    TArray<FString> Insights;
    
    // 基本的な洞察生成ロジック
    if (ActiveNodes.Num() >= 3)
    {
        Insights.Add(TEXT("複数の証拠間に隠された関連性が見つかりました"));
    }
    
    if (ActiveLines.Num() >= 5)
    {
        Insights.Add(TEXT("証拠の関係性が複雑になっています。整理が必要かもしれません"));
    }
    
    // ランダムな洞察を追加（企画書のテーマに沿った内容）
    if (FMath::RandRange(0, 100) < 30) // 30%の確率
    {
        Insights.Add(GenerateRandomInsight());
    }
    
    return Insights;
}

bool UCanvasOfThoughtWidget::ValidateTheory()
{
    // 理論の妥当性をチェック
    if (ActiveNodes.Num() < 3)
        return false;
    
    if (ActiveLines.Num() < 2)
        return false;
    
    // 循環論理をチェック
    // 矛盾をチェック
    if (CheckForContradictions())
        return false;
    
    return true;
}

void UCanvasOfThoughtWidget::SetCanvasMode(ECanvasMode NewMode)
{
    if (CanvasState.CurrentMode == NewMode)
        return;
    
    CanvasState.CurrentMode = NewMode;
    
    // 全ての接続ラインのタイプを更新
    for (UConnectionLineWidget* Line : ActiveLines)
    {
        if (Line)
        {
            switch (NewMode)
            {
                case ECanvasMode::Reality:
                    Line->SetLineType(EConnectionLineType::Reality);
                    break;
                case ECanvasMode::Dream:
                    Line->SetLineType(EConnectionLineType::Dream);
                    Line->StartFlowAnimation();
                    break;
                case ECanvasMode::Corrupted:
                    Line->SetLineType(EConnectionLineType::Corrupted);
                    break;
            }
        }
    }
    
    UpdateVisualStyle();
    UpdateModeText();
    
    OnCanvasModeChanged.Broadcast(NewMode);
}

void UCanvasOfThoughtWidget::ApplyCorruption(float CorruptionLevel)
{
    CanvasState.CorruptionLevel = FMath::Clamp(CorruptionLevel, 0.0f, 1.0f);
    
    ApplyCorruptionToNodes();
    ApplyCorruptionToLines();
    
    if (CorruptionLevel > 0.5f)
    {
        SetCanvasMode(ECanvasMode::Corrupted);
    }
}

void UCanvasOfThoughtWidget::UpdateVisualStyle()
{
    // キャンバスの背景色や全体的な見た目を更新
    // 実装は具体的なUIデザインに依存
}

void UCanvasOfThoughtWidget::SelectNode(UEvidenceNodeWidget* NodeToSelect)
{
    // 以前の選択を解除
    if (SelectedNode)
    {
        SelectedNode->SetSelected(false);
    }
    
    SelectedNode = NodeToSelect;
    
    if (SelectedNode)
    {
        SelectedNode->SetSelected(true);
        HighlightConnectedNodes(SelectedNode);
    }
}

void UCanvasOfThoughtWidget::DeselectAllNodes()
{
    for (UEvidenceNodeWidget* Node : ActiveNodes)
    {
        if (Node)
        {
            Node->SetSelected(false);
        }
    }
    
    SelectedNode = nullptr;
}

void UCanvasOfThoughtWidget::HighlightConnectedNodes(UEvidenceNodeWidget* CenterNode)
{
    if (!CenterNode)
        return;
    
    // 全ノードのハイライトを解除
    for (UEvidenceNodeWidget* Node : ActiveNodes)
    {
        if (Node && Node != CenterNode)
        {
            Node->EvidenceData.bIsHighlighted = false;
        }
    }
    
    // 接続されたノードをハイライト
    for (UConnectionLineWidget* Line : CenterNode->ConnectedLines)
    {
        if (Line)
        {
            if (Line->StartNode && Line->StartNode != CenterNode)
            {
                Line->StartNode->EvidenceData.bIsHighlighted = true;
            }
            if (Line->EndNode && Line->EndNode != CenterNode)
            {
                Line->EndNode->EvidenceData.bIsHighlighted = true;
            }
        }
    }
}

void UCanvasOfThoughtWidget::ClearCanvas()
{
    // 全ての接続を削除
    RemoveAllConnections();
    
    // 全てのノードを削除
    for (int32 i = ActiveNodes.Num() - 1; i >= 0; --i)
    {
        RemoveEvidenceNode(ActiveNodes[i]);
    }
    
    // 状態をリセット
    CanvasState.ActiveEvidenceIDs.Empty();
    CanvasState.ActiveConnections.Empty();
    SelectedNode = nullptr;
    ConnectionStartNode = nullptr;
    
    UpdateStatusText();
}

void UCanvasOfThoughtWidget::SaveCanvasState()
{
    // キャンバス状態をファイルやセーブゲームに保存
    // 実装はセーブシステムに依存
}

void UCanvasOfThoughtWidget::LoadCanvasState()
{
    // 保存されたキャンバス状態を読み込み
    // 実装はセーブシステムに依存
}

FString UCanvasOfThoughtWidget::ExportCanvasAsText()
{
    FString Result = TEXT("Canvas of Thought Export\n");
    Result += FString::Printf(TEXT("Mode: %s\n"), 
        CanvasState.CurrentMode == ECanvasMode::Reality ? TEXT("Reality") :
        CanvasState.CurrentMode == ECanvasMode::Dream ? TEXT("Dream") : TEXT("Corrupted"));
    
    Result += TEXT("\nEvidence:\n");
    for (const UEvidenceNodeWidget* Node : ActiveNodes)
    {
        if (Node)
        {
            Result += FString::Printf(TEXT("- %s (%s)\n"), 
                *Node->EvidenceData.Title, 
                *Node->EvidenceData.EvidenceID);
        }
    }
    
    Result += TEXT("\nConnections:\n");
    for (const UConnectionLineWidget* Line : ActiveLines)
    {
        if (Line && Line->StartNode && Line->EndNode)
        {
            Result += FString::Printf(TEXT("- %s -> %s (%s)\n"),
                *Line->StartNode->EvidenceData.Title,
                *Line->EndNode->EvidenceData.Title,
                *Line->ConnectionData.RelationshipType);
        }
    }
    
    return Result;
}

// イベントハンドラー
void UCanvasOfThoughtWidget::OnEvidenceNodeClicked(UEvidenceNodeWidget* ClickedNode, const FEvidenceData& EvidenceData)
{
    if (!ClickedNode)
        return;
    
    // 接続モードの場合
    if (ConnectionStartNode && ConnectionStartNode != ClickedNode)
    {
        // 接続を作成
        CreateConnection(ConnectionStartNode, ClickedNode, TEXT("Related"));
        ConnectionStartNode = nullptr;
    }
    else if (ConnectionStartNode == ClickedNode)
    {
        // 同じノードをクリックした場合、接続モードを解除
        ConnectionStartNode = nullptr;
    }
    else
    {
        // 通常の選択
        SelectNode(ClickedNode);
        
        // Shiftキーが押されている場合は接続モードを開始
        // 実装は入力システムに依存
        ConnectionStartNode = ClickedNode;
    }
}

void UCanvasOfThoughtWidget::OnEvidenceNodeMoved(UEvidenceNodeWidget* MovedNode, FVector2D NewPosition)
{
    // ノード移動時の追加処理があれば実装
}

void UCanvasOfThoughtWidget::OnConnectionLineClicked(UConnectionLineWidget* ClickedLine)
{
    if (ClickedLine)
    {
        // 接続ラインが選択された時の処理
        // 例：接続の詳細情報を表示、接続の削除など
    }
}

void UCanvasOfThoughtWidget::OnAnalyzeButtonClicked()
{
    EAnalysisResult Result = AnalyzeEvidence();
    OnAnalysisCompleted.Broadcast(Result);
    
    // 結果に応じてフィードバック
    FString ResultText;
    switch (Result)
    {
        case EAnalysisResult::NoConnection:
            ResultText = TEXT("明確な関連性は見つかりませんでした");
            break;
        case EAnalysisResult::WeakConnection:
            ResultText = TEXT("弱い関連性が見つかりました");
            break;
        case EAnalysisResult::StrongConnection:
            ResultText = TEXT("強い関連性が見つかりました");
            break;
        case EAnalysisResult::Contradiction:
            ResultText = TEXT("矛盾が検出されました");
            break;
        case EAnalysisResult::CausalChain:
            ResultText = TEXT("因果関係チェーンが発見されました");
            break;
        case EAnalysisResult::NewInsight:
            ResultText = TEXT("新たな洞察が得られました");
            break;
    }
    
    if (StatusText)
    {
        StatusText->SetText(FText::FromString(ResultText));
    }
}

void UCanvasOfThoughtWidget::OnClearButtonClicked()
{
    ClearCanvas();
}

// プライベート関数の実装
void UCanvasOfThoughtWidget::UpdateCanvasVisuals()
{
    // 腐敗レベルに応じたビジュアル更新
    ApplyCorruptionToNodes();
    ApplyCorruptionToLines();
}

void UCanvasOfThoughtWidget::UpdateModeText()
{
    if (!ModeText)
        return;
    
    FString ModeString;
    switch (CanvasState.CurrentMode)
    {
        case ECanvasMode::Reality:
            ModeString = TEXT("推理ボード");
            break;
        case ECanvasMode::Dream:
            ModeString = TEXT("ロジックボード");
            break;
        case ECanvasMode::Corrupted:
            ModeString = TEXT("境界崩壊");
            break;
    }
    
    ModeText->SetText(FText::FromString(ModeString));
}

void UCanvasOfThoughtWidget::UpdateStatusText()
{
    if (!StatusText)
        return;
    
    FString StatusString = FString::Printf(TEXT("証拠: %d/%d | 接続: %d"), 
        ActiveNodes.Num(), 
        CanvasState.MaxEvidenceCapacity, 
        ActiveLines.Num());
    
    StatusText->SetText(FText::FromString(StatusString));
}

float UCanvasOfThoughtWidget::CalculateConnectionStrength(const FEvidenceData& Evidence1, const FEvidenceData& Evidence2)
{
    float Strength = 0.0f;
    
    // 同じタイプの証拠
    if (Evidence1.Type == Evidence2.Type)
    {
        Strength += 0.2f;
    }
    
    // 同じ場所で発見
    if (Evidence1.LocationFound == Evidence2.LocationFound && !Evidence1.LocationFound.IsEmpty())
    {
        Strength += 0.3f;
    }
    
    // 同じキャラクターに関連
    if (Evidence1.RelatedCharacter == Evidence2.RelatedCharacter && !Evidence1.RelatedCharacter.IsEmpty())
    {
        Strength += 0.4f;
    }
    
    // 時間的近接性
    FTimespan TimeDiff = FMath::Abs(Evidence1.DiscoveryTime - Evidence2.DiscoveryTime);
    if (TimeDiff.GetTotalHours() < 24.0)
    {
        Strength += 0.1f;
    }
    
    return FMath::Clamp(Strength, 0.0f, 1.0f);
}

bool UCanvasOfThoughtWidget::CheckForContradictions()
{
    // 矛盾検出の基本実装
    for (int32 i = 0; i < ActiveNodes.Num(); ++i)
    {
        for (int32 j = i + 1; j < ActiveNodes.Num(); ++j)
        {
            const UEvidenceNodeWidget* Node1 = ActiveNodes[i];
            const UEvidenceNodeWidget* Node2 = ActiveNodes[j];
            
            if (!Node1 || !Node2)
                continue;
            
            // 同じIDの証拠が異なる信頼度を持つ場合
            if (Node1->EvidenceData.EvidenceID == Node2->EvidenceData.EvidenceID &&
                Node1->EvidenceData.Reliability != Node2->EvidenceData.Reliability)
            {
                return true;
            }
            
            // その他の矛盾チェックロジック
        }
    }
    
    return false;
}

TArray<FString> UCanvasOfThoughtWidget::FindCausalChains()
{
    TArray<FString> CausalChains;
    
    // 因果関係チェーンの発見アルゴリズム
    for (const UConnectionLineWidget* Line : ActiveLines)
    {
        if (Line && Line->ConnectionData.RelationshipType == TEXT("Causal"))
        {
            FString Chain = FString::Printf(TEXT("%s -> %s"),
                Line->StartNode ? *Line->StartNode->EvidenceData.Title : TEXT("Unknown"),
                Line->EndNode ? *Line->EndNode->EvidenceData.Title : TEXT("Unknown"));
            CausalChains.Add(Chain);
        }
    }
    
    return CausalChains;
}

FString UCanvasOfThoughtWidget::GenerateRandomInsight()
{
    TArray<FString> PossibleInsights = {
        TEXT("この証拠は夢の記憶と関連している可能性があります"),
        TEXT("アンドロイド化プロセスに何らかの異常が見られます"),
        TEXT("企業の実験データに一貫性がありません"),
        TEXT("ニコマコスの実験は予想以上に進んでいるようです"),
        TEXT("現実と夢の境界が曖昧になってきています")
    };
    
    if (PossibleInsights.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, PossibleInsights.Num() - 1);
        return PossibleInsights[RandomIndex];
    }
    
    return TEXT("新たな発見があります");
}

void UCanvasOfThoughtWidget::ApplyCorruptionToNodes()
{
    for (UEvidenceNodeWidget* Node : ActiveNodes)
    {
        if (Node)
        {
            Node->SetCorruptionLevel(CanvasState.CorruptionLevel);
        }
    }
}

void UCanvasOfThoughtWidget::ApplyCorruptionToLines()
{
    for (UConnectionLineWidget* Line : ActiveLines)
    {
        if (Line)
        {
            Line->ApplyCorruption(CanvasState.CorruptionLevel);
        }
    }
}

void UCanvasOfThoughtWidget::StartCorruptionAnimation()
{
    // 腐敗アニメーションの開始
    // 実装はアニメーションシステムに依存
}
