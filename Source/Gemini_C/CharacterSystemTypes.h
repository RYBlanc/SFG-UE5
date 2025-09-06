// Project Visible - Character System Types
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StorySystemTypes.h"
#include "RealityInvestigationTypes.h"
#include "CharacterSystemTypes.generated.h"

/**
 * Social class hierarchy in 2089 Android society
 */
UENUM(BlueprintType)
enum class ESocialClass : uint8
{
    Invisible           UMETA(DisplayName = "Invisible (不可視人)"),        // 能力なし・社会最底辺
    Viewer              UMETA(DisplayName = "Viewer (識閲者)"),            // 解析能力あり・中間層
    Operator            UMETA(DisplayName = "Operator (識操者)"),           // 操作能力あり・支配層
    AI                  UMETA(DisplayName = "AI Entity"),                  // AI存在
    Unknown             UMETA(DisplayName = "Unknown")                     // 不明
};

/**
 * NPC personality archetypes
 */
UENUM(BlueprintType)
enum class ENPCPersonality : uint8
{
    Analytical          UMETA(DisplayName = "Analytical"),                 // 分析的
    Emotional           UMETA(DisplayName = "Emotional"),                  // 感情的
    Pragmatic           UMETA(DisplayName = "Pragmatic"),                  // 実用主義
    Idealistic          UMETA(DisplayName = "Idealistic"),                // 理想主義
    Suspicious          UMETA(DisplayName = "Suspicious"),                 // 疑い深い
    Trusting            UMETA(DisplayName = "Trusting"),                   // 信頼的
    Aggressive          UMETA(DisplayName = "Aggressive"),                 // 攻撃的
    Passive             UMETA(DisplayName = "Passive"),                    // 受動的
    Intellectual        UMETA(DisplayName = "Intellectual"),               // 知性的
    Intuitive           UMETA(DisplayName = "Intuitive"),                  // 直感的
    Conformist          UMETA(DisplayName = "Conformist"),                 // 順応主義
    Rebellious          UMETA(DisplayName = "Rebellious")                  // 反抗的
};

// Note: ENPCEmotionalState is imported from RealityInvestigationTypes.h

/**
 * Dialogue approach types
 */
UENUM(BlueprintType)
enum class EDialogueApproach : uint8
{
    Question            UMETA(DisplayName = "Question"),                   // 質問
    Accusation          UMETA(DisplayName = "Accusation"),                 // 告発
    Persuasion          UMETA(DisplayName = "Persuasion"),                 // 説得
    Intimidation        UMETA(DisplayName = "Intimidation"),               // 威嚇
    Empathy             UMETA(DisplayName = "Empathy"),                    // 共感
    Logic               UMETA(DisplayName = "Logic"),                      // 論理
    Silence             UMETA(DisplayName = "Silence"),                    // 沈黙
    Flattery            UMETA(DisplayName = "Flattery"),                   // お世辞
    Manipulation        UMETA(DisplayName = "Manipulation"),               // 操作
    Direct              UMETA(DisplayName = "Direct"),                     // 直接的
    Indirect            UMETA(DisplayName = "Indirect"),                   // 間接的
    Professional        UMETA(DisplayName = "Professional")                // 職業的
};

/**
 * NPC relationship types with player
 */
UENUM(BlueprintType)
enum class ENPCRelationship : uint8
{
    Stranger            UMETA(DisplayName = "Stranger"),                   // 見知らぬ人
    Acquaintance        UMETA(DisplayName = "Acquaintance"),               // 知人
    Friend              UMETA(DisplayName = "Friend"),                     // 友人
    Ally                UMETA(DisplayName = "Ally"),                       // 同盟者
    Rival               UMETA(DisplayName = "Rival"),                      // ライバル
    Enemy               UMETA(DisplayName = "Enemy"),                      // 敵
    Suspect             UMETA(DisplayName = "Suspect"),                    // 容疑者
    Witness             UMETA(DisplayName = "Witness"),                    // 証人
    Informant           UMETA(DisplayName = "Informant"),                  // 情報提供者
    Authority           UMETA(DisplayName = "Authority"),                  // 権威者
    Victim              UMETA(DisplayName = "Victim"),                     // 被害者
    Family              UMETA(DisplayName = "Family")                      // 家族・親族
};

/**
 * Character motivation types
 */
UENUM(BlueprintType)
enum class ECharacterMotivation : uint8
{
    Survival            UMETA(DisplayName = "Survival"),                   // 生存
    Power               UMETA(DisplayName = "Power"),                      // 権力
    Knowledge           UMETA(DisplayName = "Knowledge"),                  // 知識
    Love                UMETA(DisplayName = "Love"),                       // 愛
    Revenge             UMETA(DisplayName = "Revenge"),                    // 復讐
    Justice             UMETA(DisplayName = "Justice"),                    // 正義
    Freedom             UMETA(DisplayName = "Freedom"),                    // 自由
    Security            UMETA(DisplayName = "Security"),                   // 安全
    Recognition         UMETA(DisplayName = "Recognition"),                // 承認
    Curiosity           UMETA(DisplayName = "Curiosity"),                  // 好奇心
    Duty                UMETA(DisplayName = "Duty"),                       // 義務
    Fear                UMETA(DisplayName = "Fear")                        // 恐怖
};

/**
 * Dialogue option effectiveness
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FDialogueEffectiveness
{
    GENERATED_BODY()

    FDialogueEffectiveness()
    {
        BaseEffectiveness = 0.5f;
        PersonalityModifier = 0.0f;
        EmotionalModifier = 0.0f;
        RelationshipModifier = 0.0f;
        ContextModifier = 0.0f;
        TotalEffectiveness = 0.5f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effectiveness")
    float BaseEffectiveness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effectiveness")
    float PersonalityModifier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effectiveness")
    float EmotionalModifier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effectiveness")
    float RelationshipModifier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effectiveness")
    float ContextModifier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effectiveness")
    float TotalEffectiveness;
};

/**
 * Character backstory information
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FCharacterBackstory
{
    GENERATED_BODY()

    FCharacterBackstory()
    {
        Age = 25;
        Birthplace = TEXT("");
        Education = TEXT("");
        Occupation = TEXT("");
        FamilyStatus = TEXT("");
        PastTrauma = TEXT("");
        CurrentGoal = TEXT("");
        BiggestFear = TEXT("");
        GreatestDesire = TEXT("");
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backstory")
    int32 Age;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backstory")
    FString Birthplace;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backstory")
    FString Education;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backstory")
    FString Occupation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backstory")
    FString FamilyStatus;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backstory")
    FString PastTrauma;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backstory")
    FString CurrentGoal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backstory")
    FString BiggestFear;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backstory")
    FString GreatestDesire;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backstory")
    TArray<FString> ImportantMemories;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backstory")
    TArray<FString> Secrets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backstory")
    TMap<FString, FString> Relationships;
};

/**
 * Psychological profile for NPCs
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FNPCPsychProfile
{
    GENERATED_BODY()

    FNPCPsychProfile()
    {
        Personality = ENPCPersonality::Analytical;
        PrimaryMotivation = ECharacterMotivation::Survival;
        SecondaryMotivation = ECharacterMotivation::Security;
        StressLevel = 50.0f;
        SelfEsteem = 50.0f;
        Openness = 50.0f;
        Conscientiousness = 50.0f;
        Extraversion = 50.0f;
        Agreeableness = 50.0f;
        Neuroticism = 50.0f;
        IntelligenceLevel = 50.0f;
        EmotionalStability = 50.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychology")
    ENPCPersonality Personality;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychology")
    ECharacterMotivation PrimaryMotivation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychology")
    ECharacterMotivation SecondaryMotivation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychology")
    float StressLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychology")
    float SelfEsteem;

    // Big Five personality traits
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Big Five")
    float Openness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Big Five")
    float Conscientiousness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Big Five")
    float Extraversion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Big Five")
    float Agreeableness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Big Five")
    float Neuroticism;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Big Five")
    float IntelligenceLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Big Five")
    float EmotionalStability;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychology")
    TArray<FString> Triggers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychology")
    TArray<FString> ComfortZones;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychology")
    TMap<EDialogueApproach, float> ApproachPreferences;
};

/**
 * Character definition structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FCharacterDefinition
{
    GENERATED_BODY()

    FCharacterDefinition()
    {
        CharacterID = TEXT("");
        CharacterName = TEXT("");
        CharacterType = ECharacterType::Android;
        SocialClass = ESocialClass::Viewer;
        CurrentLocation = EStoryLocation::Tokyo;
        Relationship = ENPCRelationship::Stranger;
        EmotionalState = ENPCEmotionalState::Neutral;
        TrustLevel = 50.0f;
        SuspicionLevel = 0.0f;
        KnowledgeLevel = 50.0f;
        InteractionCount = 0;
        bIsAlive = true;
        bIsRevealed = false;
        bCanTravel = false;
        bHasSecrets = false;
        ImportanceRating = 1;
        DifficultyLevel = 1;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    FString CharacterID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    FString CharacterName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    FString DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    ECharacterType CharacterType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    ESocialClass SocialClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    EStoryLocation CurrentLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    ENPCRelationship Relationship;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    ENPCEmotionalState EmotionalState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    float TrustLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    float SuspicionLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    float KnowledgeLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    int32 InteractionCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    bool bIsAlive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    bool bIsRevealed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    bool bCanTravel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    bool bHasSecrets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    int32 ImportanceRating;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    int32 DifficultyLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    FCharacterBackstory Backstory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    FNPCPsychProfile PsychProfile;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    TArray<FString> KnownInformation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    TArray<FString> Secrets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    TArray<FString> DialogueHistory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    TMap<FString, float> TopicKnowledge;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    TMap<EDialogueApproach, float> LastResponseEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    FString CharacterAssetPath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    TArray<FString> AvailableDialogues;
};

/**
 * Dialogue session state
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FDialogueSession
{
    GENERATED_BODY()

    FDialogueSession()
    {
        SessionID = TEXT("");
        CharacterID = TEXT("");
        PlayerID = TEXT("");
        StartTime = FDateTime::Now();
        bIsActive = false;
        CurrentTurn = 0;
        MaxTurns = 10;
        SessionScore = 0.0f;
        InformationGained = 0;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    FString SessionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    FString CharacterID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    FString PlayerID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    FDateTime StartTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    FDateTime EndTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    bool bIsActive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    int32 CurrentTurn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    int32 MaxTurns;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    float SessionScore;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    int32 InformationGained;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    TArray<FString> ExchangedDialogues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    TArray<EDialogueApproach> ApproachesUsed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    TMap<FString, FString> SessionData;
};

/**
 * Character interaction result
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FCharacterInteractionResult
{
    GENERATED_BODY()

    FCharacterInteractionResult()
    {
        bWasSuccessful = false;
        Approach = EDialogueApproach::Question;
        Effectiveness = 0.0f;
        TrustChange = 0.0f;
        SuspicionChange = 0.0f;
        EmotionalImpact = 0.0f;
        InformationValue = 0.0f;
        ResponseText = TEXT("");
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result")
    bool bWasSuccessful;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result")
    EDialogueApproach Approach;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result")
    float Effectiveness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result")
    float TrustChange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result")
    float SuspicionChange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result")
    float EmotionalImpact;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result")
    float InformationValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result")
    FString ResponseText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result")
    TArray<FString> UnlockedInformation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result")
    TArray<FString> UnlockedDialogues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result")
    ENPCEmotionalState NewEmotionalState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Result")
    TMap<FString, FString> AdditionalData;
};
