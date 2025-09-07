#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MemoryFragmentTypes.generated.h"

/**
 * Type of memory fragment based on psychological memory classification
 */
UENUM(BlueprintType)
enum class EMemoryFragmentType : uint8
{
    Episodic        UMETA(DisplayName = "Episodic"),        // エピソード記憶 - Specific events and experiences
    Semantic        UMETA(DisplayName = "Semantic"),        // 意味記憶 - General knowledge and facts
    Procedural      UMETA(DisplayName = "Procedural"),      // 手続き記憶 - Skills and procedures
    Emotional       UMETA(DisplayName = "Emotional"),       // 感情記憶 - Emotional experiences
    Sensory         UMETA(DisplayName = "Sensory"),         // 感覚記憶 - Sensory impressions
    Conceptual      UMETA(DisplayName = "Conceptual"),      // 概念記憶 - Abstract concepts
    
    MAX_TYPES       UMETA(Hidden)
};

/**
 * Emotional intensity of memory fragment
 */
UENUM(BlueprintType)
enum class EMemoryEmotionalIntensity : uint8
{
    Neutral         UMETA(DisplayName = "Neutral"),         // 中性
    Mild            UMETA(DisplayName = "Mild"),            // 軽度
    Moderate        UMETA(DisplayName = "Moderate"),        // 中度
    Strong          UMETA(DisplayName = "Strong"),          // 強度
    Intense         UMETA(DisplayName = "Intense"),         // 激烈
    
    MAX_INTENSITY   UMETA(Hidden)
};

/**
 * Clarity/accessibility of memory fragment
 */
UENUM(BlueprintType)
enum class EMemoryClarity : uint8
{
    Unclear         UMETA(DisplayName = "Unclear"),         // 不明瞭
    Hazy            UMETA(DisplayName = "Hazy"),            // ぼんやり
    Partial         UMETA(DisplayName = "Partial"),         // 部分的
    Clear           UMETA(DisplayName = "Clear"),           // 明瞭
    Vivid           UMETA(DisplayName = "Vivid"),           // 鮮明
    
    MAX_CLARITY     UMETA(Hidden)
};

/**
 * Memory fragment temporal period
 */
UENUM(BlueprintType)
enum class EMemoryTimePeriod : uint8
{
    Childhood       UMETA(DisplayName = "Childhood"),       // 幼少期
    Adolescence     UMETA(DisplayName = "Adolescence"),     // 思春期
    YoungAdult      UMETA(DisplayName = "Young Adult"),     // 青年期
    Adult           UMETA(DisplayName = "Adult"),           // 成人期
    Recent          UMETA(DisplayName = "Recent"),          // 最近
    Unknown         UMETA(DisplayName = "Unknown"),         // 不明
    
    MAX_PERIODS     UMETA(Hidden)
};

/**
 * Individual memory fragment data structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FMemoryFragment
{
    GENERATED_BODY()

    // Unique identifier for this memory fragment
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Fragment")
    FString FragmentID;

    // Display title of the memory
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Fragment")
    FText Title;

    // Detailed description of the memory
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Fragment")
    FText Description;

    // Short excerpt or key phrase
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Fragment")
    FText Excerpt;

    // Type of memory fragment
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Fragment")
    EMemoryFragmentType FragmentType = EMemoryFragmentType::Episodic;

    // Emotional intensity of the memory
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Fragment")
    EMemoryEmotionalIntensity EmotionalIntensity = EMemoryEmotionalIntensity::Neutral;

    // Clarity/accessibility of the memory
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Fragment")
    EMemoryClarity Clarity = EMemoryClarity::Partial;

    // Time period when memory was formed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Fragment")
    EMemoryTimePeriod TimePeriod = EMemoryTimePeriod::Unknown;

    // Whether this memory has been unlocked/discovered
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Fragment")
    bool bIsUnlocked = false;

    // Whether this memory has been viewed by the player
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Fragment")
    bool bHasBeenViewed = false;

    // Importance/relevance score (0-100)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Fragment", meta = (ClampMin = "0", ClampMax = "100"))
    float ImportanceScore = 50.0f;

    // Associated keywords/tags for searching
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Fragment")
    TArray<FString> Keywords;

    // Related memory fragment IDs
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Fragment")
    TArray<FString> RelatedFragments;

    // Timestamp when memory was unlocked
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Fragment")
    FDateTime UnlockedTimestamp;

    // Location or context where memory was formed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Fragment")
    FString LocationContext;

    // Character or person associated with this memory
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Fragment")
    FString AssociatedCharacter;

    FMemoryFragment()
    {
        FragmentID = TEXT("");
        Title = FText::FromString(TEXT("Untitled Memory"));
        Description = FText::FromString(TEXT("A memory fragment waiting to be discovered."));
        Excerpt = FText::FromString(TEXT("..."));
        FragmentType = EMemoryFragmentType::Episodic;
        EmotionalIntensity = EMemoryEmotionalIntensity::Neutral;
        Clarity = EMemoryClarity::Partial;
        TimePeriod = EMemoryTimePeriod::Unknown;
        bIsUnlocked = false;
        bHasBeenViewed = false;
        ImportanceScore = 50.0f;
        Keywords.Empty();
        RelatedFragments.Empty();
        UnlockedTimestamp = FDateTime::Now();
        LocationContext = TEXT("");
        AssociatedCharacter = TEXT("");
    }
};

/**
 * Memory collection - a group of related memory fragments
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FMemoryCollection
{
    GENERATED_BODY()

    // Collection identifier
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Collection")
    FString CollectionID;

    // Collection display name
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Collection")
    FText CollectionName;

    // Collection description
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Collection")
    FText Description;

    // Memory fragments in this collection
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Collection")
    TArray<FString> FragmentIDs;

    // Whether this collection is unlocked
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Collection")
    bool bIsUnlocked = false;

    // Completion percentage (0-100)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Collection", meta = (ClampMin = "0", ClampMax = "100"))
    float CompletionPercentage = 0.0f;

    // Collection theme/color
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Collection")
    FLinearColor ThemeColor = FLinearColor::White;

    FMemoryCollection()
    {
        CollectionID = TEXT("");
        CollectionName = FText::FromString(TEXT("Untitled Collection"));
        Description = FText::FromString(TEXT("A collection of related memories."));
        FragmentIDs.Empty();
        bIsUnlocked = false;
        CompletionPercentage = 0.0f;
        ThemeColor = FLinearColor(0.6f, 0.7f, 0.8f, 1.0f);
    }
};

/**
 * Memory search filter criteria
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FMemorySearchFilter
{
    GENERATED_BODY()

    // Filter by fragment type
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search Filter")
    TArray<EMemoryFragmentType> FilterTypes;

    // Filter by emotional intensity
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search Filter")
    TArray<EMemoryEmotionalIntensity> FilterIntensities;

    // Filter by clarity
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search Filter")
    TArray<EMemoryClarity> FilterClarities;

    // Filter by time period
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search Filter")
    TArray<EMemoryTimePeriod> FilterTimePeriods;

    // Text search keywords
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search Filter")
    FString SearchKeywords;

    // Only show unlocked memories
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search Filter")
    bool bOnlyUnlocked = true;

    // Only show unviewed memories
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search Filter")
    bool bOnlyUnviewed = false;

    // Minimum importance score
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search Filter", meta = (ClampMin = "0", ClampMax = "100"))
    float MinImportanceScore = 0.0f;

    FMemorySearchFilter()
    {
        FilterTypes.Empty();
        FilterIntensities.Empty();
        FilterClarities.Empty();
        FilterTimePeriods.Empty();
        SearchKeywords = TEXT("");
        bOnlyUnlocked = true;
        bOnlyUnviewed = false;
        MinImportanceScore = 0.0f;
    }
};

/**
 * Memory statistics for UI display
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FMemoryStatistics
{
    GENERATED_BODY()

    // Total number of memory fragments
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Statistics")
    int32 TotalFragments = 0;

    // Number of unlocked fragments
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Statistics")
    int32 UnlockedFragments = 0;

    // Number of viewed fragments
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Statistics")
    int32 ViewedFragments = 0;

    // Total collections
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Statistics")
    int32 TotalCollections = 0;

    // Completed collections
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Statistics")
    int32 CompletedCollections = 0;

    // Overall completion percentage
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Statistics", meta = (ClampMin = "0", ClampMax = "100"))
    float OverallCompletionPercentage = 0.0f;

    FMemoryStatistics()
    {
        TotalFragments = 0;
        UnlockedFragments = 0;
        ViewedFragments = 0;
        TotalCollections = 0;
        CompletedCollections = 0;
        OverallCompletionPercentage = 0.0f;
    }
};
