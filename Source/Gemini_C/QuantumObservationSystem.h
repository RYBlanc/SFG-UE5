// Project Visible - Quantum Observation System
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/ActorComponent.h"
#include "NiagaraSystem.h"
#include "Materials/MaterialParameterCollection.h"
#include "LandscapeOfMindTypes.h"
#include "QuantumObservationSystem.generated.h"

class AUncertainArea;

/**
 * Global quantum state information
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FQuantumState
{
    GENERATED_BODY()

    FQuantumState()
    {
        GlobalCoherence = 1.0f;
        TotalUncertainAreas = 0;
        ObservedAreas = 0;
        EntanglementStrength = 0.0f;
        DecoherenceRate = 0.01f;
    }

    UPROPERTY(BlueprintReadWrite, Category = "Quantum State")
    float GlobalCoherence;

    UPROPERTY(BlueprintReadWrite, Category = "Quantum State")
    int32 TotalUncertainAreas;

    UPROPERTY(BlueprintReadWrite, Category = "Quantum State")
    int32 ObservedAreas;

    UPROPERTY(BlueprintReadWrite, Category = "Quantum State")
    float EntanglementStrength;

    UPROPERTY(BlueprintReadWrite, Category = "Quantum State")
    TArray<int32> EntangledAreas;

    UPROPERTY(BlueprintReadWrite, Category = "Quantum State")
    float DecoherenceRate;
};

/**
 * Quantum Observation System - Manages uncertain areas and wave function collapse
 * Implements the "Observation & Selection" gameplay mechanic
 */
UCLASS(BlueprintType, Blueprintable)
class GEMINI_C_API UQuantumObservationSystem : public UObject
{
    GENERATED_BODY()

public:
    UQuantumObservationSystem();

    // Uncertain Area Management
    UFUNCTION(BlueprintCallable, Category = "Quantum Observation")
    void CreateUncertainArea(const FVector& Center, float Radius, const TArray<FTerrainPossibility>& Possibilities);

    UFUNCTION(BlueprintCallable, Category = "Quantum Observation")
    void UpdateSuperposition(int32 AreaID, float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Quantum Observation")
    void RenderSuperposition(int32 AreaID);

    // Observation & Collapse
    UFUNCTION(BlueprintCallable, Category = "Quantum Observation")
    void InitiateObservation(int32 AreaID, const FVector& ObserverLocation);

    UFUNCTION(BlueprintCallable, Category = "Quantum Observation")
    void CollapseWaveFunction(int32 AreaID, ECollapsePattern Pattern);

    UFUNCTION(BlueprintCallable, Category = "Quantum Observation")
    void PropagateCollapse(int32 SourceAreaID);

    // Chain Reaction System
    UFUNCTION(BlueprintCallable, Category = "Quantum Observation")
    void CalculateChainReaction(int32 SourceAreaID, TArray<int32>& AffectedAreas);

    UFUNCTION(BlueprintCallable, Category = "Quantum Observation")
    void ExecuteChainReaction(const TArray<int32>& AffectedAreas);

    // Visual Effects
    UFUNCTION(BlueprintCallable, Category = "Quantum Observation")
    void CreateObservationEffects(const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "Quantum Observation")
    void CreateCollapseEffects(int32 AreaID, ECollapsePattern Pattern);

    // State Access
    UFUNCTION(BlueprintCallable, Category = "Quantum Observation", BlueprintPure)
    const FQuantumState& GetGlobalQuantumState() const { return GlobalQuantumState; }

    UFUNCTION(BlueprintCallable, Category = "Quantum Observation")
    bool GetUncertainArea(int32 AreaID, FUncertainArea& OutArea);

    UFUNCTION(BlueprintCallable, Category = "Quantum Observation", BlueprintPure)
    TArray<FUncertainArea> GetAllUncertainAreas() const { return UncertainAreas; }

    // Update Functions
    UFUNCTION(BlueprintCallable, Category = "Quantum Observation")
    void UpdateQuantumSystem(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Quantum Observation")
    void RecalculateGlobalCoherence();

protected:
    // Uncertain Areas Storage
    UPROPERTY(BlueprintReadOnly, Category = "Quantum Observation")
    TArray<FUncertainArea> UncertainAreas;

    // Effect Systems
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quantum Effects")
    UNiagaraSystem* SuperpositionEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quantum Effects")
    UNiagaraSystem* ObservationEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quantum Effects")
    UNiagaraSystem* CollapseEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quantum Effects")
    UMaterialParameterCollection* QuantumMaterialParams;

    // Global State
    UPROPERTY(BlueprintReadOnly, Category = "Quantum State")
    FQuantumState GlobalQuantumState;

    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObservationStarted, int32, AreaID, const FVector&, ObserverLocation);
    UPROPERTY(BlueprintAssignable, Category = "Quantum Observation")
    FOnObservationStarted OnObservationStarted;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveFunctionCollapsed, int32, AreaID, ECollapsePattern, Pattern);
    UPROPERTY(BlueprintAssignable, Category = "Quantum Observation")
    FOnWaveFunctionCollapsed OnWaveFunctionCollapsed;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChainReactionTriggered, const TArray<int32>&, AffectedAreas);
    UPROPERTY(BlueprintAssignable, Category = "Quantum Observation")
    FOnChainReactionTriggered OnChainReactionTriggered;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGlobalCoherenceChanged, float, NewCoherence);
    UPROPERTY(BlueprintAssignable, Category = "Quantum Observation")
    FOnGlobalCoherenceChanged OnGlobalCoherenceChanged;

private:
    // Internal Methods
    void UpdateQuantumCoherence(float DeltaTime);
    void CalculateProbabilityAmplitudes(int32 AreaID);
    void RenderQuantumFog(int32 AreaID);
    void UpdateMaterialParameters();
    void OptimizeQuantumEffects();
    
    // Probability Calculations
    float CalculateObservationProbability(const FVector& ObserverLocation, const FUncertainArea& Area) const;
    ECollapsePattern SelectCollapsePattern(const FUncertainArea& Area) const;
    void NormalizeProbabilities(FUncertainArea& Area);
    
    // Entanglement System
    void CreateQuantumEntanglement(int32 AreaID1, int32 AreaID2);
    void UpdateEntangledAreas(int32 CollapsedAreaID);
    
    // Effect Management
    TMap<int32, class UNiagaraComponent*> ActiveSuperpositionEffects;
    TArray<class UNiagaraComponent*> ActiveObservationEffects;
    
    // ID Management
    int32 NextAreaID;
    
    // Constants
    static constexpr float CHAIN_REACTION_RANGE = 2000.0f;
    static constexpr float MIN_OBSERVATION_DISTANCE = 100.0f;
    static constexpr float MAX_OBSERVATION_DISTANCE = 1000.0f;
    static constexpr float ENTANGLEMENT_THRESHOLD = 0.7f;
};
