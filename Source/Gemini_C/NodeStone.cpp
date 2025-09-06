// Project Visible - Node Stone Actor Implementation
// Copyright Epic Games, Inc. All Rights Reserved.

#include "NodeStone.h"
#include "Gemini_C.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Engine.h"
#include "Engine/StaticMeshActor.h"
#include "LandscapeOfMindManager.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

ANodeStone::ANodeStone()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Initialize properties
    bIsInitialized = false;
    LastInfluenceUpdateTime = 0.0f;
    NodeID = 0;
    
    // Initialize default properties
    Properties = FNodeStoneProperties();
    
    InitializeComponents();
}

void ANodeStone::InitializeComponents()
{
    // Create root component
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    
    // Create stone mesh component
    StoneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StoneMesh"));
    StoneMesh->SetupAttachment(RootComponent);
    StoneMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    StoneMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    StoneMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    
    // Try to load a default mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere"));
    if (SphereMeshAsset.Succeeded())
    {
        StoneMesh->SetStaticMesh(SphereMeshAsset.Object);
        StoneMesh->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
    }
    
    // Create influence sphere component (for visualization and collision detection)
    InfluenceSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InfluenceSphere"));
    InfluenceSphere->SetupAttachment(RootComponent);
    InfluenceSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InfluenceSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    InfluenceSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    InfluenceSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
    InfluenceSphere->SetSphereRadius(Properties.InfluenceRadius);
    InfluenceSphere->SetVisibility(false); // Hidden by default
    
    // Create Niagara effect component
    EffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EffectComponent"));
    EffectComponent->SetupAttachment(RootComponent);
    EffectComponent->SetAutoActivate(false);
}

void ANodeStone::BeginPlay()
{
    Super::BeginPlay();
    
    SetupMaterial();
    SetupEffects();
    UpdateInfluenceRadius();
    UpdateVisualEffects();
    
    bIsInitialized = true;
    
    UE_LOG(LogProjectVisible, Log, TEXT("Node Stone %d initialized at location %s"), 
           NodeID, *GetActorLocation().ToString());
}

void ANodeStone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (!bIsInitialized || !Properties.bIsActive)
    {
        return;
    }
    
    LastInfluenceUpdateTime += DeltaTime;
    
    // Update terrain influence at regular intervals
    if (LastInfluenceUpdateTime >= INFLUENCE_UPDATE_INTERVAL)
    {
        ApplyTerrainInfluence();
        LastInfluenceUpdateTime = 0.0f;
    }
    
    // Update visual effects based on current state
    UpdateVisualEffects();
}

void ANodeStone::SetNodeStoneProperties(const FNodeStoneProperties& NewProperties)
{
    FNodeStoneProperties OldProperties = Properties;
    Properties = NewProperties;
    
    // Update components based on new properties
    UpdateInfluenceRadius();
    UpdateGlowColor(Properties.GlowColor);
    SetActiveState(Properties.bIsActive);
    
    // Execute type-specific behavior
    switch (Properties.Type)
    {
        case ENodeStoneType::Stabilizer:
            ExecuteStabilizerBehavior();
            break;
        case ENodeStoneType::Amplifier:
            ExecuteAmplifierBehavior();
            break;
        case ENodeStoneType::Redirector:
            ExecuteRedirectorBehavior();
            break;
        case ENodeStoneType::Harmonizer:
            ExecuteHarmonizerBehavior();
            break;
        case ENodeStoneType::Catalyst:
            ExecuteCatalystBehavior();
            break;
    }
    
    OnPropertiesChanged.Broadcast(this, Properties);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Node Stone %d properties updated"), NodeID);
}

void ANodeStone::UpdateInfluenceRadius()
{
    if (InfluenceSphere)
    {
        InfluenceSphere->SetSphereRadius(Properties.InfluenceRadius);
    }
}

void ANodeStone::SetActiveState(bool bNewActive)
{
    Properties.bIsActive = bNewActive;
    
    // Update visual state
    if (StoneMesh)
    {
        StoneMesh->SetVisibility(bNewActive);
    }
    
    // Update effect state
    if (EffectComponent)
    {
        if (bNewActive)
        {
            EffectComponent->Activate();
        }
        else
        {
            EffectComponent->Deactivate();
        }
    }
    
    OnNodeStoneActivated.Broadcast(this, bNewActive);
    
    UE_LOG(LogProjectVisible, Log, TEXT("Node Stone %d active state changed to %s"), 
           NodeID, bNewActive ? TEXT("Active") : TEXT("Inactive"));
}

bool ANodeStone::IsWithinInfluenceRange(const FVector& Location) const
{
    float Distance = FVector::Dist(GetActorLocation(), Location);
    return Distance <= Properties.InfluenceRadius;
}

void ANodeStone::ApplyTerrainInfluence()
{
    if (!Properties.bIsActive)
    {
        return;
    }
    
    // Get landscape manager
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            if (ULandscapeOfMindManager* LandscapeManager = GameInstance->GetSubsystem<ULandscapeOfMindManager>())
            {
                // TODO: Apply actual terrain deformation through TerrainDeformationEngine
                // For now, just log the influence
                UE_LOG(LogProjectVisible, VeryVerbose, TEXT("Node Stone %d applying terrain influence (Type: %d, Power: %f)"), 
                       NodeID, (int32)Properties.Type, Properties.StabilizationPower);
                
                OnTerrainInfluenceApplied.Broadcast(this);
            }
        }
    }
}

void ANodeStone::UpdateVisualEffects()
{
    UpdateMaterialParameters();
    
    // Update effect intensity based on activity and power
    if (EffectComponent && Properties.bIsActive)
    {
        // Set effect parameters based on node stone properties
        EffectComponent->SetFloatParameter(TEXT("InfluenceRadius"), Properties.InfluenceRadius);
        EffectComponent->SetFloatParameter(TEXT("Power"), Properties.StabilizationPower);
        EffectComponent->SetVectorParameter(TEXT("GlowColor"), FVector(Properties.GlowColor.R, Properties.GlowColor.G, Properties.GlowColor.B));
    }
}

void ANodeStone::UpdateGlowColor(const FLinearColor& NewColor)
{
    Properties.GlowColor = NewColor;
    UpdateMaterialParameters();
}

void ANodeStone::ExecuteStabilizerBehavior()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Node Stone %d executing Stabilizer behavior"), NodeID);
    
    // Stabilizers reduce terrain chaos in their area
    // They create gentle, harmonious terrain modifications
    Properties.StabilizationPower = FMath::Max(Properties.StabilizationPower, 1.0f);
    Properties.GlowColor = FLinearColor::Blue;
}

void ANodeStone::ExecuteAmplifierBehavior()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Node Stone %d executing Amplifier behavior"), NodeID);
    
    // Amplifiers increase the effect of nearby node stones
    // They boost energy flow and stabilization power
    Properties.EnergyOutput *= 1.5f;
    Properties.GlowColor = FLinearColor::Yellow;
}

void ANodeStone::ExecuteRedirectorBehavior()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Node Stone %d executing Redirector behavior"), NodeID);
    
    // Redirectors change the flow of energy and create terrain channels
    // They create paths for synapse rivers
    Properties.EnergyDirection = GetActorForwardVector();
    Properties.GlowColor = FLinearColor::Green;
}

void ANodeStone::ExecuteHarmonizerBehavior()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Node Stone %d executing Harmonizer behavior"), NodeID);
    
    // Harmonizers balance the effects of multiple nearby stones
    // They create complex, balanced terrain patterns
    Properties.InfluenceRadius *= 1.2f;
    Properties.GlowColor = FLinearColor(1.0f, 0.5f, 1.0f, 1.0f); // Purple
}

void ANodeStone::ExecuteCatalystBehavior()
{
    UE_LOG(LogProjectVisible, Log, TEXT("Node Stone %d executing Catalyst behavior"), NodeID);
    
    // Catalysts accelerate all processes in their area
    // They speed up terrain changes and energy flow
    Properties.StabilizationPower *= 2.0f;
    Properties.GlowColor = FLinearColor::Red;
}

void ANodeStone::SetupMaterial()
{
    if (StoneMesh)
    {
        // Create dynamic material instance
        UMaterialInterface* BaseMaterial = StoneMesh->GetMaterial(0);
        if (BaseMaterial)
        {
            DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
            StoneMesh->SetMaterial(0, DynamicMaterial);
        }
        else
        {
            // Create a simple default material if none exists
            UE_LOG(LogProjectVisible, Warning, TEXT("No base material found for Node Stone %d"), NodeID);
        }
    }
}

void ANodeStone::SetupEffects()
{
    if (EffectComponent)
    {
        // TODO: Load appropriate Niagara system based on node stone type
        // For now, leave it empty - effects will be added when Niagara systems are created
        UE_LOG(LogProjectVisible, Log, TEXT("Setting up effects for Node Stone %d"), NodeID);
    }
}

void ANodeStone::UpdateMaterialParameters()
{
    if (DynamicMaterial)
    {
        DynamicMaterial->SetVectorParameterValue(TEXT("GlowColor"), Properties.GlowColor);
        DynamicMaterial->SetScalarParameterValue(TEXT("GlowIntensity"), Properties.bIsActive ? 1.0f : 0.1f);
        DynamicMaterial->SetScalarParameterValue(TEXT("Power"), Properties.StabilizationPower);
    }
}

void ANodeStone::CalculateTerrainEffect(const FVector& TargetLocation, float& OutHeightChange, float& OutStabilityChange) const
{
    if (!Properties.bIsActive)
    {
        OutHeightChange = 0.0f;
        OutStabilityChange = 0.0f;
        return;
    }
    
    float Distance = FVector::Dist(GetActorLocation(), TargetLocation);
    
    if (Distance > Properties.InfluenceRadius)
    {
        OutHeightChange = 0.0f;
        OutStabilityChange = 0.0f;
        return;
    }
    
    // Calculate falloff based on distance
    float InfluenceFactor = 1.0f - (Distance / Properties.InfluenceRadius);
    InfluenceFactor = FMath::Pow(InfluenceFactor, 2.0f); // Quadratic falloff
    
    // Calculate height change based on node stone type
    switch (Properties.Type)
    {
        case ENodeStoneType::Stabilizer:
            OutHeightChange = Properties.StabilizationPower * InfluenceFactor * 50.0f;
            break;
        case ENodeStoneType::Amplifier:
            OutHeightChange = Properties.StabilizationPower * InfluenceFactor * 100.0f;
            break;
        case ENodeStoneType::Redirector:
            {
                // Create directional height changes
                FVector DirectionToTarget = (TargetLocation - GetActorLocation()).GetSafeNormal();
                float DirectionalInfluence = FVector::DotProduct(DirectionToTarget, Properties.EnergyDirection);
                OutHeightChange = Properties.StabilizationPower * InfluenceFactor * DirectionalInfluence * 75.0f;
            }
            break;
        case ENodeStoneType::Harmonizer:
            OutHeightChange = Properties.StabilizationPower * InfluenceFactor * 30.0f; // Gentle changes
            break;
        case ENodeStoneType::Catalyst:
            OutHeightChange = Properties.StabilizationPower * InfluenceFactor * 150.0f; // Strong changes
            break;
    }
    
    // Clamp height change
    OutHeightChange = FMath::Clamp(OutHeightChange, -MAX_TERRAIN_HEIGHT_CHANGE, MAX_TERRAIN_HEIGHT_CHANGE);
    
    // Calculate stability change (always positive for stabilization)
    OutStabilityChange = Properties.StabilizationPower * InfluenceFactor;
}


