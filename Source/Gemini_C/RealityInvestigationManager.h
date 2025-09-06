// Project Visible - Reality Investigation Manager
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/World.h"
#include "RealityInvestigationTypes.h"
#include "RealityInvestigationManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEvidenceFound, int32, EvidenceID, const FEvidenceItem&, Evidence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClueRevealed, int32, ClueID, const FClueData&, Clue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCaseProgressChanged, int32, CaseID, float, NewProgress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDialogueCompleted, int32, NPCID, EDialogueOption, ChosenOption, FString, Result);

/**
 * Central manager for reality investigation gameplay
 * Handles evidence collection, clue tracking, case management, and NPC interactions
 */
UCLASS()
class GEMINI_C_API URealityInvestigationManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Case Management
    UFUNCTION(BlueprintCallable, Category = "Investigation")
    int32 CreateNewCase(const FString& CaseTitle, const FString& Description, int32 Priority = 50);

    UFUNCTION(BlueprintCallable, Category = "Investigation")
    bool StartCase(int32 CaseID);

    UFUNCTION(BlueprintCallable, Category = "Investigation")
    bool CompleteCase(int32 CaseID);

    UFUNCTION(BlueprintCallable, Category = "Investigation")
    void UpdateCaseProgress(int32 CaseID);

    UFUNCTION(BlueprintCallable, Category = "Investigation", BlueprintPure)
    bool GetCaseData(int32 CaseID, FInvestigationCase& OutCase) const;

    UFUNCTION(BlueprintCallable, Category = "Investigation", BlueprintPure)
    TArray<FInvestigationCase> GetActiveCases() const;

    // Evidence Management
    UFUNCTION(BlueprintCallable, Category = "Investigation")
    int32 AddEvidence(const FString& EvidenceName, const FString& Description, 
                      EEvidenceType Type, const FVector& Location, int32 CaseID = -1);

    UFUNCTION(BlueprintCallable, Category = "Investigation")
    bool ExamineEvidence(int32 EvidenceID);

    UFUNCTION(BlueprintCallable, Category = "Investigation")
    bool AnalyzeEvidence(int32 EvidenceID);

    UFUNCTION(BlueprintCallable, Category = "Investigation")
    void ConnectEvidence(int32 EvidenceID1, int32 EvidenceID2, float ConnectionWeight = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Investigation", BlueprintPure)
    bool GetEvidenceData(int32 EvidenceID, FEvidenceItem& OutEvidence) const;

    UFUNCTION(BlueprintCallable, Category = "Investigation", BlueprintPure)
    TArray<FEvidenceItem> GetEvidenceByCase(int32 CaseID) const;

    // Clue Management
    UFUNCTION(BlueprintCallable, Category = "Investigation")
    int32 CreateClue(const FString& ClueName, const FString& Description, 
                     EClueType Type, const TArray<int32>& RequiredEvidence, int32 CaseID = -1);

    UFUNCTION(BlueprintCallable, Category = "Investigation")
    bool RevealClue(int32 ClueID);

    UFUNCTION(BlueprintCallable, Category = "Investigation")
    void CheckClueConditions();

    UFUNCTION(BlueprintCallable, Category = "Investigation", BlueprintPure)
    bool GetClueData(int32 ClueID, FClueData& OutClue) const;

    UFUNCTION(BlueprintCallable, Category = "Investigation", BlueprintPure)
    TArray<FClueData> GetRevealedClues(int32 CaseID = -1) const;

    // NPC Interaction
    UFUNCTION(BlueprintCallable, Category = "Investigation")
    int32 RegisterNPC(const FString& NPCName, float InitialTrustLevel = 50.0f);

    UFUNCTION(BlueprintCallable, Category = "Investigation")
    bool StartDialogue(int32 NPCID, EDialogueOption Option);

    UFUNCTION(BlueprintCallable, Category = "Investigation")
    void UpdateNPCEmotionalState(int32 NPCID, ENPCEmotionalState NewState);

    UFUNCTION(BlueprintCallable, Category = "Investigation")
    void ModifyNPCTrust(int32 NPCID, float TrustDelta);

    UFUNCTION(BlueprintCallable, Category = "Investigation", BlueprintPure)
    bool GetNPCData(int32 NPCID, FNPCDialogueData& OutNPCData) const;

    // Location Management
    UFUNCTION(BlueprintCallable, Category = "Investigation")
    int32 RegisterLocation(const FString& LocationName, const FString& Description, 
                          const FVector& WorldPosition);

    UFUNCTION(BlueprintCallable, Category = "Investigation")
    bool ExploreLocation(int32 LocationID);

    UFUNCTION(BlueprintCallable, Category = "Investigation")
    void UpdateLocationProgress(int32 LocationID, float ProgressDelta);

    UFUNCTION(BlueprintCallable, Category = "Investigation", BlueprintPure)
    bool GetLocationData(int32 LocationID, FInvestigationLocation& OutLocation) const;

    UFUNCTION(BlueprintCallable, Category = "Investigation", BlueprintPure)
    TArray<FInvestigationLocation> GetExploredLocations() const;

    // Deduction & Analysis
    UFUNCTION(BlueprintCallable, Category = "Investigation")
    float CalculateDeductionConfidence(const TArray<int32>& EvidenceIDs);

    UFUNCTION(BlueprintCallable, Category = "Investigation")
    TArray<FString> GenerateDeductionHypotheses(int32 CaseID);

    UFUNCTION(BlueprintCallable, Category = "Investigation")
    bool ValidateDeduction(int32 CaseID, const FString& Hypothesis);

    // Investigation AI Assistant
    UFUNCTION(BlueprintCallable, Category = "Investigation")
    TArray<FString> GetInvestigationSuggestions(int32 CaseID);

    UFUNCTION(BlueprintCallable, Category = "Investigation")
    FString AnalyzeEvidencePattern(const TArray<int32>& EvidenceIDs);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Investigation Events")
    FOnEvidenceFound OnEvidenceFound;

    UPROPERTY(BlueprintAssignable, Category = "Investigation Events")
    FOnClueRevealed OnClueRevealed;

    UPROPERTY(BlueprintAssignable, Category = "Investigation Events")
    FOnCaseProgressChanged OnCaseProgressChanged;

    UPROPERTY(BlueprintAssignable, Category = "Investigation Events")
    FOnDialogueCompleted OnDialogueCompleted;

protected:
    // Data Storage
    UPROPERTY(BlueprintReadOnly, Category = "Investigation Data")
    TArray<FInvestigationCase> Cases;

    UPROPERTY(BlueprintReadOnly, Category = "Investigation Data")
    TArray<FEvidenceItem> AllEvidence;

    UPROPERTY(BlueprintReadOnly, Category = "Investigation Data")
    TArray<FClueData> AllClues;

    UPROPERTY(BlueprintReadOnly, Category = "Investigation Data")
    TArray<FNPCDialogueData> NPCs;

    UPROPERTY(BlueprintReadOnly, Category = "Investigation Data")
    TArray<FInvestigationLocation> Locations;

    // ID Generators
    UPROPERTY()
    int32 NextCaseID;

    UPROPERTY()
    int32 NextEvidenceID;

    UPROPERTY()
    int32 NextClueID;

    UPROPERTY()
    int32 NextNPCID;

    UPROPERTY()
    int32 NextLocationID;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Settings")
    float EvidenceAnalysisTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Settings")
    float ClueRevealThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Settings")
    int32 MaxSimultaneousCases;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investigation Settings")
    bool bUseAIAssistant;

private:
    // Helper Functions
    FInvestigationCase* GetCaseByID(int32 CaseID);
    FEvidenceItem* GetEvidenceByID(int32 EvidenceID);
    FClueData* GetClueByID(int32 ClueID);
    FNPCDialogueData* GetNPCByID(int32 NPCID);
    FInvestigationLocation* GetLocationByID(int32 LocationID);

    void ProcessEvidenceConnections(int32 EvidenceID);
    void EvaluateClueReveals(int32 CaseID);
    float CalculateEvidenceReliability(const FEvidenceItem& Evidence);
    void UpdateNPCRelationships(int32 NPCID, EDialogueOption Option);
    void LogInvestigationEvent(const FString& EventDescription);
};
