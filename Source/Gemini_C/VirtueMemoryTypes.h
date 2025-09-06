// Project Visible - Virtue and Memory System Types
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "VirtueMemoryTypes.generated.h"

/**
 * Aristotelian Virtues (Four Cardinal Virtues)
 */
UENUM(BlueprintType)
enum class EVirtueType : uint8
{
    Wisdom          UMETA(DisplayName = "Wisdom (Sophia)"),          // 知恵
    Courage         UMETA(DisplayName = "Courage (Andreia)"),        // 勇気  
    Justice         UMETA(DisplayName = "Justice (Dikaiosyne)"),     // 正義
    Temperance      UMETA(DisplayName = "Temperance (Sophrosyne)")   // 節制
};

/**
 * Memory types and categories
 */
UENUM(BlueprintType)
enum class EMemoryType : uint8
{
    Episodic        UMETA(DisplayName = "Episodic Memory"),          // エピソード記憶
    Semantic        UMETA(DisplayName = "Semantic Memory"),          // 意味記憶
    Procedural      UMETA(DisplayName = "Procedural Memory"),        // 手続き記憶
    Emotional       UMETA(DisplayName = "Emotional Memory"),         // 感情記憶
    Social          UMETA(DisplayName = "Social Memory"),            // 社会記憶
    Moral           UMETA(DisplayName = "Moral Memory"),             // 道徳記憶
    Traumatic       UMETA(DisplayName = "Traumatic Memory")          // トラウマ記憶
};

/**
 * Memory importance levels
 */
UENUM(BlueprintType)
enum class EMemoryImportance : uint8
{
    Trivial         UMETA(DisplayName = "Trivial"),                 // 些細
    Low             UMETA(DisplayName = "Low"),                     // 低
    Medium          UMETA(DisplayName = "Medium"),                  // 中
    High            UMETA(DisplayName = "High"),                    // 高
    Critical        UMETA(DisplayName = "Critical"),                // 重要
    Core            UMETA(DisplayName = "Core Identity")            // 核心的アイデンティティ
};

/**
 * Virtue development states
 */
UENUM(BlueprintType)
enum class EVirtueDevelopment : uint8
{
    Deficient       UMETA(DisplayName = "Deficient"),               // 不足
    Developing      UMETA(DisplayName = "Developing"),              // 発達中
    Moderate        UMETA(DisplayName = "Moderate"),                // 適度
    Strong          UMETA(DisplayName = "Strong"),                  // 強い
    Exemplary       UMETA(DisplayName = "Exemplary"),               // 模範的
    Excessive       UMETA(DisplayName = "Excessive (Vice)")         // 過度（悪徳）
};

/**
 * Player value categories
 */
UENUM(BlueprintType)
enum class EPlayerValue : uint8
{
    Security        UMETA(DisplayName = "Security"),                // 安全
    Achievement     UMETA(DisplayName = "Achievement"),             // 達成
    SelfDirection   UMETA(DisplayName = "Self-Direction"),          // 自己指導
    Stimulation     UMETA(DisplayName = "Stimulation"),             // 刺激
    Hedonism        UMETA(DisplayName = "Hedonism"),                // 快楽主義
    Conformity      UMETA(DisplayName = "Conformity"),              // 適合
    Tradition       UMETA(DisplayName = "Tradition"),               // 伝統
    Benevolence     UMETA(DisplayName = "Benevolence"),             // 博愛
    Universalism    UMETA(DisplayName = "Universalism"),            // 普遍主義
    Power           UMETA(DisplayName = "Power")                    // 権力
};

/**
 * Individual virtue data structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FVirtueData
{
    GENERATED_BODY()

    FVirtueData()
    {
        VirtueType = EVirtueType::Wisdom;
        CurrentLevel = 50.0f;
        DevelopmentState = EVirtueDevelopment::Moderate;
        ExperiencePoints = 0;
        LastUpdated = FDateTime::Now();
        bIsActive = true;
        RecentActions = 0;
        ConsistencyScore = 50.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue")
    EVirtueType VirtueType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float CurrentLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue")
    EVirtueDevelopment DevelopmentState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue")
    int32 ExperiencePoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue")
    FDateTime LastUpdated;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue")
    bool bIsActive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue")
    int32 RecentActions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float ConsistencyScore;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue")
    TArray<FString> RecentExamples;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue")
    TMap<FString, float> SubAttributes;
};

/**
 * Memory entry structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FMemoryEntry
{
    GENERATED_BODY()

    FMemoryEntry()
    {
        MemoryID = 0;
        MemoryType = EMemoryType::Episodic;
        Importance = EMemoryImportance::Medium;
        EmotionalIntensity = 50.0f;
        Clarity = 100.0f;
        LastAccessed = FDateTime::Now();
        CreationTime = FDateTime::Now();
        AccessCount = 0;
        bIsFading = false;
        bIsRepressed = false;
        DecayRate = 1.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    int32 MemoryID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    FString MemoryTitle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    FString MemoryContent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    EMemoryType MemoryType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    EMemoryImportance Importance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float EmotionalIntensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float Clarity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    FDateTime LastAccessed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    FDateTime CreationTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    int32 AccessCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    bool bIsFading;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    bool bIsRepressed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    float DecayRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    TArray<int32> AssociatedMemories;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    TArray<EVirtueType> RelatedVirtues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    TMap<FString, FString> ContextualData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    FVector LocationContext;
};

/**
 * Player value assessment structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FPlayerValueAssessment
{
    GENERATED_BODY()

    FPlayerValueAssessment()
    {
        ValueType = EPlayerValue::Security;
        Strength = 50.0f;
        Consistency = 50.0f;
        RecentTrend = 0.0f;
        LastAssessed = FDateTime::Now();
        SampleSize = 0;
        Confidence = 50.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Values")
    EPlayerValue ValueType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Values", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float Strength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Values", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float Consistency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Values", meta = (ClampMin = "-10.0", ClampMax = "10.0"))
    float RecentTrend;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Values")
    FDateTime LastAssessed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Values")
    int32 SampleSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Values", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float Confidence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Values")
    TArray<FString> SupportingEvidence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Values")
    TMap<FString, float> BehaviorIndicators;
};

/**
 * Virtue action record
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FVirtueAction
{
    GENERATED_BODY()

    FVirtueAction()
    {
        ActionID = 0;
        VirtueType = EVirtueType::Wisdom;
        ActionType = TEXT("Unknown");
        ImpactMagnitude = 0.0f;
        bIsPositive = true;
        Timestamp = FDateTime::Now();
        ContextualWeight = 1.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue Action")
    int32 ActionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue Action")
    EVirtueType VirtueType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue Action")
    FString ActionType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue Action")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue Action", meta = (ClampMin = "0.0", ClampMax = "10.0"))
    float ImpactMagnitude;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue Action")
    bool bIsPositive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue Action")
    FDateTime Timestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue Action")
    float ContextualWeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue Action")
    TArray<EPlayerValue> AffectedValues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue Action")
    TMap<FString, FString> ActionContext;
};

/**
 * Memory capacity and management configuration
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FMemoryCapacityConfig
{
    GENERATED_BODY()

    FMemoryCapacityConfig()
    {
        MaxMemories = 1000;
        MaxDailyMemories = 50;
        DecayThreshold = 0.1f;
        ImportanceMultiplier = 2.0f;
        EmotionalRetentionBonus = 1.5f;
        bUseCapacityLimits = true;
        bAutoCleanup = true;
        CleanupFrequencyHours = 24.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Capacity")
    int32 MaxMemories;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Capacity")
    int32 MaxDailyMemories;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Capacity", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float DecayThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Capacity")
    float ImportanceMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Capacity")
    float EmotionalRetentionBonus;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Capacity")
    bool bUseCapacityLimits;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Capacity")
    bool bAutoCleanup;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Capacity")
    float CleanupFrequencyHours;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Capacity")
    TMap<EMemoryType, float> TypeWeights;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Capacity")
    TMap<EMemoryImportance, float> ImportanceWeights;
};

/**
 * Happiness and well-being metrics
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FHappinessMetrics
{
    GENERATED_BODY()

    FHappinessMetrics()
    {
        OverallHappiness = 50.0f;
        LifeSatisfaction = 50.0f;
        PositiveAffect = 50.0f;
        NegativeAffect = 50.0f;
        Eudaimonia = 50.0f;
        Flow = 50.0f;
        Meaning = 50.0f;
        Engagement = 50.0f;
        LastAssessment = FDateTime::Now();
        SampleSize = 0;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Happiness", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float OverallHappiness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Happiness", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float LifeSatisfaction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Happiness", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float PositiveAffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Happiness", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float NegativeAffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Happiness", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float Eudaimonia;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Happiness", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float Flow;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Happiness", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float Meaning;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Happiness", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float Engagement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Happiness")
    FDateTime LastAssessment;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Happiness")
    int32 SampleSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Happiness")
    TMap<FString, float> DetailedMetrics;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Happiness")
    TArray<FString> RecentInfluences;
};
