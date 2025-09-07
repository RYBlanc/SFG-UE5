// Project Visible - Quick Menu Widget Implementation
// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectVisibleQuickMenuWidget.h"
#include "ProjectVisibleUIManager.h"
#include "ProjectVisibleSettingsWidget.h"
#include "ProjectVisibleMemoryViewerWidget.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Gemini_C.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "Rendering/DrawElements.h"
#include "Framework/Application/SlateApplication.h"
#include "Fonts/FontMeasure.h"
#include "Engine/Engine.h"

UProjectVisibleQuickMenuWidget::UProjectVisibleQuickMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ScreenType = EProjectVisibleScreenType::QuickMenu;
	MenuOpacity = 0.9f;
	bIsMinimized = false;
}

void UProjectVisibleQuickMenuWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	// Set widget properties for visibility
	SetVisibility(ESlateVisibility::Visible);
	
	UE_LOG(LogProjectVisible, Warning, TEXT("Quick Menu Widget pre-constructed"));
}

void UProjectVisibleQuickMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Set a background color to make the widget visible
	SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.8f)); // More opaque black background
	
	// Force render opacity
	SetRenderOpacity(0.8f);
	
	// Initialize menu buttons
	InitializeMenuButtons();
	
	// Play opening animation
	PlayUIAnimation(EUIAnimationType::SlideIn, 0.3f);
	
	UE_LOG(LogProjectVisible, Warning, TEXT("Quick Menu Widget constructed and should be visible!"));
	
	// Also print to screen for immediate feedback
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Quick Menu Widget Created!"));
	}
}

int32 UProjectVisibleQuickMenuWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// Draw a semi-transparent background overlay
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(),
		FCoreStyle::Get().GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		FLinearColor(0.0f, 0.0f, 0.0f, 0.6f) // Semi-transparent black background
	);
	
	// Draw Quick Menu panel in the top-right corner
	FVector2D Size = AllottedGeometry.GetLocalSize();
	FVector2D PanelSize(360.0f, 450.0f);
	FVector2D PanelPos(Size.X - PanelSize.X - 20.0f, 20.0f); // Top-right with 20px margin
	
	// Main panel background
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(PanelSize, FSlateLayoutTransform(PanelPos)),
		FCoreStyle::Get().GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		FLinearColor(0.1f, 0.1f, 0.15f, 0.95f) // Dark blue-gray panel
	);
	
	// Panel border
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 2,
		AllottedGeometry.ToPaintGeometry(PanelSize, FSlateLayoutTransform(PanelPos)),
		FCoreStyle::Get().GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		FLinearColor(0.3f, 0.6f, 0.9f, 0.8f) // Blue border
	);
	
	// Title bar
	FVector2D TitleSize(PanelSize.X, 40.0f);
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 3,
		AllottedGeometry.ToPaintGeometry(TitleSize, FSlateLayoutTransform(PanelPos)),
		FCoreStyle::Get().GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		FLinearColor(0.2f, 0.4f, 0.7f, 0.9f) // Title bar background
	);
	
	// Draw menu buttons
	DrawMenuButtons(OutDrawElements, AllottedGeometry, LayerId + 4, PanelPos, PanelSize);
	
	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId + 10, InWidgetStyle, bParentEnabled);
}

void UProjectVisibleQuickMenuWidget::InitializeWidget()
{
	Super::InitializeWidget();
	
	// Initialize menu buttons
	InitializeMenuButtons();
	
	// Initialize with current data
	RefreshQuickMenuData();
	
	UE_LOG(LogProjectVisible, Log, TEXT("Quick Menu Widget initialized"));
}


void UProjectVisibleQuickMenuWidget::OpenFullSettings()
{
	UE_LOG(LogProjectVisible, Warning, TEXT("Opening Settings screen..."));
	
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
	
	// Play closing animation first
	PlayUIAnimation(EUIAnimationType::SlideOut, 0.2f);
	
	// Delay to allow closing animation to finish
	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, [=]()
	{
		// Close current Quick Menu
		UIManagerSystem->ClearAllModals();
		
		// Create Settings Widget
		UProjectVisibleSettingsWidget* SettingsWidget = CreateWidget<UProjectVisibleSettingsWidget>(PC, UProjectVisibleSettingsWidget::StaticClass());
	if (SettingsWidget)
	{
		// Set widget to fill the entire screen
		SettingsWidget->SetAnchorsInViewport(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		SettingsWidget->SetAlignmentInViewport(FVector2D(0.0f, 0.0f));
		
		SettingsWidget->AddToViewport(1000);
		SettingsWidget->SetVisibility(ESlateVisibility::Visible);
		
		// Store reference in modal stack
		UIManagerSystem->AddTestModal(SettingsWidget);
		
		// Show mouse cursor and set UI input mode
		PC->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(SettingsWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		
		// Play opening animation
		SettingsWidget->PlayUIAnimation(EUIAnimationType::FadeIn, 0.3f);
		
		UE_LOG(LogProjectVisible, Warning, TEXT("Settings widget created and shown with animation"));
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Settings Menu Opened with animation!"));
		}
	}
	else
	{
		UE_LOG(LogProjectVisible, Error, TEXT("Failed to create Settings widget"));
	}
	}, 0.25f, false); // Timer callback end
}

void UProjectVisibleQuickMenuWidget::OpenVirtueProfile()
{
    UE_LOG(LogProjectVisible, Warning, TEXT("Opening Virtue Profile..."));
    
    // TODO: Implement Virtue Profile screen (currently under development)
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Purple, TEXT("Virtue Profile - Coming Soon!"));
    }
}

void UProjectVisibleQuickMenuWidget::OpenMemoryViewer()
{
    UE_LOG(LogProjectVisible, Warning, TEXT("Opening Memory Viewer screen..."));
    
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
    
    // Play closing animation first
    PlayUIAnimation(EUIAnimationType::SlideOut, 0.2f);
    
    // Delay to allow closing animation to finish
    FTimerHandle TimerHandle;
    World->GetTimerManager().SetTimer(TimerHandle, [=]()
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("OpenMemoryViewer: Timer callback started"));
        
        // TEMPORARILY DISABLED - Close current Quick Menu
        // UIManagerSystem->ClearAllModals();
        
        // Create Memory Viewer Widget  
        UProjectVisibleMemoryViewerWidget* MemoryWidget = CreateWidget<UProjectVisibleMemoryViewerWidget>(PC, UProjectVisibleMemoryViewerWidget::StaticClass());
        UE_LOG(LogProjectVisible, Warning, TEXT("OpenMemoryViewer: MemoryWidget created: %s"), MemoryWidget ? TEXT("Success") : TEXT("Failed"));
        if (MemoryWidget)
        {
            UE_LOG(LogProjectVisible, Warning, TEXT("OpenMemoryViewer: Adding MemoryWidget to UIManager"));
            
            // Try both methods for debugging
            UIManagerSystem->AddTestModal(MemoryWidget);
            
            // BACKUP: Also add to viewport directly to ensure visibility
            MemoryWidget->SetAnchorsInViewport(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
            MemoryWidget->SetAlignmentInViewport(FVector2D(0.0f, 0.0f));
            MemoryWidget->AddToViewport(1000);
            MemoryWidget->SetVisibility(ESlateVisibility::Visible);
            
            UE_LOG(LogProjectVisible, Warning, TEXT("OpenMemoryViewer: AddTestModal and AddToViewport completed"));
            
            // Show mouse cursor and set UI input mode
            PC->SetShowMouseCursor(true);
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(MemoryWidget->TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);
            
            // Play opening animation with special quantum effect for memory viewer
            MemoryWidget->PlayUIAnimation(EUIAnimationType::Quantum, 0.4f);
            
            UE_LOG(LogProjectVisible, Warning, TEXT("Memory Viewer widget created and shown with animation"));
            
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TEXT("Memory Viewer Opened with quantum effect!"));
            }
        }
        else
        {
            UE_LOG(LogProjectVisible, Error, TEXT("Failed to create Memory Viewer widget"));
        }
    }, 0.25f, false); // Timer callback end
}

void UProjectVisibleQuickMenuWidget::SaveGame()
{
	UE_LOG(LogProjectVisible, Warning, TEXT("Saving game..."));
	
	// TODO: Implement actual save functionality
	// For now, just show a message
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Game Saved! (Placeholder)"));
	}
}

void UProjectVisibleQuickMenuWidget::LoadGame()
{
	UE_LOG(LogProjectVisible, Warning, TEXT("Loading game..."));
	
	// TODO: Implement actual load functionality
	// For now, just show a message
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor(0.0f, 1.0f, 1.0f, 1.0f), TEXT("Game Loaded! (Placeholder)"));
	}
}

void UProjectVisibleQuickMenuWidget::ReturnToMainMenu()
{
	UE_LOG(LogProjectVisible, Warning, TEXT("Returning to Main Menu..."));
	
	// Get UI Manager
	UWorld* World = GetWorld();
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	UProjectVisibleUIManager* UIManagerSystem = GameInstance ? GameInstance->GetSubsystem<UProjectVisibleUIManager>() : nullptr;
	
	if (UIManagerSystem)
	{
		// Close all modals first
		UIManagerSystem->ClearAllModals();
		
		// Switch to Main Menu screen
		UIManagerSystem->ReplaceScreen(EProjectVisibleScreenType::MainMenu);
		
		UE_LOG(LogProjectVisible, Warning, TEXT("Switched to Main Menu"));
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Returned to Main Menu!"));
		}
	}
	else
	{
		UE_LOG(LogProjectVisible, Error, TEXT("Failed to get UIManager for Main Menu transition"));
	}
}

void UProjectVisibleQuickMenuWidget::CloseQuickMenu()
{
	UE_LOG(LogProjectVisible, Warning, TEXT("Closing Quick Menu..."));
	
	// Get required objects
	UWorld* World = GetWorld();
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	UProjectVisibleUIManager* UIManagerSystem = GameInstance ? GameInstance->GetSubsystem<UProjectVisibleUIManager>() : nullptr;
	APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
	
	// Play closing animation first
	PlayUIAnimation(EUIAnimationType::FadeOut, 0.2f);
	
	if (UIManagerSystem && PC)
	{
		// Delay to allow closing animation to finish
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, [=]()
		{
			// Remove all modal widgets
			UIManagerSystem->ClearAllModals();
			
			// Restore game input
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->SetShowMouseCursor(false);
			
			UE_LOG(LogProjectVisible, Warning, TEXT("Quick Menu closed with animation"));
		}, 0.25f, false);
	}
	else
	{
		UE_LOG(LogProjectVisible, Error, TEXT("Failed to get UIManager or PlayerController for menu close"));
	}
}

void UProjectVisibleQuickMenuWidget::ToggleInventory()
{
	UE_LOG(LogProjectVisible, Warning, TEXT("Toggle Inventory - Not implemented yet"));
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Inventory - Coming Soon!"));
	}
}

void UProjectVisibleQuickMenuWidget::ToggleMap()
{
	UE_LOG(LogProjectVisible, Warning, TEXT("Toggle Map - Not implemented yet"));
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Map - Coming Soon!"));
	}
}

void UProjectVisibleQuickMenuWidget::ToggleJournal()
{
	UE_LOG(LogProjectVisible, Warning, TEXT("Toggle Journal - Not implemented yet"));
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Journal - Coming Soon!"));
	}
}

void UProjectVisibleQuickMenuWidget::RefreshQuickMenuData()
{
	// Update current objective
	CurrentObjective = TEXT("Investigate the mysterious events in Tokyo");
	
	// Update game progress (placeholder)
	GameProgressPercentage = 25.0f;
	
	// Call Blueprint events to update UI
	UpdateQuickStats();
	UpdateCurrentObjective();
	UpdateGameProgress();
	
	UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Quick Menu data refreshed"));
}

void UProjectVisibleQuickMenuWidget::UpdateMenuVisuals()
{
	// Update visual elements based on state
	// This would be implemented in Blueprint
	UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Quick Menu visuals updated"));
}

void UProjectVisibleQuickMenuWidget::InitializeMenuButtons()
{
	MenuButtons.Empty();
	
	// Button settings (increased size for Japanese text)
	const float ButtonWidth = 320.0f;
	const float ButtonHeight = 40.0f;
	const float ButtonSpacing = 50.0f;
	const float StartY = 60.0f; // Below title bar
	const float StartX = 15.0f; // Left margin
	
	// Define menu buttons
	MenuButtons.Add(FMenuButton(TEXT("設定 (Settings)"), FVector2D(StartX, StartY), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.3f, 0.5f, 0.8f, 0.9f), 1));
	MenuButtons.Add(FMenuButton(TEXT("徳性プロフィール (Virtue Profile)"), FVector2D(StartX, StartY + ButtonSpacing), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.6f, 0.3f, 0.8f, 0.9f), 2));
	MenuButtons.Add(FMenuButton(TEXT("記憶ビューア (Memory Viewer)"), FVector2D(StartX, StartY + ButtonSpacing * 2), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.3f, 0.9f, 0.6f, 0.9f), 3));
	MenuButtons.Add(FMenuButton(TEXT("ゲーム保存 (Save Game)"), FVector2D(StartX, StartY + ButtonSpacing * 3), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.9f, 0.6f, 0.3f, 0.9f), 4));
	MenuButtons.Add(FMenuButton(TEXT("ゲーム読込 (Load Game)"), FVector2D(StartX, StartY + ButtonSpacing * 4), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.9f, 0.9f, 0.3f, 0.9f), 5));
	MenuButtons.Add(FMenuButton(TEXT("メインメニュー (Main Menu)"), FVector2D(StartX, StartY + ButtonSpacing * 5), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.8f, 0.3f, 0.3f, 0.9f), 6));
	MenuButtons.Add(FMenuButton(TEXT("メニュー閉じる (Close Tab)"), FVector2D(StartX, StartY + ButtonSpacing * 6), FVector2D(ButtonWidth, ButtonHeight), FLinearColor(0.5f, 0.5f, 0.5f, 0.9f), 7));
	
	UE_LOG(LogProjectVisible, Log, TEXT("Menu buttons initialized: %d buttons"), MenuButtons.Num());
}

void UProjectVisibleQuickMenuWidget::DrawMenuButtons(FSlateWindowElementList& OutDrawElements, const FGeometry& AllottedGeometry, int32 LayerId, const FVector2D& PanelPos, const FVector2D& PanelSize) const
{
	// Draw title text
	FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Regular", 16);
	FSlateDrawElement::MakeText(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2D(PanelSize.X, 40.0f), FSlateLayoutTransform(PanelPos + FVector2D(10.0f, 5.0f))),
		FText::FromString(TEXT("Project Visible - Quick Menu")),
		TitleFont,
		ESlateDrawEffect::None,
		FLinearColor::White
	);
	
	// Draw each menu button
	for (const FMenuButton& Button : MenuButtons)
	{
		FVector2D ButtonPos = PanelPos + Button.Position;
		
		// Button background
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(Button.Size, FSlateLayoutTransform(ButtonPos)),
			FCoreStyle::Get().GetBrush("WhiteBrush"),
			ESlateDrawEffect::None,
			Button.Color
		);
		
		// Button border (thinner, darker)
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId + 2,
			AllottedGeometry.ToPaintGeometry(Button.Size + FVector2D(2.0f, 2.0f), FSlateLayoutTransform(ButtonPos - FVector2D(1.0f, 1.0f))),
			FCoreStyle::Get().GetBrush("WhiteBrush"),
			ESlateDrawEffect::None,
			FLinearColor(0.2f, 0.2f, 0.2f, 0.8f)
		);
		
		// Button text (black for visibility, centered vertically)
		FVector2D TextPos = ButtonPos + FVector2D(12.0f, 10.0f);
		FSlateFontInfo ButtonFont = FCoreStyle::GetDefaultFontStyle("Regular", 13);
		FSlateDrawElement::MakeText(
			OutDrawElements,
			LayerId + 3,
			AllottedGeometry.ToPaintGeometry(Button.Size, FSlateLayoutTransform(TextPos)),
			FText::FromString(Button.Text),
			ButtonFont,
			ESlateDrawEffect::None,
			FLinearColor::Black
		);
	}
}

FReply UProjectVisibleQuickMenuWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		FVector2D LocalPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		
		// Check if click is within the panel area
		FVector2D Size = InGeometry.GetLocalSize();
		FVector2D PanelSize(360.0f, 450.0f);
		FVector2D PanelPos(Size.X - PanelSize.X - 20.0f, 20.0f);
		
		FMenuButton* ClickedButton = GetButtonAtPosition(LocalPosition, PanelPos, PanelSize);
		if (ClickedButton)
		{
			HandleButtonClick(ClickedButton->ButtonID);
			return FReply::Handled();
		}
	}
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

UProjectVisibleQuickMenuWidget::FMenuButton* UProjectVisibleQuickMenuWidget::GetButtonAtPosition(const FVector2D& LocalPosition, const FVector2D& PanelPos, const FVector2D& PanelSize)
{
	for (FMenuButton& Button : MenuButtons)
	{
		FVector2D ButtonPos = PanelPos + Button.Position;
		FVector2D ButtonEnd = ButtonPos + Button.Size;
		
		if (LocalPosition.X >= ButtonPos.X && LocalPosition.X <= ButtonEnd.X &&
			LocalPosition.Y >= ButtonPos.Y && LocalPosition.Y <= ButtonEnd.Y)
		{
			return &Button;
		}
	}
	return nullptr;
}

void UProjectVisibleQuickMenuWidget::HandleButtonClick(int32 ButtonID)
{
	UE_LOG(LogProjectVisible, Warning, TEXT("Button clicked: ID %d"), ButtonID);
	
	switch (ButtonID)
	{
		case 1: // Settings
			OpenFullSettings();
			break;
		case 2: // Virtue Profile
			OpenVirtueProfile();
			break;
		case 3: // Memory Viewer
			OpenMemoryViewer();
			break;
		case 4: // Save Game
			SaveGame();
			break;
		case 5: // Load Game
			LoadGame();
			break;
		case 6: // Main Menu
			ReturnToMainMenu();
			break;
		case 7: // Close
			CloseQuickMenu();
			break;
		default:
			UE_LOG(LogProjectVisible, Warning, TEXT("Unknown button ID: %d"), ButtonID);
			break;
	}
	
	// Show feedback message
	if (GEngine)
	{
		FString ButtonName = TEXT("Unknown");
		if (ButtonID >= 1 && ButtonID <= MenuButtons.Num())
		{
			ButtonName = MenuButtons[ButtonID - 1].Text;
		}
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("Clicked: %s"), *ButtonName));
	}
}


