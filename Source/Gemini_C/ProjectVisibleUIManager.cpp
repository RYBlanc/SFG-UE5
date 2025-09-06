// Project Visible - CommonUI Manager Implementation
// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectVisibleUIManager.h"
#include "Gemini_C.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UProjectVisibleUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Initialize settings
    bUseCommonUI = true;
    bDebugUI = false;
    GlobalAnimationSpeedMultiplier = 1.0f;
    MaxScreenHistory = 10;
    bAutoSaveThemeSettings = true;
    
    // Initialize state
    CurrentScreen = EProjectVisibleScreenType::MainMenu;
    
    // Initialize configurations
    InitializeDefaultThemes();
    InitializeDefaultScreenClasses();
    
    UE_LOG(LogProjectVisible, Log, TEXT("Project Visible UI Manager initialized with CommonUI"));
}

void UProjectVisibleUIManager::Deinitialize()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Project Visible UI Manager shutting down"));
    
    // Clear all widgets
    ClearScreenStack();
    ActiveWidgets.Empty();
    ActiveAnimations.Empty();
    
    Super::Deinitialize();
}

// Screen Management
void UProjectVisibleUIManager::InitializeUISystem()
{
    if (!bUseCommonUI)
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("CommonUI is disabled"));
        return;
    }
    
    // Set initial theme
    ApplyTheme(CurrentTheme);
    
    // Apply accessibility settings
    ApplyAccessibilitySettings();
    
    // Update layout for current screen size
    if (UWorld* World = GetWorld())
    {
        if (UGameViewportClient* ViewportClient = World->GetGameViewport())
        {
            FVector2D ViewportSize;
            ViewportClient->GetViewportSize(ViewportSize);
            UpdateLayoutForScreenSize(ViewportSize);
        }
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("UI System initialized successfully"));
}

void UProjectVisibleUIManager::PushScreen(EProjectVisibleScreenType ScreenType, bool bAddToHistory)
{
    EProjectVisibleScreenType OldScreen = CurrentScreen;
    
    if (bAddToHistory && CurrentScreen != EProjectVisibleScreenType::MainMenu)
    {
        ScreenHistory.Add(CurrentScreen);
        
        // Limit history size
        if (ScreenHistory.Num() > MaxScreenHistory)
        {
            ScreenHistory.RemoveAt(0);
        }
    }
    
    CurrentScreen = ScreenType;
    
    // Hide current widget
    if (CurrentMainWidget.IsValid())
    {
        CurrentMainWidget->SetVisibility(ESlateVisibility::Hidden);
    }
    
    // Create or retrieve widget
    UUserWidget* ScreenWidget = GetOrCreateScreenWidget(ScreenType);
    if (ScreenWidget)
    {
        CurrentMainWidget = ScreenWidget;
        ScreenWidget->AddToViewport();
        ScreenWidget->SetVisibility(ESlateVisibility::Visible);
    }
    
    // Handle screen transition
    HandleScreenTransition(OldScreen, ScreenType);
    
    // Configure input for new screen
    ConfigureInputForScreen(ScreenType);
    
    OnScreenChanged.Broadcast(OldScreen, ScreenType);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Pushed screen: %s"), *UEnum::GetValueAsString(ScreenType));
}

void UProjectVisibleUIManager::PopScreen()
{
    if (ScreenHistory.Num() > 0)
    {
        EProjectVisibleScreenType PreviousScreen = ScreenHistory.Pop();
        ReplaceScreen(PreviousScreen);
    }
    else
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Cannot pop screen: No screen history"));
    }
}

void UProjectVisibleUIManager::ReplaceScreen(EProjectVisibleScreenType ScreenType)
{
    EProjectVisibleScreenType OldScreen = CurrentScreen;
    CurrentScreen = ScreenType;
    
    // Remove current widget
    if (CurrentMainWidget.IsValid())
    {
        CurrentMainWidget->RemoveFromParent();
        CurrentMainWidget = nullptr;
    }
    
    // Add new screen
    UUserWidget* ScreenWidget = GetOrCreateScreenWidget(ScreenType);
    if (ScreenWidget)
    {
        CurrentMainWidget = ScreenWidget;
        ScreenWidget->AddToViewport();
        ScreenWidget->SetVisibility(ESlateVisibility::Visible);
    }
    
    HandleScreenTransition(OldScreen, ScreenType);
    ConfigureInputForScreen(ScreenType);
    
    OnScreenChanged.Broadcast(OldScreen, ScreenType);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Replaced screen: %s -> %s"), 
           *UEnum::GetValueAsString(OldScreen), *UEnum::GetValueAsString(ScreenType));
}

void UProjectVisibleUIManager::ClearScreenStack()
{
    ScreenHistory.Empty();
    ActiveModals.Empty();
    
    // Remove main widget
    if (CurrentMainWidget.IsValid())
    {
        CurrentMainWidget->RemoveFromParent();
        CurrentMainWidget = nullptr;
    }
    
    // Clear modal stack
    for (TWeakObjectPtr<UUserWidget> ModalWidget : ModalWidgetStack)
    {
        if (ModalWidget.IsValid())
        {
            ModalWidget->RemoveFromParent();
        }
    }
    ModalWidgetStack.Empty();
    
    CleanupInactiveWidgets();
    
    UE_LOG(LogProjectVisible, Log, TEXT("Cleared screen stack"));
}

void UProjectVisibleUIManager::ShowModal(EProjectVisibleScreenType ScreenType, EUIInputPriority Priority)
{
    ActiveModals.AddUnique(ScreenType);
    
    UUserWidget* ModalWidget = GetOrCreateScreenWidget(ScreenType);
    if (ModalWidget)
    {
        ModalWidgetStack.Add(ModalWidget);
        ModalWidget->AddToViewport(1000); // High Z-order for modals
        ModalWidget->SetVisibility(ESlateVisibility::Visible);
    }
    
    // Set input priority
    ConfigureInputForScreen(ScreenType);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Showed modal: %s"), *UEnum::GetValueAsString(ScreenType));
}

void UProjectVisibleUIManager::HideModal(EProjectVisibleScreenType ScreenType)
{
    ActiveModals.Remove(ScreenType);
    
    if (TWeakObjectPtr<UUserWidget>* WidgetPtr = ActiveWidgets.Find(ScreenType))
    {
        if (UUserWidget* Widget = WidgetPtr->Get())
        {
            Widget->RemoveFromParent();
            ModalWidgetStack.Remove(*WidgetPtr);
        }
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Hid modal: %s"), *UEnum::GetValueAsString(ScreenType));
}

bool UProjectVisibleUIManager::IsScreenActive(EProjectVisibleScreenType ScreenType) const
{
    return CurrentScreen == ScreenType || ActiveModals.Contains(ScreenType);
}

// Theme Management
void UProjectVisibleUIManager::ApplyTheme(const FProjectVisibleUITheme& Theme)
{
    CurrentTheme = Theme;
    
    // Apply theme to all active widgets
    for (auto& WidgetPair : ActiveWidgets)
    {
        if (UUserWidget* Widget = WidgetPair.Value.Get())
        {
            ApplyThemeToWidget(Widget, Theme);
        }
    }
    
    PropagateThemeChange();
    OnThemeChanged.Broadcast(Theme);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Applied theme: %s"), *Theme.ThemeName);
}

void UProjectVisibleUIManager::SetThemeByName(const FString& ThemeName)
{
    if (FProjectVisibleUITheme* Theme = RegisteredThemes.Find(ThemeName))
    {
        ApplyTheme(*Theme);
    }
    else
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Theme not found: %s"), *ThemeName);
    }
}

void UProjectVisibleUIManager::RegisterTheme(const FString& ThemeName, const FProjectVisibleUITheme& Theme)
{
    RegisteredThemes.Add(ThemeName, Theme);
    UE_LOG(LogProjectVisible, Log, TEXT("Registered theme: %s"), *ThemeName);
}

TArray<FString> UProjectVisibleUIManager::GetAvailableThemes() const
{
    TArray<FString> ThemeNames;
    RegisteredThemes.GetKeys(ThemeNames);
    return ThemeNames;
}

void UProjectVisibleUIManager::SetDynamicColor(const FString& ColorName, const FLinearColor& Color)
{
    CurrentTheme.CustomColors.Add(ColorName, Color);
    
    // Update active widgets
    for (auto& WidgetPair : ActiveWidgets)
    {
        if (UUserWidget* Widget = WidgetPair.Value.Get())
        {
            ApplyThemeToWidget(Widget, CurrentTheme);
        }
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Set dynamic color: %s"), *ColorName);
}

FLinearColor UProjectVisibleUIManager::GetThemeColor(const FString& ColorName) const
{
    if (const FLinearColor* Color = CurrentTheme.CustomColors.Find(ColorName))
    {
        return *Color;
    }
    
    // Default colors
    if (ColorName == TEXT("Primary")) return CurrentTheme.PrimaryColor;
    if (ColorName == TEXT("Secondary")) return CurrentTheme.SecondaryColor;
    if (ColorName == TEXT("Accent")) return CurrentTheme.AccentColor;
    if (ColorName == TEXT("Background")) return CurrentTheme.BackgroundColor;
    if (ColorName == TEXT("Text")) return CurrentTheme.TextColor;
    if (ColorName == TEXT("Disabled")) return CurrentTheme.DisabledColor;
    if (ColorName == TEXT("Warning")) return CurrentTheme.WarningColor;
    if (ColorName == TEXT("Error")) return CurrentTheme.ErrorColor;
    if (ColorName == TEXT("Success")) return CurrentTheme.SuccessColor;
    
    return FLinearColor::White;
}

// Accessibility
void UProjectVisibleUIManager::SetAccessibilityConfig(const FAccessibilityConfig& Config)
{
    AccessibilityConfig = Config;
    ApplyAccessibilitySettings();
    OnAccessibilityChanged.Broadcast(Config);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Updated accessibility configuration"));
}

void UProjectVisibleUIManager::ToggleAccessibilityFeature(EAccessibilityFeature Feature, bool bEnabled)
{
    if (bEnabled)
    {
        AccessibilityConfig.EnabledFeatures.AddUnique(Feature);
    }
    else
    {
        AccessibilityConfig.EnabledFeatures.Remove(Feature);
    }
    
    ApplyAccessibilitySettings();
    OnAccessibilityChanged.Broadcast(AccessibilityConfig);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Toggled accessibility feature: %s = %s"), 
           *UEnum::GetValueAsString(Feature), bEnabled ? TEXT("Enabled") : TEXT("Disabled"));
}

bool UProjectVisibleUIManager::IsAccessibilityFeatureEnabled(EAccessibilityFeature Feature) const
{
    return AccessibilityConfig.EnabledFeatures.Contains(Feature);
}

void UProjectVisibleUIManager::SetTextSizeMultiplier(float Multiplier)
{
    AccessibilityConfig.TextSizeMultiplier = FMath::Clamp(Multiplier, 0.5f, 3.0f);
    CurrentTheme.FontScale = AccessibilityConfig.TextSizeMultiplier;
    
    // Reapply theme to update text sizes
    ApplyTheme(CurrentTheme);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Set text size multiplier: %.2f"), Multiplier);
}

void UProjectVisibleUIManager::SetHighContrastMode(bool bEnabled)
{
    AccessibilityConfig.bHighContrastMode = bEnabled;
    CurrentTheme.bHighContrast = bEnabled;
    
    if (bEnabled)
    {
        ApplyHighContrastColors();
    }
    
    ApplyTheme(CurrentTheme);
    
    UE_LOG(LogProjectVisible, Log, TEXT("High contrast mode: %s"), bEnabled ? TEXT("Enabled") : TEXT("Disabled"));
}

void UProjectVisibleUIManager::SetReduceMotion(bool bEnabled)
{
    AccessibilityConfig.bReduceMotion = bEnabled;
    CurrentTheme.bReduceMotion = bEnabled;
    
    if (bEnabled)
    {
        // Reduce animation speeds
        GlobalAnimationSpeedMultiplier = 0.1f;
        CurrentTheme.AnimationSpeed = 0.1f;
    }
    else
    {
        GlobalAnimationSpeedMultiplier = 1.0f;
        CurrentTheme.AnimationSpeed = 1.0f;
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Reduce motion: %s"), bEnabled ? TEXT("Enabled") : TEXT("Disabled"));
}

// Animation System
void UProjectVisibleUIManager::PlayUIAnimation(UUserWidget* Widget, const FUIAnimationConfig& AnimationConfig)
{
    if (!Widget)
    {
        return;
    }
    
    // Store animation config
    ActiveAnimations.Add(Widget, AnimationConfig);
    
    // Calculate actual duration based on global speed
    float ActualDuration = AnimationConfig.Duration / (GlobalAnimationSpeedMultiplier * CurrentTheme.AnimationSpeed);
    
    OnAnimationStarted.Broadcast(Widget, AnimationConfig.AnimationType);
    
    // TODO: Implement actual animation using UMG animations or Slate animations
    // For now, we'll just log and broadcast completion
    UE_LOG(LogProjectVisible, Log, TEXT("Started UI animation: %s on widget"), 
           *UEnum::GetValueAsString(AnimationConfig.AnimationType));
    
    // Simulate animation completion
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Widget, AnimationConfig]()
    {
        HandleAnimationComplete(Widget, AnimationConfig.AnimationType);
    }, ActualDuration, false);
}

void UProjectVisibleUIManager::StopUIAnimation(UUserWidget* Widget)
{
    if (ActiveAnimations.Contains(Widget))
    {
        ActiveAnimations.Remove(Widget);
        UE_LOG(LogProjectVisible, Log, TEXT("Stopped UI animation on widget"));
    }
}

void UProjectVisibleUIManager::SetGlobalAnimationSpeed(float SpeedMultiplier)
{
    GlobalAnimationSpeedMultiplier = FMath::Clamp(SpeedMultiplier, 0.1f, 5.0f);
    UE_LOG(LogProjectVisible, Log, TEXT("Set global animation speed: %.2f"), SpeedMultiplier);
}

void UProjectVisibleUIManager::CreateScreenTransition(EProjectVisibleScreenType FromScreen, EProjectVisibleScreenType ToScreen, EUIAnimationType TransitionType)
{
    // Create transition animation between screens
    FUIAnimationConfig TransitionConfig;
    TransitionConfig.AnimationType = TransitionType;
    TransitionConfig.Duration = 0.5f;
    
    UE_LOG(LogProjectVisible, Log, TEXT("Created screen transition: %s -> %s (%s)"), 
           *UEnum::GetValueAsString(FromScreen), *UEnum::GetValueAsString(ToScreen), *UEnum::GetValueAsString(TransitionType));
}

// Layout Management
void UProjectVisibleUIManager::SetLayoutConfig(const FUILayoutConfig& Config)
{
    LayoutConfig = Config;
    UpdateResponsiveLayout();
    
    UE_LOG(LogProjectVisible, Log, TEXT("Updated layout configuration"));
}

void UProjectVisibleUIManager::UpdateLayoutForScreenSize(FVector2D ScreenSize)
{
    // Calculate responsive scale based on screen size
    float BaseWidth = 1920.0f;
    float BaseHeight = 1080.0f;
    
    float WidthScale = ScreenSize.X / BaseWidth;
    float HeightScale = ScreenSize.Y / BaseHeight;
    float ResponsiveScale = FMath::Min(WidthScale, HeightScale);
    
    // Apply constraints
    ResponsiveScale = FMath::Clamp(ResponsiveScale, 0.5f, 2.0f);
    
    // Update layout config
    LayoutConfig.ResponsiveScales.Add(TEXT("Current"), ResponsiveScale);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Updated layout for screen size: %.0fx%.0f (Scale: %.2f)"), 
           ScreenSize.X, ScreenSize.Y, ResponsiveScale);
}

float UProjectVisibleUIManager::GetResponsiveScale() const
{
    if (const float* Scale = LayoutConfig.ResponsiveScales.Find(TEXT("Current")))
    {
        return *Scale;
    }
    return 1.0f;
}

FMargin UProjectVisibleUIManager::GetSafeZoneMargin() const
{
    float SafeZone = LayoutConfig.SafeZoneScale;
    float Margin = (1.0f - SafeZone) / 2.0f * 100.0f; // Convert to percentage
    
    return FMargin(Margin, Margin, Margin, Margin);
}

// Widget Factory
UUserWidget* UProjectVisibleUIManager::CreateScreenWidget(EProjectVisibleScreenType ScreenType)
{
    TSoftClassPtr<UUserWidget>* WidgetClassPtr = ScreenWidgetClasses.Find(ScreenType);
    if (!WidgetClassPtr || !WidgetClassPtr->IsValid())
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("No widget class registered for screen type: %s"), *UEnum::GetValueAsString(ScreenType));
        return nullptr;
    }
    
    UClass* WidgetClass = WidgetClassPtr->LoadSynchronous();
    if (!WidgetClass)
    {
        UE_LOG(LogProjectVisible, Error, TEXT("Failed to load widget class for screen type: %s"), *UEnum::GetValueAsString(ScreenType));
        return nullptr;
    }
    
    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            UUserWidget* Widget = CreateWidget<UUserWidget>(PC, WidgetClass);
            if (Widget)
            {
                ApplyThemeToWidget(Widget, CurrentTheme);
                UE_LOG(LogProjectVisible, Log, TEXT("Created widget for screen: %s"), *UEnum::GetValueAsString(ScreenType));
                return Widget;
            }
        }
    }
    
    return nullptr;
}

void UProjectVisibleUIManager::RegisterScreenWidgetClass(EProjectVisibleScreenType ScreenType, TSoftClassPtr<UUserWidget> WidgetClass)
{
    ScreenWidgetClasses.Add(ScreenType, WidgetClass);
    UE_LOG(LogProjectVisible, Log, TEXT("Registered widget class for screen: %s"), *UEnum::GetValueAsString(ScreenType));
}

// Helper Functions
void UProjectVisibleUIManager::InitializeDefaultThemes()
{
    // Default theme (already set in constructor)
    RegisterTheme(TEXT("Default"), CurrentTheme);
    
    // Dark theme
    FProjectVisibleUITheme DarkTheme = CurrentTheme;
    DarkTheme.ThemeName = TEXT("Dark");
    DarkTheme.BackgroundColor = FLinearColor(0.02f, 0.02f, 0.05f, 0.95f);
    DarkTheme.TextColor = FLinearColor(0.9f, 0.9f, 0.95f, 1.0f);
    RegisterTheme(TEXT("Dark"), DarkTheme);
    
    // Light theme
    FProjectVisibleUITheme LightTheme = CurrentTheme;
    LightTheme.ThemeName = TEXT("Light");
    LightTheme.BackgroundColor = FLinearColor(0.95f, 0.95f, 0.98f, 0.95f);
    LightTheme.TextColor = FLinearColor(0.1f, 0.1f, 0.15f, 1.0f);
    RegisterTheme(TEXT("Light"), LightTheme);
    
    // High Contrast theme
    FProjectVisibleUITheme HighContrastTheme = CurrentTheme;
    HighContrastTheme.ThemeName = TEXT("HighContrast");
    HighContrastTheme.bHighContrast = true;
    HighContrastTheme.PrimaryColor = FLinearColor::White;
    HighContrastTheme.BackgroundColor = FLinearColor::Black;
    HighContrastTheme.TextColor = FLinearColor::White;
    RegisterTheme(TEXT("HighContrast"), HighContrastTheme);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Initialized default themes"));
}

void UProjectVisibleUIManager::InitializeDefaultScreenClasses()
{
    // TODO: Register actual widget classes when they are created
    // For now, we'll log the initialization
    UE_LOG(LogProjectVisible, Log, TEXT("Initialized default screen widget classes"));
}

void UProjectVisibleUIManager::ApplyAccessibilitySettings()
{
    if (AccessibilityConfig.bHighContrastMode)
    {
        SetHighContrastMode(true);
    }
    
    if (AccessibilityConfig.bReduceMotion)
    {
        SetReduceMotion(true);
    }
    
    if (AccessibilityConfig.TextSizeMultiplier != 1.0f)
    {
        SetTextSizeMultiplier(AccessibilityConfig.TextSizeMultiplier);
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Applied accessibility settings"));
}

UUserWidget* UProjectVisibleUIManager::GetOrCreateScreenWidget(EProjectVisibleScreenType ScreenType)
{
    // Check if widget already exists
    if (TWeakObjectPtr<UUserWidget>* ExistingWidget = ActiveWidgets.Find(ScreenType))
    {
        if (ExistingWidget->IsValid())
        {
            return ExistingWidget->Get();
        }
        else
        {
            ActiveWidgets.Remove(ScreenType);
        }
    }
    
    // Create new widget
    UUserWidget* NewWidget = CreateScreenWidget(ScreenType);
    if (NewWidget)
    {
        ActiveWidgets.Add(ScreenType, NewWidget);
    }
    
    return NewWidget;
}

void UProjectVisibleUIManager::CleanupInactiveWidgets()
{
    TArray<EProjectVisibleScreenType> WidgetsToRemove;
    
    for (auto& WidgetPair : ActiveWidgets)
    {
        if (!WidgetPair.Value.IsValid())
        {
            WidgetsToRemove.Add(WidgetPair.Key);
        }
    }
    
    for (EProjectVisibleScreenType ScreenType : WidgetsToRemove)
    {
        ActiveWidgets.Remove(ScreenType);
    }
    
    if (WidgetsToRemove.Num() > 0)
    {
        UE_LOG(LogProjectVisible, Log, TEXT("Cleaned up %d inactive widgets"), WidgetsToRemove.Num());
    }
}

void UProjectVisibleUIManager::HandleScreenTransition(EProjectVisibleScreenType FromScreen, EProjectVisibleScreenType ToScreen)
{
    // Create transition effects based on screen types
    EUIAnimationType TransitionType = EUIAnimationType::FadeIn;
    
    // Special transitions for certain screen combinations
    if (FromScreen == EProjectVisibleScreenType::RealityMode && ToScreen == EProjectVisibleScreenType::DreamMode)
    {
        TransitionType = EUIAnimationType::Dissolve;
    }
    else if (FromScreen == EProjectVisibleScreenType::DreamMode && ToScreen == EProjectVisibleScreenType::RealityMode)
    {
        TransitionType = EUIAnimationType::Quantum;
    }
    
    CreateScreenTransition(FromScreen, ToScreen, TransitionType);
}

void UProjectVisibleUIManager::ApplyThemeToWidget(UUserWidget* Widget, const FProjectVisibleUITheme& Theme)
{
    if (!Widget)
    {
        return;
    }
    
    // TODO: Implement actual theme application to widget
    // This would involve setting colors, fonts, and other style properties
    UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Applied theme to widget: %s"), *Widget->GetName());
}

void UProjectVisibleUIManager::PropagateThemeChange()
{
    // Notify all systems about theme change
    UE_LOG(LogProjectVisible, Log, TEXT("Propagated theme change to all systems"));
}

void UProjectVisibleUIManager::HandleAnimationComplete(UUserWidget* Widget, EUIAnimationType AnimationType)
{
    if (Widget && ActiveAnimations.Contains(Widget))
    {
        ActiveAnimations.Remove(Widget);
        OnAnimationCompleted.Broadcast(Widget, AnimationType);
        
        UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Animation completed: %s"), *UEnum::GetValueAsString(AnimationType));
    }
}

void UProjectVisibleUIManager::ConfigureInputForScreen(EProjectVisibleScreenType ScreenType)
{
    // Configure input mode based on screen type
    switch (ScreenType)
    {
        case EProjectVisibleScreenType::MainMenu:
        case EProjectVisibleScreenType::Settings:
        case EProjectVisibleScreenType::VirtueProfile:
        case EProjectVisibleScreenType::MemoryViewer:
            SetUIOnlyInput();
            break;
        case EProjectVisibleScreenType::RealityMode:
        case EProjectVisibleScreenType::DreamMode:
        case EProjectVisibleScreenType::Investigation:
            RestoreGameInput();
            break;
        default:
            break;
    }
    
    UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Configured input for screen: %s"), *UEnum::GetValueAsString(ScreenType));
}

void UProjectVisibleUIManager::RestoreGameInput()
{
    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            FInputModeGameAndUI InputMode;
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            InputMode.SetHideCursorDuringCapture(false);
            PC->SetInputMode(InputMode);
        }
    }
}

void UProjectVisibleUIManager::SetUIOnlyInput()
{
    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            FInputModeUIOnly InputMode;
            PC->SetInputMode(InputMode);
            PC->SetShowMouseCursor(true);
        }
    }
}

void UProjectVisibleUIManager::ApplyHighContrastColors()
{
    CurrentTheme.PrimaryColor = FLinearColor::White;
    CurrentTheme.SecondaryColor = FLinearColor::Black;
    CurrentTheme.BackgroundColor = FLinearColor::Black;
    CurrentTheme.TextColor = FLinearColor::White;
    CurrentTheme.AccentColor = FLinearColor::Yellow;
}

// Placeholder implementations for data integration
void UProjectVisibleUIManager::UpdateVirtueDisplayData()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Updated virtue display data"));
    // TODO: Integrate with VirtueManager
}

void UProjectVisibleUIManager::UpdateMemoryDisplayData()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Updated memory display data"));
    // TODO: Integrate with MemoryManager
}

void UProjectVisibleUIManager::UpdateHappinessDisplayData()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Updated happiness display data"));
    // TODO: Integrate with MemoryManager happiness metrics
}

void UProjectVisibleUIManager::UpdateInvestigationDisplayData()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Updated investigation display data"));
    // TODO: Integrate with RealityInvestigationManager
}

void UProjectVisibleUIManager::SetInputMode(EProjectVisibleScreenType ScreenType)
{
    ConfigureInputForScreen(ScreenType);
    UE_LOG(LogProjectVisible, Log, TEXT("Set input mode for screen: %s"), *UEnum::GetValueAsString(ScreenType));
}

void UProjectVisibleUIManager::UpdateResponsiveLayout()
{
    if (UWorld* World = GetWorld())
    {
        if (UGameViewportClient* ViewportClient = World->GetGameViewport())
        {
            FVector2D ViewportSize;
            ViewportClient->GetViewportSize(ViewportSize);
            UpdateLayoutForScreenSize(ViewportSize);
        }
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Updated responsive layout"));
}
