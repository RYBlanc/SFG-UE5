// Project Visible - Character Manager
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/World.h"
#include "CharacterSystemTypes.h"
#include "CharacterManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterRegistered, const FCharacterDefinition&, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterDialogueStarted, const FString&, CharacterID, const FDialogueSession&, Session);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterDialogueEnded, const FString&, CharacterID, const FCharacterInteractionResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCharacterStateChanged, const FString&, CharacterID, ENPCEmotionalState, OldState, ENPCEmotionalState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTrustChanged, const FString&, CharacterID, float, NewTrustLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInformationRevealed, const FString&, CharacterID, const FString&, Information);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRelationshipChanged, const FString&, CharacterID, ENPCRelationship, NewRelationship);

/**
 * Central manager for Project Visible's character system and NPC interactions
 * Manages character definitions, dialogue sessions, psychological profiles, and relationship dynamics
 */
UCLASS()
class GEMINI_C_API UCharacterManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Character System Initialization
    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void InitializeCharacterSystem();

    // Character Registration & Management
    UFUNCTION(BlueprintCallable, Category = "Character Management")
    FString RegisterCharacter(const FCharacterDefinition& Character);

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void UnregisterCharacter(const FString& CharacterID);

    UFUNCTION(BlueprintCallable, Category = "Character Management", BlueprintPure)
    bool GetCharacter(const FString& CharacterID, FCharacterDefinition& OutCharacter) const;

    UFUNCTION(BlueprintCallable, Category = "Character Management", BlueprintPure)
    TArray<FCharacterDefinition> GetAllCharacters() const;

    UFUNCTION(BlueprintCallable, Category = "Character Management", BlueprintPure)
    TArray<FCharacterDefinition> GetCharactersByLocation(EStoryLocation Location) const;

    UFUNCTION(BlueprintCallable, Category = "Character Management", BlueprintPure)
    TArray<FCharacterDefinition> GetCharactersByClass(ESocialClass SocialClass) const;

    UFUNCTION(BlueprintCallable, Category = "Character Management", BlueprintPure)
    TArray<FCharacterDefinition> GetCharactersByType(ECharacterType CharacterType) const;

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void MoveCharacterToLocation(const FString& CharacterID, EStoryLocation NewLocation);

    // Dialogue System
    UFUNCTION(BlueprintCallable, Category = "Character Management")
    FString StartDialogueSession(const FString& CharacterID, const FString& PlayerID);

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    FCharacterInteractionResult ProcessDialogueApproach(const FString& SessionID, EDialogueApproach Approach, 
                                                       const FString& DialogueText);

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void EndDialogueSession(const FString& SessionID);

    UFUNCTION(BlueprintCallable, Category = "Character Management", BlueprintPure)
    bool IsDialogueActive(const FString& CharacterID) const;

    UFUNCTION(BlueprintCallable, Category = "Character Management", BlueprintPure)
    bool GetActiveDialogueSession(const FString& CharacterID, FDialogueSession& OutSession) const;

    UFUNCTION(BlueprintCallable, Category = "Character Management", BlueprintPure)
    TArray<EDialogueApproach> GetRecommendedApproaches(const FString& CharacterID) const;

    // Character State Management
    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void UpdateCharacterTrust(const FString& CharacterID, float TrustChange);

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void UpdateCharacterSuspicion(const FString& CharacterID, float SuspicionChange);

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void UpdateCharacterEmotion(const FString& CharacterID, ENPCEmotionalState NewState);

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void UpdateCharacterRelationship(const FString& CharacterID, ENPCRelationship NewRelationship);

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void AddCharacterKnowledge(const FString& CharacterID, const FString& Topic, float KnowledgeLevel);

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void RevealCharacterSecret(const FString& CharacterID, const FString& SecretID);

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void AddCharacterMemory(const FString& CharacterID, const FString& Memory);

    // Psychological Analysis
    UFUNCTION(BlueprintCallable, Category = "Character Management")
    FDialogueEffectiveness CalculateDialogueEffectiveness(const FString& CharacterID, EDialogueApproach Approach) const;

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void UpdatePsychologicalProfile(const FString& CharacterID, const FNPCPsychProfile& NewProfile);

    UFUNCTION(BlueprintCallable, Category = "Character Management", BlueprintPure)
    FNPCPsychProfile GetPsychologicalProfile(const FString& CharacterID) const;

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    ENPCEmotionalState PredictEmotionalReaction(const FString& CharacterID, EDialogueApproach Approach) const;

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    float CalculateStressLevel(const FString& CharacterID) const;

    // Social Class Interactions
    UFUNCTION(BlueprintCallable, Category = "Character Management")
    float GetClassInteractionModifier(ESocialClass PlayerClass, ESocialClass NPCClass) const;

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void ProcessClassBasedBias(const FString& CharacterID, ESocialClass PlayerClass);

    UFUNCTION(BlueprintCallable, Category = "Character Management", BlueprintPure)
    TArray<FString> GetClassConflicts(ESocialClass PlayerClass) const;

    // Information & Intelligence
    UFUNCTION(BlueprintCallable, Category = "Character Management")
    TArray<FString> ExtractInformation(const FString& CharacterID, EDialogueApproach Approach);

    UFUNCTION(BlueprintCallable, Category = "Character Management", BlueprintPure)
    float GetInformationValue(const FString& CharacterID, const FString& Topic) const;

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    bool DetectDeception(const FString& CharacterID, const FString& Statement) const;

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void ShareInformationBetweenCharacters(const FString& FromCharacterID, const FString& ToCharacterID, 
                                          const FString& Information);

    // Character Progression
    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void UpdateCharacterDevelopment(const FString& CharacterID, const FString& DevelopmentType, float Amount);

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void TriggerCharacterEvent(const FString& CharacterID, const FString& EventType, const FString& EventData);

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void ProcessCharacterArc(const FString& CharacterID, float StoryProgress);

    // Relationship Dynamics
    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void BuildRelationshipMatrix();

    UFUNCTION(BlueprintCallable, Category = "Character Management", BlueprintPure)
    TArray<FString> GetRelatedCharacters(const FString& CharacterID) const;

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void UpdateRelationshipDynamics(const FString& CharacterID, const FString& OtherCharacterID, float Change);

    // AI Behavior
    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void UpdateCharacterAI(const FString& CharacterID, float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void SetCharacterGoal(const FString& CharacterID, const FString& Goal, float Priority);

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void ProcessCharacterDecision(const FString& CharacterID, const TArray<FString>& Options);

    // Data Management & Analytics
    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void SaveCharacterData();

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void LoadCharacterData();

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void AnalyzeCharacterInteractions();

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void RecordCharacterAnalytics(const FString& EventName, const FString& CharacterID, const FString& EventData);

    // Integration with other systems
    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void SynchronizeWithStorySystem();

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void UpdateCharactersForStoryPhase(EStoryPhase Phase);

    UFUNCTION(BlueprintCallable, Category = "Character Management")
    void HandleStoryEvents(const FString& EventType, const FString& EventData);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Character Events")
    FOnCharacterRegistered OnCharacterRegistered;

    UPROPERTY(BlueprintAssignable, Category = "Character Events")
    FOnCharacterDialogueStarted OnDialogueStarted;

    UPROPERTY(BlueprintAssignable, Category = "Character Events")
    FOnCharacterDialogueEnded OnDialogueEnded;

    UPROPERTY(BlueprintAssignable, Category = "Character Events")
    FOnCharacterStateChanged OnCharacterStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Character Events")
    FOnTrustChanged OnTrustChanged;

    UPROPERTY(BlueprintAssignable, Category = "Character Events")
    FOnInformationRevealed OnInformationRevealed;

    UPROPERTY(BlueprintAssignable, Category = "Character Events")
    FOnRelationshipChanged OnRelationshipChanged;

protected:
    // Core Character Data
    UPROPERTY(BlueprintReadOnly, Category = "Character Data")
    TArray<FCharacterDefinition> RegisteredCharacters;

    UPROPERTY(BlueprintReadOnly, Category = "Character Data")
    TArray<FDialogueSession> ActiveDialogueSessions;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings")
    bool bEnablePsychologicalAnalysis;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings")
    bool bEnableClassBias;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings")
    bool bEnableEmotionalMemory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings")
    float DefaultTrustLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings")
    float TrustDecayRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings")
    int32 MaxDialogueTurns;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings")
    float InformationRevealThreshold;

    // Class Interaction Matrix (individual values for common interactions)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings")
    float InvisibleToViewerModifier = -0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings")
    float ViewerToOperatorModifier = -0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings")
    float OperatorToInvisibleModifier = -0.4f;

    // Runtime State
    UPROPERTY()
    TMap<FString, FDateTime> LastInteractionTimes;

    UPROPERTY()
    TMap<FString, int32> InteractionCounts;

    UPROPERTY()
    TMap<FString, FString> CharacterRelationships; // CharacterID -> Related Characters (comma-separated)

    UPROPERTY()
    int32 NextSessionID;

private:
    // Helper Functions
    void InitializeDefaultCharacters();
    void InitializeClassInteractionMatrix();
    void SetupCharacterAI();

    // Character Management Helpers
    FCharacterDefinition* GetCharacterByID(const FString& CharacterID);
    const FCharacterDefinition* GetCharacterByID(const FString& CharacterID) const;
    void ValidateCharacterData(const FCharacterDefinition& Character);
    void ProcessCharacterUpdate(const FString& CharacterID);

    // Dialogue Helpers
    FDialogueSession* GetDialogueSessionByID(const FString& SessionID);
    const FDialogueSession* GetDialogueSessionByID(const FString& SessionID) const;
    void ProcessDialogueEffects(const FString& CharacterID, const FCharacterInteractionResult& Result);
    void UpdateDialogueHistory(const FString& CharacterID, const FString& DialogueText);

    // Psychology Helpers
    float CalculatePersonalityMatch(const FNPCPsychProfile& Profile, EDialogueApproach Approach) const;
    float CalculateEmotionalResponse(ENPCEmotionalState CurrentState, EDialogueApproach Approach) const;
    void UpdateStressFactors(const FString& CharacterID, EDialogueApproach Approach);
    void ProcessTraumaEffects(const FString& CharacterID, const FString& TriggerType);

    // Social Dynamics Helpers
    void UpdateSocialNetwork(const FString& CharacterID);
    void ProcessGossipSpread(const FString& OriginCharacterID, const FString& Information);
    void CalculateReputation(const FString& CharacterID);

    // Information Helpers
    bool ShouldRevealInformation(const FString& CharacterID, const FString& Information, float TrustLevel) const;
    float CalculateInformationSensitivity(const FString& Information) const;
    void ProcessInformationLeak(const FString& CharacterID, const FString& Information);

    // AI Behavior Helpers
    void UpdatePersonalityBasedBehavior(const FString& CharacterID);
    void ProcessMotivationDrivenActions(const FString& CharacterID);
    void HandleCharacterConflict(const FString& CharacterID1, const FString& CharacterID2);

    // Additional Helper Functions
    FString GenerateDialogueResponse(const FCharacterDefinition* Character, EDialogueApproach Approach, bool bSuccessful) const;

    // Integration Helpers
    class UStoryManager* GetStoryManager();
    class UVirtueManager* GetVirtueManager();
    class UMemoryManager* GetMemoryManager();
    class USocialExperimentManager* GetSocialExperimentManager();
    class ULevelDesignManager* GetLevelDesignManager();
    class UProjectVisibleUIManager* GetUIManager();
};
