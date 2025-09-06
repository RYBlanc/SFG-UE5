// Project Visible - CommonUI Integration Types
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonUserWidget.h"
#include "ProjectVisibleUITypes.generated.h"

/**
 * UI screen types for Project Visible
 */
UENUM(BlueprintType)
enum class EProjectVisibleScreenType : uint8
{
    MainMenu        UMETA(DisplayName = "Main Menu"),           // メインメニュー
    RealityMode     UMETA(DisplayName = "Reality Mode"),       // 現実モード
    DreamMode       UMETA(DisplayName = "Dream Mode"),         // 夢モード
    Investigation   UMETA(DisplayName = "Investigation"),      // 推理画面
    VirtueProfile   UMETA(DisplayName = "Virtue Profile"),     // 徳プロファイル
    MemoryViewer    UMETA(DisplayName = "Memory Viewer"),      // 記憶ビューア
    Settings        UMETA(DisplayName = "Settings"),           // 設定
    BoundaryEffect  UMETA(DisplayName = "Boundary Effect"),    // 境界崩壊エフェクト
    Pause           UMETA(DisplayName = "Pause"),              // ポーズ
    GameOver        UMETA(DisplayName = "Game Over"),          // ゲームオーバー
    Loading         UMETA(DisplayName = "Loading")             // ローディング
};

/**
 * UI animation types
 */
UENUM(BlueprintType)
enum class EUIAnimationType : uint8
{
    FadeIn          UMETA(DisplayName = "Fade In"),            // フェードイン
    FadeOut         UMETA(DisplayName = "Fade Out"),           // フェードアウト
    SlideIn         UMETA(DisplayName = "Slide In"),           // スライドイン
    SlideOut        UMETA(DisplayName = "Slide Out"),          // スライドアウト
    ScaleIn         UMETA(DisplayName = "Scale In"),           // スケールイン
    ScaleOut        UMETA(DisplayName = "Scale Out"),          // スケールアウト
    Dissolve        UMETA(DisplayName = "Dissolve"),           // 溶解
    Glitch          UMETA(DisplayName = "Glitch"),             // グリッチ
    Quantum         UMETA(DisplayName = "Quantum"),            // 量子効果
    MemoryFlash     UMETA(DisplayName = "Memory Flash")        // 記憶フラッシュ
};

/**
 * Input handling priority
 */
UENUM(BlueprintType)
enum class EUIInputPriority : uint8
{
    Critical        UMETA(DisplayName = "Critical"),           // 最優先（ポーズ等）
    High            UMETA(DisplayName = "High"),               // 高（モーダル）
    Normal          UMETA(DisplayName = "Normal"),             // 通常（ゲームUI）
    Low             UMETA(DisplayName = "Low"),                // 低（背景UI）
    Background      UMETA(DisplayName = "Background")          // 背景（装飾のみ）
};

/**
 * Accessibility features
 */
UENUM(BlueprintType)
enum class EAccessibilityFeature : uint8
{
    HighContrast    UMETA(DisplayName = "High Contrast"),      // ハイコントラスト
    LargeText       UMETA(DisplayName = "Large Text"),         // 大きいテキスト
    ScreenReader    UMETA(DisplayName = "Screen Reader"),      // スクリーンリーダー
    ColorBlind      UMETA(DisplayName = "Color Blind"),        // 色覚異常対応
    MotorImpaired   UMETA(DisplayName = "Motor Impaired"),     // 運動機能支援
    HearingImpaired UMETA(DisplayName = "Hearing Impaired"),   // 聴覚障害対応
    CognitiveSupport UMETA(DisplayName = "Cognitive Support")  // 認知支援
};

/**
 * UI theme configuration
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FProjectVisibleUITheme
{
    GENERATED_BODY()

    FProjectVisibleUITheme()
    {
        ThemeName = TEXT("Default");
        PrimaryColor = FLinearColor(0.1f, 0.3f, 0.8f, 1.0f);  // 青
        SecondaryColor = FLinearColor(0.8f, 0.3f, 0.1f, 1.0f); // オレンジ
        AccentColor = FLinearColor(0.9f, 0.9f, 0.1f, 1.0f);    // 黄
        BackgroundColor = FLinearColor(0.05f, 0.05f, 0.1f, 0.9f); // 濃紺
        TextColor = FLinearColor(0.95f, 0.95f, 1.0f, 1.0f);    // 白
        DisabledColor = FLinearColor(0.5f, 0.5f, 0.5f, 0.6f);  // グレー
        WarningColor = FLinearColor(1.0f, 0.7f, 0.0f, 1.0f);   // オレンジ
        ErrorColor = FLinearColor(1.0f, 0.2f, 0.2f, 1.0f);     // 赤
        SuccessColor = FLinearColor(0.2f, 0.8f, 0.2f, 1.0f);   // 緑
        FontScale = 1.0f;
        AnimationSpeed = 1.0f;
        bHighContrast = false;
        bReduceMotion = false;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Theme")
    FString ThemeName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor PrimaryColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor SecondaryColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor AccentColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor BackgroundColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor TextColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor DisabledColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor WarningColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor ErrorColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor SuccessColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Typography")
    float FontScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float AnimationSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    bool bHighContrast;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    bool bReduceMotion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    TMap<FString, FLinearColor> CustomColors;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
    TMap<FString, TSoftObjectPtr<UTexture2D>> ThemeTextures;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
    TMap<FString, TSoftObjectPtr<UMaterialInterface>> ThemeMaterials;
};

/**
 * UI layout configuration
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FUILayoutConfig
{
    GENERATED_BODY()

    FUILayoutConfig()
    {
        SafeZoneScale = 0.9f;
        DefaultMargin = FMargin(10.0f);
        DefaultPadding = FMargin(5.0f);
        bUseResponsiveLayout = true;
        MinScreenWidth = 1280.0f;
        MinScreenHeight = 720.0f;
        MaxScreenWidth = 3840.0f;
        MaxScreenHeight = 2160.0f;
        AspectRatioTolerance = 0.1f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    float SafeZoneScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    FMargin DefaultMargin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    FMargin DefaultPadding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    bool bUseResponsiveLayout;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen")
    float MinScreenWidth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen")
    float MinScreenHeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen")
    float MaxScreenWidth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen")
    float MaxScreenHeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen")
    float AspectRatioTolerance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    TMap<FString, FVector2D> BreakpointSizes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    TMap<FString, float> ResponsiveScales;
};

/**
 * Animation configuration
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FUIAnimationConfig
{
    GENERATED_BODY()

    FUIAnimationConfig()
    {
        AnimationType = EUIAnimationType::FadeIn;
        Duration = 0.3f;
        Delay = 0.0f;
        bUseEasing = true;
        EasingType = TEXT("EaseInOut");
        EasingExponent = 2.0f;
        StartValue = 0.0f;
        EndValue = 1.0f;
        bReverseOnExit = true;
        bLooping = false;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    EUIAnimationType AnimationType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float Delay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    bool bUseEasing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    FString EasingType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float EasingExponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float StartValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float EndValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    bool bReverseOnExit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    bool bLooping;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    FVector2D StartPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    FVector2D EndPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    FVector2D StartScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    FVector2D EndScale;
};

/**
 * Accessibility configuration
 */
USTRUCT(BlueprintType)
struct GEMINI_C_API FAccessibilityConfig
{
    GENERATED_BODY()

    FAccessibilityConfig()
    {
        bEnableAccessibility = true;
        bScreenReaderSupport = false;
        bHighContrastMode = false;
        bLargeTextMode = false;
        bColorBlindFriendly = false;
        bReduceMotion = false;
        bSimplifiedUI = false;
        TextSizeMultiplier = 1.0f;
        ContrastBoost = 1.0f;
        AnimationSpeedMultiplier = 1.0f;
        InputDelayTolerance = 0.1f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    bool bEnableAccessibility;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    bool bScreenReaderSupport;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    bool bHighContrastMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    bool bLargeTextMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    bool bColorBlindFriendly;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    bool bReduceMotion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    bool bSimplifiedUI;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    float TextSizeMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    float ContrastBoost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    float AnimationSpeedMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    float InputDelayTolerance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    TArray<EAccessibilityFeature> EnabledFeatures;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    TMap<FString, FString> ScreenReaderTexts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessibility")
    TMap<FString, FString> AlternativeInputMethods;
};
