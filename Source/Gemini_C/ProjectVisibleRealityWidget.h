// Project Visible - Reality Mode Widget
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectVisibleBaseWidget.h"
#include "RealityInvestigationTypes.h"
#include "VirtueMemoryTypes.h"
#include "ProjectVisibleRealityWidget.generated.h"

class URealityInvestigationManager;
class UVirtueManager;

/**
 * Main widget for Reality Mode - handles investigation UI, virtue display, and game controls
 */
UCLASS(BlueprintType, Blueprintable)
class GEMINI_C_API UProjectVisibleRealityWidget : public UProjectVisibleBaseWidget
{
	GENERATED_BODY()

public:
	UProjectVisibleRealityWidget(const FObjectInitializer& ObjectInitializer);

protected:
	// UProjectVisibleBaseWidget interface
	virtual void NativeConstruct() override;
	virtual void InitializeWidget() override;
	virtual void SetupSystemBindings();
	virtual void OnSystemDataChanged() override;

public:
	// Investigation Controls
	UFUNCTION(BlueprintCallable, Category = "Reality Mode")
	void OpenInvestigationBoard();

	UFUNCTION(BlueprintCallable, Category = "Reality Mode")
	void CloseInvestigationBoard();

	UFUNCTION(BlueprintCallable, Category = "Reality Mode")
	void ShowEvidence(int32 EvidenceID);

	UFUNCTION(BlueprintCallable, Category = "Reality Mode")
	void ShowHypothesis(int32 HypothesisID);

	// Navigation Controls
	UFUNCTION(BlueprintCallable, Category = "Reality Mode")
	void OpenLocationMap();

	UFUNCTION(BlueprintCallable, Category = "Reality Mode")
	void TravelToLocation(int32 LocationID);

	// System Access
	UFUNCTION(BlueprintCallable, Category = "Reality Mode")
	void OpenVirtueProfile();

	UFUNCTION(BlueprintCallable, Category = "Reality Mode")
	void OpenMemoryViewer();

	UFUNCTION(BlueprintCallable, Category = "Reality Mode")
	void OpenSettings();

	// Game Mode Controls
	UFUNCTION(BlueprintCallable, Category = "Reality Mode")
	void SwitchToDreamMode();

	UFUNCTION(BlueprintCallable, Category = "Reality Mode")
	void PauseGame();

	// Data Display Updates - BlueprintImplementableEvent for UMG
	UFUNCTION(BlueprintImplementableEvent, Category = "Reality Mode")
	void UpdateInvestigationProgress(float ProgressPercentage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Reality Mode")
	void UpdateVirtueDisplay(const FVirtueData& VirtueData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Reality Mode")
	void UpdateLocationInfo(int32 LocationID, const TArray<int32>& AvailableLocations);

	UFUNCTION(BlueprintImplementableEvent, Category = "Reality Mode")
	void UpdateEvidenceCount(int32 TotalEvidence, int32 AnalyzedEvidence);

	UFUNCTION(BlueprintImplementableEvent, Category = "Reality Mode")
	void ShowNewEvidenceNotification(const FEvidenceItem& Evidence);

	UFUNCTION(BlueprintImplementableEvent, Category = "Reality Mode")
	void ShowHypothesisUpdate(const FString& Hypothesis);

	// Interaction Feedback
	UFUNCTION(BlueprintImplementableEvent, Category = "Reality Mode")
	void ShowInteractionPrompt(const FString& PromptText, const FString& ActionText);

	UFUNCTION(BlueprintImplementableEvent, Category = "Reality Mode")
	void HideInteractionPrompt();

	UFUNCTION(BlueprintImplementableEvent, Category = "Reality Mode")
	void ShowDialogueOptions(const TArray<FString>& Options);

	UFUNCTION(BlueprintImplementableEvent, Category = "Reality Mode")
	void HideDialogueOptions();

protected:
	// UI State
	UPROPERTY(BlueprintReadOnly, Category = "Reality Mode")
	bool bInvestigationBoardOpen = false;

	UPROPERTY(BlueprintReadOnly, Category = "Reality Mode")
	bool bLocationMapOpen = false;

	UPROPERTY(BlueprintReadOnly, Category = "Reality Mode")
	bool bDialogueActive = false;

	UPROPERTY(BlueprintReadOnly, Category = "Reality Mode")
	int32 CurrentLocation = 0;

	// Investigation Data
	UPROPERTY(BlueprintReadOnly, Category = "Reality Mode")
	TArray<FEvidenceItem> CollectedEvidence;

	UPROPERTY(BlueprintReadOnly, Category = "Reality Mode")
	TArray<FString> FormedHypotheses;

	UPROPERTY(BlueprintReadOnly, Category = "Reality Mode")
	float InvestigationProgress = 0.0f;

	// Virtue Data
	UPROPERTY(BlueprintReadOnly, Category = "Reality Mode")
	FVirtueData CurrentVirtueData;

	// System References
	UPROPERTY()
	TWeakObjectPtr<URealityInvestigationManager> InvestigationManager;

	UPROPERTY()
	TWeakObjectPtr<UVirtueManager> VirtueManager;

	// Event Handlers
	UFUNCTION()
	void OnEvidenceCollected(const FEvidenceItem& Evidence);

	UFUNCTION()
	void OnHypothesisFormed(const FString& Hypothesis);

	UFUNCTION()
	void OnVirtueChanged(EVirtueType VirtueType, float NewLevel);

	UFUNCTION()
	void OnLocationChanged(int32 NewLocation);

private:
	// Helper functions
	void RefreshInvestigationData();
	void RefreshVirtueData();
	void RefreshLocationData();
	void UpdateProgressCalculations();
};
