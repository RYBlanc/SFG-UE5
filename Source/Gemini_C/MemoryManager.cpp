// Project Visible - Memory System Manager Implementation
// Copyright Epic Games, Inc. All Rights Reserved.

#include "MemoryManager.h"
#include "Gemini_C.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

void UMemoryManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Initialize settings
    MemoryDecayRate = 0.05f;
    EmotionalRetentionMultiplier = 1.5f;
    ImportanceRetentionMultiplier = 2.0f;
    bUseMemoryDecay = true;
    bAutoManageCapacity = true;
    HappinessUpdateFrequency = 300.0f; // 5 minutes
    
    // Initialize ID generator
    NextMemoryID = 1;
    
    // Initialize timing
    LastCapacityCheck = FDateTime::Now();
    LastHappinessUpdate = FDateTime::Now();
    LastConsolidation = FDateTime::Now();
    
    // Initialize default capacity config
    FMemoryCapacityConfig DefaultConfig;
    InitializeMemorySystem(DefaultConfig);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Memory Manager initialized"));
}

void UMemoryManager::Deinitialize()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Memory Manager shutting down"));
    Super::Deinitialize();
}

// Memory Management
void UMemoryManager::InitializeMemorySystem(const FMemoryCapacityConfig& Config)
{
    CapacityConfig = Config;
    
    // Initialize happiness metrics
    CurrentHappiness = FHappinessMetrics();
    CurrentHappiness.OverallHappiness = 50.0f;
    CurrentHappiness.LifeSatisfaction = 50.0f;
    CurrentHappiness.PositiveAffect = 50.0f;
    CurrentHappiness.NegativeAffect = 50.0f;
    CurrentHappiness.Eudaimonia = 50.0f;
    CurrentHappiness.Flow = 50.0f;
    CurrentHappiness.Meaning = 50.0f;
    CurrentHappiness.Engagement = 50.0f;
    
    UE_LOG(LogProjectVisible, Log, TEXT("Memory system initialized with capacity: %d"), Config.MaxMemories);
}

int32 UMemoryManager::CreateMemory(const FString& Title, const FString& Content, EMemoryType Type, 
                                   EMemoryImportance Importance, float EmotionalIntensity)
{
    // Check capacity limits
    if (bAutoManageCapacity && Memories.Num() >= CapacityConfig.MaxMemories)
    {
        ManageMemoryCapacity();
    }
    
    FMemoryEntry NewMemory;
    NewMemory.MemoryID = NextMemoryID++;
    NewMemory.MemoryTitle = Title;
    NewMemory.MemoryContent = Content;
    NewMemory.MemoryType = Type;
    NewMemory.Importance = Importance;
    NewMemory.EmotionalIntensity = FMath::Clamp(EmotionalIntensity, 0.0f, 100.0f);
    NewMemory.Clarity = 100.0f;
    NewMemory.CreationTime = FDateTime::Now();
    NewMemory.LastAccessed = FDateTime::Now();
    NewMemory.AccessCount = 0;
    NewMemory.bIsFading = false;
    NewMemory.bIsRepressed = false;
    NewMemory.DecayRate = MemoryDecayRate;
    
    // Adjust decay rate based on importance and emotion
    if (Importance >= EMemoryImportance::High)
    {
        NewMemory.DecayRate *= 0.5f;
    }
    if (EmotionalIntensity > 70.0f)
    {
        NewMemory.DecayRate *= EmotionalRetentionMultiplier;
    }
    
    Memories.Add(NewMemory);
    
    OnMemoryCreated.Broadcast(NewMemory);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Created memory: %s (ID: %d, Type: %s, Importance: %s)"), 
           *Title, NewMemory.MemoryID, *UEnum::GetValueAsString(Type), *UEnum::GetValueAsString(Importance));
    
    return NewMemory.MemoryID;
}

bool UMemoryManager::AccessMemory(int32 MemoryID)
{
    FMemoryEntry* Memory = GetMemoryByID(MemoryID);
    if (!Memory || Memory->bIsRepressed)
    {
        return false;
    }
    
    Memory->LastAccessed = FDateTime::Now();
    Memory->AccessCount++;
    
    // Strengthen memory through access
    Memory->Clarity = FMath::Min(Memory->Clarity + 5.0f, 100.0f);
    
    OnMemoryAccessed.Broadcast(MemoryID);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Accessed memory: %s (Access count: %d)"), 
           *Memory->MemoryTitle, Memory->AccessCount);
    
    return true;
}

void UMemoryManager::ForgetMemory(int32 MemoryID, bool bForceForget)
{
    for (int32 i = 0; i < Memories.Num(); i++)
    {
        if (Memories[i].MemoryID == MemoryID)
        {
            // Check if memory can be forgotten
            if (!bForceForget && Memories[i].Importance >= EMemoryImportance::Critical)
            {
                UE_LOG(LogProjectVisible, Warning, TEXT("Cannot forget critical memory: %s"), *Memories[i].MemoryTitle);
                return;
            }
            
            OnMemoryForgotten.Broadcast(MemoryID);
            
            UE_LOG(LogProjectVisible, Log, TEXT("Forgot memory: %s"), *Memories[i].MemoryTitle);
            
            Memories.RemoveAt(i);
            return;
        }
    }
}

void UMemoryManager::ProcessMemoryDecay(float DeltaTime)
{
    if (!bUseMemoryDecay)
    {
        return;
    }
    
    TArray<int32> MemoriesToForget;
    
    for (FMemoryEntry& Memory : Memories)
    {
        if (!Memory.bIsRepressed)
        {
            ApplyMemoryDecay(Memory, DeltaTime);
            UpdateMemoryClarity(Memory, DeltaTime);
            
            if (ShouldForgetMemory(Memory))
            {
                MemoriesToForget.Add(Memory.MemoryID);
            }
        }
    }
    
    // Forget memories that have decayed too much
    for (int32 MemoryID : MemoriesToForget)
    {
        ForgetMemory(MemoryID, false);
    }
}

void UMemoryManager::ManageMemoryCapacity()
{
    if (Memories.Num() <= CapacityConfig.MaxMemories)
    {
        return;
    }
    
    int32 ToForget = Memories.Num() - CapacityConfig.MaxMemories;
    TArray<int32> Candidates = GetCandidatesForForgetting(ToForget);
    
    for (int32 MemoryID : Candidates)
    {
        ForgetMemory(MemoryID, false);
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Managed memory capacity: forgot %d memories"), Candidates.Num());
}

// Memory Retrieval
bool UMemoryManager::GetMemoryEntry(int32 MemoryID, FMemoryEntry& OutMemory) const
{
    const FMemoryEntry* Memory = GetMemoryByID(MemoryID);
    if (Memory && !Memory->bIsRepressed)
    {
        OutMemory = *Memory;
        return true;
    }
    return false;
}

TArray<FMemoryEntry> UMemoryManager::GetMemoriesByType(EMemoryType Type) const
{
    TArray<FMemoryEntry> TypeMemories;
    
    for (const FMemoryEntry& Memory : Memories)
    {
        if (Memory.MemoryType == Type && !Memory.bIsRepressed)
        {
            TypeMemories.Add(Memory);
        }
    }
    
    return TypeMemories;
}

TArray<FMemoryEntry> UMemoryManager::GetMemoriesByImportance(EMemoryImportance Importance) const
{
    TArray<FMemoryEntry> ImportantMemories;
    
    for (const FMemoryEntry& Memory : Memories)
    {
        if (Memory.Importance == Importance && !Memory.bIsRepressed)
        {
            ImportantMemories.Add(Memory);
        }
    }
    
    return ImportantMemories;
}

TArray<FMemoryEntry> UMemoryManager::SearchMemories(const FString& SearchTerm) const
{
    TArray<FMemoryEntry> SearchResults;
    
    for (const FMemoryEntry& Memory : Memories)
    {
        if (!Memory.bIsRepressed && 
            (Memory.MemoryTitle.Contains(SearchTerm) || Memory.MemoryContent.Contains(SearchTerm)))
        {
            SearchResults.Add(Memory);
        }
    }
    
    return SearchResults;
}

TArray<FMemoryEntry> UMemoryManager::GetRecentMemories(int32 Count) const
{
    TArray<FMemoryEntry> RecentMemories = Memories;
    
    // Sort by creation time (most recent first)
    RecentMemories.Sort([](const FMemoryEntry& A, const FMemoryEntry& B)
    {
        return A.CreationTime > B.CreationTime;
    });
    
    // Filter out repressed memories
    RecentMemories.RemoveAll([](const FMemoryEntry& Memory)
    {
        return Memory.bIsRepressed;
    });
    
    if (Count > 0 && RecentMemories.Num() > Count)
    {
        RecentMemories.SetNum(Count);
    }
    
    return RecentMemories;
}

TArray<FMemoryEntry> UMemoryManager::GetAssociatedMemories(int32 MemoryID) const
{
    TArray<FMemoryEntry> AssociatedMemories;
    
    const FMemoryEntry* Memory = GetMemoryByID(MemoryID);
    if (!Memory)
    {
        return AssociatedMemories;
    }
    
    for (int32 AssociatedID : Memory->AssociatedMemories)
    {
        const FMemoryEntry* AssociatedMemory = GetMemoryByID(AssociatedID);
        if (AssociatedMemory && !AssociatedMemory->bIsRepressed)
        {
            AssociatedMemories.Add(*AssociatedMemory);
        }
    }
    
    return AssociatedMemories;
}

// Memory Analysis
float UMemoryManager::CalculateMemoryValue(int32 MemoryID) const
{
    const FMemoryEntry* Memory = GetMemoryByID(MemoryID);
    if (!Memory)
    {
        return 0.0f;
    }
    
    float Value = 0.0f;
    
    // Base value from importance
    switch (Memory->Importance)
    {
        case EMemoryImportance::Trivial: Value = 10.0f; break;
        case EMemoryImportance::Low: Value = 25.0f; break;
        case EMemoryImportance::Medium: Value = 50.0f; break;
        case EMemoryImportance::High: Value = 75.0f; break;
        case EMemoryImportance::Critical: Value = 90.0f; break;
        case EMemoryImportance::Core: Value = 100.0f; break;
    }
    
    // Adjust for emotional intensity
    Value += Memory->EmotionalIntensity * 0.2f;
    
    // Adjust for access frequency
    Value += FMath::Min(Memory->AccessCount * 2.0f, 20.0f);
    
    // Adjust for clarity
    Value *= (Memory->Clarity / 100.0f);
    
    // Adjust for associations
    Value += Memory->AssociatedMemories.Num() * 5.0f;
    
    return FMath::Clamp(Value, 0.0f, 100.0f);
}

void UMemoryManager::CreateMemoryAssociation(int32 MemoryID1, int32 MemoryID2)
{
    FMemoryEntry* Memory1 = GetMemoryByID(MemoryID1);
    FMemoryEntry* Memory2 = GetMemoryByID(MemoryID2);
    
    if (Memory1 && Memory2)
    {
        Memory1->AssociatedMemories.AddUnique(MemoryID2);
        Memory2->AssociatedMemories.AddUnique(MemoryID1);
        
        UE_LOG(LogProjectVisible, Log, TEXT("Created memory association: %d <-> %d"), MemoryID1, MemoryID2);
    }
}

void UMemoryManager::RemoveMemoryAssociation(int32 MemoryID1, int32 MemoryID2)
{
    FMemoryEntry* Memory1 = GetMemoryByID(MemoryID1);
    FMemoryEntry* Memory2 = GetMemoryByID(MemoryID2);
    
    if (Memory1 && Memory2)
    {
        Memory1->AssociatedMemories.Remove(MemoryID2);
        Memory2->AssociatedMemories.Remove(MemoryID1);
        
        UE_LOG(LogProjectVisible, Log, TEXT("Removed memory association: %d <-> %d"), MemoryID1, MemoryID2);
    }
}

float UMemoryManager::CalculateMemoryNetworkDensity() const
{
    if (Memories.Num() <= 1)
    {
        return 0.0f;
    }
    
    int32 TotalPossibleConnections = Memories.Num() * (Memories.Num() - 1) / 2;
    int32 ActualConnections = 0;
    
    for (const FMemoryEntry& Memory : Memories)
    {
        ActualConnections += Memory.AssociatedMemories.Num();
    }
    
    ActualConnections /= 2; // Each connection is counted twice
    
    return (float)ActualConnections / TotalPossibleConnections;
}

// Capacity Management
float UMemoryManager::GetMemoryUsagePercentage() const
{
    return (float)Memories.Num() / CapacityConfig.MaxMemories * 100.0f;
}

void UMemoryManager::SetMemoryCapacity(int32 NewCapacity)
{
    int32 OldCapacity = CapacityConfig.MaxMemories;
    CapacityConfig.MaxMemories = FMath::Max(NewCapacity, 10); // Minimum 10 memories
    
    OnMemoryCapacityChanged.Broadcast(OldCapacity, CapacityConfig.MaxMemories);
    
    // Manage capacity if needed
    if (bAutoManageCapacity)
    {
        ManageMemoryCapacity();
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Memory capacity changed: %d -> %d"), OldCapacity, CapacityConfig.MaxMemories);
}

TArray<int32> UMemoryManager::GetCandidatesForForgetting(int32 Count) const
{
    TArray<FMemoryEntry> CandidateMemories;
    
    // Find memories that can be forgotten
    for (const FMemoryEntry& Memory : Memories)
    {
        if (Memory.Importance < EMemoryImportance::Critical && !Memory.bIsRepressed)
        {
            CandidateMemories.Add(Memory);
        }
    }
    
    // Sort by retention score (lowest first)
    CandidateMemories.Sort([this](const FMemoryEntry& A, const FMemoryEntry& B)
    {
        return CalculateMemoryRetentionScore(A) < CalculateMemoryRetentionScore(B);
    });
    
    TArray<int32> CandidateIDs;
    for (int32 i = 0; i < FMath::Min(Count, CandidateMemories.Num()); i++)
    {
        CandidateIDs.Add(CandidateMemories[i].MemoryID);
    }
    
    return CandidateIDs;
}

// Happiness Analysis
void UMemoryManager::UpdateHappinessMetrics()
{
    AnalyzeEmotionalMemories();
    CalculatePositiveAffect();
    CalculateNegativeAffect();
    AssessMeaningAndPurpose();
    EvaluateEngagementLevel();
    
    // Calculate overall happiness
    CurrentHappiness.OverallHappiness = (
        CurrentHappiness.LifeSatisfaction * 0.3f +
        CurrentHappiness.PositiveAffect * 0.25f +
        (100.0f - CurrentHappiness.NegativeAffect) * 0.15f +
        CurrentHappiness.Eudaimonia * 0.2f +
        CurrentHappiness.Flow * 0.1f
    );
    
    CurrentHappiness.LastAssessment = FDateTime::Now();
    CurrentHappiness.SampleSize = Memories.Num();
    
    OnHappinessUpdated.Broadcast(CurrentHappiness);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Updated happiness metrics: Overall %.1f, Life Satisfaction %.1f"), 
           CurrentHappiness.OverallHappiness, CurrentHappiness.LifeSatisfaction);
}

void UMemoryManager::RecordHappinessEvent(const FString& EventType, float HappinessImpact, float Intensity)
{
    // Create memory for the happiness event
    EMemoryImportance Importance = EMemoryImportance::Medium;
    if (FMath::Abs(HappinessImpact) > 5.0f)
    {
        Importance = EMemoryImportance::High;
    }
    
    CreateMemory(
        FString::Printf(TEXT("Happiness Event: %s"), *EventType),
        FString::Printf(TEXT("Impact: %.1f, Intensity: %.1f"), HappinessImpact, Intensity),
        EMemoryType::Emotional,
        Importance,
        50.0f + HappinessImpact * 5.0f
    );
    
    // Update happiness metrics
    UpdateHappinessMetrics();
    
    UE_LOG(LogProjectVisible, Log, TEXT("Recorded happiness event: %s (Impact: %.1f)"), *EventType, HappinessImpact);
}

float UMemoryManager::CalculateLifeSatisfaction() const
{
    // Analyze memory content for life satisfaction indicators
    float Satisfaction = 50.0f; // Baseline
    
    TArray<FMemoryEntry> RecentMemories = GetRecentMemories(50);
    
    for (const FMemoryEntry& Memory : RecentMemories)
    {
        if (Memory.MemoryType == EMemoryType::Emotional)
        {
            if (Memory.EmotionalIntensity > 70.0f)
            {
                Satisfaction += 2.0f;
            }
            else if (Memory.EmotionalIntensity < 30.0f)
            {
                Satisfaction -= 1.0f;
            }
        }
    }
    
    return FMath::Clamp(Satisfaction, 0.0f, 100.0f);
}

float UMemoryManager::CalculateEudaimonia() const
{
    // Calculate flourishing/well-being based on meaning and virtue
    float Eudaimonia = 50.0f;
    
    int32 MeaningfulMemories = 0;
    int32 TotalMemories = 0;
    
    for (const FMemoryEntry& Memory : Memories)
    {
        if (!Memory.bIsRepressed)
        {
            TotalMemories++;
            if (Memory.Importance >= EMemoryImportance::High)
            {
                MeaningfulMemories++;
            }
        }
    }
    
    if (TotalMemories > 0)
    {
        float MeaningRatio = (float)MeaningfulMemories / TotalMemories;
        Eudaimonia = 50.0f + (MeaningRatio * 50.0f);
    }
    
    return FMath::Clamp(Eudaimonia, 0.0f, 100.0f);
}

float UMemoryManager::CalculateFlowState() const
{
    // Estimate flow based on engagement memories
    float Flow = 50.0f;
    
    TArray<FMemoryEntry> EngagementMemories;
    for (const FMemoryEntry& Memory : Memories)
    {
        if (Memory.MemoryType == EMemoryType::Procedural && Memory.EmotionalIntensity > 60.0f)
        {
            EngagementMemories.Add(Memory);
        }
    }
    
    if (EngagementMemories.Num() > 0)
    {
        float AverageEngagement = 0.0f;
        for (const FMemoryEntry& Memory : EngagementMemories)
        {
            AverageEngagement += Memory.EmotionalIntensity;
        }
        AverageEngagement /= EngagementMemories.Num();
        Flow = AverageEngagement;
    }
    
    return FMath::Clamp(Flow, 0.0f, 100.0f);
}

// Helper Functions
FMemoryEntry* UMemoryManager::GetMemoryByID(int32 MemoryID)
{
    for (FMemoryEntry& Memory : Memories)
    {
        if (Memory.MemoryID == MemoryID)
        {
            return &Memory;
        }
    }
    return nullptr;
}

const FMemoryEntry* UMemoryManager::GetMemoryByID(int32 MemoryID) const
{
    for (const FMemoryEntry& Memory : Memories)
    {
        if (Memory.MemoryID == MemoryID)
        {
            return &Memory;
        }
    }
    return nullptr;
}

void UMemoryManager::UpdateMemoryClarity(FMemoryEntry& Memory, float DeltaTime)
{
    // Clarity decreases over time unless accessed recently
    FDateTime Now = FDateTime::Now();
    float DaysSinceAccess = (Now - Memory.LastAccessed).GetTotalDays();
    
    if (DaysSinceAccess > 1.0f)
    {
        float ClarityLoss = Memory.DecayRate * DeltaTime * DaysSinceAccess;
        Memory.Clarity = FMath::Max(Memory.Clarity - ClarityLoss, 0.0f);
        
        if (Memory.Clarity < 50.0f)
        {
            Memory.bIsFading = true;
        }
    }
}

void UMemoryManager::ApplyMemoryDecay(FMemoryEntry& Memory, float DeltaTime)
{
    // Different decay rates for different memory types
    float TypeMultiplier = 1.0f;
    
    switch (Memory.MemoryType)
    {
        case EMemoryType::Episodic: TypeMultiplier = 1.0f; break;
        case EMemoryType::Semantic: TypeMultiplier = 0.5f; break;
        case EMemoryType::Procedural: TypeMultiplier = 0.3f; break;
        case EMemoryType::Emotional: TypeMultiplier = 0.8f; break;
        case EMemoryType::Traumatic: TypeMultiplier = 0.2f; break;
        default: TypeMultiplier = 1.0f; break;
    }
    
    Memory.DecayRate = MemoryDecayRate * TypeMultiplier;
}

bool UMemoryManager::ShouldForgetMemory(const FMemoryEntry& Memory) const
{
    // Never forget core or critical memories
    if (Memory.Importance >= EMemoryImportance::Critical)
    {
        return false;
    }
    
    // Forget if clarity is too low
    if (Memory.Clarity < CapacityConfig.DecayThreshold * 100.0f)
    {
        return true;
    }
    
    return false;
}

float UMemoryManager::CalculateMemoryRetentionScore(const FMemoryEntry& Memory) const
{
    float Score = 0.0f;
    
    // Base score from importance
    Score += (float)Memory.Importance * 20.0f;
    
    // Add emotional weight
    Score += Memory.EmotionalIntensity * EmotionalRetentionMultiplier;
    
    // Add clarity
    Score += Memory.Clarity;
    
    // Add access frequency
    Score += FMath::Min(Memory.AccessCount * 5.0f, 50.0f);
    
    // Add associations
    Score += Memory.AssociatedMemories.Num() * 10.0f;
    
    return Score;
}

// Placeholder implementations for remaining functions
void UMemoryManager::ProcessEmotionalMemories()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Processing emotional memories"));
}

void UMemoryManager::RepressTraumaticMemory(int32 MemoryID)
{
    FMemoryEntry* Memory = GetMemoryByID(MemoryID);
    if (Memory && Memory->MemoryType == EMemoryType::Traumatic)
    {
        Memory->bIsRepressed = true;
        UE_LOG(LogProjectVisible, Log, TEXT("Repressed traumatic memory: %s"), *Memory->MemoryTitle);
    }
}

void UMemoryManager::RecoverRepressedMemory(int32 MemoryID)
{
    FMemoryEntry* Memory = GetMemoryByID(MemoryID);
    if (Memory && Memory->bIsRepressed)
    {
        Memory->bIsRepressed = false;
        UE_LOG(LogProjectVisible, Log, TEXT("Recovered repressed memory: %s"), *Memory->MemoryTitle);
    }
}

TArray<FMemoryEntry> UMemoryManager::GetEmotionalMemories(float MinIntensity) const
{
    TArray<FMemoryEntry> EmotionalMemories;
    
    for (const FMemoryEntry& Memory : Memories)
    {
        if (!Memory.bIsRepressed && Memory.EmotionalIntensity >= MinIntensity)
        {
            EmotionalMemories.Add(Memory);
        }
    }
    
    return EmotionalMemories;
}

void UMemoryManager::ConsolidateMemories()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Consolidating memories"));
    LastConsolidation = FDateTime::Now();
}

void UMemoryManager::TransferToLongTermMemory(int32 MemoryID)
{
    FMemoryEntry* Memory = GetMemoryByID(MemoryID);
    if (Memory)
    {
        Memory->DecayRate *= 0.1f; // Significantly slower decay
        UE_LOG(LogProjectVisible, Log, TEXT("Transferred to long-term memory: %s"), *Memory->MemoryTitle);
    }
}

bool UMemoryManager::IsMemoryConsolidated(int32 MemoryID) const
{
    const FMemoryEntry* Memory = GetMemoryByID(MemoryID);
    return Memory && Memory->DecayRate < 0.01f;
}

void UMemoryManager::AnalyzeEmotionalMemories()
{
    // Placeholder for emotional analysis
    UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Analyzing emotional memories"));
}

void UMemoryManager::CalculatePositiveAffect()
{
    float PositiveSum = 0.0f;
    int32 PositiveCount = 0;
    
    for (const FMemoryEntry& Memory : Memories)
    {
        if (Memory.MemoryType == EMemoryType::Emotional && Memory.EmotionalIntensity > 60.0f)
        {
            PositiveSum += Memory.EmotionalIntensity;
            PositiveCount++;
        }
    }
    
    CurrentHappiness.PositiveAffect = PositiveCount > 0 ? PositiveSum / PositiveCount : 50.0f;
}

void UMemoryManager::CalculateNegativeAffect()
{
    float NegativeSum = 0.0f;
    int32 NegativeCount = 0;
    
    for (const FMemoryEntry& Memory : Memories)
    {
        if (Memory.MemoryType == EMemoryType::Emotional && Memory.EmotionalIntensity < 40.0f)
        {
            NegativeSum += (100.0f - Memory.EmotionalIntensity);
            NegativeCount++;
        }
    }
    
    CurrentHappiness.NegativeAffect = NegativeCount > 0 ? NegativeSum / NegativeCount : 50.0f;
}

void UMemoryManager::AssessMeaningAndPurpose()
{
    CurrentHappiness.Meaning = CalculateEudaimonia();
}

void UMemoryManager::EvaluateEngagementLevel()
{
    CurrentHappiness.Engagement = CalculateFlowState();
}
