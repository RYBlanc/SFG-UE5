#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "AudioSystemTypes.generated.h"

/**
 * Audio channel types for Project Visible
 */
UENUM(BlueprintType)
enum class EAudioChannelType : uint8
{
    Master      UMETA(DisplayName = "Master"),      // マスター音量
    Music       UMETA(DisplayName = "Music"),       // 音楽
    SFX         UMETA(DisplayName = "SFX"),         // 効果音
    Voice       UMETA(DisplayName = "Voice"),       // ボイス
    Ambient     UMETA(DisplayName = "Ambient"),     // 環境音
    UI          UMETA(DisplayName = "UI"),          // UI音
    
    MAX_CHANNELS UMETA(Hidden)
};

/**
 * Project Visible audio settings data structure
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FProjectVisibleAudioSettings
{
    GENERATED_BODY()

    // Master volume (0.0 - 1.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MasterVolume = 0.8f;

    // Music volume (0.0 - 1.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MusicVolume = 0.7f;

    // SFX volume (0.0 - 1.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SFXVolume = 0.9f;

    // Voice volume (0.0 - 1.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float VoiceVolume = 1.0f;

    // Ambient volume (0.0 - 1.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float AmbientVolume = 0.6f;

    // UI volume (0.0 - 1.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float UIVolume = 0.8f;

    // Whether audio is muted
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    bool bIsMuted = false;

    // Audio quality level (0-3: Low, Medium, High, Ultra)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings", meta = (ClampMin = "0", ClampMax = "3"))
    int32 AudioQuality = 2;

    // Whether to use 3D audio
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    bool bUse3DAudio = true;

    // Whether to use audio compression
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    bool bUseAudioCompression = false;

    FProjectVisibleAudioSettings()
    {
        MasterVolume = 0.8f;
        MusicVolume = 0.7f;
        SFXVolume = 0.9f;
        VoiceVolume = 1.0f;
        AmbientVolume = 0.6f;
        UIVolume = 0.8f;
        bIsMuted = false;
        AudioQuality = 2;
        bUse3DAudio = true;
        bUseAudioCompression = false;
    }
};

/**
 * Audio event data for triggering sounds
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FAudioEvent
{
    GENERATED_BODY()

    // Unique identifier for this audio event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Event")
    FString EventID;

    // Display name of the event
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Event")
    FText EventName;

    // Audio channel type
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Event")
    EAudioChannelType ChannelType = EAudioChannelType::SFX;

    // Sound to play
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Event")
    class USoundBase* Sound = nullptr;

    // Volume multiplier for this event (0.0 - 1.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Event", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float VolumeMultiplier = 1.0f;

    // Pitch multiplier for this event (0.1 - 2.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Event", meta = (ClampMin = "0.1", ClampMax = "2.0"))
    float PitchMultiplier = 1.0f;

    // Whether this event can be interrupted
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Event")
    bool bCanBeInterrupted = true;

    // Whether this event should loop
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Event")
    bool bShouldLoop = false;

    // Maximum number of concurrent instances
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Event", meta = (ClampMin = "1", ClampMax = "10"))
    int32 MaxConcurrentInstances = 1;

    FAudioEvent()
    {
        EventID = TEXT("");
        EventName = FText::FromString(TEXT("Audio Event"));
        ChannelType = EAudioChannelType::SFX;
        Sound = nullptr;
        VolumeMultiplier = 1.0f;
        PitchMultiplier = 1.0f;
        bCanBeInterrupted = true;
        bShouldLoop = false;
        MaxConcurrentInstances = 1;
    }
};

/**
 * Audio statistics for monitoring and debugging
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FAudioStatistics
{
    GENERATED_BODY()

    // Number of currently playing sounds
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Statistics")
    int32 CurrentlyPlayingSounds = 0;

    // Number of sounds played this session
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Statistics")
    int32 TotalSoundsPlayed = 0;

    // Current audio memory usage (in MB)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Statistics")
    float AudioMemoryUsageMB = 0.0f;

    // Peak audio memory usage this session (in MB)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Statistics")
    float PeakAudioMemoryUsageMB = 0.0f;

    // Number of audio events registered
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Statistics")
    int32 RegisteredAudioEvents = 0;

    FAudioStatistics()
    {
        CurrentlyPlayingSounds = 0;
        TotalSoundsPlayed = 0;
        AudioMemoryUsageMB = 0.0f;
        PeakAudioMemoryUsageMB = 0.0f;
        RegisteredAudioEvents = 0;
    }
};

/**
 * Data table row for audio event configuration
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FAudioEventConfigRow : public FTableRowBase
{
    GENERATED_BODY()

    // Audio event data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
    FAudioEvent AudioEvent;

    // Tags for categorization
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
    TArray<FString> Tags;

    // Whether this event is enabled
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
    bool bIsEnabled = true;

    FAudioEventConfigRow()
    {
        AudioEvent = FAudioEvent();
        Tags.Empty();
        bIsEnabled = true;
    }
};