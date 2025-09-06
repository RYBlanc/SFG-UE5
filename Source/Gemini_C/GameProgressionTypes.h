// Project Visible - Game Progression Types
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StorySystemTypes.h"
#include "LevelDesignTypes.h"
#include "CharacterSystemTypes.h"
#include "GameProgressionTypes.generated.h"

/**
 * Game session state types
 */
UENUM(BlueprintType)
enum class EGameSessionState : uint8
{
    NotStarted          UMETA(DisplayName = "Not Started"),              // ゲーム未開始
    InProgress          UMETA(DisplayName = "In Progress"),              // プレイ中
    Paused              UMETA(DisplayName = "Paused"),                   // 一時停止
    Completed           UMETA(DisplayName = "Completed"),                // 完了
    Failed              UMETA(DisplayName = "Failed"),                   // 失敗
    Abandoned           UMETA(DisplayName = "Abandoned")                 // 中断
};

/**
 * Save file data types
 */
UENUM(BlueprintType)
enum class ESaveDataType : uint8
{
    QuickSave           UMETA(DisplayName = "Quick Save"),               // クイックセーブ
    ManualSave          UMETA(DisplayName = "Manual Save"),              // 手動セーブ
    AutoSave            UMETA(DisplayName = "Auto Save"),                // 自動セーブ
    CheckpointSave      UMETA(DisplayName = "Checkpoint Save"),          // チェックポイント
    ChapterSave         UMETA(DisplayName = "Chapter Save"),             // チャプター完了
    ExitSave            UMETA(DisplayName = "Exit Save"),                // 終了時セーブ
    SystemSave          UMETA(DisplayName = "System Save"),              // システムセーブ
    BackupSave          UMETA(DisplayName = "Backup Save")               // バックアップ
};

/**
 * Achievement types
 */
UENUM(BlueprintType)
enum class EAchievementType : uint8
{
    StoryProgress       UMETA(DisplayName = "Story Progress"),           // ストーリー進行
    ExplorationMaster   UMETA(DisplayName = "Exploration Master"),       // 探索マスター
    DialogueExpert      UMETA(DisplayName = "Dialogue Expert"),          // 対話エキスパート
    VirtueSeeker        UMETA(DisplayName = "Virtue Seeker"),            // 徳の探求者
    MemoryKeeper        UMETA(DisplayName = "Memory Keeper"),            // 記憶の管理者
    TruthSeeker         UMETA(DisplayName = "Truth Seeker"),             // 真実の探求者
    QuantumMaster       UMETA(DisplayName = "Quantum Master"),           // 量子マスター
    SocialAnalyst       UMETA(DisplayName = "Social Analyst"),           // 社会分析者
    BoundaryWalker      UMETA(DisplayName = "Boundary Walker"),          // 境界の歩行者
    Perfectionist       UMETA(DisplayName = "Perfectionist"),            // 完璧主義者
    Speed               UMETA(DisplayName = "Speed"),                     // スピード達成
    Hidden              UMETA(DisplayName = "Hidden")                     // 隠し実績
};

/**
 * Progress tracking types
 */
UENUM(BlueprintType)
enum class EProgressTrackingType : uint8
{
    Overall             UMETA(DisplayName = "Overall"),                  // 全体進行度
    Chapter             UMETA(DisplayName = "Chapter"),                  // チャプター進行度
    StoryLine           UMETA(DisplayName = "Story Line"),               // ストーリーライン
    Investigation       UMETA(DisplayName = "Investigation"),            // 調査進行度
    Character           UMETA(DisplayName = "Character"),                // キャラクター関係
    Exploration         UMETA(DisplayName = "Exploration"),              // 探索進行度
    Collection          UMETA(DisplayName = "Collection"),               // 収集進行度
    Achievement         UMETA(DisplayName = "Achievement"),              // 実績進行度
    Virtue              UMETA(DisplayName = "Virtue"),                   // 徳の進行度
    Memory              UMETA(DisplayName = "Memory")                    // 記憶進行度
};

/**
 * Game difficulty settings
 */
UENUM(BlueprintType)
enum class EGameDifficulty : uint8
{
    Easy                UMETA(DisplayName = "Easy"),                     // 簡単
    Normal              UMETA(DisplayName = "Normal"),                   // 普通
    Hard                UMETA(DisplayName = "Hard"),                     // 難しい
    Expert              UMETA(DisplayName = "Expert"),                   // エキスパート
    Master              UMETA(DisplayName = "Master"),                   // マスター
    Custom              UMETA(DisplayName = "Custom")                    // カスタム
};

/**
 * Checkpoint information structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FGameCheckpoint
{
    GENERATED_BODY()

    FGameCheckpoint()
    {
        CheckpointID = TEXT("");
        CheckpointName = TEXT("");
        ChapterID = 0;
        StoryPhase = EStoryPhase::Introduction;
        Location = EStoryLocation::Tokyo;
        Timestamp = FDateTime::Now();
        PlayTime = 0.0f;
        bIsAutomatic = true;
        bIsValid = true;
        ProgressPercentage = 0.0f;
        ExperiencePoints = 0;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    FString CheckpointID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    FString CheckpointName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    int32 ChapterID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    EStoryPhase StoryPhase;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    EStoryLocation Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    FDateTime Timestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    float PlayTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    bool bIsAutomatic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    bool bIsValid;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    float ProgressPercentage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    int32 ExperiencePoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    TMap<FString, FString> StateData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    TArray<FString> CompletedObjectives;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    TArray<FString> UnlockedContent;
};

/**
 * Achievement definition structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FAchievementDefinition
{
    GENERATED_BODY()

    FAchievementDefinition()
    {
        AchievementID = TEXT("");
        AchievementName = TEXT("");
        Type = EAchievementType::StoryProgress;
        bIsUnlocked = false;
        bIsSecret = false;
        ProgressCurrent = 0;
        ProgressTarget = 1;
        RewardPoints = 0;
        DifficultyRequirement = EGameDifficulty::Easy;
        UnlockTimestamp = FDateTime::MinValue();
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    FString AchievementID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    FString AchievementName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    EAchievementType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    bool bIsUnlocked;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    bool bIsSecret;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    int32 ProgressCurrent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    int32 ProgressTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    int32 RewardPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    EGameDifficulty DifficultyRequirement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    FDateTime UnlockTimestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    TArray<FString> Prerequisites;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    TArray<FString> RewardItems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    TMap<FString, FString> CustomData;
};

/**
 * Game session statistics
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FGameSessionStats
{
    GENERATED_BODY()

    FGameSessionStats()
    {
        SessionID = TEXT("");
        SessionStartTime = FDateTime::Now();
        SessionEndTime = FDateTime::MinValue();
        TotalPlayTime = 0.0f;
        ChaptersCompleted = 0;
        DialoguesCompleted = 0;
        InvestigationsCompleted = 0;
        EvidenceCollected = 0;
        CharactersInteracted = 0;
        LevelsExplored = 0;
        DeathCount = 0;
        ReloadCount = 0;
        PauseCount = 0;
        MenuAccessCount = 0;
        Score = 0;
        Difficulty = EGameDifficulty::Normal;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    FString SessionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    FDateTime SessionStartTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    FDateTime SessionEndTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    float TotalPlayTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    int32 ChaptersCompleted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    int32 DialoguesCompleted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    int32 InvestigationsCompleted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    int32 EvidenceCollected;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    int32 CharactersInteracted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    int32 LevelsExplored;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    int32 DeathCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    int32 ReloadCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    int32 PauseCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    int32 MenuAccessCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    int32 Score;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    EGameDifficulty Difficulty;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    TMap<FString, int32> CustomCounters;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    TMap<FString, float> CustomTimers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Stats")
    TArray<FString> CompletedAchievements;
};

/**
 * Comprehensive save game data structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FGameSaveData
{
    GENERATED_BODY()

    FGameSaveData()
    {
        SaveVersion = TEXT("1.0.0");
        SaveID = TEXT("");
        SaveName = TEXT("");
        SaveType = ESaveDataType::ManualSave;
        CreationTime = FDateTime::Now();
        LastModified = FDateTime::Now();
        PlayTime = 0.0f;
        GameVersion = TEXT("0.1.0");
        bIsValid = true;
        bIsCorrupted = false;
        FileSize = 0;
        CompressionLevel = 5;
        Difficulty = EGameDifficulty::Normal;
        OverallProgress = 0.0f;
    }

    // Save File Metadata
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Metadata")
    FString SaveVersion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Metadata")
    FString SaveID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Metadata")
    FString SaveName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Metadata")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Metadata")
    ESaveDataType SaveType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Metadata")
    FDateTime CreationTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Metadata")
    FDateTime LastModified;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Metadata")
    float PlayTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Metadata")
    FString GameVersion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Metadata")
    bool bIsValid;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Metadata")
    bool bIsCorrupted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Metadata")
    int32 FileSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Metadata")
    int32 CompressionLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Metadata")
    EGameDifficulty Difficulty;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Metadata")
    float OverallProgress;

    // Game State Data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game State")
    FGameCheckpoint CurrentCheckpoint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game State")
    FGameSessionStats SessionStats;

    // System States (JSON serialized)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Data")
    FString StorySystemData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Data")
    FString LevelSystemData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Data")
    FString CharacterSystemData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Data")
    FString VirtueSystemData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Data")
    FString MemorySystemData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Data")
    FString ExperimentSystemData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Data")
    FString UISystemData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Data")
    FString LandscapeSystemData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Data")
    FString BoundarySystemData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System Data")
    FString InvestigationSystemData;

    // Achievements & Progress
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Data")
    TArray<FAchievementDefinition> Achievements;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Data")
    TArray<FGameCheckpoint> Checkpoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress Data")
    TMap<EProgressTrackingType, float> ProgressTracking;

    // Player Preferences
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Data")
    TMap<FString, FString> PlayerPreferences;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Data")
    TMap<FString, float> GameSettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Data")
    TArray<FString> UnlockedContent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Data")
    TMap<FString, FString> CustomUserData;
};

/**
 * Progress tracking result structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FProgressResult
{
    GENERATED_BODY()

    FProgressResult()
    {
        Type = EProgressTrackingType::Overall;
        CurrentValue = 0.0f;
        MaxValue = 100.0f;
        ProgressPercentage = 0.0f;
        bIsCompleted = false;
        bIsNewRecord = false;
        LastUpdate = FDateTime::Now();
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    EProgressTrackingType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    float CurrentValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    float MaxValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    float ProgressPercentage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    bool bIsCompleted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    bool bIsNewRecord;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    FDateTime LastUpdate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    TArray<FString> RecentMilestones;
};
