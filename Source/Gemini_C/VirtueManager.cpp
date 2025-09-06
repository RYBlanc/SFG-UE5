// Project Visible - Virtue System Manager Implementation
// Copyright Epic Games, Inc. All Rights Reserved.

#include "VirtueManager.h"
#include "Gemini_C.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

void UVirtueManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Initialize settings
    VirtueDecayRate = 0.1f;
    ActionImpactMultiplier = 1.0f;
    MaxActionHistory = 500;
    ConsistencyRequirement = 0.7f;
    bUseVirtueDecay = true;
    bTrackPlayerValues = true;
    
    // Initialize ID generator
    NextActionID = 1;
    
    // Initialize analysis data
    LastAnalysis = FDateTime::Now();
    
    // Initialize virtue system
    InitializeVirtues();
    
    UE_LOG(LogProjectVisible, Log, TEXT("Virtue Manager initialized"));
}

void UVirtueManager::Deinitialize()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Virtue Manager shutting down"));
    Super::Deinitialize();
}

// Virtue Management
void UVirtueManager::InitializeVirtues()
{
    // Initialize all four Aristotelian virtues
    InitializeVirtue(EVirtueType::Wisdom);
    InitializeVirtue(EVirtueType::Courage);
    InitializeVirtue(EVirtueType::Justice);
    InitializeVirtue(EVirtueType::Temperance);
    
    // Initialize player values
    if (bTrackPlayerValues)
    {
        TArray<EPlayerValue> AllValues = {
            EPlayerValue::Security, EPlayerValue::Achievement, EPlayerValue::SelfDirection,
            EPlayerValue::Stimulation, EPlayerValue::Hedonism, EPlayerValue::Conformity,
            EPlayerValue::Tradition, EPlayerValue::Benevolence, EPlayerValue::Universalism,
            EPlayerValue::Power
        };
        
        for (EPlayerValue Value : AllValues)
        {
            FPlayerValueAssessment Assessment;
            Assessment.ValueType = Value;
            Assessment.Strength = 50.0f; // Start with neutral baseline
            Assessment.Consistency = 50.0f;
            Assessment.Confidence = 20.0f; // Low confidence initially
            PlayerValues.Add(Value, Assessment);
        }
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Initialized virtue system with 4 Aristotelian virtues"));
}

void UVirtueManager::RecordVirtueAction(EVirtueType VirtueType, const FString& ActionType, const FString& Description, 
                                       float ImpactMagnitude, bool bIsPositive)
{
    FVirtueAction NewAction;
    NewAction.ActionID = NextActionID++;
    NewAction.VirtueType = VirtueType;
    NewAction.ActionType = ActionType;
    NewAction.Description = Description;
    NewAction.ImpactMagnitude = FMath::Clamp(ImpactMagnitude, 0.0f, 10.0f);
    NewAction.bIsPositive = bIsPositive;
    NewAction.Timestamp = FDateTime::Now();
    NewAction.ContextualWeight = 1.0f;
    
    // Add to history
    ActionHistory.Add(NewAction);
    
    // Update virtue level
    float Delta = NewAction.ImpactMagnitude * ActionImpactMultiplier;
    if (!bIsPositive)
    {
        Delta = -Delta;
    }
    
    UpdateVirtueLevel(VirtueType, Delta, FString::Printf(TEXT("Action: %s"), *ActionType));
    
    // Update player values based on action
    UpdatePlayerValueFromAction(NewAction);
    
    // Cleanup old actions if necessary
    if (ActionHistory.Num() > MaxActionHistory)
    {
        CleanupOldActions();
    }
    
    OnVirtueActionRecorded.Broadcast(NewAction);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Recorded virtue action: %s (%s) - Impact: %.2f"), 
           *UEnum::GetValueAsString(VirtueType), *ActionType, Delta);
}

void UVirtueManager::UpdateVirtueLevel(EVirtueType VirtueType, float Delta, const FString& Reason)
{
    if (FVirtueData* Virtue = Virtues.Find(VirtueType))
    {
        float OldLevel = Virtue->CurrentLevel;
        Virtue->CurrentLevel = FMath::Clamp(Virtue->CurrentLevel + Delta, 0.0f, 100.0f);
        Virtue->LastUpdated = FDateTime::Now();
        Virtue->RecentActions++;
        
        // Update experience points
        if (Delta > 0.0f)
        {
            Virtue->ExperiencePoints += FMath::RoundToInt(Delta * 10.0f);
        }
        
        // Update development state
        UpdateVirtueDevelopmentState(VirtueType);
        
        // Update consistency score
        CheckVirtueConsistency(VirtueType);
        
        OnVirtueChanged.Broadcast(VirtueType, OldLevel, Virtue->CurrentLevel);
        
        UE_LOG(LogProjectVisible, Log, TEXT("Updated virtue %s: %.2f -> %.2f (%s)"), 
               *UEnum::GetValueAsString(VirtueType), OldLevel, Virtue->CurrentLevel, *Reason);
    }
}

float UVirtueManager::CalculateVirtueLevel(EVirtueType VirtueType)
{
    TArray<FVirtueAction> VirtueActions = GetActionsByVirtue(VirtueType, 50);
    
    if (VirtueActions.Num() == 0)
    {
        return 50.0f; // Default neutral level
    }
    
    float TotalImpact = 0.0f;
    float TotalWeight = 0.0f;
    FDateTime Now = FDateTime::Now();
    
    for (const FVirtueAction& Action : VirtueActions)
    {
        // Calculate time decay
        float DaysAgo = (Now - Action.Timestamp).GetTotalDays();
        float TimeWeight = FMath::Exp(-DaysAgo * 0.1f); // Exponential decay
        
        float Impact = Action.ImpactMagnitude * Action.ContextualWeight * TimeWeight;
        if (!Action.bIsPositive)
        {
            Impact = -Impact;
        }
        
        TotalImpact += Impact;
        TotalWeight += TimeWeight;
    }
    
    float AverageImpact = TotalWeight > 0.0f ? TotalImpact / TotalWeight : 0.0f;
    float VirtueLevel = 50.0f + (AverageImpact * 5.0f); // Scale to 0-100 range
    
    return FMath::Clamp(VirtueLevel, 0.0f, 100.0f);
}

void UVirtueManager::ProcessVirtueDecay(float DeltaTime)
{
    if (!bUseVirtueDecay)
    {
        return;
    }
    
    for (auto& VirtuePair : Virtues)
    {
        FVirtueData& Virtue = VirtuePair.Value;
        FDateTime Now = FDateTime::Now();
        float DaysSinceUpdate = (Now - Virtue.LastUpdated).GetTotalDays();
        
        // Apply decay if no recent activity
        if (DaysSinceUpdate > 1.0f)
        {
            float DecayAmount = VirtueDecayRate * DaysSinceUpdate * DeltaTime;
            float OldLevel = Virtue.CurrentLevel;
            
            // Decay towards neutral (50.0)
            if (Virtue.CurrentLevel > 50.0f)
            {
                Virtue.CurrentLevel = FMath::Max(Virtue.CurrentLevel - DecayAmount, 50.0f);
            }
            else if (Virtue.CurrentLevel < 50.0f)
            {
                Virtue.CurrentLevel = FMath::Min(Virtue.CurrentLevel + DecayAmount, 50.0f);
            }
            
            if (FMath::Abs(OldLevel - Virtue.CurrentLevel) > 0.1f)
            {
                OnVirtueChanged.Broadcast(VirtuePair.Key, OldLevel, Virtue.CurrentLevel);
            }
        }
    }
}

// Virtue Assessment
bool UVirtueManager::GetVirtueData(EVirtueType VirtueType, FVirtueData& OutVirtueData) const
{
    if (const FVirtueData* Virtue = Virtues.Find(VirtueType))
    {
        OutVirtueData = *Virtue;
        return true;
    }
    return false;
}

TArray<FVirtueData> UVirtueManager::GetAllVirtues() const
{
    TArray<FVirtueData> AllVirtues;
    
    for (const auto& VirtuePair : Virtues)
    {
        AllVirtues.Add(VirtuePair.Value);
    }
    
    return AllVirtues;
}

float UVirtueManager::GetVirtueLevel(EVirtueType VirtueType) const
{
    if (const FVirtueData* Virtue = Virtues.Find(VirtueType))
    {
        return Virtue->CurrentLevel;
    }
    return 50.0f;
}

EVirtueDevelopment UVirtueManager::GetVirtueDevelopmentState(EVirtueType VirtueType) const
{
    if (const FVirtueData* Virtue = Virtues.Find(VirtueType))
    {
        return Virtue->DevelopmentState;
    }
    return EVirtueDevelopment::Moderate;
}

float UVirtueManager::GetOverallVirtueScore() const
{
    float TotalScore = 0.0f;
    int32 Count = 0;
    
    for (const auto& VirtuePair : Virtues)
    {
        TotalScore += VirtuePair.Value.CurrentLevel;
        Count++;
    }
    
    return Count > 0 ? TotalScore / Count : 50.0f;
}

// Player Value Analysis
void UVirtueManager::AssessPlayerValues()
{
    if (!bTrackPlayerValues)
    {
        return;
    }
    
    // Analyze recent actions to update player values
    TArray<FVirtueAction> RecentActions = GetRecentActions(100);
    
    for (auto& ValuePair : PlayerValues)
    {
        FPlayerValueAssessment& Assessment = ValuePair.Value;
        
        // Count supporting actions
        int32 SupportingActions = 0;
        float TotalEvidence = 0.0f;
        
        for (const FVirtueAction& Action : RecentActions)
        {
            if (Action.AffectedValues.Contains(Assessment.ValueType))
            {
                SupportingActions++;
                TotalEvidence += Action.ImpactMagnitude * (Action.bIsPositive ? 1.0f : -1.0f);
            }
        }
        
        if (SupportingActions > 0)
        {
            Assessment.Strength = FMath::Clamp(50.0f + (TotalEvidence / SupportingActions) * 10.0f, 0.0f, 100.0f);
            Assessment.SampleSize = SupportingActions;
            Assessment.Confidence = FMath::Min(Assessment.SampleSize * 2.0f, 100.0f);
        }
        
        Assessment.LastAssessed = FDateTime::Now();
    }
    
    TArray<FPlayerValueAssessment> ValueArray;
    for (const auto& ValuePair : PlayerValues)
    {
        ValueArray.Add(ValuePair.Value);
    }
    
    OnPlayerValuesUpdated.Broadcast(ValueArray);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Assessed player values from %d recent actions"), RecentActions.Num());
}

void UVirtueManager::UpdatePlayerValue(EPlayerValue ValueType, float Evidence, const FString& Context)
{
    if (FPlayerValueAssessment* Assessment = PlayerValues.Find(ValueType))
    {
        // Update strength based on evidence
        float WeightedEvidence = Evidence * 0.1f; // Scale evidence impact
        Assessment->Strength = FMath::Clamp(Assessment->Strength + WeightedEvidence, 0.0f, 100.0f);
        
        // Update trend
        Assessment->RecentTrend = (Assessment->RecentTrend * 0.8f) + (WeightedEvidence * 0.2f);
        
        // Add supporting evidence
        Assessment->SupportingEvidence.Add(Context);
        if (Assessment->SupportingEvidence.Num() > 10)
        {
            Assessment->SupportingEvidence.RemoveAt(0);
        }
        
        Assessment->LastAssessed = FDateTime::Now();
        Assessment->SampleSize++;
        Assessment->Confidence = FMath::Min(Assessment->SampleSize * 1.5f, 100.0f);
        
        UE_LOG(LogProjectVisible, Log, TEXT("Updated player value %s: Evidence %.2f, New Strength %.2f"), 
               *UEnum::GetValueAsString(ValueType), Evidence, Assessment->Strength);
    }
}

TArray<FPlayerValueAssessment> UVirtueManager::GetPlayerValueProfile() const
{
    TArray<FPlayerValueAssessment> Profile;
    
    for (const auto& ValuePair : PlayerValues)
    {
        Profile.Add(ValuePair.Value);
    }
    
    // Sort by strength
    Profile.Sort([](const FPlayerValueAssessment& A, const FPlayerValueAssessment& B)
    {
        return A.Strength > B.Strength;
    });
    
    return Profile;
}

float UVirtueManager::GetValueStrength(EPlayerValue ValueType) const
{
    if (const FPlayerValueAssessment* Assessment = PlayerValues.Find(ValueType))
    {
        return Assessment->Strength;
    }
    return 50.0f;
}

TArray<EPlayerValue> UVirtueManager::GetDominantValues(int32 TopCount) const
{
    TArray<FPlayerValueAssessment> Profile = GetPlayerValueProfile();
    TArray<EPlayerValue> DominantValues;
    
    for (int32 i = 0; i < FMath::Min(TopCount, Profile.Num()); i++)
    {
        DominantValues.Add(Profile[i].ValueType);
    }
    
    return DominantValues;
}

// Virtue Action History
TArray<FVirtueAction> UVirtueManager::GetRecentActions(int32 Count) const
{
    TArray<FVirtueAction> RecentActions = ActionHistory;
    
    // Sort by timestamp (most recent first)
    RecentActions.Sort([](const FVirtueAction& A, const FVirtueAction& B)
    {
        return A.Timestamp > B.Timestamp;
    });
    
    if (Count > 0 && RecentActions.Num() > Count)
    {
        RecentActions.SetNum(Count);
    }
    
    return RecentActions;
}

TArray<FVirtueAction> UVirtueManager::GetActionsByVirtue(EVirtueType VirtueType, int32 Count) const
{
    TArray<FVirtueAction> VirtueActions;
    
    for (const FVirtueAction& Action : ActionHistory)
    {
        if (Action.VirtueType == VirtueType)
        {
            VirtueActions.Add(Action);
        }
    }
    
    // Sort by timestamp (most recent first)
    VirtueActions.Sort([](const FVirtueAction& A, const FVirtueAction& B)
    {
        return A.Timestamp > B.Timestamp;
    });
    
    if (Count > 0 && VirtueActions.Num() > Count)
    {
        VirtueActions.SetNum(Count);
    }
    
    return VirtueActions;
}

void UVirtueManager::ClearActionHistory()
{
    ActionHistory.Empty();
    UE_LOG(LogProjectVisible, Log, TEXT("Cleared virtue action history"));
}

// Virtue Development
void UVirtueManager::AdvanceVirtueDevelopment(EVirtueType VirtueType)
{
    if (FVirtueData* Virtue = Virtues.Find(VirtueType))
    {
        EVirtueDevelopment OldState = Virtue->DevelopmentState;
        UpdateVirtueDevelopmentState(VirtueType);
        
        if (OldState != Virtue->DevelopmentState)
        {
            OnVirtueDevelopmentChanged.Broadcast(VirtueType, Virtue->DevelopmentState);
            UE_LOG(LogProjectVisible, Log, TEXT("Virtue %s development advanced: %s -> %s"), 
                   *UEnum::GetValueAsString(VirtueType), 
                   *UEnum::GetValueAsString(OldState), 
                   *UEnum::GetValueAsString(Virtue->DevelopmentState));
        }
    }
}

bool UVirtueManager::CheckVirtueConsistency(EVirtueType VirtueType, float TimeframeDays)
{
    TArray<FVirtueAction> RecentActions = GetActionsByVirtue(VirtueType, 20);
    
    if (RecentActions.Num() < 3)
    {
        return true; // Not enough data to judge consistency
    }
    
    FDateTime Cutoff = FDateTime::Now() - FTimespan::FromDays(TimeframeDays);
    
    int32 PositiveActions = 0;
    int32 NegativeActions = 0;
    int32 TotalActions = 0;
    
    for (const FVirtueAction& Action : RecentActions)
    {
        if (Action.Timestamp >= Cutoff)
        {
            TotalActions++;
            if (Action.bIsPositive)
            {
                PositiveActions++;
            }
            else
            {
                NegativeActions++;
            }
        }
    }
    
    float ConsistencyScore = 50.0f;
    if (TotalActions > 0)
    {
        ConsistencyScore = (float)PositiveActions / TotalActions * 100.0f;
    }
    
    // Update virtue consistency score
    if (FVirtueData* Virtue = Virtues.Find(VirtueType))
    {
        Virtue->ConsistencyScore = ConsistencyScore;
    }
    
    return ConsistencyScore >= (ConsistencyRequirement * 100.0f);
}

float UVirtueManager::CalculateVirtueGrowthRate(EVirtueType VirtueType)
{
    TArray<FVirtueAction> RecentActions = GetActionsByVirtue(VirtueType, 10);
    
    if (RecentActions.Num() < 2)
    {
        return 0.0f;
    }
    
    // Calculate average time between actions
    float TotalTimeDays = 0.0f;
    for (int32 i = 1; i < RecentActions.Num(); i++)
    {
        float TimeDiff = (RecentActions[i-1].Timestamp - RecentActions[i].Timestamp).GetTotalDays();
        TotalTimeDays += TimeDiff;
    }
    
    float AverageInterval = TotalTimeDays / (RecentActions.Num() - 1);
    
    // Calculate positive action ratio
    int32 PositiveActions = 0;
    for (const FVirtueAction& Action : RecentActions)
    {
        if (Action.bIsPositive)
        {
            PositiveActions++;
        }
    }
    
    float PositiveRatio = (float)PositiveActions / RecentActions.Num();
    
    // Growth rate based on frequency and positivity
    float GrowthRate = (PositiveRatio * 2.0f - 1.0f) / FMath::Max(AverageInterval, 0.1f);
    
    return GrowthRate;
}

// Virtue-specific methods
void UVirtueManager::RecordWisdomAction(const FString& DecisionType, bool bWasWise, float Complexity)
{
    float Impact = Complexity * (bWasWise ? 1.0f : 0.5f);
    FString Description = FString::Printf(TEXT("Decision: %s (Wise: %s, Complexity: %.1f)"), 
                                          *DecisionType, bWasWise ? TEXT("Yes") : TEXT("No"), Complexity);
    
    RecordVirtueAction(EVirtueType::Wisdom, TEXT("Decision Making"), Description, Impact, bWasWise);
}

void UVirtueManager::RecordCourageAction(const FString& ThreatType, bool bShowedCourage, float RiskLevel)
{
    float Impact = RiskLevel * (bShowedCourage ? 1.0f : 0.3f);
    FString Description = FString::Printf(TEXT("Threat: %s (Courage: %s, Risk: %.1f)"), 
                                          *ThreatType, bShowedCourage ? TEXT("Yes") : TEXT("No"), RiskLevel);
    
    RecordVirtueAction(EVirtueType::Courage, TEXT("Risk Taking"), Description, Impact, bShowedCourage);
}

void UVirtueManager::RecordJusticeAction(const FString& SituationType, bool bActedJustly, float MoralWeight)
{
    float Impact = MoralWeight * (bActedJustly ? 1.0f : 0.2f);
    FString Description = FString::Printf(TEXT("Situation: %s (Just: %s, Weight: %.1f)"), 
                                          *SituationType, bActedJustly ? TEXT("Yes") : TEXT("No"), MoralWeight);
    
    RecordVirtueAction(EVirtueType::Justice, TEXT("Moral Decision"), Description, Impact, bActedJustly);
}

void UVirtueManager::RecordTemperanceAction(const FString& TemptationType, bool bShowedRestraint, float TemptationStrength)
{
    float Impact = TemptationStrength * (bShowedRestraint ? 1.0f : 0.1f);
    FString Description = FString::Printf(TEXT("Temptation: %s (Restraint: %s, Strength: %.1f)"), 
                                          *TemptationType, bShowedRestraint ? TEXT("Yes") : TEXT("No"), TemptationStrength);
    
    RecordVirtueAction(EVirtueType::Temperance, TEXT("Self Control"), Description, Impact, bShowedRestraint);
}

// Helper Functions
void UVirtueManager::InitializeVirtue(EVirtueType VirtueType)
{
    FVirtueData NewVirtue;
    NewVirtue.VirtueType = VirtueType;
    NewVirtue.CurrentLevel = 50.0f; // Start with neutral level
    NewVirtue.DevelopmentState = EVirtueDevelopment::Moderate;
    NewVirtue.ExperiencePoints = 0;
    NewVirtue.LastUpdated = FDateTime::Now();
    NewVirtue.bIsActive = true;
    NewVirtue.RecentActions = 0;
    NewVirtue.ConsistencyScore = 50.0f;
    
    Virtues.Add(VirtueType, NewVirtue);
}

void UVirtueManager::UpdateVirtueDevelopmentState(EVirtueType VirtueType)
{
    if (FVirtueData* Virtue = Virtues.Find(VirtueType))
    {
        EVirtueDevelopment OldState = Virtue->DevelopmentState;
        Virtue->DevelopmentState = DetermineVirtueDevelopmentState(Virtue->CurrentLevel, Virtue->ConsistencyScore);
        
        if (OldState != Virtue->DevelopmentState)
        {
            OnVirtueDevelopmentChanged.Broadcast(VirtueType, Virtue->DevelopmentState);
        }
    }
}

EVirtueDevelopment UVirtueManager::DetermineVirtueDevelopmentState(float VirtueLevel, float ConsistencyScore) const
{
    // Consider both level and consistency
    float AdjustedLevel = VirtueLevel * (ConsistencyScore / 100.0f);
    
    if (VirtueLevel > 95.0f)
    {
        return EVirtueDevelopment::Excessive; // Too much of a good thing
    }
    else if (AdjustedLevel >= 80.0f)
    {
        return EVirtueDevelopment::Exemplary;
    }
    else if (AdjustedLevel >= 65.0f)
    {
        return EVirtueDevelopment::Strong;
    }
    else if (AdjustedLevel >= 40.0f)
    {
        return EVirtueDevelopment::Moderate;
    }
    else if (AdjustedLevel >= 25.0f)
    {
        return EVirtueDevelopment::Developing;
    }
    else
    {
        return EVirtueDevelopment::Deficient;
    }
}

void UVirtueManager::UpdatePlayerValueFromAction(const FVirtueAction& Action)
{
    // Map virtue actions to player values
    switch (Action.VirtueType)
    {
        case EVirtueType::Wisdom:
            UpdatePlayerValue(EPlayerValue::SelfDirection, Action.ImpactMagnitude * (Action.bIsPositive ? 1.0f : -1.0f), Action.Description);
            break;
        case EVirtueType::Courage:
            UpdatePlayerValue(EPlayerValue::Achievement, Action.ImpactMagnitude * (Action.bIsPositive ? 1.0f : -1.0f), Action.Description);
            UpdatePlayerValue(EPlayerValue::Stimulation, Action.ImpactMagnitude * (Action.bIsPositive ? 0.5f : -0.5f), Action.Description);
            break;
        case EVirtueType::Justice:
            UpdatePlayerValue(EPlayerValue::Universalism, Action.ImpactMagnitude * (Action.bIsPositive ? 1.0f : -1.0f), Action.Description);
            UpdatePlayerValue(EPlayerValue::Benevolence, Action.ImpactMagnitude * (Action.bIsPositive ? 0.8f : -0.8f), Action.Description);
            break;
        case EVirtueType::Temperance:
            UpdatePlayerValue(EPlayerValue::Security, Action.ImpactMagnitude * (Action.bIsPositive ? 1.0f : -1.0f), Action.Description);
            UpdatePlayerValue(EPlayerValue::Conformity, Action.ImpactMagnitude * (Action.bIsPositive ? 0.6f : -0.6f), Action.Description);
            break;
    }
}

void UVirtueManager::CleanupOldActions()
{
    // Remove oldest actions when history gets too large
    if (ActionHistory.Num() > MaxActionHistory)
    {
        int32 ToRemove = ActionHistory.Num() - MaxActionHistory;
        ActionHistory.RemoveAt(0, ToRemove);
        UE_LOG(LogProjectVisible, Log, TEXT("Cleaned up %d old virtue actions"), ToRemove);
    }
}

// Placeholder implementations for remaining functions
void UVirtueManager::AnalyzeBehaviorPattern(const FString& BehaviorType, float Intensity)
{
    BehaviorPatterns.Add(BehaviorType, Intensity);
    UE_LOG(LogProjectVisible, Log, TEXT("Analyzed behavior pattern: %s (Intensity: %.2f)"), *BehaviorType, Intensity);
}

FString UVirtueManager::GenerateVirtueReport() const
{
    FString Report = TEXT("=== Virtue Assessment Report ===\n");
    
    for (const auto& VirtuePair : Virtues)
    {
        const FVirtueData& Virtue = VirtuePair.Value;
        Report += FString::Printf(TEXT("%s: Level %.1f (%s) - Consistency: %.1f%%\n"), 
                                  *UEnum::GetValueAsString(VirtuePair.Key),
                                  Virtue.CurrentLevel,
                                  *UEnum::GetValueAsString(Virtue.DevelopmentState),
                                  Virtue.ConsistencyScore);
    }
    
    Report += FString::Printf(TEXT("\nOverall Virtue Score: %.1f\n"), GetOverallVirtueScore());
    Report += FString::Printf(TEXT("Total Actions Recorded: %d\n"), ActionHistory.Num());
    
    return Report;
}

TMap<EVirtueType, float> UVirtueManager::GetVirtueCorrelations() const
{
    // Placeholder for correlation analysis
    TMap<EVirtueType, float> Correlations;
    
    for (const auto& VirtuePair : Virtues)
    {
        Correlations.Add(VirtuePair.Key, 0.5f); // Neutral correlation
    }
    
    return Correlations;
}
