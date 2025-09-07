#include "ProjectVisibleAudioManager.h"
#include "Gemini_C.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"

void UProjectVisibleAudioManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    UE_LOG(LogProjectVisible, Log, TEXT("ProjectVisibleAudioManager: Initializing audio system"));
    
    // Initialize default audio settings
    CurrentAudioSettings = FProjectVisibleAudioSettings();
    
    // Load saved settings
    LoadAudioSettings();
    
    // Load default audio events
    LoadDefaultAudioEvents();
    
    bIsInitialized = true;
    
    UE_LOG(LogProjectVisible, Log, TEXT("ProjectVisibleAudioManager: Audio system initialized successfully"));
}

void UProjectVisibleAudioManager::Deinitialize()
{
    UE_LOG(LogProjectVisible, Log, TEXT("ProjectVisibleAudioManager: Deinitializing audio system"));
    
    // Stop all playing sounds
    StopAllAudioEvents();
    
    // Save current settings
    SaveAudioSettings();
    
    bIsInitialized = false;
    AudioEvents.Empty();
    PlayingAudioComponents.Empty();
    
    Super::Deinitialize();
}

void UProjectVisibleAudioManager::InitializeAudioSystem()
{
    UE_LOG(LogProjectVisible, Warning, TEXT("ProjectVisibleAudioManager: Initializing audio system"));
    
    // Apply current audio settings
    ApplyAudioSettings(CurrentAudioSettings);
    
    // Load audio events
    LoadDefaultAudioEvents();
    
    // Update statistics
    UpdateAudioStatistics();
    
    UE_LOG(LogProjectVisible, Warning, TEXT("ProjectVisibleAudioManager: Audio system initialized with %d audio events"), 
        AudioEvents.Num());
    
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, 
            FString::Printf(TEXT("Audio System Initialized: %d events"), AudioEvents.Num()));
    }
}

void UProjectVisibleAudioManager::ApplyAudioSettings(const FProjectVisibleAudioSettings& NewSettings)
{
    CurrentAudioSettings = NewSettings;
    
    UE_LOG(LogProjectVisible, Log, TEXT("ProjectVisibleAudioManager: Applying audio settings"));
    
    // Apply volume settings to sound classes
    ApplyVolumeToSoundClass(EAudioChannelType::Master, CurrentAudioSettings.MasterVolume);
    ApplyVolumeToSoundClass(EAudioChannelType::Music, CurrentAudioSettings.MusicVolume);
    ApplyVolumeToSoundClass(EAudioChannelType::SFX, CurrentAudioSettings.SFXVolume);
    ApplyVolumeToSoundClass(EAudioChannelType::Voice, CurrentAudioSettings.VoiceVolume);
    ApplyVolumeToSoundClass(EAudioChannelType::Ambient, CurrentAudioSettings.AmbientVolume);
    ApplyVolumeToSoundClass(EAudioChannelType::UI, CurrentAudioSettings.UIVolume);
    
    // Apply mute state
    if (CurrentAudioSettings.bIsMuted)
    {
        UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
        // Note: In a real implementation, you would use Sound Mix or Sound Class volume modifications
    }
    
    // Broadcast volume change events
    OnVolumeChanged.Broadcast(EAudioChannelType::Master, CurrentAudioSettings.MasterVolume);
    OnMuteStateChanged.Broadcast(CurrentAudioSettings.bIsMuted);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Audio settings applied: Master=%.2f, Music=%.2f, SFX=%.2f, Muted=%s"),
        CurrentAudioSettings.MasterVolume, CurrentAudioSettings.MusicVolume, CurrentAudioSettings.SFXVolume,
        CurrentAudioSettings.bIsMuted ? TEXT("Yes") : TEXT("No"));
}

void UProjectVisibleAudioManager::LoadAudioSettings()
{
    UE_LOG(LogProjectVisible, Log, TEXT("ProjectVisibleAudioManager: Loading audio settings"));
    
    // In a real implementation, you would load from a save file or user settings
    // For now, we'll use default values
    CurrentAudioSettings = FProjectVisibleAudioSettings();
    
    UE_LOG(LogProjectVisible, Log, TEXT("Audio settings loaded (using defaults)"));
}

void UProjectVisibleAudioManager::SaveAudioSettings()
{
    UE_LOG(LogProjectVisible, Log, TEXT("ProjectVisibleAudioManager: Saving audio settings"));
    
    // In a real implementation, you would save to a file or user settings
    // For now, we'll just log that we're saving
    
    UE_LOG(LogProjectVisible, Log, TEXT("Audio settings saved"));
}

void UProjectVisibleAudioManager::ResetAudioSettings()
{
    UE_LOG(LogProjectVisible, Warning, TEXT("ProjectVisibleAudioManager: Resetting audio settings to defaults"));
    
    FProjectVisibleAudioSettings DefaultSettings;
    ApplyAudioSettings(DefaultSettings);
    
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, TEXT("Audio Settings Reset to Defaults"));
    }
}

float UProjectVisibleAudioManager::GetChannelVolume(EAudioChannelType ChannelType) const
{
    switch (ChannelType)
    {
        case EAudioChannelType::Master:   return CurrentAudioSettings.MasterVolume;
        case EAudioChannelType::Music:    return CurrentAudioSettings.MusicVolume;
        case EAudioChannelType::SFX:      return CurrentAudioSettings.SFXVolume;
        case EAudioChannelType::Voice:    return CurrentAudioSettings.VoiceVolume;
        case EAudioChannelType::Ambient:  return CurrentAudioSettings.AmbientVolume;
        case EAudioChannelType::UI:       return CurrentAudioSettings.UIVolume;
        default:                          return 1.0f;
    }
}

void UProjectVisibleAudioManager::SetChannelVolume(EAudioChannelType ChannelType, float Volume)
{
    Volume = FMath::Clamp(Volume, 0.0f, 1.0f);
    
    switch (ChannelType)
    {
        case EAudioChannelType::Master:   CurrentAudioSettings.MasterVolume = Volume; break;
        case EAudioChannelType::Music:    CurrentAudioSettings.MusicVolume = Volume; break;
        case EAudioChannelType::SFX:      CurrentAudioSettings.SFXVolume = Volume; break;
        case EAudioChannelType::Voice:    CurrentAudioSettings.VoiceVolume = Volume; break;
        case EAudioChannelType::Ambient:  CurrentAudioSettings.AmbientVolume = Volume; break;
        case EAudioChannelType::UI:       CurrentAudioSettings.UIVolume = Volume; break;
    }
    
    ApplyVolumeToSoundClass(ChannelType, Volume);
    OnVolumeChanged.Broadcast(ChannelType, Volume);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Channel %d volume set to %.2f"), (int32)ChannelType, Volume);
}

void UProjectVisibleAudioManager::AdjustChannelVolume(EAudioChannelType ChannelType, float DeltaVolume)
{
    float CurrentVolume = GetChannelVolume(ChannelType);
    float NewVolume = FMath::Clamp(CurrentVolume + DeltaVolume, 0.0f, 1.0f);
    SetChannelVolume(ChannelType, NewVolume);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Channel %d volume adjusted by %.2f (%.2f -> %.2f)"), 
        (int32)ChannelType, DeltaVolume, CurrentVolume, NewVolume);
}

void UProjectVisibleAudioManager::SetMasterVolume(float Volume)
{
    SetChannelVolume(EAudioChannelType::Master, Volume);
}

void UProjectVisibleAudioManager::SetMuted(bool bMuted)
{
    CurrentAudioSettings.bIsMuted = bMuted;
    
    // Apply mute to all channels
    if (bMuted)
    {
        // In a real implementation, you would use Sound Mix to set all volumes to 0
        UE_LOG(LogProjectVisible, Warning, TEXT("Audio muted"));
    }
    else
    {
        // Restore volumes
        ApplyAudioSettings(CurrentAudioSettings);
        UE_LOG(LogProjectVisible, Warning, TEXT("Audio unmuted"));
    }
    
    OnMuteStateChanged.Broadcast(bMuted);
    
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, bMuted ? FColor::Red : FColor::Green,
            FString::Printf(TEXT("Audio %s"), bMuted ? TEXT("MUTED") : TEXT("UNMUTED")));
    }
}

void UProjectVisibleAudioManager::ToggleMute()
{
    SetMuted(!CurrentAudioSettings.bIsMuted);
}

void UProjectVisibleAudioManager::RegisterAudioEvent(const FAudioEvent& AudioEvent)
{
    if (!AudioEvent.EventID.IsEmpty())
    {
        AudioEvents.Add(AudioEvent.EventID, AudioEvent);
        UpdateAudioStatistics();
        
        UE_LOG(LogProjectVisible, Log, TEXT("Registered audio event: %s"), *AudioEvent.EventID);
    }
}

bool UProjectVisibleAudioManager::PlayAudioEvent(const FString& EventID, AActor* SourceActor)
{
    if (const FAudioEvent* AudioEvent = AudioEvents.Find(EventID))
    {
        if (AudioEvent->Sound)
        {
            // Calculate effective volume
            float EffectiveVolume = CalculateEffectiveVolume(AudioEvent->ChannelType, AudioEvent->VolumeMultiplier);
            
            if (CurrentAudioSettings.bIsMuted)
            {
                EffectiveVolume = 0.0f;
            }
            
            // Play the sound
            UAudioComponent* AudioComponent = UGameplayStatics::SpawnSound2D(
                GetWorld(),
                AudioEvent->Sound,
                EffectiveVolume,
                AudioEvent->PitchMultiplier,
                0.0f, // Start time
                nullptr, // Concurrency settings
                false,  // Persistent
                false   // Auto destroy
            );
            
            if (AudioComponent)
            {
                // Store reference if it's a looping sound
                if (AudioEvent->bShouldLoop)
                {
                    PlayingAudioComponents.Add(EventID, AudioComponent);
                }
                
                TotalSoundsPlayedThisSession++;
                UpdateAudioStatistics();
                
                OnAudioEventPlayed.Broadcast(EventID);
                
                UE_LOG(LogProjectVisible, Log, TEXT("Played audio event: %s (Volume: %.2f)"), *EventID, EffectiveVolume);
                
                return true;
            }
        }
        else
        {
            UE_LOG(LogProjectVisible, Warning, TEXT("Audio event %s has no sound assigned"), *EventID);
        }
    }
    else
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Audio event %s not found"), *EventID);
    }
    
    return false;
}

void UProjectVisibleAudioManager::StopAudioEvent(const FString& EventID)
{
    if (UAudioComponent** AudioComponent = PlayingAudioComponents.Find(EventID))
    {
        if (*AudioComponent && IsValid(*AudioComponent))
        {
            (*AudioComponent)->Stop();
        }
        PlayingAudioComponents.Remove(EventID);
        
        UE_LOG(LogProjectVisible, Log, TEXT("Stopped audio event: %s"), *EventID);
    }
}

void UProjectVisibleAudioManager::StopAllAudioEvents()
{
    UE_LOG(LogProjectVisible, Warning, TEXT("Stopping all audio events"));
    
    for (auto& AudioComponentPair : PlayingAudioComponents)
    {
        if (AudioComponentPair.Value && IsValid(AudioComponentPair.Value))
        {
            AudioComponentPair.Value->Stop();
        }
    }
    
    PlayingAudioComponents.Empty();
    
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("All Audio Stopped"));
    }
}

bool UProjectVisibleAudioManager::IsAudioEventPlaying(const FString& EventID) const
{
    if (UAudioComponent* const* AudioComponent = PlayingAudioComponents.Find(EventID))
    {
        return *AudioComponent && IsValid(*AudioComponent) && (*AudioComponent)->IsPlaying();
    }
    return false;
}

void UProjectVisibleAudioManager::PlayUISound(const FString& SoundName)
{
    PlayAudioEvent(FString::Printf(TEXT("UI_%s"), *SoundName));
}

void UProjectVisibleAudioManager::PlaySFXSound(const FString& SoundName, float VolumeMultiplier)
{
    FString EventID = FString::Printf(TEXT("SFX_%s"), *SoundName);
    
    // Temporarily modify volume if event exists
    if (FAudioEvent* AudioEvent = AudioEvents.Find(EventID))
    {
        float OriginalVolume = AudioEvent->VolumeMultiplier;
        AudioEvent->VolumeMultiplier = VolumeMultiplier;
        PlayAudioEvent(EventID);
        AudioEvent->VolumeMultiplier = OriginalVolume;
    }
    else
    {
        PlayAudioEvent(EventID);
    }
}

void UProjectVisibleAudioManager::PlayMusicTrack(const FString& TrackName, bool bFadeIn)
{
    // Stop current music first
    StopCurrentMusic(false);
    
    FString EventID = FString::Printf(TEXT("Music_%s"), *TrackName);
    PlayAudioEvent(EventID);
    
    UE_LOG(LogProjectVisible, Warning, TEXT("Playing music track: %s"), *TrackName);
}

void UProjectVisibleAudioManager::StopCurrentMusic(bool bFadeOut)
{
    // Stop all music events
    TArray<FString> MusicEventsToStop;
    for (const auto& EventPair : AudioEvents)
    {
        if (EventPair.Value.ChannelType == EAudioChannelType::Music)
        {
            if (IsAudioEventPlaying(EventPair.Key))
            {
                MusicEventsToStop.Add(EventPair.Key);
            }
        }
    }
    
    for (const FString& EventID : MusicEventsToStop)
    {
        StopAudioEvent(EventID);
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Stopped current music"));
}

FAudioStatistics UProjectVisibleAudioManager::GetAudioStatistics() const
{
    return CachedStatistics;
}

int32 UProjectVisibleAudioManager::GetCurrentlyPlayingSoundsCount() const
{
    int32 Count = 0;
    for (const auto& AudioComponentPair : PlayingAudioComponents)
    {
        if (AudioComponentPair.Value && IsValid(AudioComponentPair.Value) && AudioComponentPair.Value->IsPlaying())
        {
            Count++;
        }
    }
    return Count;
}

float UProjectVisibleAudioManager::GetAudioMemoryUsage() const
{
    // In a real implementation, you would calculate actual memory usage
    return PlayingAudioComponents.Num() * 0.5f; // Placeholder: 0.5MB per playing sound
}

FText UProjectVisibleAudioManager::GetChannelDisplayName(EAudioChannelType ChannelType) const
{
    switch (ChannelType)
    {
        case EAudioChannelType::Master:   return FText::FromString(TEXT("マスター"));
        case EAudioChannelType::Music:    return FText::FromString(TEXT("音楽"));
        case EAudioChannelType::SFX:      return FText::FromString(TEXT("効果音"));
        case EAudioChannelType::Voice:    return FText::FromString(TEXT("ボイス"));
        case EAudioChannelType::Ambient:  return FText::FromString(TEXT("環境音"));
        case EAudioChannelType::UI:       return FText::FromString(TEXT("UI音"));
        default:                          return FText::FromString(TEXT("不明"));
    }
}

TArray<EAudioChannelType> UProjectVisibleAudioManager::GetAllChannelTypes() const
{
    return {
        EAudioChannelType::Master,
        EAudioChannelType::Music,
        EAudioChannelType::SFX,
        EAudioChannelType::Voice,
        EAudioChannelType::Ambient,
        EAudioChannelType::UI
    };
}

void UProjectVisibleAudioManager::SetAudioQuality(int32 QualityLevel)
{
    CurrentAudioSettings.AudioQuality = FMath::Clamp(QualityLevel, 0, 3);
    
    UE_LOG(LogProjectVisible, Warning, TEXT("Audio quality set to: %d"), CurrentAudioSettings.AudioQuality);
    
    if (GEngine)
    {
        FString QualityNames[] = { TEXT("Low"), TEXT("Medium"), TEXT("High"), TEXT("Ultra") };
        FString QualityName = CurrentAudioSettings.AudioQuality < 4 ? QualityNames[CurrentAudioSettings.AudioQuality] : TEXT("Custom");
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue,
            FString::Printf(TEXT("Audio Quality: %s"), *QualityName));
    }
}

void UProjectVisibleAudioManager::TestPlayButtonSound()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Test: Button Click Sound"));
    }
    
    // Play a simple beep or button sound
    PlayUISound(TEXT("ButtonClick"));
    
    UE_LOG(LogProjectVisible, Warning, TEXT("Test: Playing button sound"));
}

void UProjectVisibleAudioManager::TestPlayNotificationSound()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Test: Notification Sound"));
    }
    
    // Play a notification sound
    PlayUISound(TEXT("Notification"));
    
    UE_LOG(LogProjectVisible, Warning, TEXT("Test: Playing notification sound"));
}

void UProjectVisibleAudioManager::ApplyVolumeToSoundClass(EAudioChannelType ChannelType, float Volume)
{
    USoundClass* SoundClass = GetSoundClassForChannel(ChannelType);
    if (SoundClass)
    {
        // In a real implementation, you would use UGameplayStatics::SetSoundClassVolume
        // For now, we'll just log the action
        UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Applied volume %.2f to channel %d"), Volume, (int32)ChannelType);
    }
}

void UProjectVisibleAudioManager::UpdateAudioStatistics()
{
    CachedStatistics.CurrentlyPlayingSounds = GetCurrentlyPlayingSoundsCount();
    CachedStatistics.TotalSoundsPlayed = TotalSoundsPlayedThisSession;
    CachedStatistics.AudioMemoryUsageMB = GetAudioMemoryUsage();
    CachedStatistics.RegisteredAudioEvents = AudioEvents.Num();
    
    if (CachedStatistics.AudioMemoryUsageMB > CachedStatistics.PeakAudioMemoryUsageMB)
    {
        CachedStatistics.PeakAudioMemoryUsageMB = CachedStatistics.AudioMemoryUsageMB;
    }
}

void UProjectVisibleAudioManager::LoadDefaultAudioEvents()
{
    const TArray<FDefaultAudioEvent>& DefaultEvents = GetDefaultAudioEvents();
    
    for (const FDefaultAudioEvent& DefaultEvent : DefaultEvents)
    {
        FAudioEvent AudioEvent;
        AudioEvent.EventID = DefaultEvent.ID;
        AudioEvent.EventName = FText::FromString(DefaultEvent.Name);
        AudioEvent.ChannelType = DefaultEvent.ChannelType;
        AudioEvent.VolumeMultiplier = DefaultEvent.VolumeMultiplier;
        AudioEvent.bShouldLoop = DefaultEvent.bShouldLoop;
        
        // Note: In a real implementation, you would load the actual sound asset
        // AudioEvent.Sound = LoadObject<USoundBase>(nullptr, *DefaultEvent.SoundAssetPath);
        
        RegisterAudioEvent(AudioEvent);
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Loaded %d default audio events"), DefaultEvents.Num());
}

USoundClass* UProjectVisibleAudioManager::GetSoundClassForChannel(EAudioChannelType ChannelType) const
{
    switch (ChannelType)
    {
        case EAudioChannelType::Master:   return MasterSoundClass;
        case EAudioChannelType::Music:    return MusicSoundClass;
        case EAudioChannelType::SFX:      return SFXSoundClass;
        case EAudioChannelType::Voice:    return VoiceSoundClass;
        case EAudioChannelType::Ambient:  return AmbientSoundClass;
        case EAudioChannelType::UI:       return UISoundClass;
        default:                          return nullptr;
    }
}

float UProjectVisibleAudioManager::CalculateEffectiveVolume(EAudioChannelType ChannelType, float EventVolume) const
{
    float ChannelVolume = GetChannelVolume(ChannelType);
    float MasterVolume = GetMasterVolume();
    
    return EventVolume * ChannelVolume * MasterVolume;
}

const TArray<UProjectVisibleAudioManager::FDefaultAudioEvent>& UProjectVisibleAudioManager::GetDefaultAudioEvents()
{
    static TArray<FDefaultAudioEvent> DefaultEvents = {
        {
            TEXT("UI_ButtonClick"),
            TEXT("Button Click"),
            EAudioChannelType::UI,
            TEXT("/Game/Audio/UI/ButtonClick"),
            0.8f,
            false
        },
        {
            TEXT("UI_ButtonHover"),
            TEXT("Button Hover"),
            EAudioChannelType::UI,
            TEXT("/Game/Audio/UI/ButtonHover"),
            0.6f,
            false
        },
        {
            TEXT("UI_Notification"),
            TEXT("Notification"),
            EAudioChannelType::UI,
            TEXT("/Game/Audio/UI/Notification"),
            0.9f,
            false
        },
        {
            TEXT("SFX_MemoryUnlock"),
            TEXT("Memory Unlock"),
            EAudioChannelType::SFX,
            TEXT("/Game/Audio/SFX/MemoryUnlock"),
            1.0f,
            false
        },
        {
            TEXT("Music_MainTheme"),
            TEXT("Main Theme"),
            EAudioChannelType::Music,
            TEXT("/Game/Audio/Music/MainTheme"),
            0.7f,
            true
        }
    };
    
    return DefaultEvents;
}
