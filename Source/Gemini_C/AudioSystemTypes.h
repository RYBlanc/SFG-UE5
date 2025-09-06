// Project Visible - Audio System Types
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "StorySystemTypes.h"
#include "LevelDesignTypes.h"
#include "CharacterSystemTypes.h"
#include "GameProgressionTypes.h"
#include "AudioSystemTypes.generated.h"

/**
 * Audio layer types for the dual-world system
 */
UENUM(BlueprintType)
enum class EAudioLayer : uint8
{
    Reality             UMETA(DisplayName = "Reality Layer"),           // 現実世界の音響
    Dream               UMETA(DisplayName = "Dream Layer"),             // 夢世界の音響
    Boundary            UMETA(DisplayName = "Boundary Layer"),          // 境界崩壊時の音響
    UI                  UMETA(DisplayName = "UI Layer"),                // UIの音響
    Ambient             UMETA(DisplayName = "Ambient Layer"),           // 環境音層
    Music               UMETA(DisplayName = "Music Layer"),             // 音楽層
    Voice               UMETA(DisplayName = "Voice Layer"),             // 音声層
    Effects             UMETA(DisplayName = "Effects Layer")            // 効果音層
};

/**
 * Adaptive audio mood types
 */
UENUM(BlueprintType)
enum class EAudioMood : uint8
{
    Calm                UMETA(DisplayName = "Calm"),                    // 穏やか
    Tense               UMETA(DisplayName = "Tense"),                   // 緊張
    Mysterious          UMETA(DisplayName = "Mysterious"),              // 神秘的
    Melancholic         UMETA(DisplayName = "Melancholic"),            // 憂鬱
    Hopeful             UMETA(DisplayName = "Hopeful"),                 // 希望的
    Chaotic             UMETA(DisplayName = "Chaotic"),                 // 混沌
    Contemplative       UMETA(DisplayName = "Contemplative"),           // 瞑想的
    Energetic           UMETA(DisplayName = "Energetic"),               // エネルギッシュ
    Ethereal            UMETA(DisplayName = "Ethereal"),                // 幻想的
    Disturbing          UMETA(DisplayName = "Disturbing")               // 不安
};

/**
 * Audio trigger event types
 */
UENUM(BlueprintType)
enum class EAudioTriggerType : uint8
{
    StoryProgress       UMETA(DisplayName = "Story Progress"),          // ストーリー進行
    CharacterInteraction UMETA(DisplayName = "Character Interaction"), // キャラクター対話
    LevelTransition     UMETA(DisplayName = "Level Transition"),        // レベル遷移
    EvidenceDiscovery   UMETA(DisplayName = "Evidence Discovery"),      // 証拠発見
    VirtueChange        UMETA(DisplayName = "Virtue Change"),           // 徳の変化
    MemoryFormation     UMETA(DisplayName = "Memory Formation"),        // 記憶形成
    AchievementUnlock   UMETA(DisplayName = "Achievement Unlock"),      // 実績解除
    ProgressUpdate      UMETA(DisplayName = "Progress Update"),         // 進行度更新
    BoundaryDissolution UMETA(DisplayName = "Boundary Dissolution"),    // 境界崩壊
    PhaseTransition     UMETA(DisplayName = "Phase Transition"),        // フェーズ遷移
    PlayerChoice        UMETA(DisplayName = "Player Choice"),           // プレイヤー選択
    EmotionalShift      UMETA(DisplayName = "Emotional Shift")          // 感情変化
};

/**
 * Music track types
 */
UENUM(BlueprintType)
enum class EMusicTrackType : uint8
{
    MainTheme           UMETA(DisplayName = "Main Theme"),              // メインテーマ
    RealityAmbient      UMETA(DisplayName = "Reality Ambient"),         // 現実環境音楽
    DreamAmbient        UMETA(DisplayName = "Dream Ambient"),           // 夢環境音楽
    Investigation       UMETA(DisplayName = "Investigation"),           // 調査音楽
    Dialogue            UMETA(DisplayName = "Dialogue"),                // 対話音楽
    Contemplation       UMETA(DisplayName = "Contemplation"),           // 瞑想音楽
    Tension             UMETA(DisplayName = "Tension"),                 // 緊張音楽
    Discovery           UMETA(DisplayName = "Discovery"),               // 発見音楽
    Transition          UMETA(DisplayName = "Transition"),              // 遷移音楽
    Credits             UMETA(DisplayName = "Credits"),                 // クレジット音楽
    Victory             UMETA(DisplayName = "Victory"),                 // 勝利音楽
    Defeat              UMETA(DisplayName = "Defeat")                   // 敗北音楽
};

/**
 * Spatial audio zones
 */
UENUM(BlueprintType)
enum class ESpatialAudioZone : uint8
{
    City                UMETA(DisplayName = "City"),                    // 都市部
    Underground         UMETA(DisplayName = "Underground"),             // 地下
    Laboratory          UMETA(DisplayName = "Laboratory"),              // 研究所
    Garden              UMETA(DisplayName = "Garden"),                  // 庭園
    MindLandscape       UMETA(DisplayName = "Mind Landscape"),          // 精神地形
    TransitionSpace     UMETA(DisplayName = "Transition Space"),        // 遷移空間
    VirtualSpace        UMETA(DisplayName = "Virtual Space"),           // 仮想空間
    InteriorSpace       UMETA(DisplayName = "Interior Space")           // 室内空間
};

/**
 * Sound effect categories
 */
UENUM(BlueprintType)
enum class ESoundEffectCategory : uint8
{
    UI                  UMETA(DisplayName = "UI"),                      // UI音
    Footsteps           UMETA(DisplayName = "Footsteps"),               // 足音
    Interaction         UMETA(DisplayName = "Interaction"),             // 操作音
    Mechanical          UMETA(DisplayName = "Mechanical"),              // 機械音
    Environmental       UMETA(DisplayName = "Environmental"),           // 環境音
    Magic               UMETA(DisplayName = "Magic"),                   // 魔法音
    Electronic          UMETA(DisplayName = "Electronic"),              // 電子音
    Organic             UMETA(DisplayName = "Organic"),                 // 有機音
    Atmospheric         UMETA(DisplayName = "Atmospheric"),             // 大気音
    Impact              UMETA(DisplayName = "Impact"),                  // 衝撃音
    Movement            UMETA(DisplayName = "Movement"),                // 移動音
    Communication       UMETA(DisplayName = "Communication")            // 通信音
};

/**
 * Audio configuration for specific contexts
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FAudioConfiguration
{
    GENERATED_BODY()

    FAudioConfiguration()
    {
        ConfigurationID = TEXT("");
        ConfigurationName = TEXT("");
        MasterVolume = 1.0f;
        MusicVolume = 0.8f;
        SFXVolume = 1.0f;
        VoiceVolume = 1.0f;
        AmbientVolume = 0.7f;
        ReverbStrength = 0.5f;
        LowPassCutoff = 20000.0f;
        HighPassCutoff = 20.0f;
        bEnableSpatialAudio = true;
        bEnableAdaptiveMusic = true;
        bEnablePsychoacoustics = true;
        PrimaryMood = EAudioMood::Calm;
        CurrentZone = ESpatialAudioZone::City;
        FadeInDuration = 2.0f;
        FadeOutDuration = 1.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    FString ConfigurationID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    FString ConfigurationName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    FString Description;

    // Volume Controls
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Volume", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MasterVolume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Volume", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MusicVolume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Volume", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SFXVolume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Volume", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float VoiceVolume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Volume", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float AmbientVolume;

    // Audio Processing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Processing", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float ReverbStrength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Processing", meta = (ClampMin = "20.0", ClampMax = "20000.0"))
    float LowPassCutoff;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Processing", meta = (ClampMin = "20.0", ClampMax = "20000.0"))
    float HighPassCutoff;

    // Feature Flags
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Features")
    bool bEnableSpatialAudio;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Features")
    bool bEnableAdaptiveMusic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Features")
    bool bEnablePsychoacoustics;

    // Contextual Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    EAudioMood PrimaryMood;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    ESpatialAudioZone CurrentZone;

    // Transition Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transitions")
    float FadeInDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transitions")
    float FadeOutDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    TMap<FString, float> CustomParameters;
};

/**
 * Music track definition
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FMusicTrack
{
    GENERATED_BODY()

    FMusicTrack()
    {
        TrackID = TEXT("");
        TrackName = TEXT("");
        TrackType = EMusicTrackType::MainTheme;
        AudioAsset = nullptr;
        Loop = true;
        Volume = 1.0f;
        Pitch = 1.0f;
        Duration = 0.0f;
        BPM = 120.0f;
        PrimaryMood = EAudioMood::Calm;
        IntensityLevel = 5;
        bIsDynamic = false;
        bIsInteractive = false;
        FadeInTime = 2.0f;
        FadeOutTime = 1.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Info")
    FString TrackID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Info")
    FString TrackName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Info")
    FString Composer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Info")
    EMusicTrackType TrackType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* AudioAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    bool Loop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Volume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (ClampMin = "0.1", ClampMax = "3.0"))
    float Pitch;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Musical")
    float BPM;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Musical")
    FString Key;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Musical")
    EAudioMood PrimaryMood;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Musical", meta = (ClampMin = "1", ClampMax = "10"))
    int32 IntensityLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    bool bIsDynamic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    bool bIsInteractive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transitions")
    float FadeInTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transitions")
    float FadeOutTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conditions")
    TArray<EStoryPhase> ValidStoryPhases;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conditions")
    TArray<EStoryLocation> ValidLocations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conditions")
    TArray<EAudioMood> ValidMoods;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layers")
    TArray<EAudioLayer> AudioLayers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metadata")
    TMap<FString, FString> Metadata;
};

/**
 * Sound effect definition
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FSoundEffect
{
    GENERATED_BODY()

    FSoundEffect()
    {
        EffectID = TEXT("");
        EffectName = TEXT("");
        Category = ESoundEffectCategory::UI;
        AudioAsset = nullptr;
        Volume = 1.0f;
        Pitch = 1.0f;
        bIs3D = false;
        AttenuationRadius = 1000.0f;
        Priority = 50;
        bCanOverlap = true;
        CooldownTime = 0.0f;
        TriggerType = EAudioTriggerType::PlayerChoice;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Info")
    FString EffectID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Info")
    FString EffectName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Info")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Info")
    ESoundEffectCategory Category;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* AudioAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Volume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (ClampMin = "0.1", ClampMax = "3.0"))
    float Pitch;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spatial")
    bool bIs3D;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spatial")
    float AttenuationRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Playback", meta = (ClampMin = "0", ClampMax = "100"))
    int32 Priority;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Playback")
    bool bCanOverlap;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Playback")
    float CooldownTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triggers")
    EAudioTriggerType TriggerType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conditions")
    TArray<FString> TriggerConditions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Randomization")
    TArray<USoundBase*> VariationAssets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metadata")
    TMap<FString, FString> EffectMetadata;
};

/**
 * Adaptive audio state
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FAdaptiveAudioState
{
    GENERATED_BODY()

    FAdaptiveAudioState()
    {
        CurrentMood = EAudioMood::Calm;
        TargetMood = EAudioMood::Calm;
        IntensityLevel = 5.0f;
        StressLevel = 0.0f;
        EmotionalValence = 0.0f;
        EmotionalArousal = 0.0f;
        VirtueBalance = 0.5f;
        MemoryLoad = 0.0f;
        AttentionFocus = 0.5f;
        ImmersionLevel = 0.5f;
        ProgressMomentum = 0.0f;
        LastMoodChange = FDateTime::Now();
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mood")
    EAudioMood CurrentMood;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mood")
    EAudioMood TargetMood;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Intensity")
    float IntensityLevel; // 1-10

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychological")
    float StressLevel; // 0-1

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychological")
    float EmotionalValence; // -1 to 1 (negative to positive)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Psychological")
    float EmotionalArousal; // 0-1 (calm to excited)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game State")
    float VirtueBalance; // 0-1

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game State")
    float MemoryLoad; // 0-1

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
    float AttentionFocus; // 0-1

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
    float ImmersionLevel; // 0-1

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State")
    float ProgressMomentum; // -1 to 1

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
    FDateTime LastMoodChange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    TMap<FString, float> ContextualFactors;
};

/**
 * Audio analytics data
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FAudioAnalyticsData
{
    GENERATED_BODY()

    FAudioAnalyticsData()
    {
        SessionID = TEXT("");
        PlayTime = 0.0f;
        TracksPlayed = 0;
        EffectsTriggered = 0;
        MoodTransitions = 0;
        VolumeAdjustments = 0;
        AudioDisabled = 0;
        PreferredMood = EAudioMood::Calm;
        PreferredIntensity = 5.0f;
        AverageSessionVolume = 0.8f;
        MostPlayedTrackType = EMusicTrackType::MainTheme;
        AudioEngagementScore = 0.5f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    FString SessionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    FDateTime SessionStart;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
    float PlayTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usage")
    int32 TracksPlayed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usage")
    int32 EffectsTriggered;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usage")
    int32 MoodTransitions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usage")
    int32 VolumeAdjustments;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usage")
    int32 AudioDisabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preferences")
    EAudioMood PreferredMood;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preferences")
    float PreferredIntensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preferences")
    float AverageSessionVolume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preferences")
    EMusicTrackType MostPlayedTrackType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engagement")
    float AudioEngagementScore; // 0-1

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detailed")
    TMap<FString, int32> TrackPlayCounts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detailed")
    TMap<FString, int32> EffectUsageCounts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detailed")
    TArray<FString> MoodChangeEvents;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detailed")
    TMap<FString, float> VolumeChangeHistory;
};
