// Project Visible - Memory System Manager
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/World.h"
#include "VirtueMemoryTypes.h"
#include "MemoryManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemoryCreated, const FMemoryEntry&, Memory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemoryAccessed, int32, MemoryID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemoryForgotten, int32, MemoryID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMemoryCapacityChanged, int32, OldCapacity, int32, NewCapacity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHappinessUpdated, const FHappinessMetrics&, Metrics);

/**
 * Central manager for memory system and happiness analysis
 * Manages memory capacity, decay, and player well-being assessment
 */
UCLASS()
class GEMINI_C_API UMemoryManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Memory Management
    UFUNCTION(BlueprintCallable, Category = "Memory System")
    void InitializeMemorySystem(const FMemoryCapacityConfig& Config);

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    int32 CreateMemory(const FString& Title, const FString& Content, EMemoryType Type, 
                       EMemoryImportance Importance, float EmotionalIntensity = 50.0f);

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    bool AccessMemory(int32 MemoryID);

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    void ForgetMemory(int32 MemoryID, bool bForceForget = false);

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    void ProcessMemoryDecay(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    void ManageMemoryCapacity();

    // Memory Retrieval
    UFUNCTION(BlueprintCallable, Category = "Memory System", BlueprintPure)
    bool GetMemoryEntry(int32 MemoryID, FMemoryEntry& OutMemory) const;

    UFUNCTION(BlueprintCallable, Category = "Memory System", BlueprintPure)
    TArray<FMemoryEntry> GetMemoriesByType(EMemoryType Type) const;

    UFUNCTION(BlueprintCallable, Category = "Memory System", BlueprintPure)
    TArray<FMemoryEntry> GetMemoriesByImportance(EMemoryImportance Importance) const;

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    TArray<FMemoryEntry> SearchMemories(const FString& SearchTerm) const;

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    TArray<FMemoryEntry> GetRecentMemories(int32 Count = 10) const;

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    TArray<FMemoryEntry> GetAssociatedMemories(int32 MemoryID) const;

    // Memory Analysis
    UFUNCTION(BlueprintCallable, Category = "Memory System")
    float CalculateMemoryValue(int32 MemoryID) const;

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    void CreateMemoryAssociation(int32 MemoryID1, int32 MemoryID2);

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    void RemoveMemoryAssociation(int32 MemoryID1, int32 MemoryID2);

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    float CalculateMemoryNetworkDensity() const;

    // Capacity Management
    UFUNCTION(BlueprintCallable, Category = "Memory System", BlueprintPure)
    int32 GetCurrentMemoryCount() const { return Memories.Num(); }

    UFUNCTION(BlueprintCallable, Category = "Memory System", BlueprintPure)
    int32 GetMemoryCapacity() const { return CapacityConfig.MaxMemories; }

    UFUNCTION(BlueprintCallable, Category = "Memory System", BlueprintPure)
    float GetMemoryUsagePercentage() const;

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    void SetMemoryCapacity(int32 NewCapacity);

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    TArray<int32> GetCandidatesForForgetting(int32 Count = 5) const;

    // Happiness Analysis
    UFUNCTION(BlueprintCallable, Category = "Memory System")
    void UpdateHappinessMetrics();

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    void RecordHappinessEvent(const FString& EventType, float HappinessImpact, float Intensity = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Memory System", BlueprintPure)
    FHappinessMetrics GetCurrentHappinessMetrics() const { return CurrentHappiness; }

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    float CalculateLifeSatisfaction() const;

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    float CalculateEudaimonia() const;

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    float CalculateFlowState() const;

    // Memory Emotional Processing
    UFUNCTION(BlueprintCallable, Category = "Memory System")
    void ProcessEmotionalMemories();

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    void RepressTraumaticMemory(int32 MemoryID);

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    void RecoverRepressedMemory(int32 MemoryID);

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    TArray<FMemoryEntry> GetEmotionalMemories(float MinIntensity = 70.0f) const;

    // Memory Consolidation
    UFUNCTION(BlueprintCallable, Category = "Memory System")
    void ConsolidateMemories();

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    void TransferToLongTermMemory(int32 MemoryID);

    UFUNCTION(BlueprintCallable, Category = "Memory System")
    bool IsMemoryConsolidated(int32 MemoryID) const;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Memory Events")
    FOnMemoryCreated OnMemoryCreated;

    UPROPERTY(BlueprintAssignable, Category = "Memory Events")
    FOnMemoryAccessed OnMemoryAccessed;

    UPROPERTY(BlueprintAssignable, Category = "Memory Events")
    FOnMemoryForgotten OnMemoryForgotten;

    UPROPERTY(BlueprintAssignable, Category = "Memory Events")
    FOnMemoryCapacityChanged OnMemoryCapacityChanged;

    UPROPERTY(BlueprintAssignable, Category = "Memory Events")
    FOnHappinessUpdated OnHappinessUpdated;

protected:
    // Memory Storage
    UPROPERTY(BlueprintReadOnly, Category = "Memory Data")
    TArray<FMemoryEntry> Memories;

    UPROPERTY(BlueprintReadOnly, Category = "Memory Data")
    FMemoryCapacityConfig CapacityConfig;

    UPROPERTY(BlueprintReadOnly, Category = "Memory Data")
    FHappinessMetrics CurrentHappiness;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Settings")
    float MemoryDecayRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Settings")
    float EmotionalRetentionMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Settings")
    float ImportanceRetentionMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Settings")
    bool bUseMemoryDecay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Settings")
    bool bAutoManageCapacity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory Settings")
    float HappinessUpdateFrequency;

    // ID Generators
    UPROPERTY()
    int32 NextMemoryID;

    // Timing
    UPROPERTY()
    FDateTime LastCapacityCheck;

    UPROPERTY()
    FDateTime LastHappinessUpdate;

    UPROPERTY()
    FDateTime LastConsolidation;

private:
    // Helper Functions
    FMemoryEntry* GetMemoryByID(int32 MemoryID);
    const FMemoryEntry* GetMemoryByID(int32 MemoryID) const;
    void UpdateMemoryClarity(FMemoryEntry& Memory, float DeltaTime);
    void ApplyMemoryDecay(FMemoryEntry& Memory, float DeltaTime);
    bool ShouldForgetMemory(const FMemoryEntry& Memory) const;
    
    // Capacity Management
    void EnforceCapacityLimits();
    void SelectMemoriesToForget(int32 CountToForget);
    float CalculateMemoryRetentionScore(const FMemoryEntry& Memory) const;
    
    // Happiness Calculations
    void AnalyzeEmotionalMemories();
    void CalculatePositiveAffect();
    void CalculateNegativeAffect();
    void AssessMeaningAndPurpose();
    void EvaluateEngagementLevel();
    
    // Memory Processing
    void ProcessMemoryAssociations();
    void UpdateMemoryImportance(FMemoryEntry& Memory);
    void HandleEmotionalConsolidation(FMemoryEntry& Memory);
    
    // Analysis Functions
    TMap<EMemoryType, int32> GetMemoryTypeDistribution() const;
    TMap<EMemoryImportance, int32> GetMemoryImportanceDistribution() const;
    float CalculateAverageEmotionalIntensity() const;
    float CalculateMemoryDiversity() const;
};
