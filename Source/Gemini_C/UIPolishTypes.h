// Project Visible - UI/UX Polish Types
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/SoftObjectPtr.h"
#include "Components/Widget.h"
#include "ProjectVisibleUITypes.h"
#include "UIPolishTypes.generated.h"

/**
 * UI theme types for visual styling
 */
UENUM(BlueprintType)
enum class EUITheme : uint8
{
    Default             UMETA(DisplayName = "Default Theme"),         // デフォルトテーマ
    Dark                UMETA(DisplayName = "Dark Theme"),            // ダークテーマ
    Light               UMETA(DisplayName = "Light Theme"),           // ライトテーマ
    HighContrast        UMETA(DisplayName = "High Contrast Theme")    // 高コントラストテーマ
};

/**
 * Animation easing types for smooth UI transitions
 */
UENUM(BlueprintType)
enum class EUIEasingType : uint8
{
    Linear              UMETA(DisplayName = "Linear"),                 // 線形
    EaseIn              UMETA(DisplayName = "Ease In"),               // イーズイン
    EaseOut             UMETA(DisplayName = "Ease Out"),              // イーズアウト
    EaseInOut           UMETA(DisplayName = "Ease In Out"),           // イーズインアウト
    Bounce              UMETA(DisplayName = "Bounce"),                // バウンス
    Elastic             UMETA(DisplayName = "Elastic"),               // エラスティック
    Back                UMETA(DisplayName = "Back"),                  // バック
    Circular            UMETA(DisplayName = "Circular"),              // 円形
    Exponential         UMETA(DisplayName = "Exponential"),           // 指数関数
    Sine                UMETA(DisplayName = "Sine")                   // サイン波
};

/**
 * Visual effect types for UI elements
 */
UENUM(BlueprintType)
enum class EUIVisualEffect : uint8
{
    FadeIn              UMETA(DisplayName = "Fade In"),               // フェードイン
    FadeOut             UMETA(DisplayName = "Fade Out"),              // フェードアウト
    SlideIn             UMETA(DisplayName = "Slide In"),              // スライドイン
    SlideOut            UMETA(DisplayName = "Slide Out"),             // スライドアウト
    ScaleUp             UMETA(DisplayName = "Scale Up"),              // スケールアップ
    ScaleDown           UMETA(DisplayName = "Scale Down"),            // スケールダウン
    Rotate              UMETA(DisplayName = "Rotate"),                // 回転
    Flip                UMETA(DisplayName = "Flip"),                  // フリップ
    Shake               UMETA(DisplayName = "Shake"),                 // シェイク
    Pulse               UMETA(DisplayName = "Pulse"),                 // パルス
    Glow                UMETA(DisplayName = "Glow"),                  // グロー
    Blur                UMETA(DisplayName = "Blur"),                  // ブラー
    Ripple              UMETA(DisplayName = "Ripple"),                // リップル
    Morph               UMETA(DisplayName = "Morph"),                 // モーフ
    Particle            UMETA(DisplayName = "Particle"),              // パーティクル
    Distortion          UMETA(DisplayName = "Distortion")             // ディストーション
};

/**
 * UI responsiveness breakpoints
 */
UENUM(BlueprintType)
enum class EUIBreakpoint : uint8
{
    Mobile              UMETA(DisplayName = "Mobile"),                // モバイル (< 768px)
    Tablet              UMETA(DisplayName = "Tablet"),                // タブレット (768-1024px)
    Desktop             UMETA(DisplayName = "Desktop"),               // デスクトップ (1024-1920px)
    WidescreenDesktop   UMETA(DisplayName = "Widescreen Desktop"),    // ワイドスクリーンデスクトップ (> 1920px)
    UltraWide           UMETA(DisplayName = "Ultra Wide"),            // ウルトラワイド (> 2560px)
    Portrait            UMETA(DisplayName = "Portrait"),              // ポートレート
    Landscape           UMETA(DisplayName = "Landscape")              // ランドスケープ
};

/**
 * UI polish quality levels
 */
UENUM(BlueprintType)
enum class EUIPolishLevel : uint8
{
    Minimal             UMETA(DisplayName = "Minimal"),               // 最小限
    Basic               UMETA(DisplayName = "Basic"),                 // 基本
    Standard            UMETA(DisplayName = "Standard"),              // 標準
    Enhanced            UMETA(DisplayName = "Enhanced"),              // 強化
    Premium             UMETA(DisplayName = "Premium"),               // プレミアム
    Cinematic           UMETA(DisplayName = "Cinematic")              // シネマティック
};

/**
 * Haptic feedback types for enhanced user experience
 */
UENUM(BlueprintType)
enum class EUIHapticType : uint8
{
    None                UMETA(DisplayName = "None"),                  // なし
    Light               UMETA(DisplayName = "Light"),                 // 軽い
    Medium              UMETA(DisplayName = "Medium"),                // 中程度
    Heavy               UMETA(DisplayName = "Heavy"),                 // 強い
    Selection           UMETA(DisplayName = "Selection"),             // 選択
    Impact              UMETA(DisplayName = "Impact"),                // インパクト
    Notification        UMETA(DisplayName = "Notification"),          // 通知
    Warning             UMETA(DisplayName = "Warning"),               // 警告
    Success             UMETA(DisplayName = "Success"),               // 成功
    Error               UMETA(DisplayName = "Error")                  // エラー
};

/**
 * Interactive UI state types
 */
UENUM(BlueprintType)
enum class EUIInteractionState : uint8
{
    Normal              UMETA(DisplayName = "Normal"),                // 通常
    Hovered             UMETA(DisplayName = "Hovered"),               // ホバー
    Pressed             UMETA(DisplayName = "Pressed"),               // プレス
    Selected            UMETA(DisplayName = "Selected"),              // 選択
    Disabled            UMETA(DisplayName = "Disabled"),              // 無効
    Focused             UMETA(DisplayName = "Focused"),               // フォーカス
    Active              UMETA(DisplayName = "Active"),                // アクティブ
    Loading             UMETA(DisplayName = "Loading"),               // 読み込み中
    Error               UMETA(DisplayName = "Error"),                 // エラー
    Success             UMETA(DisplayName = "Success")                // 成功
};

/**
 * UI animation curve definition
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FUIAnimationCurve
{
    GENERATED_BODY()

    FUIAnimationCurve()
    {
        Duration = 0.3f;
        Delay = 0.0f;
        EasingType = EUIEasingType::EaseInOut;
        StartValue = 0.0f;
        EndValue = 1.0f;
        bLoop = false;
        bPingPong = false;
        LoopCount = 1;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float Delay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    EUIEasingType EasingType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float StartValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float EndValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    bool bLoop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    bool bPingPong;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    int32 LoopCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    TArray<FVector2D> CustomCurvePoints;
};

/**
 * Visual effect configuration
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FUIVisualEffectConfig
{
    GENERATED_BODY()

    FUIVisualEffectConfig()
    {
        EffectType = EUIVisualEffect::FadeIn;
        Duration = 0.5f;
        Intensity = 1.0f;
        bEnabled = true;
        bAutoPlay = false;
        Color = FLinearColor::White;
        Scale = FVector2D(1.0f, 1.0f);
        Rotation = 0.0f;
        BlurRadius = 0.0f;
        GlowIntensity = 0.0f;
        ParticleCount = 50;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    EUIVisualEffect EffectType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    float Intensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    bool bEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    bool bAutoPlay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    FVector2D Scale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    float Rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    float BlurRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    float GlowIntensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    int32 ParticleCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    FUIAnimationCurve AnimationCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effect")
    TMap<FString, float> CustomParameters;
};

/**
 * Responsive layout configuration
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FResponsiveLayoutConfig
{
    GENERATED_BODY()

    FResponsiveLayoutConfig()
    {
        Breakpoint = EUIBreakpoint::Desktop;
        WidgetSize = FVector2D(1920.0f, 1080.0f);
        WidgetPosition = FVector2D(0.0f, 0.0f);
        WidgetAnchor = FVector2D(0.5f, 0.5f);
        WidgetAlignment = FVector2D(0.5f, 0.5f);
        Margin = FMargin(0.0f);
        Padding = FMargin(10.0f);
        Scale = 1.0f;
        Opacity = 1.0f;
        bVisible = true;
        bAutoScale = true;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Responsive Layout")
    EUIBreakpoint Breakpoint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Responsive Layout")
    FVector2D WidgetSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Responsive Layout")
    FVector2D WidgetPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Responsive Layout")
    FVector2D WidgetAnchor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Responsive Layout")
    FVector2D WidgetAlignment;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Responsive Layout")
    FMargin Margin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Responsive Layout")
    FMargin Padding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Responsive Layout")
    float Scale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Responsive Layout")
    float Opacity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Responsive Layout")
    bool bVisible;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Responsive Layout")
    bool bAutoScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Responsive Layout")
    TMap<FString, FString> CustomProperties;
};

/**
 * Interactive state configuration
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FUIInteractionConfig
{
    GENERATED_BODY()

    FUIInteractionConfig()
    {
        State = EUIInteractionState::Normal;
        TransitionDuration = 0.2f;
        HoverScale = 1.05f;
        PressedScale = 0.95f;
        HapticType = EUIHapticType::Light;
        bPlaySound = true;
        bShowTooltip = true;
        TooltipDelay = 0.5f;
        Color = FLinearColor::White;
        BackgroundOpacity = 1.0f;
        BorderThickness = 1.0f;
        ShadowOffset = FVector2D(2.0f, 2.0f);
        ShadowOpacity = 0.3f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    EUIInteractionState State;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float TransitionDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float HoverScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float PressedScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    EUIHapticType HapticType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    bool bPlaySound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    bool bShowTooltip;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float TooltipDelay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float BackgroundOpacity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float BorderThickness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    FVector2D ShadowOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float ShadowOpacity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    FString SoundEffectID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    FString TooltipText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    TMap<FString, float> StateParameters;
};

/**
 * Comprehensive UI polish configuration
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FUIPolishConfig
{
    GENERATED_BODY()

    FUIPolishConfig()
    {
        PolishLevel = EUIPolishLevel::Standard;
        GlobalAnimationSpeed = 1.0f;
        bEnableParallax = true;
        bEnableParticles = true;
        bEnableHapticFeedback = true;
        bEnableVFX = true;
        bEnableCustomShaders = true;
        bReducedMotion = false;
        bHighContrast = false;
        bSimplifiedUI = false;
        GlobalScale = 1.0f;
        ContrastBoost = 1.0f;
        SaturationAdjust = 1.0f;
        BrightnessAdjust = 1.0f;
        BlurStrength = 0.0f;
        VignetteStrength = 0.0f;
        ChromaticAberration = 0.0f;
        FilmGrain = 0.0f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    EUIPolishLevel PolishLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    float GlobalAnimationSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    bool bEnableParallax;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    bool bEnableParticles;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    bool bEnableHapticFeedback;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    bool bEnableVFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    bool bEnableCustomShaders;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    bool bReducedMotion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    bool bHighContrast;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    bool bSimplifiedUI;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    float GlobalScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    float ContrastBoost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    float SaturationAdjust;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    float BrightnessAdjust;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    float BlurStrength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    float VignetteStrength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    float ChromaticAberration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    float FilmGrain;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    TArray<FResponsiveLayoutConfig> ResponsiveLayouts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    TArray<FUIVisualEffectConfig> VisualEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    TMap<EUIInteractionState, FUIInteractionConfig> InteractionStates;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    FAccessibilityConfig AccessibilitySettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    TMap<FString, FString> LocalizedTexts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Polish")
    TMap<FString, float> CustomParameters;
};

/**
 * UI polish analytics data
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FUIPolishAnalytics
{
    GENERATED_BODY()

    FUIPolishAnalytics()
    {
        SessionID = TEXT("");
        InteractionCount = 0;
        AnimationPlayCount = 0;
        EffectTriggerCount = 0;
        ResponsiveChanges = 0;
        AccessibilityUsage = 0;
        AverageResponseTime = 0.0f;
        UserSatisfactionScore = 0.0f;
        PolishEffectivenessScore = 0.0f;
        UsabilityScore = 0.0f;
        PreferredPolishLevel = EUIPolishLevel::Standard;
        MostUsedEffect = EUIVisualEffect::FadeIn;
        SessionStartTime = FDateTime::Now();
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    FString SessionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    FDateTime SessionStartTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    int32 InteractionCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    int32 AnimationPlayCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    int32 EffectTriggerCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    int32 ResponsiveChanges;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    int32 AccessibilityUsage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    float AverageResponseTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    float UserSatisfactionScore;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    float PolishEffectivenessScore;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    float UsabilityScore;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    EUIPolishLevel PreferredPolishLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    EUIVisualEffect MostUsedEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    TMap<FString, int32> EffectUsageCounts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    TMap<FString, float> InteractionTimes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    TArray<FString> UserFeedback;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Analytics")
    TMap<FString, float> CustomMetrics;
};
