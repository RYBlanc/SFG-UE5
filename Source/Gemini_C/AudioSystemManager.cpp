// Project Visible - Audio System Manager Implementation
// Copyright Epic Games, Inc. All Rights Reserved.
// TEMPORARILY DISABLED - Using ProjectVisibleAudioManager instead

#if 0 // Disable this entire file temporarily

#include "AudioSystemManager.h"
#include "Gemini_C.h"
#include "StoryManager.h"
#include "CharacterManager.h"
#include "VirtueManager.h"
#include "MemoryManager.h"
#include "GameProgressionManager.h"
#include "SocialExperimentManager.h"
#include "BoundaryDissolutionManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWave.h"
#include "AudioDevice.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void UAudioSystemManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Initialize settings
    bAudioSystemActive = true;
    bAdaptiveMusicEnabled = true;
    bSpatialAudioEnabled = true;
    bPsychoacousticEffectsEnabled = true;
    UpdateFrequency = 0.5f; // Update twice per second
    MoodTransitionSpeed = 1.0f;
    MaxConcurrentSounds = 32;
    bEnableAudioOcclusion = true;
    CurrentSpatialZone = ESpatialAudioZone::City;
    
    InitializeAudioSystem();
    
    UE_LOG(LogProjectVisible, Log, TEXT("Audio System Manager initialized"));
}

void UAudioSystemManager::Deinitialize()
{
    StopAllAudio();
    
    // Clear timer
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AdaptiveUpdateTimerHandle);
    }
    
    // Cleanup audio components
    if (MusicAudioComponent)
    {
        MusicAudioComponent->Stop();
        MusicAudioComponent = nullptr;
    }
    
    for (auto& EffectPair : ActiveSoundEffects)
    {
        if (EffectPair.Value)
        {
            EffectPair.Value->Stop();
        }
    }
    ActiveSoundEffects.Empty();
    
    Super::Deinitialize();
    UE_LOG(LogProjectVisible, Log, TEXT("Audio System Manager shutting down"));
}

void UAudioSystemManager::InitializeAudioSystem()
{
    InitializeDefaultConfiguration();
    InitializeDefaultTracks();
    InitializeDefaultEffects();
    SetupAudioComponents();
    
    // Setup adaptive update timer
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            AdaptiveUpdateTimerHandle,
            this,
            &UAudioSystemManager::UpdateAdaptiveMusic,
            UpdateFrequency,
            true // Loop
        );
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Audio System initialized with %d tracks and %d effects"), 
           RegisteredTracks.Num(), RegisteredEffects.Num());
}

// Music System Implementation
void UAudioSystemManager::RegisterMusicTrack(const FMusicTrack& Track)
{
    RegisteredTracks.Add(Track.TrackID, Track);
    UE_LOG(LogProjectVisible, Log, TEXT("Registered music track: %s"), *Track.TrackName);
}

bool UAudioSystemManager::PlayMusicTrack(const FString& TrackID, bool bFadeIn)
{
    const FMusicTrack* Track = RegisteredTracks.Find(TrackID);
    if (!Track)
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Music track not found: %s"), *TrackID);
        return false;
    }
    
    if (!CanPlayTrack(*Track))
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Cannot play track %s - conditions not met"), *Track->TrackName);
        return false;
    }
    
    // Stop current music if playing
    if (IsMusicPlaying())
    {
        StopCurrentMusic(true);
    }
    
    // Store old track for event
    FMusicTrack OldTrack = CurrentMusicTrack;
    CurrentMusicTrack = *Track;
    
    // Get or create music component
    UAudioComponent* MusicComponent = GetOrCreateMusicComponent();
    if (!MusicComponent)
    {
        return false;
    }
    
    // Configure and play
    if (Track->AudioAsset)
    {
        MusicComponent->SetSound(Track->AudioAsset);
        MusicComponent->SetVolumeMultiplier(Track->Volume * GetLayerVolume(EAudioLayer::Music));
        MusicComponent->SetPitchMultiplier(Track->Pitch);
        MusicComponent->bIsMusic = true;
        
        if (bFadeIn)
        {
            MusicComponent->FadeIn(Track->FadeInTime, Track->Volume);
        }
        else
        {
            MusicComponent->Play();
        }
        
        // Record analytics
        RecordTrackPlayback(TrackID);
        RecordAudioEvent(TEXT("Track Played"), Track->TrackName);
        
        // Broadcast event
        OnMusicTrackChanged.Broadcast(CurrentMusicTrack, OldTrack);
        
        UE_LOG(LogProjectVisible, Log, TEXT("Playing music track: %s"), *Track->TrackName);
        return true;
    }
    
    return false;
}

void UAudioSystemManager::StopCurrentMusic(bool bFadeOut)
{
    if (MusicAudioComponent && IsMusicPlaying())
    {
        if (bFadeOut && CurrentMusicTrack.FadeOutTime > 0.0f)
        {
            MusicAudioComponent->FadeOut(CurrentMusicTrack.FadeOutTime, 0.0f);
        }
        else
        {
            MusicAudioComponent->Stop();
        }
        
        UE_LOG(LogProjectVisible, Log, TEXT("Stopped music track: %s"), *CurrentMusicTrack.TrackName);
    }
}

void UAudioSystemManager::PauseMusic()
{
    if (MusicAudioComponent && IsMusicPlaying())
    {
        MusicAudioComponent->SetPaused(true);
        UE_LOG(LogProjectVisible, Log, TEXT("Paused music"));
    }
}

void UAudioSystemManager::ResumeMusic()
{
    if (MusicAudioComponent)
    {
        MusicAudioComponent->SetPaused(false);
        UE_LOG(LogProjectVisible, Log, TEXT("Resumed music"));
    }
}

bool UAudioSystemManager::IsMusicPlaying() const
{
    return MusicAudioComponent && MusicAudioComponent->IsPlaying();
}

void UAudioSystemManager::SetMusicVolume(float Volume)
{
    SetLayerVolume(EAudioLayer::Music, FMath::Clamp(Volume, 0.0f, 1.0f));
    
    if (MusicAudioComponent && IsMusicPlaying())
    {
        MusicAudioComponent->SetVolumeMultiplier(CurrentMusicTrack.Volume * Volume);
    }
}

float UAudioSystemManager::GetMusicVolume() const
{
    return GetLayerVolume(EAudioLayer::Music);
}

// Adaptive Music System
void UAudioSystemManager::UpdateAdaptiveMusic()
{
    if (!bAdaptiveMusicEnabled || !bAudioSystemActive)
    {
        return;
    }
    
    UpdateAdaptiveParameters();
    ProcessMoodTransition();
    UpdateMusicParameters();
    CleanupFinishedEffects();
    UpdateAnalyticsData();
}

FString UAudioSystemManager::SelectBestMusicTrack(EAudioMood TargetMood, EMusicTrackType PreferredType)
{
    FString BestTrackID;
    float BestScore = -1.0f;
    
    for (const auto& TrackPair : RegisteredTracks)
    {
        const FMusicTrack& Track = TrackPair.Value;
        
        if (!CanPlayTrack(Track))
        {
            continue;
        }
        
        float Score = CalculateTrackScore(Track, TargetMood);
        
        // Bonus for matching track type
        if (Track.TrackType == PreferredType)
        {
            Score += 0.2f;
        }
        
        if (Score > BestScore)
        {
            BestScore = Score;
            BestTrackID = Track.TrackID;
        }
    }
    
    return BestTrackID;
}

void UAudioSystemManager::CrossfadeTo(const FString& NewTrackID, float CrossfadeDuration)
{
    const FMusicTrack* NewTrack = RegisteredTracks.Find(NewTrackID);
    if (!NewTrack)
    {
        return;
    }
    
    // Start new track at zero volume
    PlayMusicTrack(NewTrackID, false);
    if (MusicAudioComponent)
    {
        MusicAudioComponent->SetVolumeMultiplier(0.0f);
        MusicAudioComponent->FadeIn(CrossfadeDuration, NewTrack->Volume * GetLayerVolume(EAudioLayer::Music));
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Crossfading to track: %s"), *NewTrack->TrackName);
}

void UAudioSystemManager::SetAdaptiveMusicEnabled(bool bEnabled)
{
    bAdaptiveMusicEnabled = bEnabled;
    UE_LOG(LogProjectVisible, Log, TEXT("Adaptive music %s"), bEnabled ? TEXT("enabled") : TEXT("disabled"));
}

// Sound Effects System
void UAudioSystemManager::RegisterSoundEffect(const FSoundEffect& Effect)
{
    RegisteredEffects.Add(Effect.EffectID, Effect);
    UE_LOG(LogProjectVisible, Log, TEXT("Registered sound effect: %s"), *Effect.EffectName);
}

bool UAudioSystemManager::PlaySoundEffect(const FString& EffectID, FVector Location, AActor* AttachToActor)
{
    const FSoundEffect* Effect = RegisteredEffects.Find(EffectID);
    if (!Effect)
    {
        UE_LOG(LogProjectVisible, Warning, TEXT("Sound effect not found: %s"), *EffectID);
        return false;
    }
    
    // Check cooldown
    if (IsEffectOnCooldown(EffectID))
    {
        return false;
    }
    
    // Check if overlapping is allowed
    if (!Effect->bCanOverlap && ActiveSoundEffects.Contains(EffectID))
    {
        return false;
    }
    
    UAudioComponent* EffectComponent = CreateSoundEffectComponent(*Effect);
    if (!EffectComponent)
    {
        return false;
    }
    
    // Configure spatial audio
    if (Effect->bIs3D)
    {
        ApplySpatialAudioSettings(EffectComponent, *Effect);
        
        if (AttachToActor)
        {
            EffectComponent->AttachToComponent(AttachToActor->GetRootComponent(), 
                FAttachmentTransformRules::KeepWorldTransform);
        }
        else
        {
            EffectComponent->SetWorldLocation(Location);
        }
    }
    
    // Play the effect
    EffectComponent->Play();
    
    // Store for management
    if (Effect->bCanOverlap)
    {
        FString UniqueID = FString::Printf(TEXT("%s_%d"), *EffectID, FMath::Rand());
        ActiveSoundEffects.Add(UniqueID, EffectComponent);
    }
    else
    {
        ActiveSoundEffects.Add(EffectID, EffectComponent);
    }
    
    // Set cooldown
    if (Effect->CooldownTime > 0.0f)
    {
        SoundCooldowns.Add(EffectID, FDateTime::Now());
    }
    
    // Record analytics
    RecordEffectUsage(EffectID);
    RecordAudioEvent(TEXT("Effect Played"), Effect->EffectName);
    
    // Broadcast event
    OnSoundEffectTriggered.Broadcast(*Effect);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Playing sound effect: %s"), *Effect->EffectName);
    return true;
}

void UAudioSystemManager::StopSoundEffect(const FString& EffectID)
{
    UAudioComponent** FoundComponent = ActiveSoundEffects.Find(EffectID);
    if (FoundComponent && *FoundComponent)
    {
        (*FoundComponent)->Stop();
        ActiveSoundEffects.Remove(EffectID);
        UE_LOG(LogProjectVisible, Log, TEXT("Stopped sound effect: %s"), *EffectID);
    }
}

void UAudioSystemManager::StopAllSoundEffects()
{
    for (auto& EffectPair : ActiveSoundEffects)
    {
        if (EffectPair.Value)
        {
            EffectPair.Value->Stop();
        }
    }
    ActiveSoundEffects.Empty();
    UE_LOG(LogProjectVisible, Log, TEXT("Stopped all sound effects"));
}

TArray<FSoundEffect> UAudioSystemManager::GetSoundEffectsByCategory(ESoundEffectCategory Category) const
{
    TArray<FSoundEffect> CategoryEffects;
    
    for (const auto& EffectPair : RegisteredEffects)
    {
        if (EffectPair.Value.Category == Category)
        {
            CategoryEffects.Add(EffectPair.Value);
        }
    }
    
    return CategoryEffects;
}

void UAudioSystemManager::TriggerSoundEffectByType(EAudioTriggerType TriggerType, const FString& Context)
{
    // Find effects that match the trigger type
    for (const auto& EffectPair : RegisteredEffects)
    {
        const FSoundEffect& Effect = EffectPair.Value;
        if (Effect.TriggerType == TriggerType)
        {
            PlaySoundEffect(Effect.EffectID);
            break; // Play first matching effect
        }
    }
}

// Mood and Atmosphere
void UAudioSystemManager::SetAudioMood(EAudioMood NewMood, bool bImmediate)
{
    EAudioMood OldMood = AdaptiveState.CurrentMood;
    
    if (bImmediate)
    {
        AdaptiveState.CurrentMood = NewMood;
        AdaptiveState.TargetMood = NewMood;
    }
    else
    {
        AdaptiveState.TargetMood = NewMood;
    }
    
    AdaptiveState.LastMoodChange = FDateTime::Now();
    
    // Record mood change
    RecordMoodChange(OldMood, NewMood);
    RecordAudioEvent(TEXT("Mood Changed"), FString::Printf(TEXT("%s->%s"), 
        *UEnum::GetValueAsString(OldMood), *UEnum::GetValueAsString(NewMood)));
    
    // Broadcast event
    OnAudioMoodChanged.Broadcast(OldMood, NewMood);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Audio mood changed from %s to %s"), 
           *UEnum::GetValueAsString(OldMood), *UEnum::GetValueAsString(NewMood));
}

void UAudioSystemManager::UpdateAdaptiveAudioState(const FAdaptiveAudioState& NewState)
{
    AdaptiveState = NewState;
    
    // Apply changes based on new state
    if (bAdaptiveMusicEnabled)
    {
        FString BestTrack = SelectBestMusicTrack(AdaptiveState.TargetMood);
        if (!BestTrack.IsEmpty() && BestTrack != CurrentMusicTrack.TrackID)
        {
            CrossfadeTo(BestTrack, 3.0f);
        }
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Updated adaptive audio state"));
}

void UAudioSystemManager::SetIntensityLevel(float NewIntensity)
{
    AdaptiveState.IntensityLevel = FMath::Clamp(NewIntensity, 1.0f, 10.0f);
    UE_LOG(LogProjectVisible, Log, TEXT("Set intensity level to %.1f"), AdaptiveState.IntensityLevel);
}

// Spatial Audio
void UAudioSystemManager::SetSpatialAudioZone(ESpatialAudioZone NewZone)
{
    ESpatialAudioZone OldZone = CurrentSpatialZone;
    CurrentSpatialZone = NewZone;
    
    ProcessSpatialAudioZone();
    
    OnSpatialZoneChanged.Broadcast(OldZone, NewZone);
    RecordAudioEvent(TEXT("Zone Changed"), UEnum::GetValueAsString(NewZone));
    
    UE_LOG(LogProjectVisible, Log, TEXT("Spatial audio zone changed to %s"), 
           *UEnum::GetValueAsString(NewZone));
}

void UAudioSystemManager::UpdatePlayerAudioPosition(FVector NewPosition, FRotator NewRotation)
{
    // Update listener position for spatial audio - simplified implementation
    UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Audio listener position updated: %s"), *NewPosition.ToString());
}

void UAudioSystemManager::SetSpatialAudioEnabled(bool bEnabled)
{
    bSpatialAudioEnabled = bEnabled;
    UE_LOG(LogProjectVisible, Log, TEXT("Spatial audio %s"), bEnabled ? TEXT("enabled") : TEXT("disabled"));
}

// Volume Controls
void UAudioSystemManager::SetLayerVolume(EAudioLayer Layer, float Volume)
{
    CurrentConfiguration.CustomParameters.Add(UEnum::GetValueAsString(Layer), FMath::Clamp(Volume, 0.0f, 1.0f));
    OnVolumeChanged.Broadcast(Layer, Volume);
    
    // Apply to existing audio components based on layer
    switch (Layer)
    {
        case EAudioLayer::Music:
            if (MusicAudioComponent && IsMusicPlaying())
            {
                MusicAudioComponent->SetVolumeMultiplier(CurrentMusicTrack.Volume * Volume);
            }
            break;
        case EAudioLayer::Effects:
            for (auto& EffectPair : ActiveSoundEffects)
            {
                if (EffectPair.Value)
                {
                    const FSoundEffect* Effect = RegisteredEffects.Find(EffectPair.Key);
                    if (Effect)
                    {
                        EffectPair.Value->SetVolumeMultiplier(Effect->Volume * Volume);
                    }
                }
            }
            break;
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Set %s volume to %.2f"), 
           *UEnum::GetValueAsString(Layer), Volume);
}

float UAudioSystemManager::GetLayerVolume(EAudioLayer Layer) const
{
    const float* Volume = CurrentConfiguration.CustomParameters.Find(UEnum::GetValueAsString(Layer));
    return Volume ? *Volume : 1.0f;
}

void UAudioSystemManager::SetMasterVolume(float Volume)
{
    CurrentConfiguration.MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    
    // Apply to all audio components
    if (MusicAudioComponent)
    {
        MusicAudioComponent->SetVolumeMultiplier(CurrentMusicTrack.Volume * GetLayerVolume(EAudioLayer::Music) * Volume);
    }
    
    for (auto& EffectPair : ActiveSoundEffects)
    {
        if (EffectPair.Value)
        {
            const FSoundEffect* Effect = RegisteredEffects.Find(EffectPair.Key);
            if (Effect)
            {
                EffectPair.Value->SetVolumeMultiplier(Effect->Volume * GetLayerVolume(EAudioLayer::Effects) * Volume);
            }
        }
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Set master volume to %.2f"), Volume);
}

float UAudioSystemManager::GetMasterVolume() const
{
    return CurrentConfiguration.MasterVolume;
}

void UAudioSystemManager::MuteLayer(EAudioLayer Layer, bool bMute)
{
    if (bMute)
    {
        SetLayerVolume(Layer, 0.0f);
    }
    else
    {
        // Restore to default volume
        switch (Layer)
        {
            case EAudioLayer::Music:
                SetLayerVolume(Layer, CurrentConfiguration.MusicVolume);
                break;
            case EAudioLayer::Effects:
                SetLayerVolume(Layer, CurrentConfiguration.SFXVolume);
                break;
            case EAudioLayer::Voice:
                SetLayerVolume(Layer, CurrentConfiguration.VoiceVolume);
                break;
            case EAudioLayer::Ambient:
                SetLayerVolume(Layer, CurrentConfiguration.AmbientVolume);
                break;
            default:
                SetLayerVolume(Layer, 1.0f);
                break;
        }
    }
}

bool UAudioSystemManager::IsLayerMuted(EAudioLayer Layer) const
{
    return GetLayerVolume(Layer) <= 0.0f;
}

// Game Integration Event Handlers
void UAudioSystemManager::OnStoryPhaseChanged(EStoryPhase NewPhase)
{
	// Simple mood adjustment based on story phase
	SetAudioMood(EAudioMood::Mysterious);
    TriggerSoundEffectByType(EAudioTriggerType::StoryProgress, UEnum::GetValueAsString(NewPhase));
    RecordAudioEvent(TEXT("Story Phase Changed"), UEnum::GetValueAsString(NewPhase));
}

void UAudioSystemManager::OnLocationChanged(EStoryLocation NewLocation)
{
    // Map story locations to spatial audio zones
    switch (NewLocation)
    {
        case EStoryLocation::Tokyo:
            SetSpatialAudioZone(ESpatialAudioZone::City);
            break;
        case EStoryLocation::Karuizawa:
            SetSpatialAudioZone(ESpatialAudioZone::Garden);
            break;
        case EStoryLocation::Kagawa:
            SetSpatialAudioZone(ESpatialAudioZone::Laboratory);
            break;
		default:
			SetSpatialAudioZone(ESpatialAudioZone::VirtualSpace);
			break;
    }
    
    TriggerSoundEffectByType(EAudioTriggerType::LevelTransition, UEnum::GetValueAsString(NewLocation));
    RecordAudioEvent(TEXT("Location Changed"), UEnum::GetValueAsString(NewLocation));
}

void UAudioSystemManager::OnCharacterInteraction(const FString& CharacterID, ENPCEmotionalState CharacterMood)
{
	// Simple mood adjustment for character interaction
	SetAudioMood(EAudioMood::Contemplative);
    TriggerSoundEffectByType(EAudioTriggerType::CharacterInteraction, CharacterID);
    RecordAudioEvent(TEXT("Character Interaction"), FString::Printf(TEXT("%s_%s"), 
        *CharacterID, *UEnum::GetValueAsString(CharacterMood)));
}

void UAudioSystemManager::OnVirtueChanged(const FString& VirtueType, float NewLevel)
{
    // Adjust virtue balance in adaptive state
    AdaptiveState.VirtueBalance = NewLevel / 100.0f; // Assuming virtue levels are 0-100
    
    TriggerSoundEffectByType(EAudioTriggerType::VirtueChange, VirtueType);
    RecordAudioEvent(TEXT("Virtue Changed"), FString::Printf(TEXT("%s_%.1f"), *VirtueType, NewLevel));
}

void UAudioSystemManager::OnMemoryEvent(const FString& MemoryType, float Importance)
{
    // Update memory load
    AdaptiveState.MemoryLoad = FMath::Clamp(AdaptiveState.MemoryLoad + (Importance / 100.0f), 0.0f, 1.0f);
    
    TriggerSoundEffectByType(EAudioTriggerType::MemoryFormation, MemoryType);
    RecordAudioEvent(TEXT("Memory Event"), FString::Printf(TEXT("%s_%.1f"), *MemoryType, Importance));
}

void UAudioSystemManager::OnProgressUpdate(float OverallProgress)
{
    AdaptiveState.ProgressMomentum = OverallProgress / 100.0f;
    
    // Increase intensity as progress increases
    float NewIntensity = 5.0f + (OverallProgress / 100.0f) * 3.0f; // 5-8 range
    SetIntensityLevel(NewIntensity);
    
    TriggerSoundEffectByType(EAudioTriggerType::ProgressUpdate, FString::Printf(TEXT("%.1f"), OverallProgress));
    RecordAudioEvent(TEXT("Progress Update"), FString::Printf(TEXT("%.1f"), OverallProgress));
}

void UAudioSystemManager::OnAchievementUnlocked(const FString& AchievementID)
{
    SetAudioMood(EAudioMood::Hopeful, false);
    
    TriggerSoundEffectByType(EAudioTriggerType::AchievementUnlock, AchievementID);
    RecordAudioEvent(TEXT("Achievement Unlocked"), AchievementID);
}

void UAudioSystemManager::OnBoundaryDissolution(float DissolutionLevel)
{
    // Adjust mood based on dissolution level
    if (DissolutionLevel > 0.7f)
    {
        SetAudioMood(EAudioMood::Ethereal);
    }
    else if (DissolutionLevel > 0.3f)
    {
        SetAudioMood(EAudioMood::Mysterious);
    }
    
    // Adjust intensity
    float NewIntensity = 5.0f + DissolutionLevel * 4.0f; // 5-9 range
    SetIntensityLevel(NewIntensity);
    
    TriggerSoundEffectByType(EAudioTriggerType::BoundaryDissolution, FString::Printf(TEXT("%.1f"), DissolutionLevel));
    RecordAudioEvent(TEXT("Boundary Dissolution"), FString::Printf(TEXT("%.1f"), DissolutionLevel));
}

// System State Control
void UAudioSystemManager::PauseAllAudio()
{
    if (MusicAudioComponent)
    {
        MusicAudioComponent->SetPaused(true);
    }
    
    for (auto& EffectPair : ActiveSoundEffects)
    {
        if (EffectPair.Value)
        {
            EffectPair.Value->SetPaused(true);
        }
    }
    
    bAudioSystemActive = false;
    UE_LOG(LogProjectVisible, Log, TEXT("Paused all audio"));
}

void UAudioSystemManager::ResumeAllAudio()
{
    if (MusicAudioComponent)
    {
        MusicAudioComponent->SetPaused(false);
    }
    
    for (auto& EffectPair : ActiveSoundEffects)
    {
        if (EffectPair.Value)
        {
            EffectPair.Value->SetPaused(false);
        }
    }
    
    bAudioSystemActive = true;
    UE_LOG(LogProjectVisible, Log, TEXT("Resumed all audio"));
}

void UAudioSystemManager::StopAllAudio()
{
    StopCurrentMusic(false);
    StopAllSoundEffects();
    UE_LOG(LogProjectVisible, Log, TEXT("Stopped all audio"));
}

// Helper Functions Implementation
void UAudioSystemManager::InitializeDefaultTracks()
{
    // Main Theme
    FMusicTrack MainTheme;
    MainTheme.TrackID = TEXT("MAIN_THEME");
    MainTheme.TrackName = TEXT("Project Visible メインテーマ");
    MainTheme.TrackType = EMusicTrackType::MainTheme;
    MainTheme.PrimaryMood = EAudioMood::Contemplative;
    MainTheme.IntensityLevel = 5;
    MainTheme.BPM = 80.0f;
    MainTheme.Key = TEXT("Dm");
    MainTheme.bIsDynamic = true;
    RegisterMusicTrack(MainTheme);
    
    // Reality Ambient
    FMusicTrack RealityAmbient;
    RealityAmbient.TrackID = TEXT("REALITY_AMBIENT");
    RealityAmbient.TrackName = TEXT("現実世界の環境音楽");
    RealityAmbient.TrackType = EMusicTrackType::RealityAmbient;
    RealityAmbient.PrimaryMood = EAudioMood::Tense;
    RealityAmbient.IntensityLevel = 4;
    RealityAmbient.BPM = 90.0f;
    RealityAmbient.Key = TEXT("Em");
    RealityAmbient.ValidLocations.Add(EStoryLocation::Tokyo);
    RegisterMusicTrack(RealityAmbient);
    
    // Dream Ambient
    FMusicTrack DreamAmbient;
    DreamAmbient.TrackID = TEXT("DREAM_AMBIENT");
    DreamAmbient.TrackName = TEXT("夢世界の環境音楽");
    DreamAmbient.TrackType = EMusicTrackType::DreamAmbient;
    DreamAmbient.PrimaryMood = EAudioMood::Ethereal;
    DreamAmbient.IntensityLevel = 6;
    DreamAmbient.BPM = 60.0f;
    DreamAmbient.Key = TEXT("C");
    DreamAmbient.bIsInteractive = true;
    RegisterMusicTrack(DreamAmbient);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Initialized default music tracks"));
}

void UAudioSystemManager::InitializeDefaultEffects()
{
    // UI Click
    FSoundEffect UIClick;
    UIClick.EffectID = TEXT("UI_CLICK");
    UIClick.EffectName = TEXT("UIクリック音");
    UIClick.Category = ESoundEffectCategory::UI;
    UIClick.Volume = 0.8f;
    UIClick.Priority = 80;
    UIClick.TriggerType = EAudioTriggerType::PlayerChoice;
    RegisterSoundEffect(UIClick);
    
    // Achievement Unlock
    FSoundEffect Achievement;
    Achievement.EffectID = TEXT("ACHIEVEMENT_UNLOCK");
    Achievement.EffectName = TEXT("実績解除音");
    Achievement.Category = ESoundEffectCategory::UI;
    Achievement.Volume = 1.0f;
    Achievement.Priority = 90;
    Achievement.TriggerType = EAudioTriggerType::AchievementUnlock;
    RegisterSoundEffect(Achievement);
    
    // Evidence Discovery
    FSoundEffect Evidence;
    Evidence.EffectID = TEXT("EVIDENCE_DISCOVERY");
    Evidence.EffectName = TEXT("証拠発見音");
    Evidence.Category = ESoundEffectCategory::Interaction;
    Evidence.Volume = 0.9f;
    Evidence.Priority = 85;
    Evidence.bIs3D = true;
    Evidence.AttenuationRadius = 500.0f;
    Evidence.TriggerType = EAudioTriggerType::EvidenceDiscovery;
    RegisterSoundEffect(Evidence);
    
    // Character Interaction
    FSoundEffect Dialogue;
    Dialogue.EffectID = TEXT("DIALOGUE_START");
    Dialogue.EffectName = TEXT("対話開始音");
    Dialogue.Category = ESoundEffectCategory::Communication;
    Dialogue.Volume = 0.7f;
    Dialogue.Priority = 70;
    Dialogue.TriggerType = EAudioTriggerType::CharacterInteraction;
    RegisterSoundEffect(Dialogue);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Initialized default sound effects"));
}

void UAudioSystemManager::InitializeDefaultConfiguration()
{
    CurrentConfiguration = FAudioConfiguration();
    CurrentConfiguration.ConfigurationID = TEXT("DEFAULT");
    CurrentConfiguration.ConfigurationName = TEXT("デフォルト設定");
    CurrentConfiguration.Description = TEXT("Project Visibleの標準オーディオ設定");
    
    // Initialize layer volumes
    CurrentConfiguration.CustomParameters.Add(TEXT("Reality"), 1.0f);
    CurrentConfiguration.CustomParameters.Add(TEXT("Dream"), 1.0f);
    CurrentConfiguration.CustomParameters.Add(TEXT("Boundary"), 1.0f);
    CurrentConfiguration.CustomParameters.Add(TEXT("UI"), 1.0f);
    CurrentConfiguration.CustomParameters.Add(TEXT("Ambient"), 0.7f);
    CurrentConfiguration.CustomParameters.Add(TEXT("Music"), 0.8f);
    CurrentConfiguration.CustomParameters.Add(TEXT("Voice"), 1.0f);
    CurrentConfiguration.CustomParameters.Add(TEXT("Effects"), 1.0f);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Initialized default audio configuration"));
}

void UAudioSystemManager::SetupAudioComponents()
{
    // Setup will be done when needed via GetOrCreateMusicComponent
    UE_LOG(LogProjectVisible, Log, TEXT("Audio components setup ready"));
}

UAudioComponent* UAudioSystemManager::GetOrCreateMusicComponent()
{
    if (!MusicAudioComponent)
    {
        if (UWorld* World = GetWorld())
        {
            MusicAudioComponent = UGameplayStatics::CreateSound2D(World, nullptr, 1.0f, 1.0f, 0.0f, nullptr, false, false);
        }
    }
    return MusicAudioComponent;
}

bool UAudioSystemManager::CanPlayTrack(const FMusicTrack& Track) const
{
    // Check if track has any location restrictions
    if (Track.ValidLocations.Num() > 0)
    {
        // Would need to check current location from story manager
        return true; // Simplified for now
    }
    
    // Check mood compatibility
    if (Track.ValidMoods.Num() > 0)
    {
        return Track.ValidMoods.Contains(AdaptiveState.CurrentMood);
    }
    
    return true;
}

float UAudioSystemManager::CalculateTrackScore(const FMusicTrack& Track, EAudioMood TargetMood) const
{
    float Score = 0.0f;
    
    // Mood match bonus
    if (Track.PrimaryMood == TargetMood)
    {
        Score += 1.0f;
    }
    
    // Intensity match bonus
    float IntensityDiff = FMath::Abs(Track.IntensityLevel - AdaptiveState.IntensityLevel);
    Score += (10.0f - IntensityDiff) / 10.0f;
    
    // Dynamic track bonus
    if (Track.bIsDynamic)
    {
        Score += 0.1f;
    }
    
    return Score;
}

// Analytics Implementation
void UAudioSystemManager::RecordAudioEvent(const FString& EventType, const FString& Context)
{
    // Record in social experiment system
    if (USocialExperimentManager* ExperimentManager = GetExperimentManager())
    {
        ExperimentManager->RecordBehavioralData(
            1, // Default experiment ID
            TEXT("CURRENT_PLAYER"),
            EventType,
            Context,
            1.0f
        );
    }
}

void UAudioSystemManager::RecordTrackPlayback(const FString& TrackID)
{
    AnalyticsData.TracksPlayed++;
    AnalyticsData.TrackPlayCounts.FindOrAdd(TrackID)++;
}

void UAudioSystemManager::RecordEffectUsage(const FString& EffectID)
{
    AnalyticsData.EffectsTriggered++;
    AnalyticsData.EffectUsageCounts.FindOrAdd(EffectID)++;
}

void UAudioSystemManager::RecordMoodChange(EAudioMood OldMood, EAudioMood NewMood)
{
    AnalyticsData.MoodTransitions++;
    AnalyticsData.MoodChangeEvents.Add(FString::Printf(TEXT("%s->%s"), 
        *UEnum::GetValueAsString(OldMood), *UEnum::GetValueAsString(NewMood)));
}

void UAudioSystemManager::UpdateAnalyticsData()
{
    FDateTime Now = FDateTime::Now();
    if (AnalyticsData.SessionStart == FDateTime::MinValue())
    {
        AnalyticsData.SessionStart = Now;
        AnalyticsData.SessionID = FGuid::NewGuid().ToString();
    }
    
    AnalyticsData.PlayTime = (Now - AnalyticsData.SessionStart).GetTotalSeconds();
    AnalyticsData.PreferredMood = AdaptiveState.CurrentMood;
    AnalyticsData.PreferredIntensity = AdaptiveState.IntensityLevel;
    AnalyticsData.AverageSessionVolume = GetMasterVolume();
    
    LastAnalyticsUpdate = Now;
}

void UAudioSystemManager::ResetAudioAnalytics()
{
    AnalyticsData = FAudioAnalyticsData();
    UE_LOG(LogProjectVisible, Log, TEXT("Audio analytics reset"));
}

// System Integration Helpers
UStoryManager* UAudioSystemManager::GetStoryManager()
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            return GameInstance->GetSubsystem<UStoryManager>();
        }
    }
    return nullptr;
}

UCharacterManager* UAudioSystemManager::GetCharacterManager()
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            return GameInstance->GetSubsystem<UCharacterManager>();
        }
    }
    return nullptr;
}

USocialExperimentManager* UAudioSystemManager::GetExperimentManager()
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            return GameInstance->GetSubsystem<USocialExperimentManager>();
        }
    }
    return nullptr;
}

// Simplified implementations for remaining functions
void UAudioSystemManager::ApplyAudioConfiguration(const FAudioConfiguration& Config) 
{ 
    CurrentConfiguration = Config; 
    OnAudioConfigurationChanged.Broadcast(Config);
}

void UAudioSystemManager::SaveCurrentConfiguration(const FString& ConfigName) 
{ 
    SavedConfigurations.Add(ConfigName, CurrentConfiguration); 
}

bool UAudioSystemManager::LoadConfiguration(const FString& ConfigName) 
{ 
    if (FAudioConfiguration* Config = SavedConfigurations.Find(ConfigName))
    {
        ApplyAudioConfiguration(*Config);
        return true;
    }
    return false;
}

TArray<FString> UAudioSystemManager::GetAvailableConfigurations() const 
{ 
    TArray<FString> Names;
    SavedConfigurations.GetKeys(Names);
    return Names;
}

void UAudioSystemManager::SetPsychoacousticEffectsEnabled(bool bEnabled) 
{ 
    bPsychoacousticEffectsEnabled = bEnabled; 
}

void UAudioSystemManager::ApplyReverbEffect(float ReverbStrength, float DecayTime) {}
void UAudioSystemManager::ApplyLowPassFilter(float CutoffFrequency) {}
void UAudioSystemManager::ApplyHighPassFilter(float CutoffFrequency) {}
void UAudioSystemManager::ApplyBinauralBeats(float Frequency1, float Frequency2) {}

UAudioComponent* UAudioSystemManager::CreateSoundEffectComponent(const FSoundEffect& Effect)
{
    if (UWorld* World = GetWorld())
    {
        return UGameplayStatics::CreateSound2D(World, Effect.AudioAsset, Effect.Volume, Effect.Pitch);
    }
    return nullptr;
}

void UAudioSystemManager::CleanupFinishedEffects()
{
    TArray<FString> FinishedEffects;
    for (auto& EffectPair : ActiveSoundEffects)
    {
        if (EffectPair.Value && !EffectPair.Value->IsPlaying())
        {
            FinishedEffects.Add(EffectPair.Key);
        }
    }
    
    for (const FString& EffectID : FinishedEffects)
    {
        ActiveSoundEffects.Remove(EffectID);
    }
}

bool UAudioSystemManager::IsEffectOnCooldown(const FString& EffectID) const
{
    if (const FDateTime* LastPlayed = SoundCooldowns.Find(EffectID))
    {
        const FSoundEffect* Effect = RegisteredEffects.Find(EffectID);
        if (Effect && Effect->CooldownTime > 0.0f)
        {
            return (FDateTime::Now() - *LastPlayed).GetTotalSeconds() < Effect->CooldownTime;
        }
    }
    return false;
}

void UAudioSystemManager::ApplySpatialAudioSettings(UAudioComponent* AudioComponent, const FSoundEffect& Effect)
{
    if (AudioComponent && bSpatialAudioEnabled)
    {
        AudioComponent->bAllowSpatialization = true;
        AudioComponent->AttenuationSettings = nullptr; // Would set custom attenuation
    }
}

// Placeholder implementations for remaining helper functions
void UAudioSystemManager::UpdateAdaptiveParameters() {}
void UAudioSystemManager::CalculateTargetMood() {}
void UAudioSystemManager::ProcessMoodTransition() {}
void UAudioSystemManager::UpdateMusicParameters() {}
void UAudioSystemManager::UpdateIntensityBasedOnGameState() {}
void UAudioSystemManager::ApplyPsychoacousticEffects() {}
void UAudioSystemManager::SynchronizeWithGameSystems() {}
void UAudioSystemManager::UpdateVolumeBasedOnGameState() {}
void UAudioSystemManager::ProcessSpatialAudioZone() {}
void UAudioSystemManager::ProcessMusicCrossfade() {}

UVirtueManager* UAudioSystemManager::GetVirtueManager() { return nullptr; }
UMemoryManager* UAudioSystemManager::GetMemoryManager() { return nullptr; }
UGameProgressionManager* UAudioSystemManager::GetProgressionManager() { return nullptr; }
UBoundaryDissolutionManager* UAudioSystemManager::GetBoundaryManager() { return nullptr; }

#endif // Temporarily disabled
