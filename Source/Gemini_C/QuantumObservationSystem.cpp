// Project Visible - Quantum Observation System Implementation
// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuantumObservationSystem.h"
#include "Gemini_C.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Kismet/KismetMathLibrary.h"

UQuantumObservationSystem::UQuantumObservationSystem()
{
    // Initialize quantum state
    GlobalQuantumState = FQuantumState();
    NextAreaID = 1;
    
    UE_LOG(LogProjectVisible, Log, TEXT("Quantum Observation System created"));
}

void UQuantumObservationSystem::CreateUncertainArea(const FVector& Center, float Radius, const TArray<FTerrainPossibility>& Possibilities)
{
    FUncertainArea NewArea;
    NewArea.AreaID = NextAreaID++;
    NewArea.Center = Center;
    NewArea.Radius = Radius;
    NewArea.Possibilities = Possibilities;
    NewArea.bIsObserved = false;
    NewArea.CollapsedPattern = ECollapsePattern::None;
    NewArea.CoherenceLevel = 1.0f;
    NewArea.CreationTime = FDateTime::Now();
    
    // Normalize probabilities
    NormalizeProbabilities(NewArea);
    
    UncertainAreas.Add(NewArea);
    GlobalQuantumState.TotalUncertainAreas++;
    
    // Create visual superposition effect
    RenderSuperposition(NewArea.AreaID);
    
    RecalculateGlobalCoherence();
    
    UE_LOG(LogProjectVisible, Log, TEXT("Created uncertain area %d at location %s with %d possibilities"), 
           NewArea.AreaID, *Center.ToString(), Possibilities.Num());
}

void UQuantumObservationSystem::UpdateSuperposition(int32 AreaID, float DeltaTime)
{
    FUncertainArea Area;
    if (!GetUncertainArea(AreaID, Area) || Area.bIsObserved)
    {
        return;
    }
    
    // Find the actual area in the array to modify it
    FUncertainArea* AreaPtr = UncertainAreas.FindByPredicate([AreaID](const FUncertainArea& A)
    {
        return A.AreaID == AreaID;
    });
    
    if (AreaPtr)
    {
        // Update coherence level (gradual decoherence)
        AreaPtr->CoherenceLevel -= GlobalQuantumState.DecoherenceRate * DeltaTime;
        AreaPtr->CoherenceLevel = FMath::Max(0.0f, AreaPtr->CoherenceLevel);
        
        // Update probability amplitudes
        CalculateProbabilityAmplitudes(AreaID);
        
        // Update visual effects
        RenderQuantumFog(AreaID);
    }
    
    UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Updated superposition for area %d"), AreaID);
}

void UQuantumObservationSystem::RenderSuperposition(int32 AreaID)
{
    FUncertainArea Area;
    if (!GetUncertainArea(AreaID, Area))
    {
        return;
    }
    
    // Create superposition effect if not already active
    if (!ActiveSuperpositionEffects.Contains(AreaID) && SuperpositionEffect)
    {
        if (UWorld* World = GetWorld())
        {
            UNiagaraComponent* EffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                World, SuperpositionEffect, Area.Center, FRotator::ZeroRotator
            );
            
            if (EffectComponent)
            {
                // Set effect parameters
                EffectComponent->SetFloatParameter(TEXT("Radius"), Area.Radius);
                EffectComponent->SetFloatParameter(TEXT("CoherenceLevel"), Area.CoherenceLevel);
                EffectComponent->SetIntParameter(TEXT("PossibilityCount"), Area.Possibilities.Num());
                
                ActiveSuperpositionEffects.Add(AreaID, EffectComponent);
                
                UE_LOG(LogProjectVisible, Log, TEXT("Created superposition effect for area %d"), AreaID);
            }
        }
    }
}

void UQuantumObservationSystem::InitiateObservation(int32 AreaID, const FVector& ObserverLocation)
{
    FUncertainArea Area;
    if (!GetUncertainArea(AreaID, Area) || Area.bIsObserved)
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Cannot observe area %d - already observed or doesn't exist"), AreaID);
        return;
    }
    
    // Check observation distance
    float Distance = FVector::Dist(ObserverLocation, Area.Center);
    if (Distance < MIN_OBSERVATION_DISTANCE || Distance > MAX_OBSERVATION_DISTANCE)
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Observer too far from uncertain area %d (Distance: %f)"), AreaID, Distance);
        return;
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Initiating observation of area %d from location %s"), 
           AreaID, *ObserverLocation.ToString());
    
    // Create observation effects
    CreateObservationEffects(ObserverLocation);
    
    // Calculate observation probability based on distance and coherence
    float ObservationProbability = CalculateObservationProbability(ObserverLocation, Area);
    
    // Determine if observation succeeds
    if (FMath::RandRange(0.0f, 1.0f) <= ObservationProbability)
    {
        // Select collapse pattern based on probabilities
        ECollapsePattern SelectedPattern = SelectCollapsePattern(Area);
        
        // Collapse the wave function
        CollapseWaveFunction(AreaID, SelectedPattern);
        
        // Trigger chain reaction
        TArray<int32> AffectedAreas;
        CalculateChainReaction(AreaID, AffectedAreas);
        if (AffectedAreas.Num() > 0)
        {
            ExecuteChainReaction(AffectedAreas);
        }
    }
    else
    {
        UE_LOG(LogProjectVisible, Log, TEXT("Observation of area %d failed (Probability: %f)"), AreaID, ObservationProbability);
    }
    
    OnObservationStarted.Broadcast(AreaID, ObserverLocation);
}

void UQuantumObservationSystem::CollapseWaveFunction(int32 AreaID, ECollapsePattern Pattern)
{
    // Find and update the uncertain area
    FUncertainArea* AreaPtr = UncertainAreas.FindByPredicate([AreaID](const FUncertainArea& A)
    {
        return A.AreaID == AreaID;
    });
    
    if (!AreaPtr)
    {
        return;
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Collapsing wave function for area %d with pattern %d"), AreaID, (int32)Pattern);
    
    // Set area as observed
    AreaPtr->bIsObserved = true;
    AreaPtr->CollapsedPattern = Pattern;
    AreaPtr->CoherenceLevel = 0.0f; // Fully collapsed
    
    // Update global state
    GlobalQuantumState.ObservedAreas++;
    
    // Remove superposition effect
    if (UNiagaraComponent** EffectPtr = ActiveSuperpositionEffects.Find(AreaID))
    {
        if (*EffectPtr)
        {
            (*EffectPtr)->DestroyComponent();
        }
        ActiveSuperpositionEffects.Remove(AreaID);
    }
    
    // Create collapse effects
    CreateCollapseEffects(AreaID, Pattern);
    
    // Update entangled areas
    UpdateEntangledAreas(AreaID);
    
    // Recalculate global coherence
    RecalculateGlobalCoherence();
    
    OnWaveFunctionCollapsed.Broadcast(AreaID, Pattern);
}

void UQuantumObservationSystem::PropagateCollapse(int32 SourceAreaID)
{
    // TODO: Implement propagation logic
    UE_LOG(LogProjectVisible, Log, TEXT("Propagating collapse from area %d (placeholder)"), SourceAreaID);
}

void UQuantumObservationSystem::CalculateChainReaction(int32 SourceAreaID, TArray<int32>& AffectedAreas)
{
    AffectedAreas.Empty();
    UE_LOG(LogProjectVisible, Log, TEXT("Chain reaction calculation (placeholder) for area %d"), SourceAreaID);
}

void UQuantumObservationSystem::ExecuteChainReaction(const TArray<int32>& AffectedAreas)
{
    UE_LOG(LogProjectVisible, Log, TEXT("Executing chain reaction affecting %d areas (placeholder)"), AffectedAreas.Num());
    OnChainReactionTriggered.Broadcast(AffectedAreas);
}

void UQuantumObservationSystem::CreateObservationEffects(const FVector& Location)
{
    if (ObservationEffect)
    {
        if (UWorld* World = GetWorld())
        {
            UNiagaraComponent* EffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                World, ObservationEffect, Location, FRotator::ZeroRotator
            );
            
            if (EffectComponent)
            {
                ActiveObservationEffects.Add(EffectComponent);
                UE_LOG(LogProjectVisible, Log, TEXT("Created observation effect at location %s"), *Location.ToString());
            }
        }
    }
}

void UQuantumObservationSystem::CreateCollapseEffects(int32 AreaID, ECollapsePattern Pattern)
{
    FUncertainArea Area;
    if (!GetUncertainArea(AreaID, Area) || !CollapseEffect)
    {
        return;
    }
    
    if (UWorld* World = GetWorld())
    {
        UNiagaraComponent* EffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            World, CollapseEffect, Area.Center, FRotator::ZeroRotator
        );
        
        if (EffectComponent)
        {
            // Set effect parameters based on collapse pattern
            EffectComponent->SetIntParameter(TEXT("CollapsePattern"), (int32)Pattern);
            EffectComponent->SetFloatParameter(TEXT("Radius"), Area.Radius);
            
            // Set color based on pattern
            FLinearColor PatternColor = FLinearColor::White;
            switch (Pattern)
            {
                case ECollapsePattern::Valley: PatternColor = FLinearColor::Blue; break;
                case ECollapsePattern::Hill: PatternColor = FLinearColor::Green; break;
                case ECollapsePattern::Plateau: PatternColor = FLinearColor::Yellow; break;
                case ECollapsePattern::Canyon: PatternColor = FLinearColor::Red; break;
                case ECollapsePattern::Lake: PatternColor = FLinearColor(0.0f, 1.0f, 1.0f, 1.0f); break;
                case ECollapsePattern::Ridge: PatternColor = FLinearColor(1.0f, 0.5f, 0.0f, 1.0f); break;
                case ECollapsePattern::Crater: PatternColor = FLinearColor(0.5f, 0.0f, 0.5f, 1.0f); break;
            }
            
            EffectComponent->SetVectorParameter(TEXT("PatternColor"), FVector(PatternColor.R, PatternColor.G, PatternColor.B));
            
            UE_LOG(LogProjectVisible, Log, TEXT("Created collapse effect for area %d with pattern %d"), AreaID, (int32)Pattern);
        }
    }
}

bool UQuantumObservationSystem::GetUncertainArea(int32 AreaID, FUncertainArea& OutArea)
{
    FUncertainArea* FoundArea = UncertainAreas.FindByPredicate([AreaID](const FUncertainArea& Area)
    {
        return Area.AreaID == AreaID;
    });
    
    if (FoundArea)
    {
        OutArea = *FoundArea;
        return true;
    }
    return false;
}

void UQuantumObservationSystem::UpdateQuantumSystem(float DeltaTime)
{
    // Update all uncertain areas
    for (FUncertainArea& Area : UncertainAreas)
    {
        if (!Area.bIsObserved)
        {
            UpdateSuperposition(Area.AreaID, DeltaTime);
        }
    }
    
    // Update global quantum coherence
    UpdateQuantumCoherence(DeltaTime);
    
    // Update material parameters
    UpdateMaterialParameters();
    
    // Optimize effects
    OptimizeQuantumEffects();
}

void UQuantumObservationSystem::RecalculateGlobalCoherence()
{
    float TotalCoherence = 0.0f;
    int32 UnobservedAreas = 0;
    
    for (const FUncertainArea& Area : UncertainAreas)
    {
        if (!Area.bIsObserved)
        {
            TotalCoherence += Area.CoherenceLevel;
            UnobservedAreas++;
        }
    }
    
    float OldCoherence = GlobalQuantumState.GlobalCoherence;
    
    if (UnobservedAreas > 0)
    {
        GlobalQuantumState.GlobalCoherence = TotalCoherence / UnobservedAreas;
    }
    else
    {
        GlobalQuantumState.GlobalCoherence = 0.0f;
    }
    
    // Calculate entanglement strength based on proximity of uncertain areas
    float EntanglementCount = 0.0f;
    for (int32 i = 0; i < UncertainAreas.Num(); i++)
    {
        for (int32 j = i + 1; j < UncertainAreas.Num(); j++)
        {
            if (!UncertainAreas[i].bIsObserved && !UncertainAreas[j].bIsObserved)
            {
                float Distance = FVector::Dist(UncertainAreas[i].Center, UncertainAreas[j].Center);
                float CombinedRadius = UncertainAreas[i].Radius + UncertainAreas[j].Radius;
                
                if (Distance <= CombinedRadius * 2.0f)
                {
                    EntanglementCount += 1.0f - (Distance / (CombinedRadius * 2.0f));
                }
            }
        }
    }
    
    GlobalQuantumState.EntanglementStrength = FMath::Min(1.0f, EntanglementCount / FMath::Max(1.0f, (float)UnobservedAreas));
    
    if (FMath::Abs(GlobalQuantumState.GlobalCoherence - OldCoherence) > 0.01f)
    {
        OnGlobalCoherenceChanged.Broadcast(GlobalQuantumState.GlobalCoherence);
    }
    
    UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Global coherence: %f, Entanglement: %f, Unobserved areas: %d"), 
           GlobalQuantumState.GlobalCoherence, GlobalQuantumState.EntanglementStrength, UnobservedAreas);
}

// Private Methods

void UQuantumObservationSystem::UpdateQuantumCoherence(float DeltaTime)
{
    // Update decoherence rate based on external factors
    float BaseDecoherenceRate = 0.01f;
    
    // Increase decoherence rate if many areas are observed (measurement affects remaining quantum states)
    float ObservationRatio = (float)GlobalQuantumState.ObservedAreas / FMath::Max(1.0f, (float)GlobalQuantumState.TotalUncertainAreas);
    GlobalQuantumState.DecoherenceRate = BaseDecoherenceRate * (1.0f + ObservationRatio);
}

void UQuantumObservationSystem::CalculateProbabilityAmplitudes(int32 AreaID)
{
    // Placeholder for probability amplitude calculation
    UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Calculating probability amplitudes for area %d"), AreaID);
}

void UQuantumObservationSystem::RenderQuantumFog(int32 AreaID)
{
    // Update visual representation of quantum superposition (placeholder)
    UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Rendering quantum fog for area %d"), AreaID);
}

void UQuantumObservationSystem::UpdateMaterialParameters()
{
    if (QuantumMaterialParams)
    {
        // Update global material parameters for quantum effects
        if (UWorld* World = GetWorld())
        {
            if (UMaterialParameterCollectionInstance* Instance = World->GetParameterCollectionInstance(QuantumMaterialParams))
            {
                Instance->SetScalarParameterValue(TEXT("GlobalCoherence"), GlobalQuantumState.GlobalCoherence);
                Instance->SetScalarParameterValue(TEXT("EntanglementStrength"), GlobalQuantumState.EntanglementStrength);
                Instance->SetScalarParameterValue(TEXT("DecoherenceRate"), GlobalQuantumState.DecoherenceRate);
            }
        }
    }
}

void UQuantumObservationSystem::OptimizeQuantumEffects()
{
    // Clean up finished observation effects
    ActiveObservationEffects.RemoveAll([](UNiagaraComponent* Component)
    {
        return !Component || !Component->IsActive();
    });
    
    // Limit number of active effects for performance
    const int32 MaxActiveEffects = 10;
    while (ActiveObservationEffects.Num() > MaxActiveEffects)
    {
        if (UNiagaraComponent* OldestEffect = ActiveObservationEffects[0])
        {
            OldestEffect->DestroyComponent();
        }
        ActiveObservationEffects.RemoveAt(0);
    }
}

float UQuantumObservationSystem::CalculateObservationProbability(const FVector& ObserverLocation, const FUncertainArea& Area) const
{
    float Distance = FVector::Dist(ObserverLocation, Area.Center);
    
    // Base probability based on distance
    float DistanceFactor = 1.0f - FMath::Clamp((Distance - MIN_OBSERVATION_DISTANCE) / (MAX_OBSERVATION_DISTANCE - MIN_OBSERVATION_DISTANCE), 0.0f, 1.0f);
    
    // Modify by coherence level
    float CoherenceFactor = Area.CoherenceLevel;
    
    // Modify by global quantum state
    float GlobalFactor = GlobalQuantumState.GlobalCoherence;
    
    return DistanceFactor * CoherenceFactor * GlobalFactor;
}

ECollapsePattern UQuantumObservationSystem::SelectCollapsePattern(const FUncertainArea& Area) const
{
    // Weighted random selection based on terrain possibilities
    float TotalProbability = 0.0f;
    for (const FTerrainPossibility& Possibility : Area.Possibilities)
    {
        TotalProbability += Possibility.Probability;
    }
    
    if (TotalProbability <= 0.0f)
    {
        return static_cast<ECollapsePattern>(FMath::RandRange(1, 7));
    }
    
    float RandomValue = FMath::RandRange(0.0f, TotalProbability);
    float AccumulatedProbability = 0.0f;
    
    for (int32 i = 0; i < Area.Possibilities.Num(); i++)
    {
        AccumulatedProbability += Area.Possibilities[i].Probability;
        if (RandomValue <= AccumulatedProbability)
        {
            // Map possibility index to collapse pattern
            return static_cast<ECollapsePattern>((i % 7) + 1);
        }
    }
    
    return ECollapsePattern::Hill; // Default fallback
}

void UQuantumObservationSystem::NormalizeProbabilities(FUncertainArea& Area)
{
    float TotalProbability = 0.0f;
    for (const FTerrainPossibility& Possibility : Area.Possibilities)
    {
        TotalProbability += FMath::Max(0.0f, Possibility.Probability);
    }
    
    if (TotalProbability > 0.0f)
    {
        for (FTerrainPossibility& Possibility : Area.Possibilities)
        {
            Possibility.Probability = FMath::Max(0.0f, Possibility.Probability) / TotalProbability;
        }
    }
    else
    {
        // Equal probabilities if all are zero
        float EqualProbability = 1.0f / FMath::Max(1.0f, (float)Area.Possibilities.Num());
        for (FTerrainPossibility& Possibility : Area.Possibilities)
        {
            Possibility.Probability = EqualProbability;
        }
    }
}

void UQuantumObservationSystem::CreateQuantumEntanglement(int32 AreaID1, int32 AreaID2)
{
    // Add areas to entanglement list if not already present
    GlobalQuantumState.EntangledAreas.AddUnique(AreaID1);
    GlobalQuantumState.EntangledAreas.AddUnique(AreaID2);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Created quantum entanglement between areas %d and %d"), AreaID1, AreaID2);
}

void UQuantumObservationSystem::UpdateEntangledAreas(int32 CollapsedAreaID)
{
    // Placeholder for entanglement update
    UE_LOG(LogProjectVisible, Log, TEXT("Updating entangled areas for collapsed area %d"), CollapsedAreaID);
}
