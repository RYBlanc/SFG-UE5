// Project Visible - Landscape of Mind Manager
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "LandscapeOfMindTypes.h"
#include "LandscapeOfMindManager.generated.h"

// Forward declarations will be replaced with actual implementations later

/**
 * Central manager for the Landscape of Mind system
 * Handles dynamic terrain, quantum observation, and creature AI
 */
UCLASS(BlueprintType, Blueprintable)
class GEMINI_C_API ULandscapeOfMindManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Core Landscape Management
    UFUNCTION(BlueprintCallable, Category = "Landscape of Mind")
    void InitializeMentalLandscape(const FMentalLandscapeConfig& Config);

    UFUNCTION(BlueprintCallable, Category = "Landscape of Mind")
    void UpdateLandscapeStability(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Landscape of Mind", BlueprintPure)
    float GetOverallStabilityScore() const;

    // Node Stone Operations
    UFUNCTION(BlueprintCallable, Category = "Landscape of Mind")
    void PlaceNodeStone(const FVector& Location, ENodeStoneType Type);

    UFUNCTION(BlueprintCallable, Category = "Landscape of Mind")
    void MoveNodeStone(int32 NodeID, const FVector& NewLocation);

    UFUNCTION(BlueprintCallable, Category = "Landscape of Mind")
    void RotateNodeStone(int32 NodeID, const FRotator& NewRotation);

    UFUNCTION(BlueprintCallable, Category = "Landscape of Mind")
    void RemoveNodeStone(int32 NodeID);

    // Synapse River System
    UFUNCTION(BlueprintCallable, Category = "Landscape of Mind")
    void CreateSynapseRiver(int32 StartNodeID, int32 EndNodeID);

    UFUNCTION(BlueprintCallable, Category = "Landscape of Mind")
    void UpdateRiverFlow(int32 RiverID, float FlowRate);

    UFUNCTION(BlueprintCallable, Category = "Landscape of Mind")
    void RemoveSynapseRiver(int32 RiverID);

    // Quantum Observation System
    UFUNCTION(BlueprintCallable, Category = "Landscape of Mind")
    void ObserveUncertainArea(int32 AreaID);

    UFUNCTION(BlueprintCallable, Category = "Landscape of Mind")
    void TriggerQuantumCollapse(int32 AreaID, ECollapsePattern Pattern);

    // Noise Creature Management
    UFUNCTION(BlueprintCallable, Category = "Landscape of Mind")
    void SpawnNoiseCreatures(const FVector& Location, int32 Count);

    UFUNCTION(BlueprintCallable, Category = "Landscape of Mind")
    void UpdateCreatureAI(float DeltaTime);

    // State Access
    UFUNCTION(BlueprintCallable, Category = "Landscape of Mind", BlueprintPure)
    const FMentalLandscapeState& GetCurrentState() const { return CurrentState; }

    UFUNCTION(BlueprintCallable, Category = "Landscape of Mind", BlueprintPure)
    AActor* GetCurrentLandscape() const { return CurrentLandscape; }

    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStabilityChanged, float, OldStability, float, NewStability);
    UPROPERTY(BlueprintAssignable, Category = "Landscape of Mind")
    FOnStabilityChanged OnStabilityChanged;

protected:
    // Landscape Components
    UPROPERTY(BlueprintReadOnly, Category = "Landscape of Mind")
    AActor* CurrentLandscape;

    UPROPERTY(BlueprintReadOnly, Category = "Landscape of Mind")
    TArray<AActor*> NodeStones;

    UPROPERTY(BlueprintReadOnly, Category = "Landscape of Mind")
    TArray<AActor*> SynapseRivers;

    UPROPERTY(BlueprintReadOnly, Category = "Landscape of Mind")
    TArray<FUncertainArea> UncertainAreas;

    UPROPERTY(BlueprintReadOnly, Category = "Landscape of Mind")
    TArray<AActor*> NoiseCreatures;

    // Core Systems - These will be implemented as separate classes later
    UPROPERTY(BlueprintReadOnly, Category = "Landscape of Mind")
    UObject* TerrainEngine;

    UPROPERTY(BlueprintReadOnly, Category = "Landscape of Mind")
    UObject* ObservationSystem;

    UPROPERTY(BlueprintReadOnly, Category = "Landscape of Mind")
    UObject* CreatureAI;

    // Configuration and State
    UPROPERTY(BlueprintReadOnly, Category = "Landscape of Mind")
    FMentalLandscapeConfig LandscapeConfig;

    UPROPERTY(BlueprintReadOnly, Category = "Landscape of Mind")
    FMentalLandscapeState CurrentState;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuantumCollapse, int32, AreaID);
    UPROPERTY(BlueprintAssignable, Category = "Landscape of Mind")
    FOnQuantumCollapse OnQuantumCollapse;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNodeStoneCreated, int32, NodeID, const FVector&, Location);
    UPROPERTY(BlueprintAssignable, Category = "Landscape of Mind")
    FOnNodeStoneCreated OnNodeStoneCreated;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSynapseRiverCreated, int32, RiverID, int32, StartNodeID);
    UPROPERTY(BlueprintAssignable, Category = "Landscape of Mind")
    FOnSynapseRiverCreated OnSynapseRiverCreated;

private:
    // Internal Management
    void CalculateTerrainStability();
    void UpdateEnergyFlow();
    void ProcessQuantumEffects();
    void ManageCreatureSpawning();
    void OptimizeLandscapePerformance();

    // ID Generators
    int32 NextNodeID = 1;
    int32 NextRiverID = 1;
    int32 NextAreaID = 1;
    int32 NextCreatureID = 1;

    // Timer Handles
    FTimerHandle StabilityUpdateTimerHandle;
    FTimerHandle CreatureSpawnTimerHandle;
    FTimerHandle PerformanceOptimizationTimerHandle;
};
