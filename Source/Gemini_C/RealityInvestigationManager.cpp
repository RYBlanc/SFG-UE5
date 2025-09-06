// Project Visible - Reality Investigation Manager Implementation
// Copyright Epic Games, Inc. All Rights Reserved.

#include "RealityInvestigationManager.h"
#include "Gemini_C.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

void URealityInvestigationManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Initialize ID generators
    NextCaseID = 1;
    NextEvidenceID = 1;
    NextClueID = 1;
    NextNPCID = 1;
    NextLocationID = 1;
    
    // Initialize settings
    EvidenceAnalysisTime = 3.0f;
    ClueRevealThreshold = 0.7f;
    MaxSimultaneousCases = 5;
    bUseAIAssistant = true;
    
    UE_LOG(LogProjectVisible, Log, TEXT("Reality Investigation Manager initialized"));
}

void URealityInvestigationManager::Deinitialize()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Reality Investigation Manager shutting down"));
    Super::Deinitialize();
}

// Case Management
int32 URealityInvestigationManager::CreateNewCase(const FString& CaseTitle, const FString& Description, int32 Priority)
{
    if (Cases.Num() >= MaxSimultaneousCases)
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Maximum number of simultaneous cases reached"));
        return -1;
    }
    
    FInvestigationCase NewCase;
    NewCase.CaseID = NextCaseID++;
    NewCase.CaseTitle = CaseTitle;
    NewCase.CaseDescription = Description;
    NewCase.Priority = Priority;
    NewCase.State = EInvestigationState::NotStarted;
    NewCase.StartTime = FDateTime::Now();
    NewCase.ProgressPercentage = 0.0f;
    NewCase.DifficultyLevel = 1;
    
    Cases.Add(NewCase);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Created new investigation case: %s (ID: %d)"), *CaseTitle, NewCase.CaseID);
    
    return NewCase.CaseID;
}

bool URealityInvestigationManager::StartCase(int32 CaseID)
{
    FInvestigationCase* Case = GetCaseByID(CaseID);
    if (!Case)
    {
        UE_LOG(LogProjectVisible, Error, TEXT("Case ID %d not found"), CaseID);
        return false;
    }
    
    if (Case->State != EInvestigationState::NotStarted)
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Case %d is already started"), CaseID);
        return false;
    }
    
    Case->State = EInvestigationState::InProgress;
    Case->StartTime = FDateTime::Now();
    
    UE_LOG(LogProjectVisible, Log, TEXT("Started investigation case: %s"), *Case->CaseTitle);
    
    return true;
}

bool URealityInvestigationManager::CompleteCase(int32 CaseID)
{
    FInvestigationCase* Case = GetCaseByID(CaseID);
    if (!Case)
    {
        return false;
    }
    
    Case->State = EInvestigationState::Completed;
    Case->EndTime = FDateTime::Now();
    Case->ProgressPercentage = 100.0f;
    
    UE_LOG(LogProjectVisible, Log, TEXT("Completed investigation case: %s"), *Case->CaseTitle);
    
    return true;
}

void URealityInvestigationManager::UpdateCaseProgress(int32 CaseID)
{
    FInvestigationCase* Case = GetCaseByID(CaseID);
    if (!Case)
    {
        return;
    }
    
    // Calculate progress based on evidence and clues
    int32 TotalEvidence = Case->Evidence.Num();
    int32 ExaminedEvidence = 0;
    
    for (const FEvidenceItem& Evidence : Case->Evidence)
    {
        if (Evidence.bIsExamined)
        {
            ExaminedEvidence++;
        }
    }
    
    int32 TotalClues = Case->Clues.Num();
    int32 RevealedClues = 0;
    
    for (const FClueData& Clue : Case->Clues)
    {
        if (Clue.bIsRevealed)
        {
            RevealedClues++;
        }
    }
    
    float EvidenceProgress = TotalEvidence > 0 ? (float)ExaminedEvidence / TotalEvidence : 0.0f;
    float ClueProgress = TotalClues > 0 ? (float)RevealedClues / TotalClues : 0.0f;
    
    float OldProgress = Case->ProgressPercentage;
    Case->ProgressPercentage = (EvidenceProgress * 0.6f + ClueProgress * 0.4f) * 100.0f;
    
    if (FMath::Abs(Case->ProgressPercentage - OldProgress) > 5.0f)
    {
        OnCaseProgressChanged.Broadcast(CaseID, Case->ProgressPercentage);
        UE_LOG(LogProjectVisible, Log, TEXT("Case %d progress updated to %.1f%%"), CaseID, Case->ProgressPercentage);
    }
}

bool URealityInvestigationManager::GetCaseData(int32 CaseID, FInvestigationCase& OutCase) const
{
    for (const FInvestigationCase& Case : Cases)
    {
        if (Case.CaseID == CaseID)
        {
            OutCase = Case;
            return true;
        }
    }
    return false;
}

TArray<FInvestigationCase> URealityInvestigationManager::GetActiveCases() const
{
    TArray<FInvestigationCase> ActiveCases;
    
    for (const FInvestigationCase& Case : Cases)
    {
        if (Case.State == EInvestigationState::InProgress)
        {
            ActiveCases.Add(Case);
        }
    }
    
    return ActiveCases;
}

// Evidence Management
int32 URealityInvestigationManager::AddEvidence(const FString& EvidenceName, const FString& Description, 
                                               EEvidenceType Type, const FVector& Location, int32 CaseID)
{
    FEvidenceItem NewEvidence;
    NewEvidence.EvidenceID = NextEvidenceID++;
    NewEvidence.EvidenceName = EvidenceName;
    NewEvidence.Description = Description;
    NewEvidence.EvidenceType = Type;
    NewEvidence.LocationFound = Location;
    NewEvidence.DiscoveryTimestamp = FDateTime::Now();
    NewEvidence.ReliabilityScore = CalculateEvidenceReliability(NewEvidence);
    
    AllEvidence.Add(NewEvidence);
    
    // Add to case if specified
    if (CaseID > 0)
    {
        FInvestigationCase* Case = GetCaseByID(CaseID);
        if (Case)
        {
            Case->Evidence.Add(NewEvidence);
            UpdateCaseProgress(CaseID);
        }
    }
    
    OnEvidenceFound.Broadcast(NewEvidence.EvidenceID, NewEvidence);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Added evidence: %s (ID: %d, Type: %s)"), 
           *EvidenceName, NewEvidence.EvidenceID, *UEnum::GetValueAsString(Type));
    
    return NewEvidence.EvidenceID;
}

bool URealityInvestigationManager::ExamineEvidence(int32 EvidenceID)
{
    FEvidenceItem* Evidence = GetEvidenceByID(EvidenceID);
    if (!Evidence)
    {
        return false;
    }
    
    Evidence->bIsExamined = true;
    
    // Process any evidence connections
    ProcessEvidenceConnections(EvidenceID);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Examined evidence: %s"), *Evidence->EvidenceName);
    
    return true;
}

bool URealityInvestigationManager::AnalyzeEvidence(int32 EvidenceID)
{
    FEvidenceItem* Evidence = GetEvidenceByID(EvidenceID);
    if (!Evidence || !Evidence->bIsExamined)
    {
        return false;
    }
    
    Evidence->bIsAnalyzed = true;
    
    // Check for clue reveals based on analyzed evidence
    CheckClueConditions();
    
    UE_LOG(LogProjectVisible, Log, TEXT("Analyzed evidence: %s"), *Evidence->EvidenceName);
    
    return true;
}

void URealityInvestigationManager::ConnectEvidence(int32 EvidenceID1, int32 EvidenceID2, float ConnectionWeight)
{
    FEvidenceItem* Evidence1 = GetEvidenceByID(EvidenceID1);
    FEvidenceItem* Evidence2 = GetEvidenceByID(EvidenceID2);
    
    if (!Evidence1 || !Evidence2)
    {
        return;
    }
    
    Evidence1->ConnectedEvidenceIDs.AddUnique(EvidenceID2);
    Evidence2->ConnectedEvidenceIDs.AddUnique(EvidenceID1);
    Evidence1->ConnectionWeight = ConnectionWeight;
    Evidence2->ConnectionWeight = ConnectionWeight;
    
    UE_LOG(LogProjectVisible, Log, TEXT("Connected evidence: %s <-> %s (Weight: %.2f)"), 
           *Evidence1->EvidenceName, *Evidence2->EvidenceName, ConnectionWeight);
}

bool URealityInvestigationManager::GetEvidenceData(int32 EvidenceID, FEvidenceItem& OutEvidence) const
{
    for (const FEvidenceItem& Evidence : AllEvidence)
    {
        if (Evidence.EvidenceID == EvidenceID)
        {
            OutEvidence = Evidence;
            return true;
        }
    }
    return false;
}

TArray<FEvidenceItem> URealityInvestigationManager::GetEvidenceByCase(int32 CaseID) const
{
    const FInvestigationCase* Case = nullptr;
    for (const FInvestigationCase& CaseRef : Cases)
    {
        if (CaseRef.CaseID == CaseID)
        {
            Case = &CaseRef;
            break;
        }
    }
    
    if (Case)
    {
        return Case->Evidence;
    }
    
    return TArray<FEvidenceItem>();
}

// Clue Management
int32 URealityInvestigationManager::CreateClue(const FString& ClueName, const FString& Description, 
                                              EClueType Type, const TArray<int32>& RequiredEvidence, int32 CaseID)
{
    FClueData NewClue;
    NewClue.ClueID = NextClueID++;
    NewClue.ClueName = ClueName;
    NewClue.ClueDescription = Description;
    NewClue.ClueType = Type;
    NewClue.RequiredEvidenceIDs = RequiredEvidence;
    NewClue.RequiredEvidenceCount = RequiredEvidence.Num();
    NewClue.Importance = (Type == EClueType::Primary) ? 100.0f : 50.0f;
    
    AllClues.Add(NewClue);
    
    // Add to case if specified
    if (CaseID > 0)
    {
        FInvestigationCase* Case = GetCaseByID(CaseID);
        if (Case)
        {
            Case->Clues.Add(NewClue);
        }
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Created clue: %s (ID: %d, Type: %s)"), 
           *ClueName, NewClue.ClueID, *UEnum::GetValueAsString(Type));
    
    return NewClue.ClueID;
}

bool URealityInvestigationManager::RevealClue(int32 ClueID)
{
    FClueData* Clue = GetClueByID(ClueID);
    if (!Clue || Clue->bIsRevealed)
    {
        return false;
    }
    
    Clue->bIsRevealed = true;
    
    // Unlock any connected clues
    for (int32 UnlockedClueID : Clue->UnlockedClueIDs)
    {
        FClueData* UnlockedClue = GetClueByID(UnlockedClueID);
        if (UnlockedClue)
        {
            CheckClueConditions();
        }
    }
    
    OnClueRevealed.Broadcast(ClueID, *Clue);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Revealed clue: %s"), *Clue->ClueName);
    
    return true;
}

void URealityInvestigationManager::CheckClueConditions()
{
    for (FClueData& Clue : AllClues)
    {
        if (Clue.bIsRevealed)
        {
            continue;
        }
        
        int32 AvailableEvidence = 0;
        for (int32 RequiredEvidenceID : Clue.RequiredEvidenceIDs)
        {
            FEvidenceItem* Evidence = GetEvidenceByID(RequiredEvidenceID);
            if (Evidence && Evidence->bIsAnalyzed)
            {
                AvailableEvidence++;
            }
        }
        
        // Check if conditions are met
        if (AvailableEvidence >= Clue.RequiredEvidenceCount)
        {
            RevealClue(Clue.ClueID);
        }
    }
}

bool URealityInvestigationManager::GetClueData(int32 ClueID, FClueData& OutClue) const
{
    for (const FClueData& Clue : AllClues)
    {
        if (Clue.ClueID == ClueID)
        {
            OutClue = Clue;
            return true;
        }
    }
    return false;
}

TArray<FClueData> URealityInvestigationManager::GetRevealedClues(int32 CaseID) const
{
    TArray<FClueData> RevealedClues;
    
    if (CaseID > 0)
    {
        // Get clues for specific case
        const FInvestigationCase* Case = nullptr;
        for (const FInvestigationCase& CaseRef : Cases)
        {
            if (CaseRef.CaseID == CaseID)
            {
                Case = &CaseRef;
                break;
            }
        }
        
        if (Case)
        {
            for (const FClueData& Clue : Case->Clues)
            {
                if (Clue.bIsRevealed)
                {
                    RevealedClues.Add(Clue);
                }
            }
        }
    }
    else
    {
        // Get all revealed clues
        for (const FClueData& Clue : AllClues)
        {
            if (Clue.bIsRevealed)
            {
                RevealedClues.Add(Clue);
            }
        }
    }
    
    return RevealedClues;
}

// NPC Interaction
int32 URealityInvestigationManager::RegisterNPC(const FString& NPCName, float InitialTrustLevel)
{
    FNPCDialogueData NewNPC;
    NewNPC.NPCID = NextNPCID++;
    NewNPC.NPCName = NPCName;
    NewNPC.TrustLevel = InitialTrustLevel;
    NewNPC.EmotionalState = ENPCEmotionalState::Neutral;
    NewNPC.KnowledgeLevel = FMath::RandRange(30.0f, 80.0f);
    
    NPCs.Add(NewNPC);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Registered NPC: %s (ID: %d, Trust: %.1f)"), 
           *NPCName, NewNPC.NPCID, InitialTrustLevel);
    
    return NewNPC.NPCID;
}

bool URealityInvestigationManager::StartDialogue(int32 NPCID, EDialogueOption Option)
{
    FNPCDialogueData* NPC = GetNPCByID(NPCID);
    if (!NPC)
    {
        return false;
    }
    
    NPC->ConversationCount++;
    
    // Simulate dialogue outcome based on NPC state and option
    FString Result;
    bool bSuccessful = false;
    
    float BaseChance = 0.5f;
    
    // Adjust chance based on trust level
    BaseChance += (NPC->TrustLevel - 50.0f) / 100.0f;
    
    // Adjust based on dialogue option effectiveness
    switch (Option)
    {
        case EDialogueOption::Empathy:
            if (NPC->EmotionalState == ENPCEmotionalState::Fearful || 
                NPC->EmotionalState == ENPCEmotionalState::Guilty)
            {
                BaseChance += 0.3f;
            }
            break;
        case EDialogueOption::Logic:
            if (NPC->KnowledgeLevel > 60.0f)
            {
                BaseChance += 0.2f;
            }
            break;
        case EDialogueOption::Intimidation:
            if (NPC->EmotionalState == ENPCEmotionalState::Guilty)
            {
                BaseChance += 0.4f;
            }
            else
            {
                BaseChance -= 0.3f; // Risk of backfiring
            }
            break;
    }
    
    bSuccessful = FMath::RandRange(0.0f, 1.0f) < BaseChance;
    
    if (bSuccessful)
    {
        Result = TEXT("NPC provided helpful information");
        ModifyNPCTrust(NPCID, 5.0f);
    }
    else
    {
        Result = TEXT("NPC was uncooperative");
        ModifyNPCTrust(NPCID, -2.0f);
    }
    
    // Update NPC relationships
    UpdateNPCRelationships(NPCID, Option);
    
    OnDialogueCompleted.Broadcast(NPCID, Option, Result);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Dialogue with %s: %s (Option: %s)"), 
           *NPC->NPCName, *Result, *UEnum::GetValueAsString(Option));
    
    return bSuccessful;
}

void URealityInvestigationManager::UpdateNPCEmotionalState(int32 NPCID, ENPCEmotionalState NewState)
{
    FNPCDialogueData* NPC = GetNPCByID(NPCID);
    if (NPC)
    {
        NPC->EmotionalState = NewState;
        UE_LOG(LogProjectVisible, Log, TEXT("Updated %s emotional state to %s"), 
               *NPC->NPCName, *UEnum::GetValueAsString(NewState));
    }
}

void URealityInvestigationManager::ModifyNPCTrust(int32 NPCID, float TrustDelta)
{
    FNPCDialogueData* NPC = GetNPCByID(NPCID);
    if (NPC)
    {
        NPC->TrustLevel = FMath::Clamp(NPC->TrustLevel + TrustDelta, 0.0f, 100.0f);
    }
}

bool URealityInvestigationManager::GetNPCData(int32 NPCID, FNPCDialogueData& OutNPCData) const
{
    for (const FNPCDialogueData& NPC : NPCs)
    {
        if (NPC.NPCID == NPCID)
        {
            OutNPCData = NPC;
            return true;
        }
    }
    return false;
}

// Helper Functions
FInvestigationCase* URealityInvestigationManager::GetCaseByID(int32 CaseID)
{
    for (FInvestigationCase& Case : Cases)
    {
        if (Case.CaseID == CaseID)
        {
            return &Case;
        }
    }
    return nullptr;
}

FEvidenceItem* URealityInvestigationManager::GetEvidenceByID(int32 EvidenceID)
{
    for (FEvidenceItem& Evidence : AllEvidence)
    {
        if (Evidence.EvidenceID == EvidenceID)
        {
            return &Evidence;
        }
    }
    return nullptr;
}

FClueData* URealityInvestigationManager::GetClueByID(int32 ClueID)
{
    for (FClueData& Clue : AllClues)
    {
        if (Clue.ClueID == ClueID)
        {
            return &Clue;
        }
    }
    return nullptr;
}

FNPCDialogueData* URealityInvestigationManager::GetNPCByID(int32 NPCID)
{
    for (FNPCDialogueData& NPC : NPCs)
    {
        if (NPC.NPCID == NPCID)
        {
            return &NPC;
        }
    }
    return nullptr;
}

FInvestigationLocation* URealityInvestigationManager::GetLocationByID(int32 LocationID)
{
    for (FInvestigationLocation& Location : Locations)
    {
        if (Location.LocationID == LocationID)
        {
            return &Location;
        }
    }
    return nullptr;
}

void URealityInvestigationManager::ProcessEvidenceConnections(int32 EvidenceID)
{
    // Placeholder for advanced evidence analysis
    UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Processing evidence connections for evidence %d"), EvidenceID);
}

void URealityInvestigationManager::EvaluateClueReveals(int32 CaseID)
{
    // Placeholder for clue evaluation logic
    CheckClueConditions();
}

float URealityInvestigationManager::CalculateEvidenceReliability(const FEvidenceItem& Evidence)
{
    float Reliability = 100.0f;
    
    // Adjust based on evidence type
    switch (Evidence.EvidenceType)
    {
        case EEvidenceType::Digital:
            Reliability *= 0.95f; // High reliability
            break;
        case EEvidenceType::Testimonial:
            Reliability *= 0.7f; // Lower reliability
            break;
        case EEvidenceType::Physical:
            Reliability *= 0.9f; // Good reliability
            break;
        default:
            Reliability *= 0.8f;
            break;
    }
    
    return FMath::Clamp(Reliability, 0.0f, 100.0f);
}

void URealityInvestigationManager::UpdateNPCRelationships(int32 NPCID, EDialogueOption Option)
{
    // Placeholder for relationship system
    UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Updating NPC %d relationships based on dialogue option"), NPCID);
}

void URealityInvestigationManager::LogInvestigationEvent(const FString& EventDescription)
{
    UE_LOG(LogProjectVisible, Log, TEXT("Investigation Event: %s"), *EventDescription);
}

// Placeholder implementations for remaining functions
int32 URealityInvestigationManager::RegisterLocation(const FString& LocationName, const FString& Description, const FVector& WorldPosition)
{
    FInvestigationLocation NewLocation;
    NewLocation.LocationID = NextLocationID++;
    NewLocation.LocationName = LocationName;
    NewLocation.LocationDescription = Description;
    NewLocation.WorldPosition = WorldPosition;
    
    Locations.Add(NewLocation);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Registered location: %s (ID: %d)"), *LocationName, NewLocation.LocationID);
    
    return NewLocation.LocationID;
}

bool URealityInvestigationManager::ExploreLocation(int32 LocationID)
{
    FInvestigationLocation* Location = GetLocationByID(LocationID);
    if (Location)
    {
        Location->bIsExplored = true;
        Location->ExplorationProgress = 100.0f;
        UE_LOG(LogProjectVisible, Log, TEXT("Explored location: %s"), *Location->LocationName);
        return true;
    }
    return false;
}

void URealityInvestigationManager::UpdateLocationProgress(int32 LocationID, float ProgressDelta)
{
    FInvestigationLocation* Location = GetLocationByID(LocationID);
    if (Location)
    {
        Location->ExplorationProgress = FMath::Clamp(Location->ExplorationProgress + ProgressDelta, 0.0f, 100.0f);
    }
}

bool URealityInvestigationManager::GetLocationData(int32 LocationID, FInvestigationLocation& OutLocation) const
{
    for (const FInvestigationLocation& Location : Locations)
    {
        if (Location.LocationID == LocationID)
        {
            OutLocation = Location;
            return true;
        }
    }
    return false;
}

TArray<FInvestigationLocation> URealityInvestigationManager::GetExploredLocations() const
{
    TArray<FInvestigationLocation> ExploredLocations;
    
    for (const FInvestigationLocation& Location : Locations)
    {
        if (Location.bIsExplored)
        {
            ExploredLocations.Add(Location);
        }
    }
    
    return ExploredLocations;
}

float URealityInvestigationManager::CalculateDeductionConfidence(const TArray<int32>& EvidenceIDs)
{
    float TotalConfidence = 0.0f;
    int32 ValidEvidence = 0;
    
    for (int32 EvidenceID : EvidenceIDs)
    {
        FEvidenceItem* Evidence = GetEvidenceByID(EvidenceID);
        if (Evidence && Evidence->bIsAnalyzed)
        {
            TotalConfidence += Evidence->ReliabilityScore;
            ValidEvidence++;
        }
    }
    
    return ValidEvidence > 0 ? TotalConfidence / ValidEvidence : 0.0f;
}

TArray<FString> URealityInvestigationManager::GenerateDeductionHypotheses(int32 CaseID)
{
    TArray<FString> Hypotheses;
    Hypotheses.Add(TEXT("Hypothesis based on available evidence"));
    Hypotheses.Add(TEXT("Alternative theory considering witness testimony"));
    Hypotheses.Add(TEXT("Secondary possibility from physical evidence"));
    
    UE_LOG(LogProjectVisible, Log, TEXT("Generated %d hypotheses for case %d"), Hypotheses.Num(), CaseID);
    
    return Hypotheses;
}

bool URealityInvestigationManager::ValidateDeduction(int32 CaseID, const FString& Hypothesis)
{
    UE_LOG(LogProjectVisible, Log, TEXT("Validating hypothesis for case %d: %s"), CaseID, *Hypothesis);
    return true; // Placeholder
}

TArray<FString> URealityInvestigationManager::GetInvestigationSuggestions(int32 CaseID)
{
    TArray<FString> Suggestions;
    
    if (bUseAIAssistant)
    {
        Suggestions.Add(TEXT("Consider examining location evidence more thoroughly"));
        Suggestions.Add(TEXT("Re-interview witnesses with new evidence"));
        Suggestions.Add(TEXT("Look for connections between seemingly unrelated clues"));
    }
    
    return Suggestions;
}

FString URealityInvestigationManager::AnalyzeEvidencePattern(const TArray<int32>& EvidenceIDs)
{
    return TEXT("Pattern analysis suggests temporal sequence of events");
}
