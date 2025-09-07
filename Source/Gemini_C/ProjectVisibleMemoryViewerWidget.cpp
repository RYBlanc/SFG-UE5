#include "ProjectVisibleMemoryViewerWidget.h"
#include "ProjectVisibleUIManager.h"
#include "ProjectVisibleQuickMenuWidget.h"
#include "MemoryFragmentManager.h"
#include "Gemini_C.h"
#include "Rendering/DrawElements.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"

void UProjectVisibleMemoryViewerWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
    
    CurrentFilterType = EMemoryFragmentType::Episodic;
    bShowOnlyUnlocked = true;
    SelectedFragmentID = TEXT("");
}

void UProjectVisibleMemoryViewerWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Set as semi-transparent overlay
    SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.9f));
    SetRenderOpacity(0.9f);
    
    // Get Memory Fragment Manager
    UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer: Starting manager initialization"));
    
    if (UWorld* World = GetWorld())
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer: World found"));
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer: GameInstance found"));
            MemoryManager = GameInstance->GetSubsystem<UMemoryFragmentManager>();
            if (MemoryManager)
            {
                UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer: MemoryManager found and initializing"));
                // Initialize memory system if not already done
                MemoryManager->InitializeMemorySystem();
            }
            else
            {
                UE_LOG(LogProjectVisible, Error, TEXT("Memory Viewer: MemoryManager not found!"));
            }
        }
        else
        {
            UE_LOG(LogProjectVisible, Error, TEXT("Memory Viewer: GameInstance not found!"));
        }
    }
    else
    {
        UE_LOG(LogProjectVisible, Error, TEXT("Memory Viewer: World not found!"));
    }
    
    // Enable input handling
    bIsFocusable = true;
    
    UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer Widget constructed and input enabled"));
    
    // Show on screen debug message
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TEXT("Memory Viewer Opened!"));
    }
    
    // Test code removed - details panel rendering confirmed working
}

int32 UProjectVisibleMemoryViewerWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    // Draw background overlay
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(),
        FCoreStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor(0.0f, 0.0f, 0.0f, 0.8f)
    );

    // Calculate center panel
    FVector2D ScreenSize = AllottedGeometry.GetLocalSize();
    FVector2D PanelSize = FVector2D(PANEL_WIDTH, PANEL_HEIGHT);
    FVector2D PanelPos = (ScreenSize - PanelSize) * 0.5f;

    // Draw main panel background
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(PanelSize, FSlateLayoutTransform(PanelPos)),
        FCoreStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor(0.05f, 0.1f, 0.15f, 0.95f)
    );

    // Draw panel border
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(PanelSize, FSlateLayoutTransform(PanelPos)),
        FCoreStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor(0.4f, 0.6f, 0.8f, 1.0f)
    );

    // Draw memory viewer interface
    DrawMemoryViewer(OutDrawElements, AllottedGeometry, LayerId, PanelPos, PanelSize);

    return LayerId;
}

FReply UProjectVisibleMemoryViewerWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer: Mouse button down detected"));
    
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer: Left mouse button clicked"));
        FVector2D LocalPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
        FVector2D ScreenSize = InGeometry.GetLocalSize();
        FVector2D PanelSize = FVector2D(PANEL_WIDTH, PANEL_HEIGHT);
        FVector2D PanelPos = (ScreenSize - PanelSize) * 0.5f;
        
        UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer: Click at (%f,%f), Panel at (%f,%f)"), 
               LocalPosition.X, LocalPosition.Y, PanelPos.X, PanelPos.Y);

        // Check if clicked inside the panel
        if (LocalPosition.X >= PanelPos.X && LocalPosition.X <= PanelPos.X + PanelSize.X &&
            LocalPosition.Y >= PanelPos.Y && LocalPosition.Y <= PanelPos.Y + PanelSize.Y)
        {
            UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer: Click inside panel detected"));
            // Check return button click
            FVector2D ReturnButtonPos = PanelPos + FVector2D(20.0f, 20.0f);
            FVector2D ReturnButtonSize = FVector2D(120.0f, 30.0f);
            if (LocalPosition.X >= ReturnButtonPos.X && LocalPosition.X <= ReturnButtonPos.X + ReturnButtonSize.X &&
                LocalPosition.Y >= ReturnButtonPos.Y && LocalPosition.Y <= ReturnButtonPos.Y + ReturnButtonSize.Y)
            {
                ReturnToQuickMenu();
                return FReply::Handled();
            }

            // Check if details panel is showing and handle its clicks
            if (bIsShowingDetails && !DetailMemoryID.IsEmpty())
            {
                FVector2D DetailsSize = FVector2D(500.0f, 400.0f);
                FVector2D DetailsPos = PanelPos + (PanelSize - DetailsSize) * 0.5f;
                
                // Check close button click
                FVector2D CloseButtonPos = DetailsPos + FVector2D(DetailsSize.X - 30.0f, 10.0f);
                FVector2D CloseButtonSize = FVector2D(20.0f, 20.0f);
                if (LocalPosition.X >= CloseButtonPos.X && LocalPosition.X <= CloseButtonPos.X + CloseButtonSize.X &&
                    LocalPosition.Y >= CloseButtonPos.Y && LocalPosition.Y <= CloseButtonPos.Y + CloseButtonSize.Y)
                {
                    HideMemoryDetails();
                    return FReply::Handled();
                }
                
                // If clicked inside details panel, consume the event but don't close
                if (LocalPosition.X >= DetailsPos.X && LocalPosition.X <= DetailsPos.X + DetailsSize.X &&
                    LocalPosition.Y >= DetailsPos.Y && LocalPosition.Y <= DetailsPos.Y + DetailsSize.Y)
                {
                    return FReply::Handled();
                }
            }

            // Check memory fragment button clicks (only if not showing details)
            if (!bIsShowingDetails)
            {
                FMemoryFragmentButton* ClickedMemoryButton = GetMemoryButtonAtPosition(LocalPosition, PanelPos, PanelSize);
                if (ClickedMemoryButton)
                {
                    UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer: Clicked memory button: %s"), *ClickedMemoryButton->FragmentID);
                    UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer: Currently selected: %s"), *SelectedFragmentID);
                    
                    // Check if memory is unlocked first
                    if (!ClickedMemoryButton->bIsUnlocked)
                    {
                        UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer: Memory is locked, cannot view details"));
                        if (GEngine)
                        {
                            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("この記憶はロックされています！"));
                        }
                        return FReply::Handled();
                    }
                    
                    // Temporary: Single-click to show details for testing
                    UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer: Showing details for: %s"), *ClickedMemoryButton->FragmentID);
                    ShowMemoryDetails(ClickedMemoryButton->FragmentID);
                    
                    // Also update selection
                    HandleMemoryFragmentClick(ClickedMemoryButton->FragmentID);
                    return FReply::Handled();
                }

                // Check filter button clicks
                FFilterButton* ClickedFilterButton = GetFilterButtonAtPosition(LocalPosition, PanelPos, PanelSize);
                if (ClickedFilterButton)
                {
                    HandleFilterButtonClick(ClickedFilterButton->FilterType);
                    return FReply::Handled();
                }
            }
        }
        else
        {
            // Clicked outside panel - close memory viewer
            CloseMemoryViewer();
            return FReply::Handled();
        }
    }

    return FReply::Unhandled();
}

void UProjectVisibleMemoryViewerWidget::InitializeWidget()
{
    Super::InitializeWidget();
    InitializeFilterButtons();
    InitializeMemoryButtons();
    RefreshMemoryData();
}

void UProjectVisibleMemoryViewerWidget::RefreshMemoryData()
{
    if (MemoryManager)
    {
        // Update memory buttons with current data
        InitializeMemoryButtons();
        
        UE_LOG(LogProjectVisible, Log, TEXT("Memory Viewer: Data refreshed"));
    }
    else
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer: MemoryManager not available"));
    }
}

void UProjectVisibleMemoryViewerWidget::TestUnlockRandomMemory()
{
    UE_LOG(LogProjectVisible, Warning, TEXT("TestUnlockRandomMemory: Starting test unlock"));
    
    if (MemoryManager)
    {
        TArray<FMemoryFragment> AllFragments = MemoryManager->GetAllMemoryFragments();
        UE_LOG(LogProjectVisible, Warning, TEXT("TestUnlockRandomMemory: Found %d total fragments"), AllFragments.Num());
        
        TArray<FMemoryFragment> LockedFragments;
        
        // Find locked fragments
        for (const FMemoryFragment& Fragment : AllFragments)
        {
            UE_LOG(LogProjectVisible, Warning, TEXT("TestUnlockRandomMemory: Fragment %s, Type: %d, Unlocked: %s"), 
                   *Fragment.FragmentID, (int32)Fragment.FragmentType, Fragment.bIsUnlocked ? TEXT("Yes") : TEXT("No"));
            
            if (!Fragment.bIsUnlocked)
            {
                LockedFragments.Add(Fragment);
            }
        }
        
        UE_LOG(LogProjectVisible, Warning, TEXT("TestUnlockRandomMemory: Found %d locked fragments"), LockedFragments.Num());
        
        if (LockedFragments.Num() > 0)
        {
            // Unlock a random fragment
            int32 RandomIndex = FMath::RandRange(0, LockedFragments.Num() - 1);
            FString FragmentID = LockedFragments[RandomIndex].FragmentID;
            
            if (MemoryManager->UnlockMemoryFragment(FragmentID))
            {
                RefreshMemoryData();
                
                UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer: Test unlocked fragment: %s"), *FragmentID);
                
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, 
                        FString::Printf(TEXT("Test Unlocked: %s"), *LockedFragments[RandomIndex].Title.ToString()));
                }
            }
        }
        else
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("All memories already unlocked!"));
            }
        }
    }
}

void UProjectVisibleMemoryViewerWidget::InitializeMemoryButtons()
{
    UE_LOG(LogProjectVisible, Warning, TEXT("InitializeMemoryButtons: Starting initialization"));
    MemoryButtons.Empty();

    if (!MemoryManager)
    {
        UE_LOG(LogProjectVisible, Error, TEXT("InitializeMemoryButtons: MemoryManager is null"));
        return;
    }

    // Get memory fragments based on current filter
    TArray<FMemoryFragment> DisplayFragments;
    if (bShowOnlyUnlocked)
    {
        DisplayFragments = MemoryManager->GetUnlockedMemoryFragments();
        UE_LOG(LogProjectVisible, Warning, TEXT("InitializeMemoryButtons: Got %d unlocked fragments"), DisplayFragments.Num());
    }
    else
    {
        DisplayFragments = MemoryManager->GetAllMemoryFragments();
        UE_LOG(LogProjectVisible, Warning, TEXT("InitializeMemoryButtons: Got %d all fragments"), DisplayFragments.Num());
    }

    // Temporarily disable filtering for debugging - show all unlocked fragments
    TArray<FMemoryFragment> FilteredFragments = DisplayFragments;
    
    UE_LOG(LogProjectVisible, Warning, TEXT("InitializeMemoryButtons: Using all %d fragments (filtering disabled for debug)"), 
           FilteredFragments.Num());
    
    // OLD FILTERING CODE (commented out for debugging):
    // for (const FMemoryFragment& Fragment : DisplayFragments)
    // {
    //     if (Fragment.FragmentType == CurrentFilterType)
    //     {
    //         FilteredFragments.Add(Fragment);
    //     }
    // }

    // Layout settings
    const float StartX = 20.0f;
    const float StartY = 120.0f; // Below title and filters
    const float SpacingX = MEMORY_BUTTON_WIDTH + 10.0f;
    const float SpacingY = MEMORY_BUTTON_HEIGHT + 10.0f;
    const int32 ButtonsPerRow = 4;

    // Create buttons for filtered fragments
    for (int32 i = 0; i < FilteredFragments.Num(); i++)
    {
        const FMemoryFragment& Fragment = FilteredFragments[i];
        
        int32 Column = i % ButtonsPerRow;
        int32 Row = i / ButtonsPerRow;
        
        FVector2D ButtonPos = FVector2D(StartX + Column * SpacingX, StartY + Row * SpacingY);
        
        // Get color based on fragment type
        FLinearColor ButtonColor = MemoryManager->GetMemoryFragmentTypeColor(Fragment.FragmentType);
        
        // Dim color if not unlocked
        if (!Fragment.bIsUnlocked)
        {
            ButtonColor = ButtonColor * 0.3f;
            ButtonColor.A = 0.7f;
        }
        else if (!Fragment.bHasBeenViewed)
        {
            // Highlight unviewed memories
            ButtonColor = ButtonColor * 1.2f;
            ButtonColor.A = 1.0f;
        }
        
        FMemoryFragmentButton Button(
            Fragment.FragmentID,
            Fragment.Title,
            Fragment.Excerpt,
            Fragment.FragmentType,
            ButtonPos,
            FVector2D(MEMORY_BUTTON_WIDTH, MEMORY_BUTTON_HEIGHT),
            ButtonColor,
            Fragment.bIsUnlocked,
            Fragment.bHasBeenViewed,
            Fragment.ImportanceScore
        );
        
        MemoryButtons.Add(Button);
    }

    UE_LOG(LogProjectVisible, Warning, TEXT("InitializeMemoryButtons: Successfully created %d memory buttons"), MemoryButtons.Num());
}

void UProjectVisibleMemoryViewerWidget::InitializeFilterButtons()
{
    FilterButtons.Empty();

    // Filter button settings
    const float StartX = 20.0f;
    const float StartY = 70.0f;
    const float SpacingX = FILTER_BUTTON_WIDTH + 10.0f;

    // Define filter types to show
    TArray<EMemoryFragmentType> FilterTypes = {
        EMemoryFragmentType::Episodic,
        EMemoryFragmentType::Semantic,
        EMemoryFragmentType::Procedural,
        EMemoryFragmentType::Emotional,
        EMemoryFragmentType::Sensory,
        EMemoryFragmentType::Conceptual
    };

    // Create filter buttons
    for (int32 i = 0; i < FilterTypes.Num(); i++)
    {
        EMemoryFragmentType FilterType = FilterTypes[i];
        FVector2D ButtonPos = FVector2D(StartX + i * SpacingX, StartY);
        
        // Get display name and color
        FString DisplayName = MemoryManager ? MemoryManager->GetMemoryFragmentTypeDisplayName(FilterType).ToString() : TEXT("Unknown");
        FLinearColor ButtonColor = MemoryManager ? MemoryManager->GetMemoryFragmentTypeColor(FilterType) : FLinearColor::Gray;
        
        // Highlight selected filter
        bool bIsSelected = (FilterType == CurrentFilterType);
        if (bIsSelected)
        {
            ButtonColor = ButtonColor * 1.5f;
            ButtonColor.A = 1.0f;
        }
        else
        {
            ButtonColor = ButtonColor * 0.7f;
            ButtonColor.A = 0.8f;
        }
        
        FFilterButton Button(
            FilterType,
            DisplayName,
            ButtonPos,
            FVector2D(FILTER_BUTTON_WIDTH, FILTER_BUTTON_HEIGHT),
            ButtonColor,
            bIsSelected
        );
        
        FilterButtons.Add(Button);
    }

    UE_LOG(LogProjectVisible, Log, TEXT("Memory Viewer: Initialized %d filter buttons"), FilterButtons.Num());
}

void UProjectVisibleMemoryViewerWidget::HandleMemoryFragmentClick(const FString& FragmentID)
{
    UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer: Fragment clicked: %s"), *FragmentID);
    
    if (MemoryManager && MemoryManager->IsMemoryFragmentUnlocked(FragmentID))
    {
        // Mark as viewed
        MemoryManager->ViewMemoryFragment(FragmentID);
        
        // Update selected fragment
        SelectedFragmentID = FragmentID;
        
        // Get fragment details
        FMemoryFragment Fragment = MemoryManager->GetMemoryFragment(FragmentID);
        
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue,
                FString::Printf(TEXT("Viewing Memory: %s"), *Fragment.Title.ToString()));
        }
        
        // Refresh display
        RefreshMemoryData();
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("This memory is locked!"));
        }
    }
}

void UProjectVisibleMemoryViewerWidget::HandleFilterButtonClick(EMemoryFragmentType FilterType)
{
    UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer: Filter clicked: %d"), (int32)FilterType);
    
    CurrentFilterType = FilterType;
    
    // Refresh buttons and display
    InitializeFilterButtons();
    InitializeMemoryButtons();
    
    if (GEngine)
    {
        FString TypeName = MemoryManager ? MemoryManager->GetMemoryFragmentTypeDisplayName(FilterType).ToString() : TEXT("Unknown");
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
            FString::Printf(TEXT("Filter: %s"), *TypeName));
    }
}

UProjectVisibleMemoryViewerWidget::FMemoryFragmentButton* UProjectVisibleMemoryViewerWidget::GetMemoryButtonAtPosition(const FVector2D& LocalPosition, const FVector2D& PanelPos, const FVector2D& PanelSize)
{
    UE_LOG(LogProjectVisible, Warning, TEXT("GetMemoryButtonAtPosition: Click at (%f,%f), checking %d memory buttons"), 
           LocalPosition.X, LocalPosition.Y, MemoryButtons.Num());
    
    // Check test button first
    FVector2D TestButtonPos = PanelPos + FVector2D(PanelSize.X - 150.0f, PanelSize.Y - 40.0f);
    FVector2D TestButtonSize = FVector2D(130.0f, 30.0f);
    
    if (LocalPosition.X >= TestButtonPos.X && LocalPosition.X <= TestButtonPos.X + TestButtonSize.X &&
        LocalPosition.Y >= TestButtonPos.Y && LocalPosition.Y <= TestButtonPos.Y + TestButtonSize.Y)
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("GetMemoryButtonAtPosition: Test button clicked"));
        // Test button clicked
        const_cast<UProjectVisibleMemoryViewerWidget*>(this)->TestUnlockRandomMemory();
        return nullptr;
    }

    // Check memory fragment buttons
    for (int32 i = 0; i < MemoryButtons.Num(); i++)
    {
        FMemoryFragmentButton& Button = MemoryButtons[i];
        FVector2D ButtonPos = PanelPos + Button.Position;
        
        UE_LOG(LogProjectVisible, Warning, TEXT("GetMemoryButtonAtPosition: Button %d at (%f,%f) size (%f,%f), calculated pos (%f,%f)"), 
               i, Button.Position.X, Button.Position.Y, Button.Size.X, Button.Size.Y, ButtonPos.X, ButtonPos.Y);
        
        if (LocalPosition.X >= ButtonPos.X && LocalPosition.X <= ButtonPos.X + Button.Size.X &&
            LocalPosition.Y >= ButtonPos.Y && LocalPosition.Y <= ButtonPos.Y + Button.Size.Y)
        {
            UE_LOG(LogProjectVisible, Warning, TEXT("GetMemoryButtonAtPosition: Found matching button %d: %s"), i, *Button.FragmentID);
            return &Button;
        }
    }
    
    UE_LOG(LogProjectVisible, Warning, TEXT("GetMemoryButtonAtPosition: No button found at click position"));
    return nullptr;
}

UProjectVisibleMemoryViewerWidget::FFilterButton* UProjectVisibleMemoryViewerWidget::GetFilterButtonAtPosition(const FVector2D& LocalPosition, const FVector2D& PanelPos, const FVector2D& PanelSize)
{
    for (FFilterButton& Button : FilterButtons)
    {
        FVector2D ButtonPos = PanelPos + Button.Position;
        if (LocalPosition.X >= ButtonPos.X && LocalPosition.X <= ButtonPos.X + Button.Size.X &&
            LocalPosition.Y >= ButtonPos.Y && LocalPosition.Y <= ButtonPos.Y + Button.Size.Y)
        {
            return &Button;
        }
    }
    return nullptr;
}

void UProjectVisibleMemoryViewerWidget::CloseMemoryViewer()
{
    UE_LOG(LogProjectVisible, Warning, TEXT("Closing Memory Viewer"));
    
    // Get UI Manager and close this modal
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            if (UProjectVisibleUIManager* UIManagerSystem = GameInstance->GetSubsystem<UProjectVisibleUIManager>())
            {
                UIManagerSystem->ClearAllModals();
                
                // Restore game input
                if (APlayerController* PC = World->GetFirstPlayerController())
                {
                    FInputModeGameOnly InputMode;
                    PC->SetInputMode(InputMode);
                    PC->SetShowMouseCursor(false);
                }
            }
        }
    }
}

void UProjectVisibleMemoryViewerWidget::ReturnToQuickMenu()
{
    UE_LOG(LogProjectVisible, Warning, TEXT("Returning to Quick Menu from Memory Viewer..."));
    
    // Play closing animation
    PlayUIAnimation(EUIAnimationType::FadeOut, 0.2f);
    
    // Get required objects
    UWorld* World = GetWorld();
    UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
    UProjectVisibleUIManager* UIManagerSystem = GameInstance ? GameInstance->GetSubsystem<UProjectVisibleUIManager>() : nullptr;
    APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
    
    if (!UIManagerSystem || !PC)
    {
        UE_LOG(LogProjectVisible, Error, TEXT("Failed to get UIManager or PlayerController"));
        return;
    }
    
    // Delay to allow closing animation to finish
    FTimerHandle TimerHandle;
    World->GetTimerManager().SetTimer(TimerHandle, [=]()
    {
        // Close Memory Viewer
        UIManagerSystem->ClearAllModals();
        
        // Create Quick Menu Widget
        UProjectVisibleQuickMenuWidget* QuickMenuWidget = CreateWidget<UProjectVisibleQuickMenuWidget>(PC, UProjectVisibleQuickMenuWidget::StaticClass());
        if (QuickMenuWidget)
        {
            // Set widget properties
            QuickMenuWidget->SetAnchorsInViewport(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
            QuickMenuWidget->SetAlignmentInViewport(FVector2D(0.0f, 0.0f));
            QuickMenuWidget->AddToViewport(1000);
            QuickMenuWidget->SetVisibility(ESlateVisibility::Visible);
            
            // Add to modal stack
            UIManagerSystem->AddTestModal(QuickMenuWidget);
            
            // Set UI input mode
            PC->SetShowMouseCursor(true);
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(QuickMenuWidget->TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);
            
            // Play opening animation
            QuickMenuWidget->PlayUIAnimation(EUIAnimationType::SlideIn, 0.3f);
            
            UE_LOG(LogProjectVisible, Warning, TEXT("Quick Menu restored from Memory Viewer"));
            
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Returned to Quick Menu!"));
            }
        }
        else
        {
            UE_LOG(LogProjectVisible, Error, TEXT("Failed to create Quick Menu Widget"));
        }
    }, 0.25f, false);
}

void UProjectVisibleMemoryViewerWidget::ShowMemoryDetails(const FString& MemoryID)
{
    UE_LOG(LogProjectVisible, Warning, TEXT("ShowMemoryDetails called with ID: %s"), *MemoryID);
    
    if (!MemoryManager)
    {
        UE_LOG(LogProjectVisible, Error, TEXT("Memory Manager not available for showing details"));
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Memory Manager not available!"));
        }
        return;
    }
    
    FMemoryFragment Fragment = MemoryManager->GetMemoryFragment(MemoryID);
    UE_LOG(LogProjectVisible, Warning, TEXT("Retrieved fragment title: %s"), *Fragment.Title.ToString());
    
    if (!Fragment.Title.IsEmpty())
    {
        bIsShowingDetails = true;
        DetailMemoryID = MemoryID;
        
        UE_LOG(LogProjectVisible, Warning, TEXT("Successfully showing details for memory: %s"), *Fragment.Title.ToString());
        
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, 
                FString::Printf(TEXT("詳細表示: %s"), *Fragment.Title.ToString()));
        }
    }
    else
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Memory fragment not found or title is empty: %s"), *MemoryID);
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("記憶が見つかりません！"));
        }
    }
}

void UProjectVisibleMemoryViewerWidget::HideMemoryDetails()
{
    bIsShowingDetails = false;
    DetailMemoryID.Empty();
    
    UE_LOG(LogProjectVisible, Log, TEXT("Hidden memory details"));
}

void UProjectVisibleMemoryViewerWidget::DrawMemoryViewer(FSlateWindowElementList& OutDrawElements, const FGeometry& AllottedGeometry, int32 LayerId, const FVector2D& PanelPos, const FVector2D& PanelSize) const
{
    // Draw title
    FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 18);
    FSlateDrawElement::MakeText(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(FVector2D(200.0f, 30.0f), FSlateLayoutTransform(PanelPos + FVector2D(300.0f, 20.0f))),
        FText::FromString(TEXT("記憶ビューア (Memory Viewer)")),
        TitleFont,
        ESlateDrawEffect::None,
        FLinearColor::White
    );

    // Draw return button
    FVector2D ReturnButtonPos = PanelPos + FVector2D(20.0f, 20.0f);
    FVector2D ReturnButtonSize = FVector2D(120.0f, 30.0f);
    
    // Return button background
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(ReturnButtonSize, FSlateLayoutTransform(ReturnButtonPos)),
        FCoreStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor(0.6f, 0.3f, 0.9f, 0.8f) // Purple color
    );

    // Return button border
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(ReturnButtonSize, FSlateLayoutTransform(ReturnButtonPos)),
        FCoreStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor(0.8f, 0.4f, 1.0f, 1.0f) // Bright purple border
    );

    // Return button text
    FSlateFontInfo ButtonFont = FCoreStyle::GetDefaultFontStyle("Regular", 12);
    FSlateDrawElement::MakeText(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(ReturnButtonSize, FSlateLayoutTransform(ReturnButtonPos + FVector2D(15.0f, 6.0f))),
        FText::FromString(TEXT("← クイックメニュー")),
        ButtonFont,
        ESlateDrawEffect::None,
        FLinearColor::White
    );

    // Draw filter buttons
    for (const FFilterButton& FilterBtn : FilterButtons)
    {
        // Filter button background
        FLinearColor FilterColor = FilterBtn.bIsSelected ? FilterBtn.Color : FilterBtn.Color * 0.6f;
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId++,
            AllottedGeometry.ToPaintGeometry(FilterBtn.Size, FSlateLayoutTransform(PanelPos + FilterBtn.Position)),
            FCoreStyle::Get().GetBrush("WhiteBrush"),
            ESlateDrawEffect::None,
            FilterColor
        );

        // Filter button text
        FSlateDrawElement::MakeText(
            OutDrawElements,
            LayerId++,
            AllottedGeometry.ToPaintGeometry(FilterBtn.Size, FSlateLayoutTransform(PanelPos + FilterBtn.Position + FVector2D(5.0f, 5.0f))),
            FText::FromString(FilterBtn.Text),
            ButtonFont,
            ESlateDrawEffect::None,
            FLinearColor::White
        );
    }

    // Draw memory fragment buttons
    for (const FMemoryFragmentButton& MemoryBtn : MemoryButtons)
    {
        // Memory button background
        FLinearColor MemoryColor = MemoryBtn.bIsUnlocked ? MemoryBtn.Color : FLinearColor(0.2f, 0.2f, 0.2f, 0.8f);
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId++,
            AllottedGeometry.ToPaintGeometry(MemoryBtn.Size, FSlateLayoutTransform(PanelPos + MemoryBtn.Position)),
            FCoreStyle::Get().GetBrush("WhiteBrush"),
            ESlateDrawEffect::None,
            MemoryColor
        );

        // Memory button border
        FLinearColor BorderColor = (SelectedFragmentID == MemoryBtn.FragmentID) ? FLinearColor::Yellow : FLinearColor(0.6f, 0.6f, 0.6f, 1.0f);
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId++,
            AllottedGeometry.ToPaintGeometry(MemoryBtn.Size + FVector2D(2.0f, 2.0f), FSlateLayoutTransform(PanelPos + MemoryBtn.Position - FVector2D(1.0f, 1.0f))),
            FCoreStyle::Get().GetBrush("WhiteBrush"),
            ESlateDrawEffect::None,
            BorderColor
        );

        // Memory button title
        FSlateFontInfo MemoryTitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 10);
        FSlateDrawElement::MakeText(
            OutDrawElements,
            LayerId++,
            AllottedGeometry.ToPaintGeometry(FVector2D(170.0f, 20.0f), FSlateLayoutTransform(PanelPos + MemoryBtn.Position + FVector2D(5.0f, 5.0f))),
            MemoryBtn.Title,
            MemoryTitleFont,
            ESlateDrawEffect::None,
            FLinearColor::White
        );

        // Memory button excerpt
        FSlateFontInfo ExcerptFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);
        FSlateDrawElement::MakeText(
            OutDrawElements,
            LayerId++,
            AllottedGeometry.ToPaintGeometry(FVector2D(170.0f, 80.0f), FSlateLayoutTransform(PanelPos + MemoryBtn.Position + FVector2D(5.0f, 25.0f))),
            MemoryBtn.Excerpt,
            ExcerptFont,
            ESlateDrawEffect::None,
            FLinearColor(0.9f, 0.9f, 0.9f, 1.0f)
        );

        // Lock icon for locked memories
        if (!MemoryBtn.bIsUnlocked)
        {
            FSlateDrawElement::MakeText(
                OutDrawElements,
                LayerId++,
                AllottedGeometry.ToPaintGeometry(FVector2D(20.0f, 20.0f), FSlateLayoutTransform(PanelPos + MemoryBtn.Position + FVector2D(150.0f, 5.0f))),
                FText::FromString(TEXT("🔒")),
                MemoryTitleFont,
                ESlateDrawEffect::None,
                FLinearColor::Red
            );
        }
    }

    // Draw details panel if showing details
    UE_LOG(LogProjectVisible, Warning, TEXT("DrawMemoryViewer: bIsShowingDetails=%s, DetailMemoryID=%s"), 
           bIsShowingDetails ? TEXT("true") : TEXT("false"), 
           *DetailMemoryID);
    
    if (bIsShowingDetails && !DetailMemoryID.IsEmpty())
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("DrawMemoryViewer: Drawing details panel for %s"), *DetailMemoryID);
        DrawMemoryDetailsPanel(OutDrawElements, AllottedGeometry, LayerId, PanelPos, PanelSize);
    }
}

void UProjectVisibleMemoryViewerWidget::DrawMemoryDetailsPanel(FSlateWindowElementList& OutDrawElements, const FGeometry& AllottedGeometry, int32 LayerId, const FVector2D& PanelPos, const FVector2D& PanelSize) const
{
    UE_LOG(LogProjectVisible, Warning, TEXT("DrawMemoryDetailsPanel called for DetailMemoryID: %s"), *DetailMemoryID);
    
    if (!MemoryManager)
    {
        UE_LOG(LogProjectVisible, Error, TEXT("DrawMemoryDetailsPanel: MemoryManager is null"));
        return;
    }

    // Get the memory fragment
    FMemoryFragment DetailFragment = MemoryManager->GetMemoryFragment(DetailMemoryID);
    UE_LOG(LogProjectVisible, Warning, TEXT("DrawMemoryDetailsPanel: Retrieved fragment with title: %s"), *DetailFragment.Title.ToString());
    
    if (DetailFragment.Title.IsEmpty())
    {
        UE_LOG(LogProjectVisible, Error, TEXT("DrawMemoryDetailsPanel: Fragment title is empty"));
        return;
    }

    // Details panel dimensions and position
    FVector2D DetailsSize = FVector2D(500.0f, 400.0f);
    FVector2D DetailsPos = PanelPos + (PanelSize - DetailsSize) * 0.5f;
    
    UE_LOG(LogProjectVisible, Warning, TEXT("DrawMemoryDetailsPanel: Drawing panel at (%f,%f) with size (%f,%f)"), 
           DetailsPos.X, DetailsPos.Y, DetailsSize.X, DetailsSize.Y);

    // Draw details panel background
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(DetailsSize, FSlateLayoutTransform(DetailsPos)),
        FCoreStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor(0.1f, 0.15f, 0.2f, 0.95f) // Darker background
    );
    
    UE_LOG(LogProjectVisible, Warning, TEXT("DrawMemoryDetailsPanel: Background panel drawn at LayerId %d"), LayerId-1);

    // Draw details panel border
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(DetailsSize + FVector2D(4.0f, 4.0f), FSlateLayoutTransform(DetailsPos - FVector2D(2.0f, 2.0f))),
        FCoreStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor(0.2f, 0.6f, 0.9f, 1.0f) // Blue border
    );

    // Close button (X)
    FVector2D CloseButtonPos = DetailsPos + FVector2D(DetailsSize.X - 30.0f, 10.0f);
    FVector2D CloseButtonSize = FVector2D(20.0f, 20.0f);
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(CloseButtonSize, FSlateLayoutTransform(CloseButtonPos)),
        FCoreStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor(0.8f, 0.2f, 0.2f, 0.8f) // Red close button
    );

    FSlateFontInfo CloseFont = FCoreStyle::GetDefaultFontStyle("Bold", 12);
    FSlateDrawElement::MakeText(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(CloseButtonSize, FSlateLayoutTransform(CloseButtonPos + FVector2D(6.0f, 1.0f))),
        FText::FromString(TEXT("×")),
        CloseFont,
        ESlateDrawEffect::None,
        FLinearColor::White
    );

    // Title
    FSlateFontInfo DetailTitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 16);
    FSlateDrawElement::MakeText(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(FVector2D(460.0f, 30.0f), FSlateLayoutTransform(DetailsPos + FVector2D(20.0f, 15.0f))),
        DetailFragment.Title,
        DetailTitleFont,
        ESlateDrawEffect::None,
        FLinearColor::White
    );

    // Memory type
    FString TypeString = UEnum::GetValueAsString(DetailFragment.FragmentType);
    TypeString = TypeString.Right(TypeString.Len() - TypeString.Find("::") - 2); // Remove enum prefix
    FSlateFontInfo TypeFont = FCoreStyle::GetDefaultFontStyle("Italic", 12);
    FSlateDrawElement::MakeText(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(FVector2D(200.0f, 20.0f), FSlateLayoutTransform(DetailsPos + FVector2D(20.0f, 45.0f))),
        FText::FromString(FString::Printf(TEXT("種類: %s"), *TypeString)),
        TypeFont,
        ESlateDrawEffect::None,
        FLinearColor(0.8f, 0.8f, 0.8f, 1.0f)
    );

    // Importance score
    FSlateDrawElement::MakeText(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(FVector2D(200.0f, 20.0f), FSlateLayoutTransform(DetailsPos + FVector2D(20.0f, 65.0f))),
        FText::FromString(FString::Printf(TEXT("重要度: %.1f/10"), DetailFragment.ImportanceScore)),
        TypeFont,
        ESlateDrawEffect::None,
        FLinearColor(0.8f, 0.8f, 0.8f, 1.0f)
    );

    // Status
    FString StatusString = DetailFragment.bIsUnlocked ? TEXT("解放済み") : TEXT("ロック中");
    if (DetailFragment.bHasBeenViewed)
    {
        StatusString += TEXT(" (閲覧済み)");
    }
    FSlateDrawElement::MakeText(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(FVector2D(200.0f, 20.0f), FSlateLayoutTransform(DetailsPos + FVector2D(250.0f, 45.0f))),
        FText::FromString(FString::Printf(TEXT("状態: %s"), *StatusString)),
        TypeFont,
        ESlateDrawEffect::None,
        FLinearColor(0.8f, 0.8f, 0.8f, 1.0f)
    );

    // Description (using excerpt as content)
    FSlateFontInfo ContentFont = FCoreStyle::GetDefaultFontStyle("Regular", 11);
    FSlateDrawElement::MakeText(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry(FVector2D(460.0f, 250.0f), FSlateLayoutTransform(DetailsPos + FVector2D(20.0f, 95.0f))),
        DetailFragment.Excerpt,
        ContentFont,
        ESlateDrawEffect::None,
        FLinearColor(0.95f, 0.95f, 0.95f, 1.0f)
    );
}
