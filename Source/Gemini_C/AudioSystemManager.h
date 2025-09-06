// Project Visible - Audio System Manager
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/World.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundMix.h"
#include "AudioSystemTypes.h"
#include "AudioSystemManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMusicTrackChanged, const FMusicTrack&, NewTrack, const FMusicTrack&, OldTrack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAudioMoodChanged, EAudioMood, OldMood, EAudioMood, NewMood);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVolumeChanged, EAudioLayer, Layer, float, NewVolume);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoundEffectTriggered, const FSoundEffect&, Effect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAudioConfigurationChanged, const FAudioConfiguration&, NewConfig);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpatialZoneChanged, ESpatialAudioZone, OldZone, ESpatialAudioZone, NewZone);

/**
 * Central manager for Project Visible's adaptive audio system
 * Manages dynamic music, psychoacoustic effects, spatial audio, and mood-based audio adaptation
 */
UCLASS()
class GEMINI_C_API UAudioSystemManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // System Initialization
    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void InitializeAudioSystem();

    // Music System
    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void RegisterMusicTrack(const FMusicTrack& Track);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    bool PlayMusicTrack(const FString& TrackID, bool bFadeIn = true);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void StopCurrentMusic(bool bFadeOut = true);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void PauseMusic();

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void ResumeMusic();

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    FMusicTrack GetCurrentMusicTrack() const { return CurrentMusicTrack; }

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    bool IsMusicPlaying() const;

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void SetMusicVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    float GetMusicVolume() const;

    // Adaptive Music System
    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void UpdateAdaptiveMusic();

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    FString SelectBestMusicTrack(EAudioMood TargetMood, EMusicTrackType PreferredType = EMusicTrackType::MainTheme);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void CrossfadeTo(const FString& NewTrackID, float CrossfadeDuration = 3.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void SetAdaptiveMusicEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    bool IsAdaptiveMusicEnabled() const { return bAdaptiveMusicEnabled; }

    // Sound Effects System
    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void RegisterSoundEffect(const FSoundEffect& Effect);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    bool PlaySoundEffect(const FString& EffectID, FVector Location = FVector::ZeroVector, AActor* AttachToActor = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void StopSoundEffect(const FString& EffectID);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void StopAllSoundEffects();

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    TArray<FSoundEffect> GetSoundEffectsByCategory(ESoundEffectCategory Category) const;

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void TriggerSoundEffectByType(EAudioTriggerType TriggerType, const FString& Context = TEXT(""));

    // Mood and Atmosphere
    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void SetAudioMood(EAudioMood NewMood, bool bImmediate = false);

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    EAudioMood GetCurrentAudioMood() const { return AdaptiveState.CurrentMood; }

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void UpdateAdaptiveAudioState(const FAdaptiveAudioState& NewState);

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    FAdaptiveAudioState GetAdaptiveAudioState() const { return AdaptiveState; }

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void SetIntensityLevel(float NewIntensity);

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    float GetIntensityLevel() const { return AdaptiveState.IntensityLevel; }

    // Spatial Audio
    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void SetSpatialAudioZone(ESpatialAudioZone NewZone);

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    ESpatialAudioZone GetCurrentSpatialZone() const { return CurrentSpatialZone; }

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void UpdatePlayerAudioPosition(FVector NewPosition, FRotator NewRotation);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void SetSpatialAudioEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    bool IsSpatialAudioEnabled() const { return bSpatialAudioEnabled; }

    // Volume Controls
    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void SetLayerVolume(EAudioLayer Layer, float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    float GetLayerVolume(EAudioLayer Layer) const;

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void SetMasterVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    float GetMasterVolume() const;

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void MuteLayer(EAudioLayer Layer, bool bMute);

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    bool IsLayerMuted(EAudioLayer Layer) const;

    // Configuration Management
    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void ApplyAudioConfiguration(const FAudioConfiguration& Config);

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    FAudioConfiguration GetCurrentConfiguration() const { return CurrentConfiguration; }

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void SaveCurrentConfiguration(const FString& ConfigName);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    bool LoadConfiguration(const FString& ConfigName);

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    TArray<FString> GetAvailableConfigurations() const;

    // Psychoacoustic Effects
    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void SetPsychoacousticEffectsEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    bool IsPsychoacousticEffectsEnabled() const { return bPsychoacousticEffectsEnabled; }

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void ApplyReverbEffect(float ReverbStrength, float DecayTime = 2.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void ApplyLowPassFilter(float CutoffFrequency);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void ApplyHighPassFilter(float CutoffFrequency);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void ApplyBinauralBeats(float Frequency1, float Frequency2);

    // Game Integration
    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void OnStoryPhaseChanged(EStoryPhase NewPhase);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void OnLocationChanged(EStoryLocation NewLocation);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void OnCharacterInteraction(const FString& CharacterID, ENPCEmotionalState CharacterMood);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void OnVirtueChanged(const FString& VirtueType, float NewLevel);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void OnMemoryEvent(const FString& MemoryType, float Importance);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void OnProgressUpdate(float OverallProgress);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void OnAchievementUnlocked(const FString& AchievementID);

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void OnBoundaryDissolution(float DissolutionLevel);

    // Analytics
    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void RecordAudioEvent(const FString& EventType, const FString& Context);

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    FAudioAnalyticsData GetAudioAnalytics() const { return AnalyticsData; }

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void ResetAudioAnalytics();

    // System State
    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void PauseAllAudio();

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void ResumeAllAudio();

    UFUNCTION(BlueprintCallable, Category = "Audio System")
    void StopAllAudio();

    UFUNCTION(BlueprintCallable, Category = "Audio System", BlueprintPure)
    bool IsAudioSystemActive() const { return bAudioSystemActive; }

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Audio Events")
    FOnMusicTrackChanged OnMusicTrackChanged;

    UPROPERTY(BlueprintAssignable, Category = "Audio Events")
    FOnAudioMoodChanged OnAudioMoodChanged;

    UPROPERTY(BlueprintAssignable, Category = "Audio Events")
    FOnVolumeChanged OnVolumeChanged;

    UPROPERTY(BlueprintAssignable, Category = "Audio Events")
    FOnSoundEffectTriggered OnSoundEffectTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Audio Events")
    FOnAudioConfigurationChanged OnAudioConfigurationChanged;

    UPROPERTY(BlueprintAssignable, Category = "Audio Events")
    FOnSpatialZoneChanged OnSpatialZoneChanged;

protected:
    // Core Audio Data
    UPROPERTY(BlueprintReadOnly, Category = "Audio Data")
    TMap<FString, FMusicTrack> RegisteredTracks;

    UPROPERTY(BlueprintReadOnly, Category = "Audio Data")
    TMap<FString, FSoundEffect> RegisteredEffects;

    UPROPERTY(BlueprintReadOnly, Category = "Audio Data")
    FMusicTrack CurrentMusicTrack;

    UPROPERTY(BlueprintReadOnly, Category = "Audio Data")
    FAdaptiveAudioState AdaptiveState;

    UPROPERTY(BlueprintReadOnly, Category = "Audio Data")
    FAudioConfiguration CurrentConfiguration;

    UPROPERTY(BlueprintReadOnly, Category = "Audio Data")
    FAudioAnalyticsData AnalyticsData;

    // Audio Components
    UPROPERTY(BlueprintReadOnly, Category = "Audio Components")
    UAudioComponent* MusicAudioComponent;

    UPROPERTY(BlueprintReadOnly, Category = "Audio Components")
    TMap<FString, UAudioComponent*> ActiveSoundEffects;

    UPROPERTY(BlueprintReadOnly, Category = "Audio Components")
    TMap<EAudioLayer, UAudioComponent*> LayerComponents;

    // System State
    UPROPERTY(BlueprintReadOnly, Category = "Audio State")
    ESpatialAudioZone CurrentSpatialZone;

    UPROPERTY(BlueprintReadOnly, Category = "Audio State")
    bool bAudioSystemActive;

    UPROPERTY(BlueprintReadOnly, Category = "Audio State")
    bool bAdaptiveMusicEnabled;

    UPROPERTY(BlueprintReadOnly, Category = "Audio State")
    bool bSpatialAudioEnabled;

    UPROPERTY(BlueprintReadOnly, Category = "Audio State")
    bool bPsychoacousticEffectsEnabled;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    float UpdateFrequency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    float MoodTransitionSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    int32 MaxConcurrentSounds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    bool bEnableAudioOcclusion;

    // Runtime State
    UPROPERTY()
    FTimerHandle AdaptiveUpdateTimerHandle;

    UPROPERTY()
    FDateTime LastAnalyticsUpdate;

    UPROPERTY()
    TMap<FString, FDateTime> SoundCooldowns;

    UPROPERTY()
    TMap<FString, FAudioConfiguration> SavedConfigurations;

private:
    // Helper Functions
    void InitializeDefaultTracks();
    void InitializeDefaultEffects();
    void InitializeDefaultConfiguration();
    void SetupAudioComponents();

    // Music System Helpers
    UAudioComponent* GetOrCreateMusicComponent();
    void UpdateMusicParameters();
    void ProcessMusicCrossfade();
    bool CanPlayTrack(const FMusicTrack& Track) const;
    float CalculateTrackScore(const FMusicTrack& Track, EAudioMood TargetMood) const;

    // Effect System Helpers
    UAudioComponent* CreateSoundEffectComponent(const FSoundEffect& Effect);
    void CleanupFinishedEffects();
    bool IsEffectOnCooldown(const FString& EffectID) const;
    void ApplySpatialAudioSettings(UAudioComponent* AudioComponent, const FSoundEffect& Effect);

    // Adaptive System Helpers
    void UpdateAdaptiveParameters();
    void CalculateTargetMood();
    void ProcessMoodTransition();
    void UpdateIntensityBasedOnGameState();
    void ApplyPsychoacousticEffects();

    // Integration Helpers
    void SynchronizeWithGameSystems();
    void UpdateVolumeBasedOnGameState();
    void ProcessSpatialAudioZone();

    // Analytics Helpers
    void UpdateAnalyticsData();
    void RecordTrackPlayback(const FString& TrackID);
    void RecordEffectUsage(const FString& EffectID);
    void RecordMoodChange(EAudioMood OldMood, EAudioMood NewMood);

    // System References
    class UStoryManager* GetStoryManager();
    class UCharacterManager* GetCharacterManager();
    class UVirtueManager* GetVirtueManager();
    class UMemoryManager* GetMemoryManager();
    class UGameProgressionManager* GetProgressionManager();
    class USocialExperimentManager* GetExperimentManager();
    class UBoundaryDissolutionManager* GetBoundaryManager();
};
