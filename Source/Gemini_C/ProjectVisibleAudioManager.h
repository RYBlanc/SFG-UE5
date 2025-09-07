#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AudioSystemTypes.h"
#include "Engine/DataTable.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "Components/AudioComponent.h"
#include "ProjectVisibleAudioManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVolumeChanged, EAudioChannelType, ChannelType, float, NewVolume);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAudioEventPlayed, const FString&, EventID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMuteStateChanged, bool, bIsMuted);

/**
 * Project Visible Audio Manager - Game Instance Subsystem
 * Manages audio settings, sound playback, and volume control
 */
UCLASS(BlueprintType)
class GEMINI_C_API UProjectVisibleAudioManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // === Audio Settings Management ===

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void InitializeAudioSystem();

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    FProjectVisibleAudioSettings GetCurrentAudioSettings() const { return CurrentAudioSettings; }

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void ApplyAudioSettings(const FProjectVisibleAudioSettings& NewSettings);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void LoadAudioSettings();

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void SaveAudioSettings();

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void ResetAudioSettings();

    // === Volume Control ===

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    float GetChannelVolume(EAudioChannelType ChannelType) const;

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void SetChannelVolume(EAudioChannelType ChannelType, float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void AdjustChannelVolume(EAudioChannelType ChannelType, float DeltaVolume);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    float GetMasterVolume() const { return CurrentAudioSettings.MasterVolume; }

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void SetMasterVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    bool IsMuted() const { return CurrentAudioSettings.bIsMuted; }

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void SetMuted(bool bMuted);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void ToggleMute();

    // === Audio Event System ===

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void RegisterAudioEvent(const FAudioEvent& AudioEvent);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    bool PlayAudioEvent(const FString& EventID, AActor* SourceActor = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void StopAudioEvent(const FString& EventID);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void StopAllAudioEvents();

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    bool IsAudioEventPlaying(const FString& EventID) const;

    // === Quick Audio Functions ===

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void PlayUISound(const FString& SoundName);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void PlaySFXSound(const FString& SoundName, float VolumeMultiplier = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void PlayMusicTrack(const FString& TrackName, bool bFadeIn = true);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void StopCurrentMusic(bool bFadeOut = true);

    // === Audio Statistics ===

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    FAudioStatistics GetAudioStatistics() const;

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    int32 GetCurrentlyPlayingSoundsCount() const;

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    float GetAudioMemoryUsage() const;

    // === Utility Functions ===

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    FText GetChannelDisplayName(EAudioChannelType ChannelType) const;

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    TArray<EAudioChannelType> GetAllChannelTypes() const;

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void SetAudioQuality(int32 QualityLevel);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    int32 GetAudioQuality() const { return CurrentAudioSettings.AudioQuality; }

    // === Events ===

    UPROPERTY(BlueprintAssignable, Category = "Audio System")
    FOnVolumeChanged OnVolumeChanged;

    UPROPERTY(BlueprintAssignable, Category = "Audio System")
    FOnAudioEventPlayed OnAudioEventPlayed;

    UPROPERTY(BlueprintAssignable, Category = "Audio System")
    FOnMuteStateChanged OnMuteStateChanged;

    // === Test Functions ===

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void TestPlayButtonSound();

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void TestPlayNotificationSound();

protected:
    // Current audio settings
    UPROPERTY(BlueprintReadOnly, Category = "Audio System")
    FProjectVisibleAudioSettings CurrentAudioSettings;

    // Registered audio events
    UPROPERTY(BlueprintReadOnly, Category = "Audio System")
    TMap<FString, FAudioEvent> AudioEvents;

    // Currently playing audio components
    UPROPERTY()
    TMap<FString, UAudioComponent*> PlayingAudioComponents;

    // Sound mix for volume control
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    USoundMix* MainSoundMix = nullptr;

    // Sound classes for different audio channels
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    USoundClass* MasterSoundClass = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    USoundClass* MusicSoundClass = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    USoundClass* SFXSoundClass = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    USoundClass* VoiceSoundClass = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    USoundClass* AmbientSoundClass = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    USoundClass* UISoundClass = nullptr;

    // Configuration data table
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    UDataTable* AudioEventDataTable = nullptr;

private:
    // === Internal Helper Functions ===
    void ApplyVolumeToSoundClass(EAudioChannelType ChannelType, float Volume);
    void UpdateAudioStatistics();
    void LoadDefaultAudioEvents();
    USoundClass* GetSoundClassForChannel(EAudioChannelType ChannelType) const;
    float CalculateEffectiveVolume(EAudioChannelType ChannelType, float EventVolume) const;

    // === Default Audio Data ===
    struct FDefaultAudioEvent
    {
        FString ID;
        FString Name;
        EAudioChannelType ChannelType;
        FString SoundAssetPath;
        float VolumeMultiplier;
        bool bShouldLoop;
    };

    static const TArray<FDefaultAudioEvent>& GetDefaultAudioEvents();

    // Internal state
    bool bIsInitialized = false;
    FAudioStatistics CachedStatistics;
    int32 TotalSoundsPlayedThisSession = 0;
};
