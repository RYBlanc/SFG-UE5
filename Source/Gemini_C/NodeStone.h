// Project Visible - Node Stone Actor
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "LandscapeOfMindTypes.h"
#include "NodeStone.generated.h"

/**
 * Node Stone - Interactive objects that manipulate the mental landscape
 * Core mechanic for the "Annealing & Zen Gardening" gameplay
 */
UCLASS(BlueprintType, Blueprintable)
class GEMINI_C_API ANodeStone : public AActor
{
    GENERATED_BODY()

public:
    ANodeStone();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Node Stone")
    UStaticMeshComponent* StoneMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Node Stone")
    USphereComponent* InfluenceSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Node Stone")
    UNiagaraComponent* EffectComponent;

    // Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Stone")
    FNodeStoneProperties Properties;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Stone")
    int32 NodeID;

    UPROPERTY(BlueprintReadOnly, Category = "Node Stone")
    UMaterialInstanceDynamic* DynamicMaterial;

public:
    // Node Stone Operations
    UFUNCTION(BlueprintCallable, Category = "Node Stone")
    void SetNodeStoneProperties(const FNodeStoneProperties& NewProperties);

    UFUNCTION(BlueprintCallable, Category = "Node Stone", BlueprintPure)
    const FNodeStoneProperties& GetNodeStoneProperties() const { return Properties; }

    UFUNCTION(BlueprintCallable, Category = "Node Stone")
    void SetNodeID(int32 NewNodeID) { NodeID = NewNodeID; }

    UFUNCTION(BlueprintCallable, Category = "Node Stone", BlueprintPure)
    int32 GetNodeID() const { return NodeID; }

    // Terrain Influence
    UFUNCTION(BlueprintCallable, Category = "Node Stone")
    void ApplyTerrainInfluence();

    UFUNCTION(BlueprintCallable, Category = "Node Stone")
    void UpdateInfluenceRadius();

    UFUNCTION(BlueprintCallable, Category = "Node Stone")
    void SetActiveState(bool bNewActive);

    UFUNCTION(BlueprintCallable, Category = "Node Stone", BlueprintPure)
    bool IsWithinInfluenceRange(const FVector& Location) const;

    // Visual Updates
    UFUNCTION(BlueprintCallable, Category = "Node Stone")
    void UpdateVisualEffects();

    UFUNCTION(BlueprintCallable, Category = "Node Stone")
    void UpdateGlowColor(const FLinearColor& NewColor);

    // Type-specific behaviors
    UFUNCTION(BlueprintCallable, Category = "Node Stone")
    void ExecuteStabilizerBehavior();

    UFUNCTION(BlueprintCallable, Category = "Node Stone")
    void ExecuteAmplifierBehavior();

    UFUNCTION(BlueprintCallable, Category = "Node Stone")
    void ExecuteRedirectorBehavior();

    UFUNCTION(BlueprintCallable, Category = "Node Stone")
    void ExecuteHarmonizerBehavior();

    UFUNCTION(BlueprintCallable, Category = "Node Stone")
    void ExecuteCatalystBehavior();

    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNodeStoneActivated, ANodeStone*, NodeStone, bool, bIsActive);
    UPROPERTY(BlueprintAssignable, Category = "Node Stone")
    FOnNodeStoneActivated OnNodeStoneActivated;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTerrainInfluenceApplied, ANodeStone*, NodeStone);
    UPROPERTY(BlueprintAssignable, Category = "Node Stone")
    FOnTerrainInfluenceApplied OnTerrainInfluenceApplied;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPropertiesChanged, ANodeStone*, NodeStone, const FNodeStoneProperties&, NewProperties);
    UPROPERTY(BlueprintAssignable, Category = "Node Stone")
    FOnPropertiesChanged OnPropertiesChanged;

protected:
    // Internal Methods
    void InitializeComponents();
    void SetupMaterial();
    void SetupEffects();
    void UpdateMaterialParameters();
    void CalculateTerrainEffect(const FVector& TargetLocation, float& OutHeightChange, float& OutStabilityChange) const;

private:
    // Internal state
    bool bIsInitialized;
    float LastInfluenceUpdateTime;
    TArray<FVector> AffectedTerrainPoints;

    // Constants
    static constexpr float INFLUENCE_UPDATE_INTERVAL = 0.1f;
    static constexpr float MAX_TERRAIN_HEIGHT_CHANGE = 500.0f;
    static constexpr float BASE_STABILIZATION_RANGE = 1000.0f;
};


