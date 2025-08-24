// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class ECreatureState : uint8
{
	Idle	UMETA(DisplayName = "Idle"),
	Patrol	UMETA(DisplayName = "Patrol"),
	Chase	UMETA(DisplayName = "Chase"),
	Investigate UMETA(DisplayName = "Investigate"),
	Attack	UMETA(DisplayName = "Attack"),
	Communicate UMETA(DisplayName = "Communicate"),
	Check UMETA(DisplayName = "Check"),


};