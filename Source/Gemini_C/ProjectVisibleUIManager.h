// Project Visible - CommonUI Manager
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "ProjectVisibleUITypes.h"
#include "ProjectVisibleUIManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScreenChanged, EProjectVisibleScreenType, OldScreen, EProjectVisibleScreenType, NewScreen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnThemeChanged, const FProjectVisibleUITheme&, NewTheme);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAccessibilityChanged, const FAccessibilityConfig&, NewConfig);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnimationStarted, UUserWidget*, Widget, EUIAnimationType, AnimationType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnimationCompleted, UUserWidget*, Widget, EUIAnimationType, AnimationType);

/**
 * Central manager for Project Visible's CommonUI integration
 * Handles screen management, theming, accessibility, and animations
 */
UCLASS()
class GEMINI_C_API UProjectVisibleUIManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Screen Management
    UFUNCTION(BlueprintCallable, Category = "UI Management")
    void InitializeUISystem();

    UFUNCTION(BlueprintCallable, Category = "UI Management")
    void PushScreen(EProjectVisibleScreenType ScreenType, bool bAddToHistory = true);

    UFUNCTION(BlueprintCallable, Category = "UI Management")
    void PopScreen();

    UFUNCTION(BlueprintCallable, Category = "UI Management")
    void ReplaceScreen(EProjectVisibleScreenType ScreenType);

    UFUNCTION(BlueprintCallable, Category = "UI Management")
    void ClearScreenStack();

    UFUNCTION(BlueprintCallable, Category = "UI Management")
    void ShowModal(EProjectVisibleScreenType ScreenType, EUIInputPriority Priority = EUIInputPriority::High);

    UFUNCTION(BlueprintCallable, Category = "UI Management")
    void HideModal(EProjectVisibleScreenType ScreenType);

    UFUNCTION(BlueprintCallable, Category = "UI Management", BlueprintPure)
    EProjectVisibleScreenType GetCurrentScreen() const { return CurrentScreen; }

    UFUNCTION(BlueprintCallable, Category = "UI Management", BlueprintPure)
    bool IsScreenActive(EProjectVisibleScreenType ScreenType) const;

    // Theme Management
    UFUNCTION(BlueprintCallable, Category = "UI Theming")
    void ApplyTheme(const FProjectVisibleUITheme& Theme);

    UFUNCTION(BlueprintCallable, Category = "UI Theming")
    void SetThemeByName(const FString& ThemeName);

    UFUNCTION(BlueprintCallable, Category = "UI Theming", BlueprintPure)
    FProjectVisibleUITheme GetCurrentTheme() const { return CurrentTheme; }

    UFUNCTION(BlueprintCallable, Category = "UI Theming")
    void RegisterTheme(const FString& ThemeName, const FProjectVisibleUITheme& Theme);

    UFUNCTION(BlueprintCallable, Category = "UI Theming", BlueprintPure)
    TArray<FString> GetAvailableThemes() const;

    UFUNCTION(BlueprintCallable, Category = "UI Theming")
    void SetDynamicColor(const FString& ColorName, const FLinearColor& Color);

    UFUNCTION(BlueprintCallable, Category = "UI Theming", BlueprintPure)
    FLinearColor GetThemeColor(const FString& ColorName) const;

    // Accessibility
    UFUNCTION(BlueprintCallable, Category = "UI Accessibility")
    void SetAccessibilityConfig(const FAccessibilityConfig& Config);

    UFUNCTION(BlueprintCallable, Category = "UI Accessibility", BlueprintPure)
    FAccessibilityConfig GetAccessibilityConfig() const { return AccessibilityConfig; }

    UFUNCTION(BlueprintCallable, Category = "UI Accessibility")
    void ToggleAccessibilityFeature(EAccessibilityFeature Feature, bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UI Accessibility", BlueprintPure)
    bool IsAccessibilityFeatureEnabled(EAccessibilityFeature Feature) const;

    UFUNCTION(BlueprintCallable, Category = "UI Accessibility")
    void SetTextSizeMultiplier(float Multiplier);

    UFUNCTION(BlueprintCallable, Category = "UI Accessibility")
    void SetHighContrastMode(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "UI Accessibility")
    void SetReduceMotion(bool bEnabled);

    // Animation System
    UFUNCTION(BlueprintCallable, Category = "UI Animation")
    void PlayUIAnimation(UUserWidget* Widget, const FUIAnimationConfig& AnimationConfig);

    UFUNCTION(BlueprintCallable, Category = "UI Animation")
    void StopUIAnimation(UUserWidget* Widget);

    UFUNCTION(BlueprintCallable, Category = "UI Animation")
    void SetGlobalAnimationSpeed(float SpeedMultiplier);

    UFUNCTION(BlueprintCallable, Category = "UI Animation")
    void CreateScreenTransition(EProjectVisibleScreenType FromScreen, EProjectVisibleScreenType ToScreen, 
                               EUIAnimationType TransitionType = EUIAnimationType::FadeIn);

    // Layout Management
    UFUNCTION(BlueprintCallable, Category = "UI Layout")
    void SetLayoutConfig(const FUILayoutConfig& Config);

    UFUNCTION(BlueprintCallable, Category = "UI Layout", BlueprintPure)
    FUILayoutConfig GetLayoutConfig() const { return LayoutConfig; }

    UFUNCTION(BlueprintCallable, Category = "UI Layout")
    void UpdateLayoutForScreenSize(FVector2D ScreenSize);

    UFUNCTION(BlueprintCallable, Category = "UI Layout", BlueprintPure)
    float GetResponsiveScale() const;

    UFUNCTION(BlueprintCallable, Category = "UI Layout", BlueprintPure)
    FMargin GetSafeZoneMargin() const;

    // Widget Factory
    UFUNCTION(BlueprintCallable, Category = "UI Factory")
    UUserWidget* CreateScreenWidget(EProjectVisibleScreenType ScreenType);

    UFUNCTION(BlueprintCallable, Category = "UI Factory")
    void RegisterScreenWidgetClass(EProjectVisibleScreenType ScreenType, TSoftClassPtr<UUserWidget> WidgetClass);

    // Input Management
    UFUNCTION(BlueprintCallable, Category = "UI Input")
    void SetInputMode(EProjectVisibleScreenType ScreenType);

    UFUNCTION(BlueprintCallable, Category = "UI Input")
    void RestoreGameInput();

    UFUNCTION(BlueprintCallable, Category = "UI Input")
    void SetUIOnlyInput();

    // Data Integration
    UFUNCTION(BlueprintCallable, Category = "UI Data")
    void UpdateVirtueDisplayData();

    UFUNCTION(BlueprintCallable, Category = "UI Data")
    void UpdateMemoryDisplayData();

    UFUNCTION(BlueprintCallable, Category = "UI Data")
    void UpdateHappinessDisplayData();

    UFUNCTION(BlueprintCallable, Category = "UI Data")
    void UpdateInvestigationDisplayData();

    // Events
    UPROPERTY(BlueprintAssignable, Category = "UI Events")
    FOnScreenChanged OnScreenChanged;

    UPROPERTY(BlueprintAssignable, Category = "UI Events")
    FOnThemeChanged OnThemeChanged;

    UPROPERTY(BlueprintAssignable, Category = "UI Events")
    FOnAccessibilityChanged OnAccessibilityChanged;

    UPROPERTY(BlueprintAssignable, Category = "UI Events")
    FOnAnimationStarted OnAnimationStarted;

    UPROPERTY(BlueprintAssignable, Category = "UI Events")
    FOnAnimationCompleted OnAnimationCompleted;

protected:
    // Core UI State
    UPROPERTY(BlueprintReadOnly, Category = "UI State")
    EProjectVisibleScreenType CurrentScreen;

    UPROPERTY(BlueprintReadOnly, Category = "UI State")
    TArray<EProjectVisibleScreenType> ScreenHistory;

    UPROPERTY(BlueprintReadOnly, Category = "UI State")
    TArray<EProjectVisibleScreenType> ActiveModals;

    // Configuration
    UPROPERTY(BlueprintReadOnly, Category = "UI Configuration")
    FProjectVisibleUITheme CurrentTheme;

    UPROPERTY(BlueprintReadOnly, Category = "UI Configuration")
    FAccessibilityConfig AccessibilityConfig;

    UPROPERTY(BlueprintReadOnly, Category = "UI Configuration")
    FUILayoutConfig LayoutConfig;

    // Theme Registry
    UPROPERTY()
    TMap<FString, FProjectVisibleUITheme> RegisteredThemes;

    // Widget Registry
    UPROPERTY()
    TMap<EProjectVisibleScreenType, TSoftClassPtr<UUserWidget>> ScreenWidgetClasses;

    UPROPERTY()
    TMap<EProjectVisibleScreenType, TWeakObjectPtr<UUserWidget>> ActiveWidgets;

    // Animation Management
    UPROPERTY()
    TMap<TWeakObjectPtr<UUserWidget>, FUIAnimationConfig> ActiveAnimations;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Settings")
    bool bUseCommonUI;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Settings")
    bool bDebugUI;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Settings")
    float GlobalAnimationSpeedMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Settings")
    int32 MaxScreenHistory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Settings")
    bool bAutoSaveThemeSettings;

    // References
    UPROPERTY()
    TWeakObjectPtr<UUserWidget> CurrentMainWidget;

    UPROPERTY()
    TArray<TWeakObjectPtr<UUserWidget>> ModalWidgetStack;

private:
    // Helper Functions
    void InitializeDefaultThemes();
    void InitializeDefaultScreenClasses();
    void ApplyAccessibilitySettings();
    void UpdateResponsiveLayout();
    
    // Screen Management Helpers
    UUserWidget* GetOrCreateScreenWidget(EProjectVisibleScreenType ScreenType);
    void CleanupInactiveWidgets();
    void HandleScreenTransition(EProjectVisibleScreenType FromScreen, EProjectVisibleScreenType ToScreen);
    
    // Theme Helpers
    void ApplyThemeToWidget(UUserWidget* Widget, const FProjectVisibleUITheme& Theme);
    void PropagateThemeChange();
    
    // Animation Helpers
    void ProcessAnimationQueue();
    void HandleAnimationComplete(UUserWidget* Widget, EUIAnimationType AnimationType);
    
    // Input Helpers
    void ConfigureInputForScreen(EProjectVisibleScreenType ScreenType);
    void SetupCommonInputActions();
    
    // Data Integration Helpers
    void RefreshUIData();
    void HandleGameStateChange();
    
    // Accessibility Helpers
    void ApplyHighContrastColors();
    void AdjustTextSizes();
    void ConfigureScreenReader();
    void HandleMotionPreferences();
};
