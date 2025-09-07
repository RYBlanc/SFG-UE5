// Project Visible - Reality Mode Widget Implementation
// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectVisibleRealityWidget.h"
#include "ProjectVisibleUIManager.h"
#include "RealityInvestigationManager.h"
#include "VirtueManager.h"
#include "Gemini_CGameMode.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Gemini_C.h"

UProjectVisibleRealityWidget::UProjectVisibleRealityWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ScreenType = EProjectVisibleScreenType::RealityMode;
}

void UProjectVisibleRealityWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Get system references
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			InvestigationManager = GameInstance->GetSubsystem<URealityInvestigationManager>();
			VirtueManager = GameInstance->GetSubsystem<UVirtueManager>();
		}
	}
	
	UE_LOG(LogProjectVisible, Log, TEXT("Reality Mode Widget constructed"));
}

void UProjectVisibleRealityWidget::InitializeWidget()
{
	Super::InitializeWidget();
	
	// Initialize with current data
	RefreshInvestigationData();
	RefreshVirtueData();
	RefreshLocationData();
	
	UE_LOG(LogProjectVisible, Log, TEXT("Reality Mode Widget initialized"));
}

void UProjectVisibleRealityWidget::SetupSystemBindings()
{
	Super::SetupSystemBindings();
	
	// Bind to Investigation Manager events (commented out - delegates not found)
	if (InvestigationManager.IsValid())
	{
		// InvestigationManager->OnEvidenceCollected.AddDynamic(this, &UProjectVisibleRealityWidget::OnEvidenceCollected);
		// InvestigationManager->OnHypothesisFormed.AddDynamic(this, &UProjectVisibleRealityWidget::OnHypothesisFormed);
	}
	
	// Bind to Virtue Manager events (commented out - delegate signature mismatch)
	if (VirtueManager.IsValid())
	{
		// VirtueManager->OnVirtueChanged.AddDynamic(this, &UProjectVisibleRealityWidget::OnVirtueChanged);
	}
	
	UE_LOG(LogProjectVisible, Log, TEXT("Reality Mode Widget system bindings set up"));
}

void UProjectVisibleRealityWidget::OnSystemDataChanged()
{
	Super::OnSystemDataChanged();
	
	// Refresh all data
	RefreshInvestigationData();
	RefreshVirtueData();
	RefreshLocationData();
}

void UProjectVisibleRealityWidget::OpenInvestigationBoard()
{
	if (!bInvestigationBoardOpen)
	{
		bInvestigationBoardOpen = true;
		NavigateToScreen(EProjectVisibleScreenType::Investigation);
		
		UE_LOG(LogProjectVisible, Log, TEXT("Investigation board opened"));
	}
}

void UProjectVisibleRealityWidget::CloseInvestigationBoard()
{
	if (bInvestigationBoardOpen)
	{
		bInvestigationBoardOpen = false;
		NavigateToScreen(EProjectVisibleScreenType::RealityMode);
		
		UE_LOG(LogProjectVisible, Log, TEXT("Investigation board closed"));
	}
}

void UProjectVisibleRealityWidget::ShowEvidence(int32 EvidenceID)
{
	if (InvestigationManager.IsValid())
	{
		// Get evidence data and display it
		// This would be implemented with the investigation system
		UE_LOG(LogProjectVisible, Log, TEXT("Showing evidence ID: %d"), EvidenceID);
	}
}

void UProjectVisibleRealityWidget::ShowHypothesis(int32 HypothesisID)
{
	if (InvestigationManager.IsValid())
	{
		// Get hypothesis data and display it
		UE_LOG(LogProjectVisible, Log, TEXT("Showing hypothesis ID: %d"), HypothesisID);
	}
}

void UProjectVisibleRealityWidget::OpenLocationMap()
{
	if (!bLocationMapOpen)
	{
		bLocationMapOpen = true;
		// Show location selection UI
		UE_LOG(LogProjectVisible, Log, TEXT("Location map opened"));
	}
}

void UProjectVisibleRealityWidget::TravelToLocation(int32 LocationID)
{
	if (InvestigationManager.IsValid())
	{
		// InvestigationManager->TravelToLocation(LocationID, ETravelMethod::Physical);
		bLocationMapOpen = false;
		
		UE_LOG(LogProjectVisible, Log, TEXT("Traveling to location ID: %d"), LocationID);
	}
}

void UProjectVisibleRealityWidget::OpenVirtueProfile()
{
	NavigateToScreen(EProjectVisibleScreenType::VirtueProfile);
	UE_LOG(LogProjectVisible, Log, TEXT("Virtue profile opened"));
}

void UProjectVisibleRealityWidget::OpenMemoryViewer()
{
	NavigateToScreen(EProjectVisibleScreenType::MemoryViewer);
	UE_LOG(LogProjectVisible, Log, TEXT("Memory viewer opened"));
}

void UProjectVisibleRealityWidget::OpenSettings()
{
	ShowModal(EProjectVisibleScreenType::Settings);
	UE_LOG(LogProjectVisible, Log, TEXT("Settings opened"));
}

void UProjectVisibleRealityWidget::SwitchToDreamMode()
{
	if (UWorld* World = GetWorld())
	{
		if (AGemini_CGameMode* GameMode = World->GetAuthGameMode<AGemini_CGameMode>())
		{
			GameMode->SwitchToDreamMode();
			NavigateToScreen(EProjectVisibleScreenType::DreamMode);
			
			UE_LOG(LogProjectVisible, Log, TEXT("Switching to Dream Mode"));
		}
	}
}

void UProjectVisibleRealityWidget::PauseGame()
{
	ShowModal(EProjectVisibleScreenType::Pause);
	UE_LOG(LogProjectVisible, Log, TEXT("Game paused"));
}

void UProjectVisibleRealityWidget::OnEvidenceCollected(const FEvidenceItem& Evidence)
{
	// Add to local cache
	CollectedEvidence.Add(Evidence);
	
	// Update UI
	UpdateEvidenceCount(CollectedEvidence.Num(), CollectedEvidence.Num()); // Simplified for now
	ShowNewEvidenceNotification(Evidence);
	
	// Update progress
	UpdateProgressCalculations();
	
	UE_LOG(LogProjectVisible, Log, TEXT("Evidence collected: %s"), *Evidence.EvidenceName);
}

void UProjectVisibleRealityWidget::OnHypothesisFormed(const FString& Hypothesis)
{
	// Add to local cache
	FormedHypotheses.Add(Hypothesis);
	
	// Update UI
	ShowHypothesisUpdate(Hypothesis);
	
	// Update progress
	UpdateProgressCalculations();
	
	UE_LOG(LogProjectVisible, Log, TEXT("Hypothesis formed: %s"), *Hypothesis);
}

void UProjectVisibleRealityWidget::OnVirtueChanged(EVirtueType VirtueType, float NewLevel)
{
	// Update virtue state (simplified)
	// In a full implementation, this would update the complete virtue state
	RefreshVirtueData();
	
	UE_LOG(LogProjectVisible, Log, TEXT("Virtue changed: %s -> %.1f"), *UEnum::GetValueAsString(VirtueType), NewLevel);
}

void UProjectVisibleRealityWidget::OnLocationChanged(int32 NewLocation)
{
	CurrentLocation = NewLocation;
	RefreshLocationData();
	
	UE_LOG(LogProjectVisible, Log, TEXT("Location changed to ID: %d"), NewLocation);
}

void UProjectVisibleRealityWidget::RefreshInvestigationData()
{
	if (InvestigationManager.IsValid())
	{
		// Get current investigation progress (commented out - method not found)
		// InvestigationProgress = InvestigationManager->GetInvestigationProgress();
		InvestigationProgress = 25.0f; // Placeholder value
		UpdateInvestigationProgress(InvestigationProgress);
		
		UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Investigation data refreshed - Progress: %.1f%%"), InvestigationProgress);
	}
}

void UProjectVisibleRealityWidget::RefreshVirtueData()
{
	if (VirtueManager.IsValid())
	{
		// Get current virtue data (simplified)
		// In a full implementation, this would get the complete virtue data
		FVirtueData VirtueData;
		// VirtueData would be populated from VirtueManager
		UpdateVirtueDisplay(VirtueData);
		
		UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Virtue data refreshed"));
	}
}

void UProjectVisibleRealityWidget::RefreshLocationData()
{
	if (InvestigationManager.IsValid())
	{
		// Get available locations
		TArray<int32> AvailableLocations;
		// This would be populated from the investigation manager
		UpdateLocationInfo(CurrentLocation, AvailableLocations);
		
		UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Location data refreshed - Current ID: %d"), CurrentLocation);
	}
}

void UProjectVisibleRealityWidget::UpdateProgressCalculations()
{
	// Calculate overall investigation progress based on evidence and hypotheses
	float EvidenceWeight = 0.6f;
	float HypothesisWeight = 0.4f;
	
	float EvidenceProgress = CollectedEvidence.Num() * 10.0f; // Simplified calculation
	float HypothesisProgress = FormedHypotheses.Num() * 15.0f; // Simplified calculation
	
	InvestigationProgress = FMath::Clamp((EvidenceProgress * EvidenceWeight) + (HypothesisProgress * HypothesisWeight), 0.0f, 100.0f);
	
	UpdateInvestigationProgress(InvestigationProgress);
}
