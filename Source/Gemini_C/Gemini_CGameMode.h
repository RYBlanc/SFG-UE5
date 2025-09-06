// Project Visible - Main Game Mode
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LandscapeOfMindTypes.h"
#include "StorySystemTypes.h"
#include "LevelDesignTypes.h"
#include "CharacterSystemTypes.h"
#include "GameProgressionTypes.h"
#include "AudioSystemTypes.h"
#include "Gemini_CGameMode.generated.h"

class ULandscapeOfMindManager;
class UBoundaryDissolutionManager;
class USocialExperimentManager;
class UVirtueManager;
class UMemoryManager;
class URealityInvestigationManager;
class UProjectVisibleUIManager;
class UStoryManager;
class ULevelDesignManager;
class UCharacterManager;
class UGameProgressionManager;
class UAudioSystemManager;

/**
 * Project Visible Game Mode - Manages the dual reality/dream world system
 */
UCLASS(minimalapi)
class AGemini_CGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGemini_CGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Game State Management
	UFUNCTION(BlueprintCallable, Category = "Project Visible")
	void InitializeProjectVisible();

	UFUNCTION(BlueprintCallable, Category = "Project Visible")
	void SwitchToRealityMode();

	UFUNCTION(BlueprintCallable, Category = "Project Visible")
	void SwitchToDreamMode();

	UFUNCTION(BlueprintCallable, Category = "Project Visible")
	void TriggerBoundaryDissolution(float IntensityLevel);

	// System Access
	UFUNCTION(BlueprintCallable, Category = "Project Visible", BlueprintPure)
	ULandscapeOfMindManager* GetLandscapeManager() const;

	UFUNCTION(BlueprintCallable, Category = "Project Visible", BlueprintPure)
	UVirtueManager* GetVirtueManager() const;

	UFUNCTION(BlueprintCallable, Category = "Project Visible", BlueprintPure)
	UMemoryManager* GetMemoryManager() const;

	UFUNCTION(BlueprintCallable, Category = "Project Visible", BlueprintPure)
	URealityInvestigationManager* GetRealityInvestigationManager() const;

	UFUNCTION(BlueprintCallable, Category = "Project Visible", BlueprintPure)
	UBoundaryDissolutionManager* GetBoundaryDissolutionManager() const;

	UFUNCTION(BlueprintCallable, Category = "Project Visible", BlueprintPure)
	USocialExperimentManager* GetSocialExperimentManager() const;

	UFUNCTION(BlueprintCallable, Category = "Project Visible", BlueprintPure)
	UProjectVisibleUIManager* GetUIManager() const;

	UFUNCTION(BlueprintCallable, Category = "Project Visible", BlueprintPure)
	UStoryManager* GetStoryManager() const;

	UFUNCTION(BlueprintCallable, Category = "Project Visible", BlueprintPure)
	ULevelDesignManager* GetLevelDesignManager() const;

	UFUNCTION(BlueprintCallable, Category = "Project Visible", BlueprintPure)
	UCharacterManager* GetCharacterManager() const;

	UFUNCTION(BlueprintCallable, Category = "Project Visible", BlueprintPure)
	UGameProgressionManager* GetGameProgressionManager() const;

	UFUNCTION(BlueprintCallable, Category = "Project Visible", BlueprintPure)
	UAudioSystemManager* GetAudioSystemManager() const;

	UFUNCTION(BlueprintCallable, Category = "Project Visible", BlueprintPure)
	ELandscapePhase GetCurrentGamePhase() const { return CurrentGamePhase; }

	// Game Events
	UFUNCTION(BlueprintCallable, Category = "Project Visible")
	void OnPlayerDiscoveryMade(const FString& DiscoveryType, const FString& Evidence);

	UFUNCTION(BlueprintCallable, Category = "Project Visible")
	void OnLandscapeStabilized(float OldStability, float NewStability);

	UFUNCTION(BlueprintCallable, Category = "Project Visible")
	void OnExperimentCompleted(const FString& ExperimentType);

	UFUNCTION(BlueprintCallable, Category = "Project Visible")
	void OnVirtueActionPerformed(const FString& ActionType, const FString& VirtueType, bool bIsPositive);

	UFUNCTION(BlueprintCallable, Category = "Project Visible")
	void OnMemoryCreated(const FString& MemoryTitle, const FString& MemoryType, float EmotionalIntensity);

	UFUNCTION(BlueprintCallable, Category = "Project Visible")
	void OnHappinessChanged(float NewHappinessLevel);

	UFUNCTION(BlueprintCallable, Category = "Project Visible")
	void OnSocialExperimentTriggered(const FString& ExperimentType, const FString& Context);

	UFUNCTION(BlueprintCallable, Category = "Project Visible")
	void OnPlayerBehaviorObserved(const FString& BehaviorType, const FString& Response, float Intensity);

	UFUNCTION()
	void OnStoryPhaseChanged(EStoryPhase NewPhase);

	UFUNCTION()
	void OnStoryChapterCompleted(const FStoryChapter& Chapter);

	UFUNCTION()
	void OnLevelLoaded(const FLevelDefinition& Level);

	UFUNCTION()
	void OnLevelCompleted(const FLevelDefinition& Level);

	UFUNCTION()
	void OnLevelTransition(const FString& FromLevel, const FString& ToLevel);

	UFUNCTION()
	void OnCharacterRegistered(const FCharacterDefinition& Character);

	UFUNCTION()
	void OnDialogueStarted(const FString& CharacterID, const FDialogueSession& Session);

	UFUNCTION()
	void OnDialogueEnded(const FString& CharacterID, const FCharacterInteractionResult& Result);

	UFUNCTION()
	void OnCharacterTrustChanged(const FString& CharacterID, float NewTrustLevel);

	UFUNCTION()
	void OnInformationRevealed(const FString& CharacterID, const FString& Information);

	UFUNCTION()
	void OnGameSaved(const FGameSaveData& SaveData);

	UFUNCTION()
	void OnGameLoaded(const FGameSaveData& LoadedData);

	UFUNCTION()
	void OnCheckpointReached(const FGameCheckpoint& Checkpoint);

	UFUNCTION()
	void OnAchievementUnlocked(const FAchievementDefinition& Achievement);

	UFUNCTION()
	void OnProgressUpdated(EProgressTrackingType Type, float NewProgress);

	UFUNCTION()
	void OnMusicTrackChanged(const FMusicTrack& NewTrack, const FMusicTrack& OldTrack);

	UFUNCTION()
	void OnAudioMoodChanged(EAudioMood OldMood, EAudioMood NewMood);

	UFUNCTION()
	void OnSoundEffectTriggered(const FSoundEffect& Effect);

protected:
	// Game State
	UPROPERTY(BlueprintReadOnly, Category = "Project Visible")
	ELandscapePhase CurrentGamePhase;

	UPROPERTY(BlueprintReadOnly, Category = "Project Visible")
	bool bIsInDreamMode;

	UPROPERTY(BlueprintReadOnly, Category = "Project Visible")
	float BoundaryDissolutionLevel;

	// Configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Project Visible")
	FMentalLandscapeConfig DefaultLandscapeConfig;

	// Events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePhaseChanged, ELandscapePhase, NewPhase);
	UPROPERTY(BlueprintAssignable, Category = "Project Visible")
	FOnGamePhaseChanged OnGamePhaseChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModeSwitch, bool, bIsNowDreamMode);
	UPROPERTY(BlueprintAssignable, Category = "Project Visible")
	FOnModeSwitch OnModeSwitch;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBoundaryDissolution, float, DissolutionLevel);
	UPROPERTY(BlueprintAssignable, Category = "Project Visible")
	FOnBoundaryDissolution OnBoundaryDissolution;

private:
	// Internal Management
	void UpdateGamePhase();
	void ProcessBoundaryEffects(float DeltaTime);
	void HandleSystemIntegration();

	// Timer Handles
	FTimerHandle GamePhaseUpdateTimer;
	FTimerHandle BoundaryEffectTimer;

	// Constants
	static constexpr float GAME_PHASE_UPDATE_INTERVAL = 2.0f;
	static constexpr float BOUNDARY_EFFECT_UPDATE_INTERVAL = 0.1f;
};



