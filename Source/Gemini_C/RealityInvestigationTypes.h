// Project Visible - Reality Investigation System Types
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RealityInvestigationTypes.generated.h"

/**
 * Types of evidence that can be collected
 */
UENUM(BlueprintType)
enum class EEvidenceType : uint8
{
    Physical        UMETA(DisplayName = "Physical Evidence"),
    Digital         UMETA(DisplayName = "Digital Evidence"),
    Testimonial     UMETA(DisplayName = "Testimonial Evidence"),
    Photographic    UMETA(DisplayName = "Photographic Evidence"),
    Audio           UMETA(DisplayName = "Audio Evidence"),
    Document        UMETA(DisplayName = "Document Evidence"),
    Environmental   UMETA(DisplayName = "Environmental Evidence")
};

/**
 * Investigation states
 */
UENUM(BlueprintType)
enum class EInvestigationState : uint8
{
    NotStarted      UMETA(DisplayName = "Not Started"),
    InProgress      UMETA(DisplayName = "In Progress"),
    Completed       UMETA(DisplayName = "Completed"),
    Failed          UMETA(DisplayName = "Failed"),
    Abandoned       UMETA(DisplayName = "Abandoned")
};

/**
 * Types of clues that can be discovered
 */
UENUM(BlueprintType)
enum class EClueType : uint8
{
    Primary         UMETA(DisplayName = "Primary Clue"),
    Secondary       UMETA(DisplayName = "Secondary Clue"),
    RedHerring      UMETA(DisplayName = "Red Herring"),
    Connecting      UMETA(DisplayName = "Connecting Clue"),
    Background      UMETA(DisplayName = "Background Information")
};

/**
 * Dialogue options and conversation states
 */
UENUM(BlueprintType)
enum class EDialogueOption : uint8
{
    Inquiry         UMETA(DisplayName = "Inquiry"),
    Accusation      UMETA(DisplayName = "Accusation"),
    Persuasion      UMETA(DisplayName = "Persuasion"),
    Intimidation    UMETA(DisplayName = "Intimidation"),
    Empathy         UMETA(DisplayName = "Empathy"),
    Logic           UMETA(DisplayName = "Logic")
};

/**
 * NPC emotional states
 */
UENUM(BlueprintType)
enum class ENPCEmotionalState : uint8
{
    Neutral         UMETA(DisplayName = "Neutral"),
    Cooperative     UMETA(DisplayName = "Cooperative"),
    Suspicious      UMETA(DisplayName = "Suspicious"),
    Hostile         UMETA(DisplayName = "Hostile"),
    Fearful         UMETA(DisplayName = "Fearful"),
    Guilty          UMETA(DisplayName = "Guilty"),
    Confident       UMETA(DisplayName = "Confident")
};

/**
 * Evidence item structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FEvidenceItem
{
    GENERATED_BODY()

    FEvidenceItem()
    {
        EvidenceID = 0;
        EvidenceType = EEvidenceType::Physical;
        ReliabilityScore = 100.0f;
        bIsExamined = false;
        bIsAnalyzed = false;
        DiscoveryTimestamp = FDateTime::Now();
        ConnectionWeight = 1.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evidence")
    int32 EvidenceID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evidence")
    FString EvidenceName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evidence")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evidence")
    EEvidenceType EvidenceType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evidence")
    float ReliabilityScore;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evidence")
    FVector LocationFound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evidence")
    bool bIsExamined;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evidence")
    bool bIsAnalyzed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evidence")
    FDateTime DiscoveryTimestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evidence")
    TArray<int32> ConnectedEvidenceIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evidence")
    float ConnectionWeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evidence")
    TMap<FString, FString> Metadata;
};

/**
 * Clue information structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FClueData
{
    GENERATED_BODY()

    FClueData()
    {
        ClueID = 0;
        ClueType = EClueType::Primary;
        Importance = 50.0f;
        bIsRevealed = false;
        bIsConnected = false;
        RequiredEvidenceCount = 1;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clue")
    int32 ClueID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clue")
    FString ClueName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clue")
    FString ClueDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clue")
    EClueType ClueType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clue")
    float Importance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clue")
    bool bIsRevealed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clue")
    bool bIsConnected;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clue")
    TArray<int32> RequiredEvidenceIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clue")
    int32 RequiredEvidenceCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clue")
    TArray<int32> UnlockedClueIDs;
};

/**
 * Investigation case structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FInvestigationCase
{
    GENERATED_BODY()

    FInvestigationCase()
    {
        CaseID = 0;
        State = EInvestigationState::NotStarted;
        Priority = 50;
        StartTime = FDateTime::Now();
        ProgressPercentage = 0.0f;
        DifficultyLevel = 1;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    int32 CaseID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    FString CaseTitle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    FString CaseDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    EInvestigationState State;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    int32 Priority;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    FDateTime StartTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    FDateTime EndTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    float ProgressPercentage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    int32 DifficultyLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    TArray<FEvidenceItem> Evidence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    TArray<FClueData> Clues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    TArray<FString> Suspects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    TArray<FString> Witnesses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Case")
    TMap<FString, FString> CaseNotes;
};

/**
 * NPC dialogue data structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FNPCDialogueData
{
    GENERATED_BODY()

    FNPCDialogueData()
    {
        NPCID = 0;
        EmotionalState = ENPCEmotionalState::Neutral;
        TrustLevel = 50.0f;
        KnowledgeLevel = 50.0f;
        bIsInterrogated = false;
        ConversationCount = 0;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    int32 NPCID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    FString NPCName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    ENPCEmotionalState EmotionalState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    float TrustLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    float KnowledgeLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    bool bIsInterrogated;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    int32 ConversationCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    TArray<FString> DialogueHistory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    TArray<FString> KnownInformation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    TMap<EDialogueOption, float> ResponsePreferences;
};

/**
 * Investigation location structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FInvestigationLocation
{
    GENERATED_BODY()

    FInvestigationLocation()
    {
        LocationID = 0;
        bIsExplored = false;
        bIsAccessible = true;
        ExplorationProgress = 0.0f;
        DangerLevel = 0;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    int32 LocationID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    FString LocationName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    FString LocationDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    FVector WorldPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    bool bIsExplored;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    bool bIsAccessible;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    float ExplorationProgress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    int32 DangerLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    TArray<FEvidenceItem> AvailableEvidence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    TArray<FString> InteractableObjects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
    TArray<int32> PresentNPCIDs;
};
