// Project Visible - UI/UX Polish Manager
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/World.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"
#include "UIPolishTypes.h"
#include "UIPolishManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUIAnimationCompleted, UWidget*, Widget, EUIVisualEffect, EffectType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUIEffectTriggered, UWidget*, Widget, const FUIVisualEffectConfig&, EffectConfig);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUIInteractionStateChanged, UWidget*, Widget, EUIInteractionState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResponsiveLayoutChanged, EUIBreakpoint, OldBreakpoint, EUIBreakpoint, NewBreakpoint);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUIPolishLevelChanged, EUIPolishLevel, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAccessibilitySettingsChanged, const FAccessibilityConfig&, NewSettings);

/**
 * Central manager for Project Visible's UI/UX polish and enhancement system
 * Manages animations, visual effects, responsive design, and accessibility features
 */
UCLASS()
class GEMINI_C_API UUIPolishManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Core Polish System
    UFUNCTION(BlueprintCallable, Category = "UI Polish")
    void InitializeUIPolishSystem();

    UFUNCTION(BlueprintCallable, Category = "UI Polish")
    void SetUIPolishLevel(EUIPolishLevel NewLevel);

    UFUNCTION(BlueprintCallable, Category = "UI Polish", BlueprintPure)
    EUIPolishLevel GetUIPolishLevel() const { return CurrentPolishConfig.PolishLevel; }

    UFUNCTION(BlueprintCallable, Category = "UI Polish")
    void ApplyUIPolishConfig(const FUIPolishConfig& Config);

    UFUNCTION(BlueprintCallable, Category = "UI Polish", BlueprintPure)
    FUIPolishConfig GetCurrentPolishConfig() const { return CurrentPolishConfig; }

    // Animation System
    UFUNCTION(BlueprintCallable, Category = "UI Polish | Animation")
    void PlayUIAnimation(UWidget* Widget, EUIVisualEffect EffectType, float Duration = 0.5f);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Animation")
    void PlayCustomUIAnimation(UWidget* Widget, const FUIVisualEffectConfig& EffectConfig);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Animation")
    void StopUIAnimation(UWidget* Widget);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Animation")
    void StopAllUIAnimations();

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Animation", BlueprintPure)
    bool IsWidgetAnimating(UWidget* Widget) const;

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Animation")
    void SetGlobalAnimationSpeed(float Speed);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Animation", BlueprintPure)
    float GetGlobalAnimationSpeed() const { return CurrentPolishConfig.GlobalAnimationSpeed; }

    // Visual Effects
    UFUNCTION(BlueprintCallable, Category = "UI Polish | Effects")
    void ApplyVisualEffect(UWidget* Widget, EUIVisualEffect EffectType, float Intensity = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Effects")
    void RemoveVisualEffect(UWidget* Widget, EUIVisualEffect EffectType);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Effects")
    void ClearAllVisualEffects(UWidget* Widget);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Effects")
    void EnableParticleEffects(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Effects")
    void EnablePostProcessEffects(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Effects")
    void SetEffectQuality(EUIPolishLevel QualityLevel);

    // Interaction System
    UFUNCTION(BlueprintCallable, Category = "UI Polish | Interaction")
    void RegisterInteractiveWidget(UWidget* Widget, const FUIInteractionConfig& Config);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Interaction")
    void UnregisterInteractiveWidget(UWidget* Widget);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Interaction")
    void SetWidgetInteractionState(UWidget* Widget, EUIInteractionState NewState);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Interaction", BlueprintPure)
    EUIInteractionState GetWidgetInteractionState(UWidget* Widget) const;

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Interaction")
    void TriggerHapticFeedback(EUIHapticType HapticType);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Interaction")
    void PlayUISound(const FString& SoundID);

    // Responsive Design
    UFUNCTION(BlueprintCallable, Category = "UI Polish | Responsive")
    void UpdateResponsiveLayout();

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Responsive")
    void SetCurrentBreakpoint(EUIBreakpoint NewBreakpoint);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Responsive", BlueprintPure)
    EUIBreakpoint GetCurrentBreakpoint() const { return CurrentBreakpoint; }

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Responsive")
    void RegisterResponsiveWidget(UWidget* Widget, const TArray<FResponsiveLayoutConfig>& Layouts);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Responsive")
    void ApplyResponsiveLayout(UWidget* Widget, EUIBreakpoint Breakpoint);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Responsive")
    EUIBreakpoint CalculateBreakpointFromScreenSize(FVector2D ScreenSize) const;

    // Accessibility System
    UFUNCTION(BlueprintCallable, Category = "UI Polish | Accessibility")
    void ApplyAccessibilitySettings(const FAccessibilityConfig& Config);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Accessibility", BlueprintPure)
    FAccessibilityConfig GetAccessibilitySettings() const { return CurrentPolishConfig.AccessibilitySettings; }

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Accessibility")
    void EnableHighContrast(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Accessibility")
    void EnableReducedMotion(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Accessibility")
    void SetTextSizeMultiplier(float Multiplier);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Accessibility")
    void EnableColorBlindFriendlyMode(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Accessibility")
    void SetScreenReaderText(UWidget* Widget, const FString& Text);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Accessibility")
    void AnnounceToScreenReader(const FString& Text);

    // Theme Management
    UFUNCTION(BlueprintCallable, Category = "UI Polish | Theme")
    void ApplyTheme(EUITheme Theme);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Theme")
    void SetCustomThemeColors(const TMap<FString, FLinearColor>& ColorMap);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Theme")
    void UpdateThemeForAccessibility();

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Theme", BlueprintPure)
    FLinearColor GetThemeColor(const FString& ColorName) const;

    // Performance Optimization
    UFUNCTION(BlueprintCallable, Category = "UI Polish | Performance")
    void OptimizeUIPerformance();

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Performance")
    void SetAnimationLOD(int32 LODLevel);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Performance")
    void EnableUIBatching(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Performance")
    void CullOffscreenWidgets(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Performance")
    void SetMaxConcurrentAnimations(int32 MaxAnimations);

    // Analytics and Monitoring
    UFUNCTION(BlueprintCallable, Category = "UI Polish | Analytics")
    void RecordUIInteraction(UWidget* Widget, const FString& InteractionType);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Analytics")
    void RecordAnimationUsage(EUIVisualEffect EffectType);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Analytics")
    void RecordUserFeedback(const FString& Feedback, float SatisfactionScore);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Analytics", BlueprintPure)
    FUIPolishAnalytics GetPolishAnalytics() const { return AnalyticsData; }

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Analytics")
    void ResetAnalytics();

    // Game Integration
    UFUNCTION(BlueprintCallable, Category = "UI Polish | Integration")
    void OnGameStateChanged(const FString& NewGameState);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Integration")
    void OnPerformanceLevelChanged(EPerformanceLevel PerformanceLevel);

    // UFUNCTION(BlueprintCallable, Category = "UI Polish | Integration") // Temporarily disabled
    // void OnAudioMoodChanged(EAudioMood AudioMood);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Integration")
    void OnStoryPhaseChanged(EStoryPhase StoryPhase);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Integration")
    void OnBoundaryDissolutionChanged(float DissolutionLevel);

    // Configuration Management
    UFUNCTION(BlueprintCallable, Category = "UI Polish | Config")
    void SavePolishConfiguration(const FString& ConfigName);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Config")
    bool LoadPolishConfiguration(const FString& ConfigName);

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Config", BlueprintPure)
    TArray<FString> GetAvailableConfigurations() const;

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Config")
    void ResetToDefaultConfiguration();

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Config")
    FString ExportPolishConfiguration() const;

    UFUNCTION(BlueprintCallable, Category = "UI Polish | Config")
    bool ImportPolishConfiguration(const FString& JsonData);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "UI Polish | Events")
    FOnUIAnimationCompleted OnUIAnimationCompleted;

    UPROPERTY(BlueprintAssignable, Category = "UI Polish | Events")
    FOnUIEffectTriggered OnUIEffectTriggered;

    UPROPERTY(BlueprintAssignable, Category = "UI Polish | Events")
    FOnUIInteractionStateChanged OnUIInteractionStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "UI Polish | Events")
    FOnResponsiveLayoutChanged OnResponsiveLayoutChanged;

    UPROPERTY(BlueprintAssignable, Category = "UI Polish | Events")
    FOnUIPolishLevelChanged OnUIPolishLevelChanged;

    UPROPERTY(BlueprintAssignable, Category = "UI Polish | Events")
    FOnAccessibilitySettingsChanged OnAccessibilitySettingsChanged;

protected:
    // Core Data
    UPROPERTY(BlueprintReadOnly, Category = "UI Polish Data")
    FUIPolishConfig CurrentPolishConfig;

    UPROPERTY(BlueprintReadOnly, Category = "UI Polish Data")
    EUIBreakpoint CurrentBreakpoint;

    UPROPERTY(BlueprintReadOnly, Category = "UI Polish Data")
    FUIPolishAnalytics AnalyticsData;

    // Widget Management
    UPROPERTY(BlueprintReadOnly, Category = "UI Polish Data")
    TMap<UWidget*, FUIInteractionConfig> RegisteredWidgets;

    TMap<UWidget*, TArray<FResponsiveLayoutConfig>> ResponsiveWidgets;

    TMap<UWidget*, TArray<FUIVisualEffectConfig>> ActiveEffects;

    UPROPERTY(BlueprintReadOnly, Category = "UI Polish Data")
    TSet<UWidget*> AnimatingWidgets;

    // Theme and Style
    UPROPERTY(BlueprintReadOnly, Category = "UI Polish Data")
    TMap<FString, FLinearColor> CurrentThemeColors;

    UPROPERTY(BlueprintReadOnly, Category = "UI Polish Data")
    TMap<FString, FUIPolishConfig> SavedConfigurations;

    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish Settings")
    int32 MaxConcurrentAnimations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish Settings")
    bool bEnableUIBatching;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish Settings")
    bool bCullOffscreenWidgets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish Settings")
    int32 AnimationLODLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish Settings")
    float PerformanceScalingFactor;

private:
    // Internal Animation System
    void ProcessAnimationQueue();
    void UpdateActiveAnimations(float DeltaTime);
    void CleanupCompletedAnimations();
    float CalculateEasing(EUIEasingType EasingType, float Time) const;
    void ApplyAnimationToWidget(UWidget* Widget, const FUIVisualEffectConfig& Effect, float Progress);

    // Visual Effects Implementation
    void ApplyFadeEffect(UWidget* Widget, float Progress, bool bFadeIn);
    void ApplySlideEffect(UWidget* Widget, float Progress, bool bSlideIn);
    void ApplyScaleEffect(UWidget* Widget, float Progress, bool bScaleUp);
    void ApplyRotationEffect(UWidget* Widget, float Progress);
    void ApplyShakeEffect(UWidget* Widget, float Progress);
    void ApplyPulseEffect(UWidget* Widget, float Progress);
    void ApplyGlowEffect(UWidget* Widget, float Progress);
    void ApplyBlurEffect(UWidget* Widget, float Progress);

    // Responsive Layout Helpers
    void UpdateAllResponsiveWidgets();
    void ApplyLayoutToWidget(UWidget* Widget, const FResponsiveLayoutConfig& Layout);
    FResponsiveLayoutConfig GetLayoutForBreakpoint(const TArray<FResponsiveLayoutConfig>& Layouts, EUIBreakpoint Breakpoint) const;

    // Accessibility Helpers
    void ApplyAccessibilityToWidget(UWidget* Widget);
    void UpdateUIForColorBlindness(UWidget* Widget);
    void UpdateTextSizeForWidget(UWidget* Widget);
    void ApplyHighContrastToWidget(UWidget* Widget);

    // Performance Optimization
    void OptimizeWidgetPerformance(UWidget* Widget);
    void BatchUIUpdates();
    void CullWidget(UWidget* Widget, bool bShouldCull);

    // Analytics Helpers
    void UpdateAnalyticsData();
    void RecordPerformanceMetrics();

    // Integration Helpers
    class UProjectVisibleUIManager* GetUIManager();
    class UPerformanceMonitoringManager* GetPerformanceManager();
    // class UAudioSystemManager* GetAudioManager(); // Temporarily disabled
    class UStoryManager* GetStoryManager();
    class UBoundaryDissolutionManager* GetBoundaryManager();
    class USocialExperimentManager* GetExperimentManager();

    // Timer Handles
    UPROPERTY()
    FTimerHandle AnimationUpdateTimer;

    UPROPERTY()
    FTimerHandle PerformanceOptimizationTimer;

    UPROPERTY()
    FTimerHandle AnalyticsUpdateTimer;
};
