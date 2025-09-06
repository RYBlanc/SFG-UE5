// Project Visible - Boundary Dissolution Manager
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/World.h"
#include "Materials/MaterialParameterCollection.h"
#include "Components/PostProcessComponent.h"
#include "Sound/SoundClass.h"
#include "BoundaryDissolutionTypes.h"
#include "BoundaryDissolutionManager.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class UAudioComponent;
class UMaterialParameterCollectionInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDissolutionPhaseChanged, EDissolutionPhase, OldPhase, EDissolutionPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDissolutionIntensityChanged, float, OldIntensity, float, NewIntensity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDissolutionZoneActivated, int32, ZoneID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDissolutionZoneDeactivated, int32, ZoneID);

/**
 * Central manager for boundary dissolution between reality and dream worlds
 * Handles visual effects, audio transitions, and reality blending
 */
UCLASS()
class GEMINI_C_API UBoundaryDissolutionManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    void TickSubsystem(float DeltaTime);
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

    // Core Dissolution Control
    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void StartDissolution(EBoundaryDissolutionType Type, float NewTargetIntensity = 1.0f, float Duration = 10.0f);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void StopDissolution(float FadeOutTime = 3.0f);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void SetDissolutionIntensity(float NewIntensity, bool bSmooth = true);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void AdvancePhase(EDissolutionPhase NewPhase);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void TriggerManualDissolution(const FVector& Location, float Radius = 1000.0f);

    // Zone Management
    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    int32 CreateDissolutionZone(const FString& ZoneName, const FVector& Center, float Radius, 
                               const FBoundaryDissolutionConfig& Config);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    bool ActivateDissolutionZone(int32 ZoneID);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    bool DeactivateDissolutionZone(int32 ZoneID);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void UpdateZoneConfig(int32 ZoneID, const FBoundaryDissolutionConfig& NewConfig);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution", BlueprintPure)
    bool GetZoneData(int32 ZoneID, FDissolutionZone& OutZone) const;

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution", BlueprintPure)
    TArray<FDissolutionZone> GetActiveZones() const;

    // Visual Effects
    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void AddVisualEffect(EBoundaryEffectType EffectType, const FBoundaryVisualEffect& EffectData);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void RemoveVisualEffect(EBoundaryEffectType EffectType);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void UpdateVisualEffect(EBoundaryEffectType EffectType, const FBoundaryVisualEffect& NewData);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void SetGlobalEffectIntensity(float Intensity);

    // Audio Effects
    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void AddAudioEffect(EBoundaryAudioType AudioType, const FBoundaryAudioEffect& AudioData);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void RemoveAudioEffect(EBoundaryAudioType AudioType);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void UpdateAudioEffect(EBoundaryAudioType AudioType, const FBoundaryAudioEffect& NewData);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void SetGlobalAudioIntensity(float Intensity);

    // Reality Blending
    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void SetRealityBlend(const FRealityLayerBlend& BlendData);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void BlendToReality(float BlendTime = 2.0f);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void BlendToDream(float BlendTime = 2.0f);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void BlendToMixed(float RealityWeight, float DreamWeight, float BlendTime = 2.0f);

    // Trigger System
    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void SetTriggerCondition(EDissolutionTrigger TriggerType, float Threshold);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void CheckTriggerConditions();

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    bool EvaluateTrigger(EDissolutionTrigger TriggerType, float CurrentValue);

    // Player Interaction
    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void OnPlayerEnterZone(int32 ZoneID, const FVector& PlayerLocation);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    void OnPlayerExitZone(int32 ZoneID, const FVector& PlayerLocation);

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution")
    float CalculatePlayerInfluence(const FVector& PlayerLocation);

    // State Access
    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution", BlueprintPure)
    EDissolutionPhase GetCurrentPhase() const { return CurrentPhase; }

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution", BlueprintPure)
    float GetCurrentIntensity() const { return CurrentIntensity; }

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution", BlueprintPure)
    bool IsActive() const { return bIsActive; }

    UFUNCTION(BlueprintCallable, Category = "Boundary Dissolution", BlueprintPure)
    FRealityLayerBlend GetCurrentBlend() const { return CurrentBlend; }

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Boundary Dissolution Events")
    FOnDissolutionPhaseChanged OnPhaseChanged;

    UPROPERTY(BlueprintAssignable, Category = "Boundary Dissolution Events")
    FOnDissolutionIntensityChanged OnIntensityChanged;

    UPROPERTY(BlueprintAssignable, Category = "Boundary Dissolution Events")
    FOnDissolutionZoneActivated OnZoneActivated;

    UPROPERTY(BlueprintAssignable, Category = "Boundary Dissolution Events")
    FOnDissolutionZoneDeactivated OnZoneDeactivated;

protected:
    // Core State
    UPROPERTY(BlueprintReadOnly, Category = "Dissolution State")
    EDissolutionPhase CurrentPhase;

    UPROPERTY(BlueprintReadOnly, Category = "Dissolution State")
    EBoundaryDissolutionType CurrentType;

    UPROPERTY(BlueprintReadOnly, Category = "Dissolution State")
    float CurrentIntensity;

    UPROPERTY(BlueprintReadOnly, Category = "Dissolution State")
    float TargetIntensity;

    UPROPERTY(BlueprintReadOnly, Category = "Dissolution State")
    bool bIsActive;

    UPROPERTY(BlueprintReadOnly, Category = "Dissolution State")
    bool bIsTransitioning;

    // Zones
    UPROPERTY(BlueprintReadOnly, Category = "Dissolution Zones")
    TArray<FDissolutionZone> DissolutionZones;

    UPROPERTY(BlueprintReadOnly, Category = "Dissolution Zones")
    int32 NextZoneID;

    // Reality Blending
    UPROPERTY(BlueprintReadOnly, Category = "Reality Blend")
    FRealityLayerBlend CurrentBlend;

    UPROPERTY(BlueprintReadOnly, Category = "Reality Blend")
    FRealityLayerBlend TargetBlend;

    // Effects
    UPROPERTY(BlueprintReadOnly, Category = "Effects")
    TMap<EBoundaryEffectType, FBoundaryVisualEffect> ActiveVisualEffects;

    UPROPERTY(BlueprintReadOnly, Category = "Effects")
    TMap<EBoundaryAudioType, FBoundaryAudioEffect> ActiveAudioEffects;

    // Components and Assets
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Assets")
    UMaterialParameterCollection* DissolutionParameters;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Assets")
    TMap<EBoundaryEffectType, UNiagaraSystem*> EffectSystems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Assets")
    TMap<EBoundaryAudioType, USoundBase*> AudioAssets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Assets")
    USoundClass* DissolutionSoundClass;

    // Runtime Components
    UPROPERTY()
    TMap<EBoundaryEffectType, UNiagaraComponent*> ActiveEffectComponents;

    UPROPERTY()
    TMap<EBoundaryAudioType, UAudioComponent*> ActiveAudioComponents;

    UPROPERTY()
    UMaterialParameterCollectionInstance* ParameterInstance;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Settings")
    float IntensityBlendSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Settings")
    float PhaseTransitionTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Settings")
    float EffectUpdateFrequency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Settings")
    bool bUsePostProcessing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Settings")
    bool bUseAudioMixing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Settings")
    int32 MaxActiveZones;

private:
    // Internal State
    float LastEffectUpdate;
    float IntensityBlendTimer;
    float PhaseTransitionTimer;
    
    // Trigger State
    TMap<EDissolutionTrigger, float> TriggerThresholds;
    TMap<EDissolutionTrigger, float> LastTriggerValues;

    // Helper Functions
    void UpdateIntensityBlending(float DeltaTime);
    void UpdatePhaseTransition(float DeltaTime);
    void UpdateVisualEffects(float DeltaTime);
    void UpdateAudioEffects(float DeltaTime);
    void UpdateMaterialParameters();
    void UpdatePostProcessing();
    
    FDissolutionZone* GetZoneByID(int32 ZoneID);
    void ProcessZoneBlending();
    float CalculateZoneInfluence(const FDissolutionZone& Zone, const FVector& Location);
    void CleanupInactiveEffects();
    
    // Effect Management
    void SpawnVisualEffect(EBoundaryEffectType EffectType, const FBoundaryVisualEffect& EffectData);
    void DestroyVisualEffect(EBoundaryEffectType EffectType);
    void SpawnAudioEffect(EBoundaryAudioType AudioType, const FBoundaryAudioEffect& AudioData);
    void DestroyAudioEffect(EBoundaryAudioType AudioType);
    
    // Blend Calculations
    FRealityLayerBlend CalculateBlendedState();
    void ApplyRealityBlend(const FRealityLayerBlend& Blend);
    void InterpolateBlend(const FRealityLayerBlend& From, const FRealityLayerBlend& To, float Alpha);
};
