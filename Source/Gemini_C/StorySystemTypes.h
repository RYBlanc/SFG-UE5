// Project Visible - Story System Types
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StorySystemTypes.generated.h"

/**
 * Story progression phases
 */
UENUM(BlueprintType)
enum class EStoryPhase : uint8
{
    Introduction    UMETA(DisplayName = "Introduction"),          // イントロダクション
    Chapter1        UMETA(DisplayName = "Chapter 1"),            // 第1章：東京での出会い
    Chapter2        UMETA(DisplayName = "Chapter 2"),            // 第2章：軽井沢の記憶
    Chapter3        UMETA(DisplayName = "Chapter 3"),            // 第3章：香川の実験
    Chapter4        UMETA(DisplayName = "Chapter 4"),            // 第4章：山形の真実
    Chapter5        UMETA(DisplayName = "Chapter 5"),            // 第5章：境界崩壊
    Epilogue        UMETA(DisplayName = "Epilogue"),             // エピローグ
    Completed       UMETA(DisplayName = "Completed")             // 完了
};

/**
 * Story locations in 2089 Japan
 */
UENUM(BlueprintType)
enum class EStoryLocation : uint8
{
    Tokyo           UMETA(DisplayName = "Tokyo"),               // 東京：現代的なアンドロイド社会
    Karuizawa       UMETA(DisplayName = "Karuizawa"),           // 軽井沢：記憶の別荘地
    Kagawa          UMETA(DisplayName = "Kagawa"),              // 香川：実験施設
    Yamagata        UMETA(DisplayName = "Yamagata"),            // 山形：ニコマコスの隠れ家
    DreamLandscape  UMETA(DisplayName = "Dream Landscape"),     // 夢の精神地形
    BoundaryZone    UMETA(DisplayName = "Boundary Zone")        // 境界崩壊エリア
};

/**
 * Story character types
 */
UENUM(BlueprintType)
enum class ECharacterType : uint8
{
    Protagonist     UMETA(DisplayName = "Protagonist"),         // 主人公（中流アンドロイド）
    Nicomachos      UMETA(DisplayName = "Nicomachos"),          // ニコマコス（謎の研究者）
    Android         UMETA(DisplayName = "Android"),             // 一般アンドロイド
    AI              UMETA(DisplayName = "AI"),                  // AI存在
    Memory          UMETA(DisplayName = "Memory"),              // 記憶の残像
    Experiment      UMETA(DisplayName = "Experiment"),          // 実験対象
    Guardian        UMETA(DisplayName = "Guardian")             // 境界の守護者
};

/**
 * Dialogue choice types based on virtue system
 */
UENUM(BlueprintType)
enum class EDialogueChoiceType : uint8
{
    Question        UMETA(DisplayName = "Question"),            // 質問（知恵）
    Accusation      UMETA(DisplayName = "Accusation"),          // 告発（正義）
    Persuasion      UMETA(DisplayName = "Persuasion"),          // 説得（勇気）
    Intimidation    UMETA(DisplayName = "Intimidation"),        // 威嚇（勇気）
    Empathy         UMETA(DisplayName = "Empathy"),             // 共感（節制）
    Logic           UMETA(DisplayName = "Logic"),               // 論理（知恵）
    Silence         UMETA(DisplayName = "Silence"),             // 沈黙（節制）
    Virtue          UMETA(DisplayName = "Virtue")               // 徳に基づく選択
};

/**
 * Story event types
 */
UENUM(BlueprintType)
enum class EStoryEventType : uint8
{
    Investigation   UMETA(DisplayName = "Investigation"),       // 調査イベント
    Dialogue        UMETA(DisplayName = "Dialogue"),            // 対話イベント
    Discovery       UMETA(DisplayName = "Discovery"),           // 発見イベント
    Revelation      UMETA(DisplayName = "Revelation"),          // 真実の発覚
    Experiment      UMETA(DisplayName = "Experiment"),          // 社会実験
    DreamSequence   UMETA(DisplayName = "Dream Sequence"),      // 夢シーケンス
    BoundaryShift   UMETA(DisplayName = "Boundary Shift"),      // 境界変化
    MemoryFlash     UMETA(DisplayName = "Memory Flash"),        // 記憶のフラッシュ
    ChoicePoint     UMETA(DisplayName = "Choice Point"),        // 重要な選択
    Climax          UMETA(DisplayName = "Climax")               // クライマックス
};

/**
 * Story investigation case structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FStoryInvestigationCase
{
    GENERATED_BODY()

    FStoryInvestigationCase()
    {
        CaseID = 0;
        CaseName = TEXT("");
        Description = TEXT("");
        Location = EStoryLocation::Tokyo;
        IsCompleted = false;
        ProgressPercentage = 0.0f;
        TimeSpentMinutes = 0.0f;
        DifficultyLevel = 1;
        RequiredEvidenceCount = 0;
        CollectedEvidenceCount = 0;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    int32 CaseID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    FString CaseName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    EStoryLocation Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    bool IsCompleted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    float ProgressPercentage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    float TimeSpentMinutes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    int32 DifficultyLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    int32 RequiredEvidenceCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    int32 CollectedEvidenceCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    TArray<FString> RequiredClues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    TArray<FString> CollectedClues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    TArray<FString> Hypotheses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    FString ConclusionText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    TMap<FString, FString> CaseData;
};

/**
 * Story character data
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FStoryCharacter
{
    GENERATED_BODY()

    FStoryCharacter()
    {
        CharacterID = TEXT("");
        CharacterName = TEXT("");
        DisplayName = TEXT("");
        CharacterType = ECharacterType::Android;
        CurrentLocation = EStoryLocation::Tokyo;
        TrustLevel = 50.0f;
        EmotionalState = 50.0f;
        IsAlive = true;
        IsRevealed = false;
        KnowledgeLevel = 0;
        ImportanceRating = 1;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    FString CharacterID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    FString CharacterName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    FString DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    ECharacterType CharacterType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    EStoryLocation CurrentLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    float TrustLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    float EmotionalState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    bool IsAlive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    bool IsRevealed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    int32 KnowledgeLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    int32 ImportanceRating;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    FString BackgroundStory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    TArray<FString> Secrets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    TArray<FString> Relationships;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    TMap<FString, FString> CharacterTraits;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    TArray<FString> DialogueHistory;
};

/**
 * Story dialogue structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FStoryDialogue
{
    GENERATED_BODY()

    FStoryDialogue()
    {
        DialogueID = TEXT("");
        SpeakerID = TEXT("");
        ListenerID = TEXT("");
        ChoiceType = EDialogueChoiceType::Question;
        EmotionalWeight = 50.0f;
        TrustImpact = 0.0f;
        VirtueAlignment = TEXT("");
        IsRepeatable = false;
        HasConsequences = false;
        RequiredKnowledge = 0;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FString DialogueID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FString SpeakerID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FString ListenerID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FString DialogueText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    EDialogueChoiceType ChoiceType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    float EmotionalWeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    float TrustImpact;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FString VirtueAlignment;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    bool IsRepeatable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    bool HasConsequences;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    int32 RequiredKnowledge;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TArray<FString> ChoiceOptions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TArray<FString> Responses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TMap<FString, FString> ConditionalResponses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TArray<FString> UnlockedClues;
};

/**
 * Story chapter structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FStoryChapter
{
    GENERATED_BODY()

    FStoryChapter()
    {
        ChapterID = 0;
        ChapterTitle = TEXT("");
        ChapterPhase = EStoryPhase::Introduction;
        PrimaryLocation = EStoryLocation::Tokyo;
        IsCompleted = false;
        CompletionPercentage = 0.0f;
        PlayTimeMinutes = 0.0f;
        ExpectedDurationMinutes = 60.0f;
        DifficultyRating = 1;
        RequiredCases = 0;
        CompletedCases = 0;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    int32 ChapterID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    FString ChapterTitle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    FString ChapterDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    EStoryPhase ChapterPhase;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    EStoryLocation PrimaryLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    bool IsCompleted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    float CompletionPercentage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    float PlayTimeMinutes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    float ExpectedDurationMinutes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    int32 DifficultyRating;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    int32 RequiredCases;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    int32 CompletedCases;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    TArray<FStoryInvestigationCase> Cases;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    TArray<FStoryCharacter> Characters;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    TArray<FString> KeyEvents;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    TArray<FString> Objectives;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    TMap<FString, FString> ChapterData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chapter")
    TArray<FString> UnlockedLocations;
};

/**
 * Story progression save data
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FStoryProgress
{
    GENERATED_BODY()

    FStoryProgress()
    {
        CurrentPhase = EStoryPhase::Introduction;
        CurrentLocation = EStoryLocation::Tokyo;
        CurrentChapterID = 0;
        TotalPlayTimeMinutes = 0.0f;
        OverallCompletionPercentage = 0.0f;
        TotalChoicesMade = 0;
        TotalCasesCompleted = 0;
        TotalDialoguesCompleted = 0;
        ExperimentsCompleted = 0;
        TruthDiscoveryPercentage = 0.0f;
        SaveGameVersion = TEXT("1.0");
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    EStoryPhase CurrentPhase;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    EStoryLocation CurrentLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    int32 CurrentChapterID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    float TotalPlayTimeMinutes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    float OverallCompletionPercentage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    int32 TotalChoicesMade;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    int32 TotalCasesCompleted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    int32 TotalDialoguesCompleted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    int32 ExperimentsCompleted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    float TruthDiscoveryPercentage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    FString SaveGameVersion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    TArray<FStoryChapter> CompletedChapters;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    TArray<FString> UnlockedSecrets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    TArray<FString> DiscoveredTruths;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    TMap<FString, FString> PlayerChoices;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    TMap<FString, float> CharacterRelationships;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    TArray<FString> CollectedEvidence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
    FDateTime LastSaveTime;
};
