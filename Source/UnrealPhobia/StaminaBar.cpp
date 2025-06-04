// Fill out your copyright notice in the Description page of Project Settings.


#include "StaminaBar.h"
#include "Components/ProgressBar.h"

void UStaminaBar::NativeConstruct()
{
	Super::NativeConstruct();
}

void UStaminaBar::SetStaminaPercent(float Percent)
{
	if (StaminaBar)
	{
		StaminaBar->SetPercent(Percent);
	}
}
