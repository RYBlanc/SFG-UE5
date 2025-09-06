// Project Visible - Test Actor for System Validation
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LandscapeOfMindTypes.h"
#include "ProjectVisibleTestActor.generated.h"

class ULandscapeOfMindManager;
class UQuantumObservationSystem;
class UNoiseCreatureAI;

UCLASS()
class GEMINI_C_API AProjectVisibleTestActor : public AActor
{
    GENERATED_BODY()
    
public:    
    AProjectVisibleTestActor();

protected:
    virtual void BeginPlay() override;

public:    
    virtual void Tick(float DeltaTime) override;

    // Test Functions
    UFUNCTION(BlueprintCallable, Category = "Project Visible Tests")
    void RunAllTests();

    UFUNCTION(BlueprintCallable, Category = "Project Visible Tests")
    void TestNodeStonePlacement();

    UFUNCTION(BlueprintCallable, Category = "Project Visible Tests")
    void TestQuantumObservation();

    UFUNCTION(BlueprintCallable, Category = "Project Visible Tests")
    void TestNoiseCreatureSpawn();

    UFUNCTION(BlueprintCallable, Category = "Project Visible Tests")
    void TestGamePhaseTransition();

    UFUNCTION(BlueprintCallable, Category = "Project Visible Tests")
    void TestSystemIntegration();

protected:
    // Test Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Settings")
    bool bAutoRunTests;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Settings")
    float TestInterval;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Settings")
    int32 MaxNodeStones;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Settings")
    int32 MaxUncertainAreas;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Settings")
    int32 MaxCreatures;

    // Test State
    UPROPERTY(BlueprintReadOnly, Category = "Test Results")
    TArray<FString> TestResults;

    UPROPERTY(BlueprintReadOnly, Category = "Test Results")
    int32 PassedTests;

    UPROPERTY(BlueprintReadOnly, Category = "Test Results")
    int32 FailedTests;

private:
    // Helper Functions
    void LogTestResult(const FString& TestName, bool bPassed, const FString& Details = TEXT(""));
    void ResetTestResults();
    
    // System References
    UPROPERTY()
    ULandscapeOfMindManager* LandscapeManager;

    UPROPERTY()
    UQuantumObservationSystem* ObservationSystem;

    UPROPERTY()
    UNoiseCreatureAI* CreatureAI;

    // Test Timer
    FTimerHandle TestTimerHandle;
    int32 CurrentTestIndex;
    TArray<TFunction<void()>> TestSequence;
};
