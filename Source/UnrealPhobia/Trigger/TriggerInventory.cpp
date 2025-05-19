// Fill out your copyright notice in the Description page of Project Settings.

#include "TriggerInventory.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UTriggerInventory::UTriggerInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	UE_LOG(LogTemp, Log, TEXT("UTriggerInventory Created"));
	Inventory.Init(ETriggerName::None, MaxInventorySize);

	if (SetCameraComponent())
	{
		UE_LOG(LogTemp, Display, TEXT("CameraComponent is set"));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Failed To Set CameraComponent"));
	}

	// ...
}

bool UTriggerInventory::SetCameraComponent()
{
	APawn *Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
	{
		UE_LOG(LogTemp, Display, TEXT("There Is No Owner"));
		return false;
	}
	// 카메라 컴포넌트가 있다면
	CamComp = Pawn->FindComponentByClass<UCameraComponent>();
	if (CamComp)
		return true;
	else
		return false;
}

// Called when the game starts
void UTriggerInventory::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UTriggerInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UTriggerInventory::Reach(FHitResult &OutHR) const
{
	FVector Start, End;
	Start = CamComp->GetComponentLocation();
	End = Start + CamComp->GetForwardVector() * MaxReachDistance;
	DrawDebugSphere(GetWorld(), End, ReachRadius, 10, FColor::Red, false, 3);
	FCollisionShape Sphere = FCollisionShape::MakeSphere(ReachRadius); // FCollisionShape : 충돌 디버깅에 사용되는 형상
	return GetWorld()->SweepSingleByChannel(						   // SweepSingleByChannel : 단일 충돌 검사를 수행하는 함수, 성공 여부를 Return
		OutHR,
		Start, End,
		FQuat::Identity,
		ECC_GameTraceChannel1,
		Sphere);
}
void UTriggerInventory::PickUp()
{
	FHitResult HR;
	if (Reach(HR))
	{
		if (CurrentInventorySize >= MaxInventorySize)
		{
			UE_LOG(LogTemp, Log, TEXT("Inventory is Full![%d/%d]"), Inventory.Num(), MaxInventorySize);
		}
		else
		{
			AActor *HitActor = HR.GetActor();
			if (!HitActor)
				return;
			ATrigger *HitTrigger = Cast<ATrigger>(HitActor);
			int32 ValidIndex = Inventory.Find(ETriggerName::None);
			if (ValidIndex != -1)
				Inventory[ValidIndex] = HitTrigger->TriggerName;
			UE_LOG(LogTemp, Log, TEXT("You got %s! [%d/%d]"), *HitActor->GetName(), ++CurrentInventorySize, MaxInventorySize);
			HitActor->Destroy();
		}
	}
}

void UTriggerInventory::DropOff()
{
	USkeletalMeshComponent *SkeletalMeshComp = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	if ((CurrentInventorySize == 0) || (Inventory[SelectedIndex] == ETriggerName::None))
	{
		UE_LOG(LogTemp, Log, TEXT("There Is No Trigger To Drop"));
	}
	else
	{
		FActorSpawnParameters Params;
		ATrigger *DroppedTrigger = GetWorld()->SpawnActor<ATrigger>(
			ATrigger::StaticClass(),
			SkeletalMeshComp->GetComponentLocation() + FVector(0, 0, 110),
			CamComp->GetComponentRotation(),
			Params);
		if (DroppedTrigger)
		{
			DroppedTrigger->SetTriggerName(Inventory[SelectedIndex]);
			DroppedTrigger->BaseMeshComponent->AddImpulse(DroppedTrigger->GetActorForwardVector() * 500, NAME_None, true);
			CurrentInventorySize--;
			Inventory[SelectedIndex] = ETriggerName::None;
			UE_LOG(LogTemp, Log, TEXT("Trigger Dropped"));
		}
	}
}

void UTriggerInventory::SelectSlot(int32 Index)
{
	if (Index <= MaxInventorySize)
	{
		SelectedIndex = Index - 1;
		UE_LOG(LogTemp, Log, TEXT("Slot Changed to %d"), SelectedIndex + 1);
	}
}