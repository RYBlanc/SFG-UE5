// Project Visible - Virtue System Manager
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/World.h"
#include "VirtueMemoryTypes.h"
#include "VirtueManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnVirtueChanged, EVirtueType, VirtueType, float, OldLevel, float, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVirtueDevelopmentChanged, EVirtueType, VirtueType, EVirtueDevelopment, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVirtueActionRecorded, const FVirtueAction&, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerValuesUpdated, const TArray<FPlayerValueAssessment>&, Values);

/**
 * Central manager for Aristotelian virtue system
 * Tracks player actions and develops virtue levels based on ethical behavior
 */
UCLASS()
class GEMINI_C_API UVirtueManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Virtue Management
    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    void InitializeVirtues();

    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    void RecordVirtueAction(EVirtueType VirtueType, const FString& ActionType, const FString& Description, 
                           float ImpactMagnitude, bool bIsPositive = true);

    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    void UpdateVirtueLevel(EVirtueType VirtueType, float Delta, const FString& Reason = TEXT(""));

    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    float CalculateVirtueLevel(EVirtueType VirtueType);

    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    void ProcessVirtueDecay(float DeltaTime);

    // Virtue Assessment
    UFUNCTION(BlueprintCallable, Category = "Virtue System", BlueprintPure)
    bool GetVirtueData(EVirtueType VirtueType, FVirtueData& OutVirtueData) const;

    UFUNCTION(BlueprintCallable, Category = "Virtue System", BlueprintPure)
    TArray<FVirtueData> GetAllVirtues() const;

    UFUNCTION(BlueprintCallable, Category = "Virtue System", BlueprintPure)
    float GetVirtueLevel(EVirtueType VirtueType) const;

    UFUNCTION(BlueprintCallable, Category = "Virtue System", BlueprintPure)
    EVirtueDevelopment GetVirtueDevelopmentState(EVirtueType VirtueType) const;

    UFUNCTION(BlueprintCallable, Category = "Virtue System", BlueprintPure)
    float GetOverallVirtueScore() const;

    // Player Value Analysis
    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    void AssessPlayerValues();

    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    void UpdatePlayerValue(EPlayerValue ValueType, float Evidence, const FString& Context);

    UFUNCTION(BlueprintCallable, Category = "Virtue System", BlueprintPure)
    TArray<FPlayerValueAssessment> GetPlayerValueProfile() const;

    UFUNCTION(BlueprintCallable, Category = "Virtue System", BlueprintPure)
    float GetValueStrength(EPlayerValue ValueType) const;

    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    TArray<EPlayerValue> GetDominantValues(int32 TopCount = 3) const;

    // Virtue Action History
    UFUNCTION(BlueprintCallable, Category = "Virtue System", BlueprintPure)
    TArray<FVirtueAction> GetRecentActions(int32 Count = 10) const;

    UFUNCTION(BlueprintCallable, Category = "Virtue System", BlueprintPure)
    TArray<FVirtueAction> GetActionsByVirtue(EVirtueType VirtueType, int32 Count = 10) const;

    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    void ClearActionHistory();

    // Virtue Development
    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    void AdvanceVirtueDevelopment(EVirtueType VirtueType);

    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    bool CheckVirtueConsistency(EVirtueType VirtueType, float TimeframeDays = 7.0f);

    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    float CalculateVirtueGrowthRate(EVirtueType VirtueType);

    // Behavioral Analysis
    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    void AnalyzeBehaviorPattern(const FString& BehaviorType, float Intensity);

    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    FString GenerateVirtueReport() const;

    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    TMap<EVirtueType, float> GetVirtueCorrelations() const;

    // Wisdom-specific methods
    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    void RecordWisdomAction(const FString& DecisionType, bool bWasWise, float Complexity = 1.0f);

    // Courage-specific methods
    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    void RecordCourageAction(const FString& ThreatType, bool bShowedCourage, float RiskLevel = 1.0f);

    // Justice-specific methods
    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    void RecordJusticeAction(const FString& SituationType, bool bActedJustly, float MoralWeight = 1.0f);

    // Temperance-specific methods
    UFUNCTION(BlueprintCallable, Category = "Virtue System")
    void RecordTemperanceAction(const FString& TemptationType, bool bShowedRestraint, float TemptationStrength = 1.0f);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Virtue Events")
    FOnVirtueChanged OnVirtueChanged;

    UPROPERTY(BlueprintAssignable, Category = "Virtue Events")
    FOnVirtueDevelopmentChanged OnVirtueDevelopmentChanged;

    UPROPERTY(BlueprintAssignable, Category = "Virtue Events")
    FOnVirtueActionRecorded OnVirtueActionRecorded;

    UPROPERTY(BlueprintAssignable, Category = "Virtue Events")
    FOnPlayerValuesUpdated OnPlayerValuesUpdated;

protected:
    // Virtue Data
    UPROPERTY(BlueprintReadOnly, Category = "Virtue Data")
    TMap<EVirtueType, FVirtueData> Virtues;

    UPROPERTY(BlueprintReadOnly, Category = "Virtue Data")
    TArray<FVirtueAction> ActionHistory;

    UPROPERTY(BlueprintReadOnly, Category = "Virtue Data")
    TMap<EPlayerValue, FPlayerValueAssessment> PlayerValues;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue Settings")
    float VirtueDecayRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue Settings")
    float ActionImpactMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue Settings")
    int32 MaxActionHistory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue Settings")
    float ConsistencyRequirement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue Settings")
    bool bUseVirtueDecay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Virtue Settings")
    bool bTrackPlayerValues;

    // ID Generators
    UPROPERTY()
    int32 NextActionID;

    // Analysis Data
    UPROPERTY()
    FDateTime LastAnalysis;

    UPROPERTY()
    TMap<FString, float> BehaviorPatterns;

private:
    // Helper Functions
    void InitializeVirtue(EVirtueType VirtueType);
    void UpdateVirtueDevelopmentState(EVirtueType VirtueType);
    float CalculateVirtueExperience(const TArray<FVirtueAction>& Actions) const;
    void ProcessVirtueCorrelations();
    void UpdatePlayerValueFromAction(const FVirtueAction& Action);
    EVirtueDevelopment DetermineVirtueDevelopmentState(float VirtueLevel, float ConsistencyScore) const;
    
    // Analysis Functions
    void AnalyzeVirtueConsistency();
    void AnalyzeValueAlignment();
    float CalculateActionImpact(const FVirtueAction& Action) const;
    void CleanupOldActions();
    
    // Virtue-specific calculations
    float CalculateWisdomImpact(const FVirtueAction& Action) const;
    float CalculateCourageImpact(const FVirtueAction& Action) const;
    float CalculateJusticeImpact(const FVirtueAction& Action) const;
    float CalculateTemperanceImpact(const FVirtueAction& Action) const;
};
