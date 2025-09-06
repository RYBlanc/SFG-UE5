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
#include "StoryManager.h"
#include "LevelDesignManager.h"
#include "CharacterManager.h"
#include "GameProgressionManager.h"
#include "AudioSystemManager.h"
#include "PerformanceMonitoringManager.h"
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
	
	// Initialize Story Manager
	if (UStoryManager* StoryManager = GetStoryManager())
	{
		StoryManager->InitializeStorySystem();
		
		// Start the story from the introduction
		StoryManager->StartChapter(1);
		
		// Register for story events
		StoryManager->OnPhaseChanged.AddDynamic(this, &AGemini_CGameMode::OnStoryPhaseChanged);
		StoryManager->OnChapterCompleted.AddDynamic(this, &AGemini_CGameMode::OnStoryChapterCompleted);
		
		UE_LOG(LogProjectVisible, Log, TEXT("Story Manager initialized - Starting Chapter 1"));
	}
	else
	{
		UE_LOG(LogProjectVisible, Error, TEXT("Failed to get Story Manager"));
	}
	
	// Initialize Level Design Manager
	if (ULevelDesignManager* LevelManager = GetLevelDesignManager())
	{
		LevelManager->InitializeLevelSystem();
		
		// Load the initial level (Tokyo Main)
		LevelManager->LoadLevel(TEXT("TOKYO_MAIN"));
		
		// Register for level events
		LevelManager->OnLevelLoaded.AddDynamic(this, &AGemini_CGameMode::OnLevelLoaded);
		LevelManager->OnLevelCompleted.AddDynamic(this, &AGemini_CGameMode::OnLevelCompleted);
		LevelManager->OnLevelTransition.AddDynamic(this, &AGemini_CGameMode::OnLevelTransition);
		
		UE_LOG(LogProjectVisible, Log, TEXT("Level Design Manager initialized - Loaded Tokyo Main"));
	}
	else
	{
		UE_LOG(LogProjectVisible, Error, TEXT("Failed to get Level Design Manager"));
	}
	
	// Initialize Character Manager
	if (UCharacterManager* CharacterManager = GetCharacterManager())
	{
		CharacterManager->InitializeCharacterSystem();
		
		// Register for character events
		CharacterManager->OnCharacterRegistered.AddDynamic(this, &AGemini_CGameMode::OnCharacterRegistered);
		CharacterManager->OnDialogueStarted.AddDynamic(this, &AGemini_CGameMode::OnDialogueStarted);
		CharacterManager->OnDialogueEnded.AddDynamic(this, &AGemini_CGameMode::OnDialogueEnded);
		CharacterManager->OnTrustChanged.AddDynamic(this, &AGemini_CGameMode::OnCharacterTrustChanged);
		CharacterManager->OnInformationRevealed.AddDynamic(this, &AGemini_CGameMode::OnInformationRevealed);
		
		UE_LOG(LogProjectVisible, Log, TEXT("Character Manager initialized - %d characters registered"), 
			   CharacterManager->GetAllCharacters().Num());
	}
	else
	{
		UE_LOG(LogProjectVisible, Error, TEXT("Failed to get Character Manager"));
	}
	
	// Initialize Game Progression Manager
	if (UGameProgressionManager* ProgressionManager = GetGameProgressionManager())
	{
		ProgressionManager->InitializeProgressionSystem();
		
		// Start new game session
		ProgressionManager->StartNewSession();
		
		// Register for progression events
		ProgressionManager->OnGameSaved.AddDynamic(this, &AGemini_CGameMode::OnGameSaved);
		ProgressionManager->OnGameLoaded.AddDynamic(this, &AGemini_CGameMode::OnGameLoaded);
		ProgressionManager->OnCheckpointReached.AddDynamic(this, &AGemini_CGameMode::OnCheckpointReached);
		ProgressionManager->OnAchievementUnlocked.AddDynamic(this, &AGemini_CGameMode::OnAchievementUnlocked);
		ProgressionManager->OnProgressUpdated.AddDynamic(this, &AGemini_CGameMode::OnProgressUpdated);
		
		UE_LOG(LogProjectVisible, Log, TEXT("Game Progression Manager initialized - Auto-save: %s"), 
			   ProgressionManager->IsAutoSaveEnabled() ? TEXT("Enabled") : TEXT("Disabled"));
	}
	else
	{
		UE_LOG(LogProjectVisible, Error, TEXT("Failed to get Game Progression Manager"));
	}
	
	// Initialize Audio System Manager
	if (UAudioSystemManager* AudioManager = GetAudioSystemManager())
	{
		AudioManager->InitializeAudioSystem();
		
		// Register for audio events
		AudioManager->OnMusicTrackChanged.AddDynamic(this, &AGemini_CGameMode::OnMusicTrackChanged);
		AudioManager->OnAudioMoodChanged.AddDynamic(this, &AGemini_CGameMode::OnAudioMoodChanged);
		AudioManager->OnSoundEffectTriggered.AddDynamic(this, &AGemini_CGameMode::OnSoundEffectTriggered);
		
		// Set initial audio mood and location
		AudioManager->SetAudioMood(EAudioMood::Contemplative);
		AudioManager->OnLocationChanged(EStoryLocation::Tokyo);
		
		UE_LOG(LogProjectVisible, Log, TEXT("Audio System Manager initialized - Adaptive music: %s"), 
			   AudioManager->IsAdaptiveMusicEnabled() ? TEXT("Enabled") : TEXT("Disabled"));
	}
	else
	{
		UE_LOG(LogProjectVisible, Error, TEXT("Failed to get Audio System Manager"));
	}
	
	// Initialize Performance Monitoring Manager
	if (UPerformanceMonitoringManager* PerfManager = GetPerformanceMonitoringManager())
	{
		PerfManager->StartPerformanceMonitoring();
		
		// Register for performance events
		PerfManager->OnPerformanceAlert.AddDynamic(this, &AGemini_CGameMode::OnPerformanceAlert);
		PerfManager->OnPerformanceMetricsUpdated.AddDynamic(this, &AGemini_CGameMode::OnPerformanceMetricsUpdated);
		PerfManager->OnQualityLevelChanged.AddDynamic(this, &AGemini_CGameMode::OnQualityLevelChanged);
		
		// Set initial performance level
		PerfManager->SetPerformanceLevel(EPerformanceLevel::Medium);
		PerfManager->SetAutoOptimizationEnabled(true);
		
		UE_LOG(LogProjectVisible, Log, TEXT("Performance Monitoring Manager initialized - Auto-optimization: %s"), 
			   PerfManager->IsAutoOptimizationEnabled() ? TEXT("Enabled") : TEXT("Disabled"));
	}
	else
	{
		UE_LOG(LogProjectVisible, Error, TEXT("Failed to get Performance Monitoring Manager"));
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

UStoryManager* AGemini_CGameMode::GetStoryManager() const
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UStoryManager>();
		}
	}
	return nullptr;
}

ULevelDesignManager* AGemini_CGameMode::GetLevelDesignManager() const
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<ULevelDesignManager>();
		}
	}
	return nullptr;
}

UCharacterManager* AGemini_CGameMode::GetCharacterManager() const
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UCharacterManager>();
		}
	}
	return nullptr;
}

UGameProgressionManager* AGemini_CGameMode::GetGameProgressionManager() const
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UGameProgressionManager>();
		}
	}
	return nullptr;
}

UAudioSystemManager* AGemini_CGameMode::GetAudioSystemManager() const
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UAudioSystemManager>();
		}
	}
	return nullptr;
}

UPerformanceMonitoringManager* AGemini_CGameMode::GetPerformanceMonitoringManager() const
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UPerformanceMonitoringManager>();
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

void AGemini_CGameMode::OnStoryPhaseChanged(EStoryPhase NewPhase)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Story phase changed to: %s"), *UEnum::GetValueAsString(NewPhase));
	
	// Update game mode based on story phase
	switch (NewPhase)
	{
		case EStoryPhase::Chapter1:
		case EStoryPhase::Chapter2:
		case EStoryPhase::Chapter3:
		case EStoryPhase::Chapter4:
			// Reality-focused chapters
			if (bIsInDreamMode)
			{
				SwitchToRealityMode();
			}
			break;
		case EStoryPhase::Chapter5:
			// Boundary dissolution chapter
			TriggerBoundaryDissolution(0.8f);
			break;
		case EStoryPhase::Epilogue:
		case EStoryPhase::Completed:
			// Final phase
			TriggerBoundaryDissolution(1.0f);
			break;
		default:
			break;
	}
	
	// Update UI based on story phase
	if (UProjectVisibleUIManager* UIManager = GetUIManager())
	{
		if (NewPhase == EStoryPhase::Completed)
		{
			UIManager->PushScreen(EProjectVisibleScreenType::GameOver);
		}
	}
}

void AGemini_CGameMode::OnStoryChapterCompleted(const FStoryChapter& Chapter)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Chapter completed: %s (%.1f%% completion)"), 
		   *Chapter.ChapterTitle, Chapter.CompletionPercentage);
	
	// Award virtue points for chapter completion
	if (UVirtueManager* VirtueManager = GetVirtueManager())
	{
		VirtueManager->RecordWisdomAction(TEXT("Chapter Completion"), true, 5.0f);
		VirtueManager->RecordCourageAction(TEXT("Story Progress"), true, 3.0f);
	}
	
	// Create a significant memory of chapter completion
	if (UMemoryManager* MemoryManager = GetMemoryManager())
	{
		MemoryManager->CreateMemory(
			FString::Printf(TEXT("Chapter: %s"), *Chapter.ChapterTitle),
			FString::Printf(TEXT("Completed chapter: %s"), *Chapter.ChapterDescription),
			EMemoryType::Episodic,
			EMemoryImportance::High,
			80.0f
		);
	}
	
	// Record in social experiment
	if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
	{
		ExperimentManager->RecordBehavioralData(
			1, // Default experiment ID
			TEXT("CURRENT_PLAYER"),
			TEXT("Chapter Completion"),
			Chapter.ChapterTitle,
			Chapter.PlayTimeMinutes
		);
	}
	
	// Trigger boundary dissolution based on progress
	float OverallProgress = 0.0f;
	if (UStoryManager* StoryManager = GetStoryManager())
	{
		OverallProgress = StoryManager->GetOverallProgress() / 100.0f;
	}
	
	// Gradual boundary dissolution as story progresses
	if (OverallProgress > 0.5f)
	{
		TriggerBoundaryDissolution(OverallProgress * 0.3f);
	}
}

void AGemini_CGameMode::OnLevelLoaded(const FLevelDefinition& Level)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Level loaded: %s (%s)"), *Level.LevelName, *Level.LevelID);
	
	// Update UI to show current level
	if (UProjectVisibleUIManager* UIManager = GetUIManager())
	{
		UIManager->UpdateInvestigationDisplayData();
	}
	
	// Apply level-specific settings
	if (Level.LevelType == ELevelType::DreamLandscape)
	{
		// Switch to dream mode if not already
		if (!bIsInDreamMode)
		{
			SwitchToDreamMode();
		}
	}
	else
	{
		// Switch to reality mode if in dream
		if (bIsInDreamMode)
		{
			SwitchToRealityMode();
		}
	}
	
	// Record level start in analytics
	if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
	{
		ExperimentManager->RecordBehavioralData(
			1, // Default experiment ID
			TEXT("CURRENT_PLAYER"),
			TEXT("Level Started"),
			Level.LevelID,
			1.0f
		);
	}
}

void AGemini_CGameMode::OnLevelCompleted(const FLevelDefinition& Level)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Level completed: %s (%.1f%% completion)"), 
		   *Level.LevelName, Level.CompletionPercentage);
	
	// Unlock next levels based on story progression
	if (ULevelDesignManager* LevelManager = GetLevelDesignManager())
	{
		// Update levels for current story phase
		if (UStoryManager* StoryManager = GetStoryManager())
		{
			LevelManager->UpdateLevelsForStoryPhase(StoryManager->GetCurrentPhase());
		}
	}
	
	// Award virtue points for level completion
	if (UVirtueManager* VirtueManager = GetVirtueManager())
	{
		VirtueManager->RecordWisdomAction(TEXT("Level Completion"), true, 4.0f);
		VirtueManager->RecordCourageAction(TEXT("Exploration Complete"), true, 3.0f);
	}
	
	// Create significant memory
	if (UMemoryManager* MemoryManager = GetMemoryManager())
	{
		MemoryManager->CreateMemory(
			FString::Printf(TEXT("Completed: %s"), *Level.LevelName),
			FString::Printf(TEXT("Successfully completed level: %s"), *Level.Description),
			EMemoryType::Episodic,
			EMemoryImportance::High,
			85.0f
		);
	}
	
	// Record completion analytics
	if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
	{
		ExperimentManager->RecordBehavioralData(
			1, // Default experiment ID
			TEXT("CURRENT_PLAYER"),
			TEXT("Level Completed"),
			Level.LevelID,
			Level.CompletionPercentage / 100.0f
		);
	}
}

void AGemini_CGameMode::OnLevelTransition(const FString& FromLevel, const FString& ToLevel)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Level transition: %s -> %s"), *FromLevel, *ToLevel);
	
	// Update UI for transition
	if (UProjectVisibleUIManager* UIManager = GetUIManager())
	{
		UIManager->PushScreen(EProjectVisibleScreenType::Loading);
		// TODO: Show loading screen with transition animation
	}
	
	// Apply boundary dissolution effects during transition
	if (ULevelDesignManager* LevelManager = GetLevelDesignManager())
	{
		FLevelDefinition ToLevelDef;
		if (LevelManager->GetLevelDefinition(ToLevel, ToLevelDef))
		{
			// Increase boundary dissolution for chapter transitions
			if (ToLevelDef.LevelType == ELevelType::BoundaryTransition)
			{
				TriggerBoundaryDissolution(0.8f);
			}
			else if (ToLevelDef.LevelType == ELevelType::DreamLandscape)
			{
				TriggerBoundaryDissolution(0.4f);
			}
		}
	}
	
	// Record transition in experiments
	if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
	{
		ExperimentManager->RecordBehavioralData(
			1, // Default experiment ID
			TEXT("CURRENT_PLAYER"),
			TEXT("Level Transition"),
			FString::Printf(TEXT("%s_to_%s"), *FromLevel, *ToLevel),
			1.0f
		);
	}
}

void AGemini_CGameMode::OnCharacterRegistered(const FCharacterDefinition& Character)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Character registered: %s (%s) - Class: %s"), 
		   *Character.CharacterName, *Character.CharacterID, *UEnum::GetValueAsString(Character.SocialClass));
	
	// Update UI to reflect new character availability
	if (UProjectVisibleUIManager* UIManager = GetUIManager())
	{
		UIManager->UpdateInvestigationDisplayData();
	}
}

void AGemini_CGameMode::OnDialogueStarted(const FString& CharacterID, const FDialogueSession& Session)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Dialogue started with character %s (Session: %s)"), 
		   *CharacterID, *Session.SessionID);
	
	// Switch UI to dialogue mode
	if (UProjectVisibleUIManager* UIManager = GetUIManager())
	{
		UIManager->PushScreen(EProjectVisibleScreenType::Investigation);
	}
	
	// Record dialogue start in social experiment
	if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
	{
		ExperimentManager->RecordBehavioralData(
			1, // Default experiment ID
			Session.PlayerID,
			TEXT("Dialogue Started"),
			CharacterID,
			1.0f
		);
	}
}

void AGemini_CGameMode::OnDialogueEnded(const FString& CharacterID, const FCharacterInteractionResult& Result)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Dialogue ended with %s - Success: %s, Info gained: %.1f"), 
		   *CharacterID, Result.bWasSuccessful ? TEXT("Yes") : TEXT("No"), Result.InformationValue);
	
	// Award virtue points based on dialogue approach
	if (UVirtueManager* VirtueManager = GetVirtueManager())
	{
		switch (Result.Approach)
		{
			case EDialogueApproach::Empathy:
				VirtueManager->RecordJusticeAction(TEXT("Empathetic Dialogue"), Result.bWasSuccessful, 2.0f);
				break;
			case EDialogueApproach::Logic:
				VirtueManager->RecordWisdomAction(TEXT("Logical Dialogue"), Result.bWasSuccessful, 2.0f);
				break;
			case EDialogueApproach::Intimidation:
				// Negative virtue impact for intimidation
				VirtueManager->RecordJusticeAction(TEXT("Intimidating Dialogue"), false, -1.0f);
				break;
			default:
				VirtueManager->RecordWisdomAction(TEXT("General Dialogue"), Result.bWasSuccessful, 1.0f);
				break;
		}
	}
	
	// Create memory of significant dialogue
	if (UMemoryManager* MemoryManager = GetMemoryManager())
	{
		if (Result.InformationValue > 30.0f)
		{
			MemoryManager->CreateMemory(
				FString::Printf(TEXT("重要な対話: %s"), *CharacterID),
				FString::Printf(TEXT("対話で重要な情報を得た: %.1f"), Result.InformationValue),
				EMemoryType::Episodic,
				EMemoryImportance::High,
				Result.InformationValue
			);
		}
	}
}

void AGemini_CGameMode::OnCharacterTrustChanged(const FString& CharacterID, float NewTrustLevel)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Character %s trust changed to %.1f"), *CharacterID, NewTrustLevel);
	
	// Trigger boundary dissolution effects based on trust levels
	if (NewTrustLevel > 80.0f)
	{
		// High trust can lead to boundary dissolution
		TriggerBoundaryDissolution(0.2f);
	}
	else if (NewTrustLevel < 20.0f)
	{
		// Very low trust can also cause reality distortion
		TriggerBoundaryDissolution(0.1f);
	}
}

void AGemini_CGameMode::OnInformationRevealed(const FString& CharacterID, const FString& Information)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Information revealed by %s: %s"), *CharacterID, *Information);
	
	// Add to story system
	if (UStoryManager* StoryManager = GetStoryManager())
	{
		StoryManager->AddClueToCase(TEXT("MAIN_INVESTIGATION"), Information);
	}
	
	// Create important memory
	if (UMemoryManager* MemoryManager = GetMemoryManager())
	{
		MemoryManager->CreateMemory(
			FString::Printf(TEXT("情報入手: %s"), *CharacterID),
			Information,
			EMemoryType::Semantic,
			EMemoryImportance::High,
			80.0f
		);
	}
	
	// Record in social experiment
	if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
	{
		ExperimentManager->RecordBehavioralData(
			1, // Default experiment ID
			TEXT("CURRENT_PLAYER"),
			TEXT("Information Revealed"),
			Information,
			1.0f
		);
	}
}

void AGemini_CGameMode::OnGameSaved(const FGameSaveData& SaveData)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Game saved: %s (%.1f%% complete)"), 
		   *SaveData.SaveName, SaveData.OverallProgress);
	
	// Update UI to show save confirmation
	if (UProjectVisibleUIManager* UIManager = GetUIManager())
	{
		UIManager->UpdateInvestigationDisplayData();
	}
	
	// Record in social experiment
	if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
	{
		ExperimentManager->RecordBehavioralData(
			1, // Default experiment ID
			TEXT("CURRENT_PLAYER"),
			TEXT("Game Saved"),
			SaveData.SaveType == ESaveDataType::ManualSave ? TEXT("Manual") : TEXT("Auto"),
			1.0f
		);
	}
}

void AGemini_CGameMode::OnGameLoaded(const FGameSaveData& LoadedData)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Game loaded: %s (Play time: %.1f minutes)"), 
		   *LoadedData.SaveName, LoadedData.PlayTime / 60.0f);
	
	// Update game state based on loaded data
	if (LoadedData.Difficulty != EGameDifficulty::Normal)
	{
		// Apply difficulty settings if different from default
		UE_LOG(LogProjectVisible, Log, TEXT("Applied difficulty: %s"), *UEnum::GetValueAsString(LoadedData.Difficulty));
	}
	
	// Update UI with loaded state
	if (UProjectVisibleUIManager* UIManager = GetUIManager())
	{
		UIManager->UpdateInvestigationDisplayData();
	}
}

void AGemini_CGameMode::OnCheckpointReached(const FGameCheckpoint& Checkpoint)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Checkpoint reached: %s (%.1f%% progress)"), 
		   *Checkpoint.CheckpointName, Checkpoint.ProgressPercentage);
	
	// Award virtue points for reaching checkpoints
	if (UVirtueManager* VirtueManager = GetVirtueManager())
	{
		VirtueManager->RecordWisdomAction(TEXT("Checkpoint Reached"), true, 1.0f);
	}
	
	// Create memory of checkpoint
	if (UMemoryManager* MemoryManager = GetMemoryManager())
	{
		MemoryManager->CreateMemory(
			FString::Printf(TEXT("チェックポイント: %s"), *Checkpoint.CheckpointName),
			Checkpoint.Description.IsEmpty() ? TEXT("重要な進歩を遂げた") : Checkpoint.Description,
			EMemoryType::Episodic,
			EMemoryImportance::Medium,
			50.0f
		);
	}
	
	// Trigger boundary dissolution based on progress
	if (Checkpoint.ProgressPercentage > 75.0f)
	{
		TriggerBoundaryDissolution(0.3f);
	}
}

void AGemini_CGameMode::OnAchievementUnlocked(const FAchievementDefinition& Achievement)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Achievement unlocked: %s (+%d points)"), 
		   *Achievement.AchievementName, Achievement.RewardPoints);
	
	// Award virtue points for achievements
	if (UVirtueManager* VirtueManager = GetVirtueManager())
	{
		switch (Achievement.Type)
		{
			case EAchievementType::VirtueSeeker:
				VirtueManager->RecordJusticeAction(TEXT("Virtue Achievement"), true, 3.0f);
				break;
			case EAchievementType::TruthSeeker:
				VirtueManager->RecordWisdomAction(TEXT("Truth Achievement"), true, 3.0f);
				break;
			case EAchievementType::ExplorationMaster:
				VirtueManager->RecordCourageAction(TEXT("Exploration Achievement"), true, 3.0f);
				break;
			case EAchievementType::DialogueExpert:
				VirtueManager->RecordTemperanceAction(TEXT("Dialogue Achievement"), true, 3.0f);
				break;
			default:
				VirtueManager->RecordWisdomAction(TEXT("General Achievement"), true, 2.0f);
				break;
		}
	}
	
	// Create important memory of achievement
	if (UMemoryManager* MemoryManager = GetMemoryManager())
	{
		MemoryManager->CreateMemory(
			FString::Printf(TEXT("実績解除: %s"), *Achievement.AchievementName),
			Achievement.Description,
			EMemoryType::Episodic,
			EMemoryImportance::High,
			75.0f
		);
	}
	
	// Show achievement notification in UI
	if (UProjectVisibleUIManager* UIManager = GetUIManager())
	{
		UIManager->UpdateInvestigationDisplayData();
	}
}

void AGemini_CGameMode::OnProgressUpdated(EProgressTrackingType Type, float NewProgress)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Progress updated - %s: %.1f%%"), 
		   *UEnum::GetValueAsString(Type), NewProgress);
	
	// Update phase based on overall progress
	if (Type == EProgressTrackingType::Overall)
	{
		if (NewProgress >= 75.0f && CurrentGamePhase != ELandscapePhase::Completion)
		{
			CurrentGamePhase = ELandscapePhase::Completion;
			UE_LOG(LogProjectVisible, Log, TEXT("Game phase advanced to Completion"));
		}
		else if (NewProgress >= 50.0f && CurrentGamePhase != ELandscapePhase::Defense)
		{
			CurrentGamePhase = ELandscapePhase::Defense;
			UE_LOG(LogProjectVisible, Log, TEXT("Game phase advanced to Defense"));
		}
		else if (NewProgress >= 25.0f && CurrentGamePhase != ELandscapePhase::Observation)
		{
			CurrentGamePhase = ELandscapePhase::Observation;
			UE_LOG(LogProjectVisible, Log, TEXT("Game phase advanced to Observation"));
		}
	}
	
	// Record progress in social experiment
	if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
	{
		ExperimentManager->RecordBehavioralData(
			1, // Default experiment ID
			TEXT("CURRENT_PLAYER"),
			TEXT("Progress Updated"),
			FString::Printf(TEXT("%s_%.1f"), *UEnum::GetValueAsString(Type), NewProgress),
			NewProgress / 100.0f
		);
	}
}

void AGemini_CGameMode::OnMusicTrackChanged(const FMusicTrack& NewTrack, const FMusicTrack& OldTrack)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Music track changed: %s -> %s (Mood: %s)"), 
		   *OldTrack.TrackName, *NewTrack.TrackName, *UEnum::GetValueAsString(NewTrack.PrimaryMood));
	
	// Update boundary dissolution based on music mood
	if (NewTrack.PrimaryMood == EAudioMood::Ethereal || NewTrack.PrimaryMood == EAudioMood::Mysterious)
	{
		TriggerBoundaryDissolution(0.2f);
	}
	
	// Record in social experiment
	if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
	{
		ExperimentManager->RecordBehavioralData(
			1, // Default experiment ID
			TEXT("CURRENT_PLAYER"),
			TEXT("Music Track Changed"),
			FString::Printf(TEXT("%s->%s"), *OldTrack.TrackName, *NewTrack.TrackName),
			1.0f
		);
	}
}

void AGemini_CGameMode::OnAudioMoodChanged(EAudioMood OldMood, EAudioMood NewMood)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Audio mood changed: %s -> %s"), 
		   *UEnum::GetValueAsString(OldMood), *UEnum::GetValueAsString(NewMood));
	
	// Adjust game atmosphere based on mood
	if (UBoundaryDissolutionManager* BoundaryManager = GetBoundaryDissolutionManager())
	{
		switch (NewMood)
		{
			case EAudioMood::Ethereal:
				BoundaryManager->SetDissolutionIntensity(0.4f);
				break;
			case EAudioMood::Mysterious:
				BoundaryManager->SetDissolutionIntensity(0.3f);
				break;
			case EAudioMood::Chaotic:
				BoundaryManager->SetDissolutionIntensity(0.5f);
				break;
			case EAudioMood::Calm:
			case EAudioMood::Contemplative:
				BoundaryManager->SetDissolutionIntensity(0.1f);
				break;
		}
	}
	
	// Update UI theme based on mood
	if (UProjectVisibleUIManager* UIManager = GetUIManager())
	{
		UIManager->UpdateInvestigationDisplayData();
	}
	
	// Record mood analytics
	if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
	{
		ExperimentManager->RecordBehavioralData(
			1, // Default experiment ID
			TEXT("CURRENT_PLAYER"),
			TEXT("Audio Mood Changed"),
			FString::Printf(TEXT("%s->%s"), *UEnum::GetValueAsString(OldMood), *UEnum::GetValueAsString(NewMood)),
			1.0f
		);
	}
}

void AGemini_CGameMode::OnSoundEffectTriggered(const FSoundEffect& Effect)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Sound effect triggered: %s (Category: %s)"), 
		   *Effect.EffectName, *UEnum::GetValueAsString(Effect.Category));
	
	// Special handling for certain effect types
	switch (Effect.Category)
	{
		case ESoundEffectCategory::UI:
			// UI feedback - could trigger haptic feedback on controllers
			break;
		case ESoundEffectCategory::Interaction:
			// Interaction feedback - record player engagement
			if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
			{
				ExperimentManager->RecordBehavioralData(
					1, // Default experiment ID
					TEXT("CURRENT_PLAYER"),
					TEXT("Interaction Sound"),
					Effect.EffectName,
					1.0f
				);
			}
			break;
		case ESoundEffectCategory::Atmospheric:
			// Atmospheric effects - enhance immersion
			if (UMemoryManager* MemoryManager = GetMemoryManager())
			{
				MemoryManager->CreateMemory(
					FString::Printf(TEXT("音響体験: %s"), *Effect.EffectName),
					TEXT("印象的な音響効果を体験した"),
					EMemoryType::Procedural,
					EMemoryImportance::Low,
					25.0f
				);
			}
			break;
	}
}

void AGemini_CGameMode::OnPerformanceAlert(const FPerformanceAlert& Alert)
{
	UE_LOG(LogProjectVisible, Warning, TEXT("Performance Alert: %s - %s (Severity: %.1f)"), 
		   *UEnum::GetValueAsString(Alert.AlertType), *Alert.Description, Alert.Severity);
	
	// Handle different alert types
	switch (Alert.AlertType)
	{
		case EPerformanceAlertType::LowFrameRate:
			// Auto-reduce quality if severe frame rate drop
			if (Alert.Severity > 7.0f)
			{
				if (UPerformanceMonitoringManager* PerfManager = GetPerformanceMonitoringManager())
				{
					PerfManager->AutoAdjustQualityLevel();
				}
			}
			break;
			
		case EPerformanceAlertType::HighMemoryUsage:
			// Trigger memory cleanup
			if (UPerformanceMonitoringManager* PerfManager = GetPerformanceMonitoringManager())
			{
				PerfManager->OptimizeMemoryUsage();
			}
			break;
			
		case EPerformanceAlertType::SystemOverload:
			// Reduce overall system load
			if (UAudioSystemManager* AudioManager = GetAudioSystemManager())
			{
				AudioManager->SetIntensityLevel(3.0f); // Reduce audio complexity
			}
			break;
	}
	
	// Create memory of performance issue for social experiment
	if (UMemoryManager* MemoryManager = GetMemoryManager())
	{
		MemoryManager->CreateMemory(
			FString::Printf(TEXT("パフォーマンス警告: %s"), *UEnum::GetValueAsString(Alert.AlertType)),
			Alert.Description,
			EMemoryType::Procedural,
			EMemoryImportance::Medium,
			Alert.Severity * 10.0f
		);
	}
	
	// Record performance alert in social experiment
	if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
	{
		ExperimentManager->RecordBehavioralData(
			1, // Default experiment ID
			TEXT("CURRENT_PLAYER"),
			TEXT("Performance Alert"),
			FString::Printf(TEXT("%s_%.1f"), *UEnum::GetValueAsString(Alert.AlertType), Alert.Severity),
			Alert.Severity / 10.0f
		);
	}
}

void AGemini_CGameMode::OnPerformanceMetricsUpdated(const FPerformanceMetrics& NewMetrics, const FPerformanceMetrics& PreviousMetrics)
{
	// Log significant performance changes
	float FrameRateDelta = NewMetrics.FrameRate - PreviousMetrics.FrameRate;
	if (FMath::Abs(FrameRateDelta) > 5.0f)
	{
		UE_LOG(LogProjectVisible, Log, TEXT("Frame rate changed: %.1f -> %.1f FPS (Delta: %.1f)"), 
			   PreviousMetrics.FrameRate, NewMetrics.FrameRate, FrameRateDelta);
	}
	
	// Update UI with performance information
	if (UProjectVisibleUIManager* UIManager = GetUIManager())
	{
		UIManager->UpdateInvestigationDisplayData();
	}
	
	// Adjust game atmosphere based on performance
	if (NewMetrics.OverallPerformanceScore < 50.0f)
	{
		// Poor performance - reduce intensity
		if (UBoundaryDissolutionManager* BoundaryManager = GetBoundaryDissolutionManager())
		{
			BoundaryManager->SetDissolutionIntensity(0.2f);
		}
		
		if (UAudioSystemManager* AudioManager = GetAudioSystemManager())
		{
			AudioManager->SetAudioMood(EAudioMood::Calm);
		}
	}
	else if (NewMetrics.OverallPerformanceScore > 80.0f)
	{
		// Good performance - can increase intensity
		if (UBoundaryDissolutionManager* BoundaryManager = GetBoundaryDissolutionManager())
		{
			BoundaryManager->SetDissolutionIntensity(0.6f);
		}
	}
}

void AGemini_CGameMode::OnQualityLevelChanged(EPerformanceLevel OldLevel, EPerformanceLevel NewLevel)
{
	UE_LOG(LogProjectVisible, Log, TEXT("Quality level changed: %s -> %s"), 
		   *UEnum::GetValueAsString(OldLevel), *UEnum::GetValueAsString(NewLevel));
	
	// Adjust audio quality based on performance level
	if (UAudioSystemManager* AudioManager = GetAudioSystemManager())
	{
		switch (NewLevel)
		{
			case EPerformanceLevel::Lowest:
			case EPerformanceLevel::Low:
				AudioManager->SetIntensityLevel(3.0f);
				break;
			case EPerformanceLevel::Medium:
				AudioManager->SetIntensityLevel(5.0f);
				break;
			case EPerformanceLevel::High:
			case EPerformanceLevel::Highest:
				AudioManager->SetIntensityLevel(7.0f);
				break;
		}
	}
	
	// Create memory of quality change
	if (UMemoryManager* MemoryManager = GetMemoryManager())
	{
		MemoryManager->CreateMemory(
			TEXT("画質設定変更"),
			FString::Printf(TEXT("ゲーム品質が%sから%sに変更された"), 
				*UEnum::GetValueAsString(OldLevel), *UEnum::GetValueAsString(NewLevel)),
			EMemoryType::Procedural,
			EMemoryImportance::Low,
			20.0f
		);
	}
	
	// Award virtue for accepting performance optimization
	if (static_cast<uint8>(NewLevel) < static_cast<uint8>(OldLevel))
	{
		if (UVirtueManager* VirtueManager = GetVirtueManager())
		{
			VirtueManager->UpdateVirtueLevel(EVirtueType::Temperance, 2.0f, TEXT("パフォーマンス最適化を受け入れた"));
		}
	}
	
	// Record quality change in social experiment
	if (USocialExperimentManager* ExperimentManager = GetSocialExperimentManager())
	{
		ExperimentManager->RecordBehavioralData(
			1, // Default experiment ID
			TEXT("CURRENT_PLAYER"),
			TEXT("Quality Level Changed"),
			FString::Printf(TEXT("%s->%s"), *UEnum::GetValueAsString(OldLevel), *UEnum::GetValueAsString(NewLevel)),
			static_cast<float>(static_cast<uint8>(NewLevel)) / 5.0f
		);
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
