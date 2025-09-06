// Project Visible - Story Manager
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/World.h"
#include "StorySystemTypes.h"
#include "StoryManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChapterStarted, const FStoryChapter&, Chapter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChapterCompleted, const FStoryChapter&, Chapter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, EStoryPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLocationChanged, EStoryLocation, NewLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDialogueStarted, const FStoryCharacter&, Character, const FStoryDialogue&, Dialogue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnChoiceMade, const FString&, ChoiceID, EDialogueChoiceType, ChoiceType, const FString&, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCaseCompleted, const FStoryInvestigationCase&, Case);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTruthRevealed, const FString&, TruthID, const FString&, TruthDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoryEventTriggered, EStoryEventType, EventType);

/**
 * Central manager for Project Visible's story progression and narrative systems
 * Manages chapters, characters, dialogues, and player choices
 */
UCLASS()
class GEMINI_C_API UStoryManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Story Progression
    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void InitializeStorySystem();

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void StartChapter(int32 ChapterID);

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void CompleteChapter(int32 ChapterID);

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void AdvanceToNextPhase();

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void SetCurrentLocation(EStoryLocation NewLocation);

    UFUNCTION(BlueprintCallable, Category = "Story Management", BlueprintPure)
    EStoryPhase GetCurrentPhase() const { return CurrentProgress.CurrentPhase; }

    UFUNCTION(BlueprintCallable, Category = "Story Management", BlueprintPure)
    EStoryLocation GetCurrentLocation() const { return CurrentProgress.CurrentLocation; }

    UFUNCTION(BlueprintCallable, Category = "Story Management", BlueprintPure)
    float GetOverallProgress() const { return CurrentProgress.OverallCompletionPercentage; }

    // Chapter Management
    UFUNCTION(BlueprintCallable, Category = "Story Management")
    bool CreateChapter(int32 ChapterID, const FString& Title, const FString& Description, 
                      EStoryPhase Phase, EStoryLocation Location);

    UFUNCTION(BlueprintCallable, Category = "Story Management", BlueprintPure)
    bool GetChapter(int32 ChapterID, FStoryChapter& OutChapter) const;

    UFUNCTION(BlueprintCallable, Category = "Story Management", BlueprintPure)
    FStoryChapter GetCurrentChapter() const;

    UFUNCTION(BlueprintCallable, Category = "Story Management", BlueprintPure)
    TArray<FStoryChapter> GetAvailableChapters() const;

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void UpdateChapterProgress(int32 ChapterID, float ProgressPercentage);

    // Character Management
    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void RegisterCharacter(const FString& CharacterID, const FString& Name, ECharacterType Type, 
                          EStoryLocation Location);

    UFUNCTION(BlueprintCallable, Category = "Story Management", BlueprintPure)
    bool GetCharacter(const FString& CharacterID, FStoryCharacter& OutCharacter) const;

    UFUNCTION(BlueprintCallable, Category = "Story Management", BlueprintPure)
    TArray<FStoryCharacter> GetCharactersAtLocation(EStoryLocation Location) const;

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void UpdateCharacterTrust(const FString& CharacterID, float TrustChange);

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void UpdateCharacterEmotion(const FString& CharacterID, float EmotionalState);

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void RevealCharacterSecret(const FString& CharacterID, const FString& SecretID);

    // Dialogue System
    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void StartDialogue(const FString& CharacterID, const FString& DialogueID);

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void MakeDialogueChoice(const FString& DialogueID, const FString& ChoiceID, 
                           EDialogueChoiceType ChoiceType);

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void EndDialogue(const FString& DialogueID);

    UFUNCTION(BlueprintCallable, Category = "Story Management", BlueprintPure)
    TArray<FStoryDialogue> GetAvailableDialogues(const FString& CharacterID) const;

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void RegisterDialogue(const FString& DialogueID, const FString& SpeakerID, 
                         const FString& Text, EDialogueChoiceType ChoiceType);

    // Investigation System
    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void StartInvestigation(const FString& CaseID, const FString& CaseName, 
                           EStoryLocation Location);

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void CompleteInvestigation(const FString& CaseID, const FString& Conclusion);

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void AddEvidenceToCase(const FString& CaseID, const FString& EvidenceID);

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void AddClueToCase(const FString& CaseID, const FString& ClueID);

    UFUNCTION(BlueprintCallable, Category = "Story Management", BlueprintPure)
    TArray<FStoryInvestigationCase> GetActiveCases() const;

    UFUNCTION(BlueprintCallable, Category = "Story Management", BlueprintPure)
    bool GetCase(const FString& CaseID, FStoryInvestigationCase& OutCase) const;

    // Choice Tracking and Consequences
    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void RecordPlayerChoice(const FString& ChoiceID, const FString& ChoiceText, 
                           const FString& Context);

    UFUNCTION(BlueprintCallable, Category = "Story Management", BlueprintPure)
    TArray<FString> GetPlayerChoiceHistory() const;

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void TriggerChoiceConsequence(const FString& ChoiceID, const FString& ConsequenceText);

    // Truth and Discovery System
    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void RevealTruth(const FString& TruthID, const FString& TruthDescription);

    UFUNCTION(BlueprintCallable, Category = "Story Management", BlueprintPure)
    TArray<FString> GetDiscoveredTruths() const;

    UFUNCTION(BlueprintCallable, Category = "Story Management", BlueprintPure)
    float GetTruthDiscoveryPercentage() const;

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void UnlockSecret(const FString& SecretID, const FString& SecretDescription);

    // Story Events
    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void TriggerStoryEvent(EStoryEventType EventType, const FString& EventData);

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void ScheduleDelayedEvent(EStoryEventType EventType, const FString& EventData, 
                             float DelaySeconds);

    // Save/Load System
    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void SaveStoryProgress();

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void LoadStoryProgress();

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void CreateSaveData(FStoryProgress& OutSaveData);

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void LoadFromSaveData(const FStoryProgress& SaveData);

    // Analytics Integration
    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void RecordStoryAnalytics(const FString& EventName, const FString& EventData);

    UFUNCTION(BlueprintCallable, Category = "Story Management")
    void AnalyzePlayerBehavior();

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Story Events")
    FOnChapterStarted OnChapterStarted;

    UPROPERTY(BlueprintAssignable, Category = "Story Events")
    FOnChapterCompleted OnChapterCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Story Events")
    FOnPhaseChanged OnPhaseChanged;

    UPROPERTY(BlueprintAssignable, Category = "Story Events")
    FOnLocationChanged OnLocationChanged;

    UPROPERTY(BlueprintAssignable, Category = "Story Events")
    FOnDialogueStarted OnDialogueStarted;

    UPROPERTY(BlueprintAssignable, Category = "Story Events")
    FOnChoiceMade OnChoiceMade;

    UPROPERTY(BlueprintAssignable, Category = "Story Events")
    FOnCaseCompleted OnCaseCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Story Events")
    FOnTruthRevealed OnTruthRevealed;

    UPROPERTY(BlueprintAssignable, Category = "Story Events")
    FOnStoryEventTriggered OnStoryEventTriggered;

protected:
    // Core Story Data
    UPROPERTY(BlueprintReadOnly, Category = "Story Data")
    FStoryProgress CurrentProgress;

    UPROPERTY(BlueprintReadOnly, Category = "Story Data")
    TArray<FStoryChapter> Chapters;

    UPROPERTY(BlueprintReadOnly, Category = "Story Data")
    TArray<FStoryCharacter> Characters;

    UPROPERTY(BlueprintReadOnly, Category = "Story Data")
    TArray<FStoryInvestigationCase> Cases;

    UPROPERTY(BlueprintReadOnly, Category = "Story Data")
    TArray<FStoryDialogue> Dialogues;

    // Story Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story Settings")
    bool bAutoSaveEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story Settings")
    float AutoSaveIntervalMinutes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story Settings")
    bool bAnalyticsEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story Settings")
    bool bSkipIntroduction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story Settings")
    float DialogueSpeedMultiplier;

    // Runtime State
    UPROPERTY()
    FString CurrentDialogueID;

    UPROPERTY()
    FString CurrentCaseID;

    UPROPERTY()
    TArray<FString> PendingEvents;

    UPROPERTY()
    TMap<FString, FDateTime> ScheduledEvents;

private:
    // Helper Functions
    void InitializeDefaultChapters();
    void InitializeDefaultCharacters();
    void UpdateProgressCalculations();
    
    // Chapter Helpers
    FStoryChapter* GetChapterByID(int32 ChapterID);
    const FStoryChapter* GetChapterByID(int32 ChapterID) const;
    void ValidateChapterRequirements(int32 ChapterID);
    
    // Character Helpers
    FStoryCharacter* GetCharacterByID(const FString& CharacterID);
    const FStoryCharacter* GetCharacterByID(const FString& CharacterID) const;
    void ProcessCharacterInteraction(const FString& CharacterID, EDialogueChoiceType ChoiceType);
    
    // Case Helpers
    FStoryInvestigationCase* GetCaseByID(const FString& CaseID);
    const FStoryInvestigationCase* GetCaseByID(const FString& CaseID) const;
    void UpdateCaseProgress(const FString& CaseID);
    
    // Dialogue Helpers
    FStoryDialogue* GetDialogueByID(const FString& DialogueID);
    const FStoryDialogue* GetDialogueByID(const FString& DialogueID) const;
    void ProcessDialogueConsequences(const FStoryDialogue& Dialogue, EDialogueChoiceType ChoiceType);
    
    // Save/Load Helpers
    FString GetSaveGameSlotName();
    void ProcessAutoSave();
    void ValidateSaveData(const FStoryProgress& SaveData);
    
    // Event Processing
    void ProcessScheduledEvents();
    void ExecuteStoryEvent(EStoryEventType EventType, const FString& EventData);
    
    // Analytics Helpers
    void RecordChoiceAnalytics(const FString& ChoiceID, EDialogueChoiceType ChoiceType);
    void RecordProgressAnalytics();
    void RecordTimeAnalytics();

    // Integration with other systems
    class UVirtueManager* GetVirtueManager();
    class UMemoryManager* GetMemoryManager();
    class USocialExperimentManager* GetSocialExperimentManager();
    class UProjectVisibleUIManager* GetUIManager();
};
