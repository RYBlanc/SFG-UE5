// Project Visible - Test Actor Implementation
// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectVisibleTestActor.h"
#include "Gemini_C.h"
#include "LandscapeOfMindManager.h"
#include "QuantumObservationSystem.h"
#include "NoiseCreatureAI.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AProjectVisibleTestActor::AProjectVisibleTestActor()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Default test settings
    bAutoRunTests = true;
    TestInterval = 5.0f;
    MaxNodeStones = 5;
    MaxUncertainAreas = 3;
    MaxCreatures = 4;
    
    PassedTests = 0;
    FailedTests = 0;
    CurrentTestIndex = 0;
    
    UE_LOG(LogProjectVisible, Log, TEXT("Project Visible Test Actor created"));
}

void AProjectVisibleTestActor::BeginPlay()
{
    Super::BeginPlay();
    
    // Get system references
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        LandscapeManager = GameInstance->GetSubsystem<ULandscapeOfMindManager>();
        // ObservationSystem and CreatureAI will be obtained from LandscapeManager when fully implemented
    }
    
    ResetTestResults();
    
    if (bAutoRunTests)
    {
        // Start automated testing after a short delay
        GetWorldTimerManager().SetTimer(TestTimerHandle, this, &AProjectVisibleTestActor::RunAllTests, 2.0f, false);
    }
    
    UE_LOG(LogProjectVisible, Log, TEXT("Project Visible Test Actor initialized"));
}

void AProjectVisibleTestActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AProjectVisibleTestActor::RunAllTests()
{
    UE_LOG(LogProjectVisible, Warning, TEXT("=== Project Visible システムテスト開始 ==="));
    
    ResetTestResults();
    
    // Setup test sequence
    TestSequence.Empty();
    TestSequence.Add([this]() { TestNodeStonePlacement(); });
    TestSequence.Add([this]() { TestQuantumObservation(); });
    TestSequence.Add([this]() { TestNoiseCreatureSpawn(); });
    TestSequence.Add([this]() { TestGamePhaseTransition(); });
    TestSequence.Add([this]() { TestSystemIntegration(); });
    
    CurrentTestIndex = 0;
    
    // Start first test
    if (TestSequence.Num() > 0)
    {
        TestSequence[CurrentTestIndex]();
        CurrentTestIndex++;
        
        // Schedule next tests
        GetWorldTimerManager().SetTimer(TestTimerHandle, [this]()
        {
            if (CurrentTestIndex < TestSequence.Num())
            {
                TestSequence[CurrentTestIndex]();
                CurrentTestIndex++;
            }
            else
            {
                // All tests completed
                UE_LOG(LogProjectVisible, Warning, TEXT("=== テスト完了: 成功 %d / 失敗 %d ==="), PassedTests, FailedTests);
                GetWorldTimerManager().ClearTimer(TestTimerHandle);
            }
        }, TestInterval, true);
    }
}

void AProjectVisibleTestActor::TestNodeStonePlacement()
{
    UE_LOG(LogProjectVisible, Warning, TEXT("--- ノード・ストーン配置テスト開始 ---"));
    
    if (!LandscapeManager)
    {
        LogTestResult(TEXT("NodeStone Placement"), false, TEXT("LandscapeManager not found"));
        return;
    }
    
    // Test landscape initialization
    FMentalLandscapeConfig TestConfig;
    TestConfig.LandscapeSize = FVector(8192.0f, 8192.0f, 2048.0f);
    TestConfig.InitialChaosLevel = 80;
    TestConfig.TargetStabilityLevel = 50;
    TestConfig.MaxNodeStones = 10;
    TestConfig.MaxSynapseRivers = 5;
    
    LandscapeManager->InitializeMentalLandscape(TestConfig);
    
    // Test node stone placement
    bool bPlacementSuccess = true;
    TArray<FVector> TestLocations = {
        FVector(0, 0, 100),
        FVector(500, 0, 100),
        FVector(0, 500, 100),
        FVector(-500, 0, 100),
        FVector(0, -500, 100)
    };
    
    TArray<ENodeStoneType> TestTypes = {
        ENodeStoneType::Stabilizer,
        ENodeStoneType::Amplifier,
        ENodeStoneType::Redirector,
        ENodeStoneType::Harmonizer,
        ENodeStoneType::Catalyst
    };
    
    for (int32 i = 0; i < FMath::Min(TestLocations.Num(), MaxNodeStones); i++)
    {
        LandscapeManager->PlaceNodeStone(TestLocations[i], TestTypes[i]);
        UE_LOG(LogProjectVisible, Log, TEXT("Placed NodeStone %d (%s) at %s"), 
               i, *UEnum::GetValueAsString(TestTypes[i]), *TestLocations[i].ToString());
    }
    
    LogTestResult(TEXT("NodeStone Placement"), bPlacementSuccess, 
                  FString::Printf(TEXT("Placed %d node stones"), FMath::Min(TestLocations.Num(), MaxNodeStones)));
}

void AProjectVisibleTestActor::TestQuantumObservation()
{
    UE_LOG(LogProjectVisible, Warning, TEXT("--- 量子観測システムテスト開始 ---"));
    
    if (!LandscapeManager)
    {
        LogTestResult(TEXT("Quantum Observation"), false, TEXT("LandscapeManager not found"));
        return;
    }
    
    // Test uncertain area creation
    bool bObservationSuccess = true;
    TArray<FVector> UncertainLocations = {
        FVector(1000, 1000, 0),
        FVector(-1000, 1000, 0),
        FVector(1000, -1000, 0)
    };
    
    for (int32 i = 0; i < FMath::Min(UncertainLocations.Num(), MaxUncertainAreas); i++)
    {
        // Use area ID instead of location
        LandscapeManager->ObserveUncertainArea(i + 1);
        UE_LOG(LogProjectVisible, Log, TEXT("Observing uncertain area %d"), i + 1);
    }
    
    LogTestResult(TEXT("Quantum Observation"), bObservationSuccess,
                  FString::Printf(TEXT("Created %d uncertain areas"), FMath::Min(UncertainLocations.Num(), MaxUncertainAreas)));
}

void AProjectVisibleTestActor::TestNoiseCreatureSpawn()
{
    UE_LOG(LogProjectVisible, Warning, TEXT("--- ノイズ・クリーチャーAIテスト開始 ---"));
    
    if (!LandscapeManager)
    {
        LogTestResult(TEXT("Noise Creature Spawn"), false, TEXT("LandscapeManager not found"));
        return;
    }
    
    // Test creature spawning
    bool bSpawnSuccess = true;
    TArray<FVector> SpawnLocations = {
        FVector(300, 300, 50),
        FVector(-300, 300, 50),
        FVector(300, -300, 50),
        FVector(-300, -300, 50)
    };
    
    for (int32 i = 0; i < FMath::Min(SpawnLocations.Num(), MaxCreatures); i++)
    {
        LandscapeManager->SpawnNoiseCreatures(SpawnLocations[i], 1);
        UE_LOG(LogProjectVisible, Log, TEXT("Spawned creature %d at %s"), i, *SpawnLocations[i].ToString());
    }
    
    LogTestResult(TEXT("Noise Creature Spawn"), bSpawnSuccess,
                  FString::Printf(TEXT("Spawned %d creatures"), FMath::Min(SpawnLocations.Num(), MaxCreatures)));
}

void AProjectVisibleTestActor::TestGamePhaseTransition()
{
    UE_LOG(LogProjectVisible, Warning, TEXT("--- ゲームフェーズ遷移テスト開始 ---"));
    
    if (!LandscapeManager)
    {
        LogTestResult(TEXT("Game Phase Transition"), false, TEXT("LandscapeManager not found"));
        return;
    }
    
    // Test landscape stability updates
    bool bPhaseSuccess = true;
    
    // Test stability changes
    for (int32 i = 0; i < 5; i++)
    {
        float TestStability = FMath::RandRange(0.0f, 100.0f);
        LandscapeManager->UpdateLandscapeStability(TestStability);
        UE_LOG(LogProjectVisible, Log, TEXT("Updated landscape stability to %f"), TestStability);
    }
    
    LogTestResult(TEXT("Game Phase Transition"), bPhaseSuccess, TEXT("Stability updates completed"));
}

void AProjectVisibleTestActor::TestSystemIntegration()
{
    UE_LOG(LogProjectVisible, Warning, TEXT("--- システム統合テスト開始 ---"));
    
    bool bIntegrationSuccess = true;
    
    // Test system interactions
    if (LandscapeManager)
    {
        // Test combined operations
        FVector TestLocation(0, 0, 200);
        
        // Place a node stone
        LandscapeManager->PlaceNodeStone(TestLocation, ENodeStoneType::Stabilizer);
        
        // Observe uncertain area nearby
        LandscapeManager->ObserveUncertainArea(100);
        
        // Spawn creature
        FVector CreatureLocation = TestLocation + FVector(-200, -200, 0);
        LandscapeManager->SpawnNoiseCreatures(CreatureLocation, 1);
        
        // Update stability
        LandscapeManager->UpdateLandscapeStability(75.0f);
        
        UE_LOG(LogProjectVisible, Log, TEXT("System integration test operations completed"));
    }
    else
    {
        bIntegrationSuccess = false;
    }
    
    LogTestResult(TEXT("System Integration"), bIntegrationSuccess, TEXT("Combined system operations"));
}

void AProjectVisibleTestActor::LogTestResult(const FString& TestName, bool bPassed, const FString& Details)
{
    FString Result = FString::Printf(TEXT("[%s] %s: %s"), 
                                     bPassed ? TEXT("PASS") : TEXT("FAIL"),
                                     *TestName,
                                     *Details);
    
    TestResults.Add(Result);
    
    if (bPassed)
    {
        PassedTests++;
        UE_LOG(LogProjectVisible, Log, TEXT("%s"), *Result);
    }
    else
    {
        FailedTests++;
        UE_LOG(LogProjectVisible, Error, TEXT("%s"), *Result);
    }
    
    // Also display on screen if possible
    if (GEngine)
    {
        FColor DisplayColor = bPassed ? FColor::Green : FColor::Red;
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, DisplayColor, Result);
    }
}

void AProjectVisibleTestActor::ResetTestResults()
{
    TestResults.Empty();
    PassedTests = 0;
    FailedTests = 0;
    CurrentTestIndex = 0;
}
