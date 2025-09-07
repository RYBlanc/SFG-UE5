// Project Visible - Base Widget Class
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommonActivatableWidget.h"
#include "ProjectVisibleUITypes.h"
#include "ProjectVisibleBaseWidget.generated.h"

class UProjectVisibleUIManager;

/**
 * Base class for all Project Visible UI widgets
 * Integrates with CommonUI and provides theme/accessibility support
 */
UCLASS(BlueprintType, Blueprintable)
class GEMINI_C_API UProjectVisibleBaseWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UProjectVisibleBaseWidget(const FObjectInitializer& ObjectInitializer);

protected:
	// UUserWidget interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Common UI interface
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

public:
	// Widget Lifecycle
	UFUNCTION(BlueprintCallable, Category = "Project Visible UI")
	virtual void InitializeWidget();

	UFUNCTION(BlueprintCallable, Category = "Project Visible UI")
	virtual void UpdateWidget();

	UFUNCTION(BlueprintCallable, Category = "Project Visible UI")
	virtual void RefreshData();

	// Theme Management
	UFUNCTION(BlueprintCallable, Category = "Project Visible UI")
	virtual void ApplyTheme(const FProjectVisibleUITheme& Theme);

	UFUNCTION(BlueprintCallable, Category = "Project Visible UI")
	virtual void OnThemeChanged(const FProjectVisibleUITheme& NewTheme);

	// Screen Management
	UFUNCTION(BlueprintCallable, Category = "Project Visible UI")
	void NavigateToScreen(EProjectVisibleScreenType TargetScreen);

	UFUNCTION(BlueprintCallable, Category = "Project Visible UI")
	void ShowModal(EProjectVisibleScreenType ModalType);

	UFUNCTION(BlueprintCallable, Category = "Project Visible UI")
	void HideModal();

	// Accessibility
	UFUNCTION(BlueprintCallable, Category = "Project Visible UI")
	virtual void ApplyAccessibilitySettings(const FAccessibilityConfig& Config);

	UFUNCTION(BlueprintCallable, Category = "Project Visible UI")
	void SetTextSizeMultiplier(float Multiplier);

	UFUNCTION(BlueprintCallable, Category = "Project Visible UI")
	void SetHighContrastMode(bool bEnabled);

	// Animation
	UFUNCTION(BlueprintCallable, Category = "Project Visible UI")
	void PlayUIAnimation(EUIAnimationType AnimationType, float Duration = 0.3f);

	// Data Binding (Project Visible specific)
	UFUNCTION(BlueprintCallable, Category = "Project Visible UI")
	void BindToVirtueSystem();

	UFUNCTION(BlueprintCallable, Category = "Project Visible UI")
	void BindToMemorySystem();

	UFUNCTION(BlueprintCallable, Category = "Project Visible UI")
	void BindToInvestigationSystem();

	// Events - BlueprintImplementableEvent for UMG implementation
	UFUNCTION(BlueprintImplementableEvent, Category = "Project Visible UI")
	void OnWidgetInitialized();

	UFUNCTION(BlueprintImplementableEvent, Category = "Project Visible UI")
	void OnDataUpdated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Project Visible UI")
	void OnThemeApplied(const FProjectVisibleUITheme& Theme);

	UFUNCTION(BlueprintImplementableEvent, Category = "Project Visible UI")
	void OnAnimationTriggered(EUIAnimationType AnimationType);

protected:
	// Widget Properties
	UPROPERTY(BlueprintReadOnly, Category = "Project Visible UI")
	EProjectVisibleScreenType ScreenType;

	UPROPERTY(BlueprintReadOnly, Category = "Project Visible UI")
	FProjectVisibleUITheme CurrentTheme;

	UPROPERTY(BlueprintReadOnly, Category = "Project Visible UI")
	FAccessibilityConfig AccessibilityConfig;

	UPROPERTY(BlueprintReadOnly, Category = "Project Visible UI")
	bool bIsInitialized = false;

	UPROPERTY(BlueprintReadOnly, Category = "Project Visible UI")
	bool bIsDataBound = false;

	// System References
	UPROPERTY()
	TWeakObjectPtr<UProjectVisibleUIManager> UIManager;

	// Helper Functions
	UFUNCTION(BlueprintCallable, Category = "Project Visible UI", BlueprintPure)
	UProjectVisibleUIManager* GetUIManager() const;

	UFUNCTION(BlueprintCallable, Category = "Project Visible UI")
	void SetupSystemBindings();

	UFUNCTION(BlueprintCallable, Category = "Project Visible UI")
	virtual void OnSystemDataChanged();

	// Data Update Events
	UFUNCTION()
	virtual void OnVirtueDataChanged();

	UFUNCTION()
	virtual void OnMemoryDataChanged();

	UFUNCTION()
	virtual void OnInvestigationDataChanged();

private:
	// Internal state
	FTimerHandle RefreshTimer;
	bool bNeedsRefresh = false;
	float LastUpdateTime = 0.0f;

	// Helper functions
	void ScheduleRefresh();
	void InternalRefresh();
};
