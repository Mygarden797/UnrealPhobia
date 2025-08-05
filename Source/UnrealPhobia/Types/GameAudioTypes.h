// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAudioTypes.generated.h"

/**
*       Name				        : GameAudioTypes
*       Description		        : Manage volume of whole games
*       LastUpate				: 2025/08/05
*       Todo                       :
*/
UENUM(BlueprintType)
enum class EAudioCategory : uint8
{
    Master      UMETA(DisplayName = "Master"),
    Music       UMETA(DisplayName = "Music"),
    SFX         UMETA(DisplayName = "SFX"),
    UI              UMETA(DisplayName = "UI")
};
