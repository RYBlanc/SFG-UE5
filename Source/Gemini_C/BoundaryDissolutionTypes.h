// Project Visible - Boundary Dissolution System Types
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BoundaryDissolutionTypes.generated.h"

/**
 * Types of boundary dissolution effects
 */
UENUM(BlueprintType)
enum class EBoundaryDissolutionType : uint8
{
    None            UMETA(DisplayName = "No Dissolution"),
    Subtle          UMETA(DisplayName = "Subtle Blending"),
    Moderate        UMETA(DisplayName = "Moderate Fusion"),
    Strong          UMETA(DisplayName = "Strong Overlay"),
    Complete        UMETA(DisplayName = "Complete Integration")
};

/**
 * Phases of boundary dissolution
 */
UENUM(BlueprintType)
enum class EDissolutionPhase : uint8
{
    Stable          UMETA(DisplayName = "Stable Boundary"),
    Weakening       UMETA(DisplayName = "Boundary Weakening"),
    Fluctuation     UMETA(DisplayName = "Reality Fluctuation"),
    Blending        UMETA(DisplayName = "Active Blending"),
    Fusion          UMETA(DisplayName = "Reality Fusion"),
    Collapse        UMETA(DisplayName = "Boundary Collapse")
};

/**
 * Visual effect types for boundary dissolution
 */
UENUM(BlueprintType)
enum class EBoundaryEffectType : uint8
{
    RealityRipple   UMETA(DisplayName = "Reality Ripple"),
    DreamOverlay    UMETA(DisplayName = "Dream Overlay"),
    QuantumShimmer  UMETA(DisplayName = "Quantum Shimmer"),
    MemoryBleed     UMETA(DisplayName = "Memory Bleed"),
    TimeDistortion  UMETA(DisplayName = "Time Distortion"),
    SpaceWarp       UMETA(DisplayName = "Space Warp"),
    ConsciousnessFog UMETA(DisplayName = "Consciousness Fog")
};

/**
 * Audio effect types for boundary dissolution
 */
UENUM(BlueprintType)
enum class EBoundaryAudioType : uint8
{
    RealityHum      UMETA(DisplayName = "Reality Hum"),
    DreamEcho       UMETA(DisplayName = "Dream Echo"),
    WhisperVoices   UMETA(DisplayName = "Whisper Voices"),
    QuantumStatic   UMETA(DisplayName = "Quantum Static"),
    MemoryChime     UMETA(DisplayName = "Memory Chime"),
    HeartbeatSync   UMETA(DisplayName = "Heartbeat Sync"),
    AmbienceMorph   UMETA(DisplayName = "Ambience Morph")
};

/**
 * Dissolution trigger conditions
 */
UENUM(BlueprintType)
enum class EDissolutionTrigger : uint8
{
    Manual          UMETA(DisplayName = "Manual Trigger"),
    StoryProgress   UMETA(DisplayName = "Story Progress"),
    EmotionalState  UMETA(DisplayName = "Emotional State"),
    MemoryAccess    UMETA(DisplayName = "Memory Access"),
    StabilityLevel  UMETA(DisplayName = "Stability Level"),
    PlayerProximity UMETA(DisplayName = "Player Proximity"),
    TimeOfDay       UMETA(DisplayName = "Time of Day")
};

/**
 * Boundary dissolution configuration
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FBoundaryDissolutionConfig
{
    GENERATED_BODY()

    FBoundaryDissolutionConfig()
    {
        DissolutionType = EBoundaryDissolutionType::None;
        Phase = EDissolutionPhase::Stable;
        Intensity = 0.0f;
        Duration = 10.0f;
        FadeInTime = 2.0f;
        FadeOutTime = 3.0f;
        bIsActive = false;
        bAutoProgress = true;
        TriggerCondition = EDissolutionTrigger::Manual;
        TriggerThreshold = 50.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Config")
    EBoundaryDissolutionType DissolutionType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Config")
    EDissolutionPhase Phase;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Config", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Intensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Config")
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Config")
    float FadeInTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Config")
    float FadeOutTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Config")
    bool bIsActive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Config")
    bool bAutoProgress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Config")
    EDissolutionTrigger TriggerCondition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Config")
    float TriggerThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Config")
    TArray<EBoundaryEffectType> ActiveEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Config")
    TArray<EBoundaryAudioType> ActiveAudioEffects;
};

/**
 * Visual effect data structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FBoundaryVisualEffect
{
    GENERATED_BODY()

    FBoundaryVisualEffect()
    {
        EffectType = EBoundaryEffectType::RealityRipple;
        Intensity = 1.0f;
        Scale = FVector(1.0f, 1.0f, 1.0f);
        Color = FLinearColor::White;
        Speed = 1.0f;
        Frequency = 1.0f;
        bIsLooping = true;
        bIsVisible = true;
        BlendMode = 0; // Alpha blend
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    EBoundaryEffectType EffectType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect", meta = (ClampMin = "0.0", ClampMax = "2.0"))
    float Intensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    FVector Scale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    float Speed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    float Frequency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    bool bIsLooping;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    bool bIsVisible;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    int32 BlendMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    TMap<FString, float> EffectParameters;
};

/**
 * Audio effect data structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FBoundaryAudioEffect
{
    GENERATED_BODY()

    FBoundaryAudioEffect()
    {
        AudioType = EBoundaryAudioType::RealityHum;
        Volume = 0.5f;
        Pitch = 1.0f;
        FadeInTime = 1.0f;
        FadeOutTime = 1.0f;
        bIsLooping = true;
        bIsPlaying = false;
        Spatialization = 0.0f; // 2D by default
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Effect")
    EBoundaryAudioType AudioType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Effect", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Volume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Effect", meta = (ClampMin = "0.1", ClampMax = "3.0"))
    float Pitch;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Effect")
    float FadeInTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Effect")
    float FadeOutTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Effect")
    bool bIsLooping;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Effect")
    bool bIsPlaying;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Effect", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Spatialization;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Effect")
    TMap<FString, float> AudioParameters;
};

/**
 * Dissolution zone structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FDissolutionZone
{
    GENERATED_BODY()

    FDissolutionZone()
    {
        ZoneID = 0;
        Center = FVector::ZeroVector;
        Radius = 1000.0f;
        Height = 500.0f;
        bIsActive = false;
        Priority = 1;
        BlendWeight = 1.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Zone")
    int32 ZoneID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Zone")
    FString ZoneName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Zone")
    FVector Center;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Zone")
    float Radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Zone")
    float Height;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Zone")
    bool bIsActive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Zone")
    int32 Priority;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Zone")
    float BlendWeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Zone")
    FBoundaryDissolutionConfig DissolutionConfig;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Zone")
    TArray<FBoundaryVisualEffect> VisualEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dissolution Zone")
    TArray<FBoundaryAudioEffect> AudioEffects;
};

/**
 * Reality layer blend data
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FRealityLayerBlend
{
    GENERATED_BODY()

    FRealityLayerBlend()
    {
        RealityWeight = 1.0f;
        DreamWeight = 0.0f;
        BlendMode = 0; // Linear blend
        bUseAdvancedBlending = false;
        ColorGrading = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
        Saturation = 1.0f;
        Contrast = 1.0f;
        Brightness = 0.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality Blend", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float RealityWeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality Blend", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float DreamWeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality Blend")
    int32 BlendMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality Blend")
    bool bUseAdvancedBlending;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality Blend")
    FLinearColor ColorGrading;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality Blend")
    float Saturation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality Blend")
    float Contrast;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality Blend")
    float Brightness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reality Blend")
    TMap<FString, float> CustomParameters;
};
