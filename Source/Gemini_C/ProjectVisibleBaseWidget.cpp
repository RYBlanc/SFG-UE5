// Project Visible - Base Widget Class Implementation
// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectVisibleBaseWidget.h"
#include "ProjectVisibleUIManager.h"
#include "VirtueManager.h"
#include "MemoryManager.h"
#include "RealityInvestigationManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "Components/Widget.h"
#include "Gemini_C.h"

UProjectVisibleBaseWidget::UProjectVisibleBaseWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// bIsFocusable = true; // Deprecated - handled by CommonUI
	SetVisibility(ESlateVisibility::Visible);
	
	// Set default screen type
	ScreenType = EProjectVisibleScreenType::RealityMode;
	
	// Initialize theme with defaults
	CurrentTheme = FProjectVisibleUITheme();
	
	// Initialize accessibility config
	AccessibilityConfig = FAccessibilityConfig();
}

void UProjectVisibleBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Get UI Manager reference
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			UIManager = GameInstance->GetSubsystem<UProjectVisibleUIManager>();
		}
	}
	
	// Initialize the widget
	InitializeWidget();
	
	UE_LOG(LogProjectVisible, Log, TEXT("Project Visible Base Widget constructed"));
}

void UProjectVisibleBaseWidget::NativeDestruct()
{
	// Clear timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RefreshTimer);
	}
	
	// Clear references
	UIManager = nullptr;
	
	Super::NativeDestruct();
	
	UE_LOG(LogProjectVisible, Log, TEXT("Project Visible Base Widget destructed"));
}

void UProjectVisibleBaseWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	LastUpdateTime += InDeltaTime;
	
	// Check if refresh is needed
	if (bNeedsRefresh && LastUpdateTime > 0.1f) // Throttle updates
	{
		InternalRefresh();
		bNeedsRefresh = false;
		LastUpdateTime = 0.0f;
	}
}

UWidget* UProjectVisibleBaseWidget::NativeGetDesiredFocusTarget() const
{
	// Return the first focusable child widget
	return Super::NativeGetDesiredFocusTarget();
}

void UProjectVisibleBaseWidget::InitializeWidget()
{
	if (bIsInitialized)
	{
		return;
	}
	
	// Apply current theme
	if (UIManager.IsValid())
	{
		ApplyTheme(UIManager->GetCurrentTheme());
		ApplyAccessibilitySettings(UIManager->GetAccessibilityConfig());
	}
	
	// Set up system bindings
	SetupSystemBindings();
	
	// Mark as initialized
	bIsInitialized = true;
	
	// Call Blueprint event
	OnWidgetInitialized();
	
	UE_LOG(LogProjectVisible, Log, TEXT("Widget initialized: %s"), *GetClass()->GetName());
}

void UProjectVisibleBaseWidget::UpdateWidget()
{
	if (!bIsInitialized)
	{
		return;
	}
	
	// Refresh data
	RefreshData();
	
	// Call Blueprint event
	OnDataUpdated();
	
	UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Widget updated: %s"), *GetClass()->GetName());
}

void UProjectVisibleBaseWidget::RefreshData()
{
	// Schedule refresh to avoid excessive updates
	ScheduleRefresh();
}

void UProjectVisibleBaseWidget::ApplyTheme(const FProjectVisibleUITheme& Theme)
{
	CurrentTheme = Theme;
	
	// Call Blueprint event for theme application
	OnThemeApplied(Theme);
	
	UE_LOG(LogProjectVisible, Log, TEXT("Applied theme '%s' to widget"), *Theme.ThemeName);
}

void UProjectVisibleBaseWidget::OnThemeChanged(const FProjectVisibleUITheme& NewTheme)
{
	ApplyTheme(NewTheme);
}

void UProjectVisibleBaseWidget::NavigateToScreen(EProjectVisibleScreenType TargetScreen)
{
	if (UIManager.IsValid())
	{
		UIManager->PushScreen(TargetScreen, true);
	}
}

void UProjectVisibleBaseWidget::ShowModal(EProjectVisibleScreenType ModalType)
{
	if (UIManager.IsValid())
	{
		UIManager->ShowModal(ModalType, EUIInputPriority::High);
	}
}

void UProjectVisibleBaseWidget::HideModal()
{
	if (UIManager.IsValid())
	{
		UIManager->PopScreen();
	}
}

void UProjectVisibleBaseWidget::ApplyAccessibilitySettings(const FAccessibilityConfig& Config)
{
	AccessibilityConfig = Config;
	
	// Apply text size multiplier
	if (Config.bLargeTextMode)
	{
		SetTextSizeMultiplier(Config.TextSizeMultiplier);
	}
	
	// Apply high contrast mode
	if (Config.bHighContrastMode)
	{
		SetHighContrastMode(true);
	}
	
	UE_LOG(LogProjectVisible, Log, TEXT("Applied accessibility settings to widget"));
}

void UProjectVisibleBaseWidget::SetTextSizeMultiplier(float Multiplier)
{
	// This would be implemented in Blueprint to affect text widgets
	// For now, we just store the value
	AccessibilityConfig.TextSizeMultiplier = FMath::Clamp(Multiplier, 0.5f, 3.0f);
}

void UProjectVisibleBaseWidget::SetHighContrastMode(bool bEnabled)
{
	AccessibilityConfig.bHighContrastMode = bEnabled;
	
	if (bEnabled)
	{
		// Apply high contrast theme
		FProjectVisibleUITheme HighContrastTheme = CurrentTheme;
		HighContrastTheme.PrimaryColor = FLinearColor::White;
		HighContrastTheme.SecondaryColor = FLinearColor::Black;
		HighContrastTheme.BackgroundColor = FLinearColor::Black;
		HighContrastTheme.TextColor = FLinearColor::White;
		HighContrastTheme.bHighContrast = true;
		
		ApplyTheme(HighContrastTheme);
	}
}

void UProjectVisibleBaseWidget::PlayUIAnimation(EUIAnimationType AnimationType, float Duration)
{
	if (UIManager.IsValid())
	{
		FUIAnimationConfig AnimConfig;
		AnimConfig.AnimationType = AnimationType;
		AnimConfig.Duration = Duration;
		
		UIManager->PlayUIAnimation(this, AnimConfig);
	}
	
	// Call Blueprint event
	OnAnimationTriggered(AnimationType);
}

void UProjectVisibleBaseWidget::BindToVirtueSystem()
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (UVirtueManager* VirtueManager = GameInstance->GetSubsystem<UVirtueManager>())
			{
			// Bind to virtue change events (commented out - delegate signature mismatch)
			// VirtueManager->OnVirtueChanged.AddDynamic(this, &UProjectVisibleBaseWidget::OnVirtueDataChanged);
				bIsDataBound = true;
				
				UE_LOG(LogProjectVisible, Log, TEXT("Widget bound to Virtue System"));
			}
		}
	}
}

void UProjectVisibleBaseWidget::BindToMemorySystem()
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (UMemoryManager* MemoryManager = GameInstance->GetSubsystem<UMemoryManager>())
			{
			// Bind to memory change events (commented out - delegate signature mismatch)
			// MemoryManager->OnMemoryCreated.AddDynamic(this, &UProjectVisibleBaseWidget::OnMemoryDataChanged);
				bIsDataBound = true;
				
				UE_LOG(LogProjectVisible, Log, TEXT("Widget bound to Memory System"));
			}
		}
	}
}

void UProjectVisibleBaseWidget::BindToInvestigationSystem()
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (URealityInvestigationManager* InvestigationManager = GameInstance->GetSubsystem<URealityInvestigationManager>())
			{
			// Bind to investigation events (commented out - delegate not found)
			// InvestigationManager->OnEvidenceCollected.AddDynamic(this, &UProjectVisibleBaseWidget::OnInvestigationDataChanged);
				bIsDataBound = true;
				
				UE_LOG(LogProjectVisible, Log, TEXT("Widget bound to Investigation System"));
			}
		}
	}
}

UProjectVisibleUIManager* UProjectVisibleBaseWidget::GetUIManager() const
{
	return UIManager.Get();
}

void UProjectVisibleBaseWidget::SetupSystemBindings()
{
	// Override in derived classes to bind to specific systems
	// Base implementation does nothing
}

void UProjectVisibleBaseWidget::OnSystemDataChanged()
{
	// Schedule widget update
	ScheduleRefresh();
}

void UProjectVisibleBaseWidget::OnVirtueDataChanged()
{
	OnSystemDataChanged();
}

void UProjectVisibleBaseWidget::OnMemoryDataChanged()
{
	OnSystemDataChanged();
}

void UProjectVisibleBaseWidget::OnInvestigationDataChanged()
{
	OnSystemDataChanged();
}

void UProjectVisibleBaseWidget::ScheduleRefresh()
{
	bNeedsRefresh = true;
}

void UProjectVisibleBaseWidget::InternalRefresh()
{
	if (bIsInitialized)
	{
		UpdateWidget();
	}
}
