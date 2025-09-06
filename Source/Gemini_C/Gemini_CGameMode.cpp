// Project Visible - Main Game Mode Implementation
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Gemini_CGameMode.h"
#include "Gemini_C.h"
#include "Gemini_CCharacter.h"
#include "LandscapeOfMindManager.h"
#include "VirtueManager.h"
#include "MemoryManager.h"
#include "RealityInvestigationManager.h"
#include "BoundaryDissolutionManager.h"
#include "SocialExperimentManager.h"
#include "ProjectVisibleUIManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AGemini_CGameMode::AGemini_CGameMode()
{
	// Set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// Enable ticking for this game mode
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Initialize game state
	CurrentGamePhase = ELandscapePhase::Exploration;
	bIsInDreamMode = false;
	BoundaryDissolutionLevel = 0.0f;

	// Set up default landscape configuration
	DefaultLandscapeConfig = FMentalLandscapeConfig();

	UE_LOG(LogProjectVisible, Log, TEXT("Project Visible Game Mode created"));
}

void AGemini_CGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogProjectVisible, Log, TEXT("Project Visible Game Mode beginning play"));

	// Initialize Project Visible systems
	InitializeProjectVisible();

	// Set up periodic updates
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();

		// Game phase update timer
		TimerManager.SetTimer(GamePhaseUpdateTimer, [this]()
		{
			UpdateGamePhase();
		}, GAME_PHASE_UPDATE_INTERVAL, true);

		// Boundary effect update timer
		TimerManager.SetTimer(BoundaryEffectTimer, [this]()
		{
			ProcessBoundaryEffects(BOUNDARY_EFFECT_UPDATE_INTERVAL);
		}, BOUNDARY_EFFECT_UPDATE_INTERVAL, true);
	}
}

void AGemini_CGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update any frame-dependent systems
	HandleSystemIntegration();
}

void AGemini_CGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Project Visible Game Mode ending play"));

	// Clear timers
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.ClearTimer(GamePhaseUpdateTimer);
		TimerManager.ClearTimer(BoundaryEffectTimer);
	}

	Super::EndPlay(EndPlayReason);
}

void AGemini_CGameMode::InitializeProjectVisible()
{
	UE_LOG(LogProjectVisible, Log, TEXT("Initializing Project Visible systems"));

	// Initialize Landscape of Mind Manager
	if (ULandscapeOfMindManager* LandscapeManager = GetLandscapeManager())
	{
		LandscapeManager->InitializeMentalLandscape(DefaultLandscapeConfig);
		LandscapeManager->OnStabilityChanged.AddDynamic(this, &AGemini_CGameMode::OnLandscapeStabilized);
		UE_LOG(LogProjectVisible, Log, TEXT("Landscape of Mind Manager initialized"));
	}
	else
	{
		UE_LOG(LogProjectVisible, Error, TEXT("Failed to get Landscape of Mind Manager"));
	}
	
	// Initialize Virtue Manager
	if (UVirtueManager* VirtueManager = GetVirtueManager())
	{
		VirtueManager->InitializeVirtues();
		UE_LOG(LogProjectVisible, Log, TEXT("Virtue Manager initialized"));
	}
	else
	{
		UE_LOG(LogProjectVisible, Error, TEXT("Failed to get Virtue Manager"));
	}
	
	// Initialize Memory Manager
	if (UMemoryManager* MemoryManager = GetMemoryManager())
	{
		FMemoryCapacityConfig DefaultConfig;
		MemoryManager->InitializeMemorySystem(DefaultConfig);
		UE_LOG(LogProjectVisible, Log, TEXT("Memory Manager initialized"));
	}
	else
	{
		UE_LOG(LogProjectVisible, Error, TEXT("Failed to get Memory Manager"));
	}
	
	// Initialize Reality Investigation Manager
	if (URealityInvestigationManager* RealityManager = GetRealityInvestigationManager())
	{
		UE_LOG(LogProjectVisible, Log, TEXT("Reality Investigation Manager initialized"));
	}
	else
	{
		UE_LOG(LogProjectVisible, Error, TEXT("Failed to get Reality Investigation Manager"));
	}
	
	// Initialize Boundary Dissolution Manager
	if (UBoundaryDissolutionManager* BoundaryManager = GetBoundaryDissolutionManager())
	{
		UE_LOG(LogProjectVisible, Log, TEXT("Boundary Dissolution Manager initialized"));
	}
	else
	{
		UE_LOG(LogProjectVisible, Error, TEXT("Failed to get Boundary Dissolution Manager"));
	}
	
	// Initialize Social Experiment Manager
	if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
	{
		FDataPrivacyConfig PrivacyConfig;
		FAnalyticsConfig AnalyticsConfig;
		ExperimentManager->InitializeExperimentSystem(PrivacyConfig, AnalyticsConfig);
		
		// Register current participant
		FString ParticipantID = ExperimentManager->RegisterParticipant(25, TEXT("Unknown"), TEXT("Unknown"));
		ExperimentManager->RequestConsent(ParticipantID, TEXT("Gameplay Analysis"), TEXT("Anonymous behavioral data"));
		
		UE_LOG(LogProjectVisible, Log, TEXT("Social Experiment Manager initialized - Participant: %s"), *ParticipantID);
	}
	else
	{
		UE_LOG(LogProjectVisible, Error, TEXT("Failed to get Social Experiment Manager"));
	}
	
	// Initialize UI Manager
	if (UProjectVisibleUIManager* UIManager = GetUIManager())
	{
		UIManager->InitializeUISystem();
		
		// Set up default theme
		FProjectVisibleUITheme DefaultTheme;
		UIManager->ApplyTheme(DefaultTheme);
		
		// Enable accessibility features
		FAccessibilityConfig AccessibilityConfig;
		AccessibilityConfig.bEnableAccessibility = true;
		UIManager->SetAccessibilityConfig(AccessibilityConfig);
		
		// Show main menu
		UIManager->PushScreen(EProjectVisibleScreenType::MainMenu, false);
		
		UE_LOG(LogProjectVisible, Log, TEXT("UI Manager initialized with CommonUI"));
	}
	else
	{
		UE_LOG(LogProjectVisible, Error, TEXT("Failed to get UI Manager"));
	}

	UE_LOG(LogProjectVisible, Log, TEXT("Project Visible systems initialized - All systems operational"));
}

void AGemini_CGameMode::SwitchToRealityMode()
{
	if (bIsInDreamMode)
	{
		UE_LOG(LogProjectVisible, Log, TEXT("Switching to Reality Mode"));

		bIsInDreamMode = false;
		BoundaryDissolutionLevel = FMath::Max(0.0f, BoundaryDissolutionLevel - 0.1f);

		// Update UI to reality mode
		if (UProjectVisibleUIManager* UIManager = GetUIManager())
		{
			UIManager->PushScreen(EProjectVisibleScreenType::RealityMode);
			UIManager->UpdateInvestigationDisplayData();
		}

		OnModeSwitch.Broadcast(false);

		// TODO: Trigger reality mode specific changes
		// - Hide dream landscape
		// - Show investigation interface
		// - Change audio/visual style
	}
}

void AGemini_CGameMode::SwitchToDreamMode()
{
	if (!bIsInDreamMode)
	{
		UE_LOG(LogProjectVisible, Log, TEXT("Switching to Dream Mode"));

		bIsInDreamMode = true;

		OnModeSwitch.Broadcast(true);

		// TODO: Trigger dream mode specific changes
		// - Show landscape of mind
		// - Hide investigation interface
		// - Change audio/visual style
	}
}

void AGemini_CGameMode::TriggerBoundaryDissolution(float IntensityLevel)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Triggering boundary dissolution with intensity %f"), IntensityLevel);

	BoundaryDissolutionLevel = FMath::Clamp(IntensityLevel, 0.0f, 1.0f);

	OnBoundaryDissolution.Broadcast(BoundaryDissolutionLevel);

	// TODO: Apply boundary dissolution effects
	// - Blend visual elements between reality and dream
	// - Mix audio layers
	// - Create transition effects
}

ULandscapeOfMindManager* AGemini_CGameMode::GetLandscapeManager() const
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<ULandscapeOfMindManager>();
		}
	}
	return nullptr;
}

UVirtueManager* AGemini_CGameMode::GetVirtueManager() const
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UVirtueManager>();
		}
	}
	return nullptr;
}

UMemoryManager* AGemini_CGameMode::GetMemoryManager() const
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UMemoryManager>();
		}
	}
	return nullptr;
}

URealityInvestigationManager* AGemini_CGameMode::GetRealityInvestigationManager() const
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<URealityInvestigationManager>();
		}
	}
	return nullptr;
}

UBoundaryDissolutionManager* AGemini_CGameMode::GetBoundaryDissolutionManager() const
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UBoundaryDissolutionManager>();
		}
	}
	return nullptr;
}

USocialExperimentManager* AGemini_CGameMode::GetSocialExperimentManager() const
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<USocialExperimentManager>();
		}
	}
	return nullptr;
}

UProjectVisibleUIManager* AGemini_CGameMode::GetUIManager() const
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UProjectVisibleUIManager>();
		}
	}
	return nullptr;
}

void AGemini_CGameMode::OnPlayerDiscoveryMade(const FString& DiscoveryType, const FString& Evidence)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Player discovery made - Type: %s, Evidence: %s"), *DiscoveryType, *Evidence);

	// TODO: Process discovery for:
	// - Story progression
	// - Social experiment data collection
	// - Boundary dissolution triggers
	// - Virtue system updates
}

void AGemini_CGameMode::OnLandscapeStabilized(float OldStability, float NewStability)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Landscape stability changed from %f to %f"), OldStability, NewStability);

	// Check if player has achieved significant stabilization
	if (NewStability > 75.0f && OldStability <= 75.0f)
	{
		// Trigger major story progression
		UE_LOG(LogProjectVisible, Log, TEXT("High landscape stability achieved - triggering story progression"));

        // TODO: Trigger boundary dissolution event
        this->TriggerBoundaryDissolution(0.3f);
	}

	// Update game phase based on stability
	this->UpdateGamePhase();
}

void AGemini_CGameMode::OnExperimentCompleted(const FString& ExperimentType)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Experiment completed: %s"), *ExperimentType);

	// TODO: Handle experiment completion
	// - Update player profile
	// - Trigger story events
	// - Adjust difficulty
}

void AGemini_CGameMode::OnVirtueActionPerformed(const FString& ActionType, const FString& VirtueType, bool bIsPositive)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Virtue action performed - Type: %s, Virtue: %s, Positive: %s"), 
		   *ActionType, *VirtueType, bIsPositive ? TEXT("Yes") : TEXT("No"));
	
	// Create memory of virtue action
	if (UMemoryManager* MemoryManager = GetMemoryManager())
	{
		FString MemoryContent = FString::Printf(TEXT("Performed %s action related to %s"), *ActionType, *VirtueType);
		MemoryManager->CreateMemory(
			FString::Printf(TEXT("Virtue Action: %s"), *ActionType),
			MemoryContent,
			EMemoryType::Moral,
			bIsPositive ? EMemoryImportance::Medium : EMemoryImportance::Low,
			bIsPositive ? 70.0f : 30.0f
		);
	}
	
	// Record happiness impact
	if (UMemoryManager* MemoryManager = GetMemoryManager())
	{
		float HappinessImpact = bIsPositive ? 2.0f : -1.5f;
		MemoryManager->RecordHappinessEvent(
			FString::Printf(TEXT("Virtue: %s"), *VirtueType),
			HappinessImpact,
			1.0f
		);
	}
}

void AGemini_CGameMode::OnMemoryCreated(const FString& MemoryTitle, const FString& MemoryType, float EmotionalIntensity)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Memory created - Title: %s, Type: %s, Intensity: %.1f"), 
		   *MemoryTitle, *MemoryType, EmotionalIntensity);
	
	// High emotional intensity memories might trigger boundary dissolution
	if (EmotionalIntensity > 80.0f)
	{
		UE_LOG(LogProjectVisible, Log, TEXT("High emotional intensity memory - triggering boundary effects"));
		this->TriggerBoundaryDissolution(0.2f);
	}
}

void AGemini_CGameMode::OnHappinessChanged(float NewHappinessLevel)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Player happiness changed to: %.1f"), NewHappinessLevel);
	
	// Record happiness data in social experiment
	if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
	{
		// Find current participant
		TArray<FSocialExperiment> ActiveExperiments = ExperimentManager->GetActiveExperiments();
		if (ActiveExperiments.Num() > 0)
		{
			ExperimentManager->RecordEmotionalResponse(
				ActiveExperiments[0].ExperimentID,
				TEXT("CURRENT_PLAYER"), // TODO: Get actual participant ID
				TEXT("HappinessChange"),
				NewHappinessLevel,
				NewHappinessLevel > 50.0f ? TEXT("Positive") : TEXT("Negative")
			);
		}
	}
	
	// Happiness affects game world
	if (NewHappinessLevel > 75.0f)
	{
		// High happiness - enhance positive effects
		if (ULandscapeOfMindManager* LandscapeManager = GetLandscapeManager())
		{
			LandscapeManager->PlaceNodeStone(FVector(0, 0, 100), ENodeStoneType::Harmonizer);
		}
	}
	else if (NewHappinessLevel < 25.0f)
	{
		// Low happiness - increase challenges
		if (ULandscapeOfMindManager* LandscapeManager = GetLandscapeManager())
		{
			LandscapeManager->SpawnNoiseCreatures(FVector(0, 0, 0), 2);
		}
	}
}

void AGemini_CGameMode::OnSocialExperimentTriggered(const FString& ExperimentType, const FString& Context)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Social experiment triggered: %s in context: %s"), *ExperimentType, *Context);
	
	if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
	{
		// Convert string to enum
		ESocialExperimentType Type = ESocialExperimentType::MoralDilemma; // Default
		if (ExperimentType.Contains(TEXT("Trust")))
		{
			Type = ESocialExperimentType::TrustTest;
		}
		else if (ExperimentType.Contains(TEXT("Altruism")))
		{
			Type = ESocialExperimentType::AltruismMeasure;
		}
		else if (ExperimentType.Contains(TEXT("Risk")))
		{
			Type = ESocialExperimentType::RiskTaking;
		}
		
		ExperimentManager->TriggerHiddenExperiment(Type, Context);
	}
}

void AGemini_CGameMode::OnPlayerBehaviorObserved(const FString& BehaviorType, const FString& Response, float Intensity)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Player behavior observed: %s - %s (Intensity: %.2f)"), 
		   *BehaviorType, *Response, Intensity);
	
	if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
	{
		// Record implicit behavior
		ExperimentManager->RecordImplicitBehavior(BehaviorType, Intensity, Response);
		
		// Also create memory of significant behaviors
		if (Intensity > 70.0f)
		{
			if (UMemoryManager* MemoryManager = GetMemoryManager())
			{
				MemoryManager->CreateMemory(
					FString::Printf(TEXT("Behavior: %s"), *BehaviorType),
					FString::Printf(TEXT("Displayed %s behavior: %s"), *BehaviorType, *Response),
					EMemoryType::Social,
					Intensity > 85.0f ? EMemoryImportance::High : EMemoryImportance::Medium,
					Intensity
				);
			}
		}
	}
}

// Private Methods

void AGemini_CGameMode::UpdateGamePhase()
{
	if (ULandscapeOfMindManager* LandscapeManager = GetLandscapeManager())
	{
		const FMentalLandscapeState& LandscapeState = LandscapeManager->GetCurrentState();
		ELandscapePhase NewPhase = LandscapeState.CurrentPhase;

		if (NewPhase != CurrentGamePhase)
		{
			UE_LOG(LogProjectVisible, Log, TEXT("Game phase changed from %d to %d"), 
				   (int32)CurrentGamePhase, (int32)NewPhase);

			CurrentGamePhase = NewPhase;
			OnGamePhaseChanged.Broadcast(CurrentGamePhase);

			// Handle phase-specific changes
			switch (CurrentGamePhase)
			{
				case ELandscapePhase::Exploration:
					// Enable exploration mechanics
					break;
				case ELandscapePhase::Observation:
					// Enable quantum observation mechanics
					break;
				case ELandscapePhase::Defense:
					// Enable creature defense mechanics
					break;
				case ELandscapePhase::Completion:
					// Trigger completion events
					break;
			}
		}
	}
}

void AGemini_CGameMode::ProcessBoundaryEffects(float DeltaTime)
{
	if (BoundaryDissolutionLevel > 0.0f)
	{
		// Update boundary dissolution effects
		// TODO: Apply visual and audio blending based on dissolution level

		// Gradually reduce dissolution over time if not actively triggered
		if (BoundaryDissolutionLevel > 0.1f)
		{
			BoundaryDissolutionLevel -= 0.01f * DeltaTime;
			BoundaryDissolutionLevel = FMath::Max(0.0f, BoundaryDissolutionLevel);
		}
	}
}

void AGemini_CGameMode::HandleSystemIntegration()
{
	// Handle real-time integration between different systems
	// TODO: Coordinate between:
	// - Landscape Manager
	// - Social Experiment Manager
	// - Virtue System
	// - Memory System
	// - Boundary Dissolution Manager
}
