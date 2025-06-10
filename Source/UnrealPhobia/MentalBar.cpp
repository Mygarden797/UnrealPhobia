// Fill out your copyright notice in the Description page of Project Settings.

#include "MentalBar.h"
#include "Components/ProgressBar.h"

void UMentalBar::NativeConstruct()
{
    Super::NativeConstruct();
}

void UMentalBar::SetMentalPercent(float Percent)
{
    if (MentalBar)
    {
        MentalBar->SetPercent(Percent);
    }
}
