#pragma once

#include "CoreMinimal.h"
#include "ProjectVisibleBaseWidget.h"
#include "MemoryFragmentTypes.h"
#include "MemoryFragmentManager.h"
#include "ProjectVisibleMemoryViewerWidget.generated.h"

/**
 * Memory Viewer Widget for Project Visible
 * Displays and manages memory fragments in an interactive interface
 */
UCLASS(BlueprintType, Blueprintable)
class GEMINI_C_API UProjectVisibleMemoryViewerWidget : public UProjectVisibleBaseWidget
{
    GENERATED_BODY()

protected:
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
    // Initialize widget content
    virtual void InitializeWidget() override;

    // Update memory fragment display data
    UFUNCTION(BlueprintCallable, Category = "Memory Viewer")
    void RefreshMemoryData();

    // Test functions for development
    UFUNCTION(BlueprintCallable, Category = "Memory Viewer")
    void TestUnlockRandomMemory();

protected:
    // Memory Fragment Manager reference
    UPROPERTY()
    class UMemoryFragmentManager* MemoryManager = nullptr;

    // Current display mode
    UPROPERTY(BlueprintReadOnly, Category = "Memory Viewer")
    bool bShowOnlyUnlocked = true;

    // Current filter type
    UPROPERTY(BlueprintReadOnly, Category = "Memory Viewer")
    EMemoryFragmentType CurrentFilterType = EMemoryFragmentType::Episodic;

    // Navigation functions
    UFUNCTION(BlueprintCallable, Category = "Memory Viewer")
    void ReturnToQuickMenu();

    // Detail view functions
    UFUNCTION(BlueprintCallable, Category = "Memory Viewer")
    void ShowMemoryDetails(const FString& MemoryID);

    UFUNCTION(BlueprintCallable, Category = "Memory Viewer")
    void HideMemoryDetails();

private:
    struct FMemoryFragmentButton
    {
        FString FragmentID;
        FText Title;
        FText Excerpt;
        EMemoryFragmentType Type;
        FVector2D Position;
        FVector2D Size;
        FLinearColor Color;
        bool bIsUnlocked;
        bool bIsViewed;
        float ImportanceScore;

        FMemoryFragmentButton() {}
        FMemoryFragmentButton(const FString& InID, const FText& InTitle, const FText& InExcerpt, EMemoryFragmentType InType,
                             const FVector2D& InPos, const FVector2D& InSize, const FLinearColor& InColor, 
                             bool bInUnlocked, bool bInViewed, float InImportance)
            : FragmentID(InID), Title(InTitle), Excerpt(InExcerpt), Type(InType), Position(InPos), Size(InSize), 
              Color(InColor), bIsUnlocked(bInUnlocked), bIsViewed(bInViewed), ImportanceScore(InImportance) {}
    };

    struct FFilterButton
    {
        EMemoryFragmentType FilterType;
        FString Text;
        FVector2D Position;
        FVector2D Size;
        FLinearColor Color;
        bool bIsSelected;

        FFilterButton() {}
        FFilterButton(EMemoryFragmentType InType, const FString& InText, const FVector2D& InPos, const FVector2D& InSize, const FLinearColor& InColor, bool bSelected)
            : FilterType(InType), Text(InText), Position(InPos), Size(InSize), Color(InColor), bIsSelected(bSelected) {}
    };

    void DrawMemoryViewer(FSlateWindowElementList& OutDrawElements, const FGeometry& AllottedGeometry, int32 LayerId, const FVector2D& PanelPos, const FVector2D& PanelSize) const;
    void DrawMemoryDetailsPanel(FSlateWindowElementList& OutDrawElements, const FGeometry& AllottedGeometry, int32 LayerId, const FVector2D& PanelPos, const FVector2D& PanelSize) const;
    void InitializeMemoryButtons();
    void InitializeFilterButtons();
    void HandleMemoryFragmentClick(const FString& FragmentID);
    void HandleFilterButtonClick(EMemoryFragmentType FilterType);
    void CloseMemoryViewer();

    FMemoryFragmentButton* GetMemoryButtonAtPosition(const FVector2D& LocalPosition, const FVector2D& PanelPos, const FVector2D& PanelSize);
    FFilterButton* GetFilterButtonAtPosition(const FVector2D& LocalPosition, const FVector2D& PanelPos, const FVector2D& PanelSize);

    TArray<FMemoryFragmentButton> MemoryButtons;
    TArray<FFilterButton> FilterButtons;
    FString SelectedFragmentID;
    
    // Detail view state
    bool bIsShowingDetails = false;
    FString DetailMemoryID;

    // Layout constants
    static constexpr float PANEL_WIDTH = 800.0f;
    static constexpr float PANEL_HEIGHT = 600.0f;
    static constexpr float MEMORY_BUTTON_WIDTH = 180.0f;
    static constexpr float MEMORY_BUTTON_HEIGHT = 120.0f;
    static constexpr float FILTER_BUTTON_WIDTH = 100.0f;
    static constexpr float FILTER_BUTTON_HEIGHT = 30.0f;
};
