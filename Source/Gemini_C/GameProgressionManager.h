// Project Visible - Game Progression Manager
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/World.h"
#include "GameProgressionTypes.h"
#include "GameProgressionManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSaved, const FGameSaveData&, SaveData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameLoaded, const FGameSaveData&, LoadedData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCheckpointReached, const FGameCheckpoint&, Checkpoint);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAchievementUnlocked, const FAchievementDefinition&, Achievement);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProgressUpdated, EProgressTrackingType, Type, float, NewProgress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDifficultyChanged, EGameDifficulty, OldDifficulty, EGameDifficulty, NewDifficulty);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionStarted, const FGameSessionStats&, SessionStats);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionEnded, const FGameSessionStats&, FinalStats);

/**
 * Central manager for Project Visible's game progression, save/load, achievements, and session tracking
 * Manages comprehensive game state, progress tracking, and player experience continuity
 */
UCLASS()
class GEMINI_C_API UGameProgressionManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // System Initialization
    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void InitializeProgressionSystem();

    // Save/Load System
    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    bool SaveGame(const FString& SaveName, ESaveDataType SaveType = ESaveDataType::ManualSave);

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    bool LoadGame(const FString& SaveID);

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    bool DeleteSave(const FString& SaveID);

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    TArray<FGameSaveData> GetAvailableSaves() const;

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    bool DoesSaveExist(const FString& SaveID) const;

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    bool CreateQuickSave();

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    bool LoadQuickSave();

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void AutoSave();

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void SetAutoSaveEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    bool IsAutoSaveEnabled() const { return bAutoSaveEnabled; }

    // Checkpoint System
    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    FString CreateCheckpoint(const FString& CheckpointName, const FString& Description = TEXT(""));

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    bool LoadCheckpoint(const FString& CheckpointID);

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    TArray<FGameCheckpoint> GetAvailableCheckpoints() const;

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    bool GetCheckpoint(const FString& CheckpointID, FGameCheckpoint& OutCheckpoint) const;

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void ClearOldCheckpoints(int32 MaxCheckpoints = 10);

    // Progress Tracking
    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void UpdateProgress(EProgressTrackingType Type, float NewValue);

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void AddProgress(EProgressTrackingType Type, float AddedValue);

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    float GetProgress(EProgressTrackingType Type) const;

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    FProgressResult GetDetailedProgress(EProgressTrackingType Type) const;

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    float GetOverallProgress() const;

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void ResetProgress(EProgressTrackingType Type);

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void ResetAllProgress();

    // Achievement System
    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void RegisterAchievement(const FAchievementDefinition& Achievement);

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    bool UnlockAchievement(const FString& AchievementID, const FString& UnlockReason = TEXT(""));

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void UpdateAchievementProgress(const FString& AchievementID, int32 NewProgress);

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    TArray<FAchievementDefinition> GetAllAchievements() const;

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    TArray<FAchievementDefinition> GetUnlockedAchievements() const;

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    TArray<FAchievementDefinition> GetAchievementsByType(EAchievementType Type) const;

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    bool IsAchievementUnlocked(const FString& AchievementID) const;

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    int32 GetTotalAchievementPoints() const;

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    float GetAchievementCompletionPercentage() const;

    // Session Management
    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void StartNewSession();

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void EndCurrentSession();

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void PauseSession();

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void ResumeSession();

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    FGameSessionStats GetCurrentSessionStats() const { return CurrentSessionStats; }

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    float GetCurrentSessionPlayTime() const;

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    EGameSessionState GetSessionState() const { return CurrentSessionState; }

    // Statistics & Analytics
    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void RecordPlayerAction(const FString& ActionType, const FString& ActionData = TEXT(""));

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void IncrementCounter(const FString& CounterName, int32 Amount = 1);

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void UpdateTimer(const FString& TimerName, float NewTime);

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    int32 GetCounter(const FString& CounterName) const;

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    float GetTimer(const FString& TimerName) const;

    // Game Settings & Difficulty
    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void SetGameDifficulty(EGameDifficulty NewDifficulty);

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    EGameDifficulty GetGameDifficulty() const { return CurrentDifficulty; }

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void SetPlayerPreference(const FString& Key, const FString& Value);

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    FString GetPlayerPreference(const FString& Key, const FString& DefaultValue = TEXT("")) const;

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void SetGameSetting(const FString& SettingName, float Value);

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    float GetGameSetting(const FString& SettingName, float DefaultValue = 0.0f) const;

    // Content Management
    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void UnlockContent(const FString& ContentID);

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    bool IsContentUnlocked(const FString& ContentID) const;

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    TArray<FString> GetUnlockedContent() const;

    // System Integration
    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void SynchronizeWithAllSystems();

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void CollectSystemData();

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void RestoreSystemData();

    // File Management
    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void CleanupOldSaves(int32 MaxSaves = 20);

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void ValidateAllSaves();

    UFUNCTION(BlueprintCallable, Category = "Game Progression")
    void BackupSaveData();

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    int64 GetTotalSaveFileSize() const;

    UFUNCTION(BlueprintCallable, Category = "Game Progression", BlueprintPure)
    FString GetSaveDirectory() const;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Progression Events")
    FOnGameSaved OnGameSaved;

    UPROPERTY(BlueprintAssignable, Category = "Progression Events")
    FOnGameLoaded OnGameLoaded;

    UPROPERTY(BlueprintAssignable, Category = "Progression Events")
    FOnCheckpointReached OnCheckpointReached;

    UPROPERTY(BlueprintAssignable, Category = "Progression Events")
    FOnAchievementUnlocked OnAchievementUnlocked;

    UPROPERTY(BlueprintAssignable, Category = "Progression Events")
    FOnProgressUpdated OnProgressUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Progression Events")
    FOnDifficultyChanged OnDifficultyChanged;

    UPROPERTY(BlueprintAssignable, Category = "Progression Events")
    FOnSessionStarted OnSessionStarted;

    UPROPERTY(BlueprintAssignable, Category = "Progression Events")
    FOnSessionEnded OnSessionEnded;

protected:
    // Core Progression Data
    UPROPERTY(BlueprintReadOnly, Category = "Progression Data")
    FGameSaveData CurrentGameData;

    UPROPERTY(BlueprintReadOnly, Category = "Progression Data")
    TArray<FAchievementDefinition> RegisteredAchievements;

    UPROPERTY(BlueprintReadOnly, Category = "Progression Data")
    TArray<FGameCheckpoint> GameCheckpoints;

    UPROPERTY(BlueprintReadOnly, Category = "Progression Data")
    FGameSessionStats CurrentSessionStats;

    // System State
    UPROPERTY(BlueprintReadOnly, Category = "Progression State")
    EGameSessionState CurrentSessionState;

    UPROPERTY(BlueprintReadOnly, Category = "Progression State")
    EGameDifficulty CurrentDifficulty;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression Settings")
    bool bAutoSaveEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression Settings")
    float AutoSaveInterval;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression Settings")
    int32 MaxSaveSlots;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression Settings")
    int32 MaxCheckpoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression Settings")
    bool bEnableAchievements;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression Settings")
    bool bEnableStatistics;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression Settings")
    bool bCompressSaveData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression Settings")
    bool bEncryptSaveData;

    // Runtime State
    UPROPERTY()
    FTimerHandle AutoSaveTimerHandle;

    UPROPERTY()
    FDateTime SessionStartTime;

    UPROPERTY()
    FDateTime LastAutoSaveTime;

    UPROPERTY()
    TMap<FString, FGameSaveData> LoadedSaveFiles;

private:
    // Helper Functions
    void InitializeDefaultAchievements();
    void InitializeDefaultSettings();
    void SetupAutoSave();

    // Save/Load Helpers
    FString GenerateSaveID();
    FString GetSaveFilePath(const FString& SaveID) const;
    bool SaveToFile(const FGameSaveData& SaveData, const FString& FilePath);
    bool LoadFromFile(const FString& FilePath, FGameSaveData& OutSaveData) const;
    bool ValidateSaveData(const FGameSaveData& SaveData) const;
    void CompressSaveData(FGameSaveData& SaveData);
    void DecompressSaveData(FGameSaveData& SaveData);

    // Progress Helpers
    void UpdateProgressInternal(EProgressTrackingType Type, float NewValue, bool bAdditive);
    void CalculateOverallProgress();
    void CheckProgressMilestones(EProgressTrackingType Type, float OldValue, float NewValue);

    // Achievement Helpers
    FAchievementDefinition* GetAchievementByID(const FString& AchievementID);
    const FAchievementDefinition* GetAchievementByID(const FString& AchievementID) const;
    void CheckAchievementUnlockConditions();
    void ProcessAchievementUnlock(FAchievementDefinition& Achievement, const FString& UnlockReason);

    // Session Helpers
    void UpdateSessionStats();
    void RecordSessionEvent(const FString& EventType, const FString& EventData);

    // System Integration Helpers
    FString SerializeStorySystemData();
    FString SerializeLevelSystemData();
    FString SerializeCharacterSystemData();
    FString SerializeVirtueSystemData();
    FString SerializeMemorySystemData();
    FString SerializeExperimentSystemData();
    FString SerializeUISystemData();
    FString SerializeLandscapeSystemData();
    FString SerializeBoundarySystemData();
    FString SerializeInvestigationSystemData();

    void RestoreStorySystemData(const FString& Data);
    void RestoreLevelSystemData(const FString& Data);
    void RestoreCharacterSystemData(const FString& Data);
    void RestoreVirtueSystemData(const FString& Data);
    void RestoreMemorySystemData(const FString& Data);
    void RestoreExperimentSystemData(const FString& Data);
    void RestoreUISystemData(const FString& Data);
    void RestoreLandscapeSystemData(const FString& Data);
    void RestoreBoundarySystemData(const FString& Data);
    void RestoreInvestigationSystemData(const FString& Data);

    // System Reference Helpers
    class UStoryManager* GetStoryManager();
    class ULevelDesignManager* GetLevelDesignManager();
    class UCharacterManager* GetCharacterManager();
    class UVirtueManager* GetVirtueManager();
    class UMemoryManager* GetMemoryManager();
    class USocialExperimentManager* GetSocialExperimentManager();
    class UProjectVisibleUIManager* GetUIManager();
    class ULandscapeOfMindManager* GetLandscapeManager();
    class UBoundaryDissolutionManager* GetBoundaryManager();
    class URealityInvestigationManager* GetInvestigationManager();
};
