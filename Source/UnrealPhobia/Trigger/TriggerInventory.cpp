// Fill out your copyright notice in the Description page of Project Settings.

#include "TriggerInventory.h"
#include "Engine/World.h"

UTriggerInventory::UTriggerInventory()
{
	PrimaryComponentTick.bCanEverTick = true;
	UE_LOG(LogTemp, Log, TEXT("UTriggerInventory Created"));
	Inventory.Init(ETriggerName::None, MaxInventorySize);
    TriggerIDs.Init(0, MaxInventorySize); 

	if (SetCameraComponent())
	{
		UE_LOG(LogTemp, Display, TEXT("CameraComponent is set"));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Failed To Set CameraComponent"));
	}
}

// 트리거 줍기/버리기 동작을 수행하기 위해 카메라 컴포넌트를 설정
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

void UTriggerInventory::BeginPlay()
{
	Super::BeginPlay();
}

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
	// DrawDebugSphere(GetWorld(), End, ReachRadius, 10, FColor::Red, false, 3);
	FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(ReachRadius); // 충돌에 사용되는 구체
	return GetWorld()->SweepSingleByChannel(									// SweepSingleByChannel : 단일 충돌 검사를 수행하는 함수, 성공 여부를 Return
		OutHR,
		Start, End,
		FQuat::Identity,
		ECC_GameTraceChannel1,
		CollisionSphere);
}
// 인벤토리 사이즈를 확인하여 Reach로 받은 Trigger를 인벤토리에 추가
void UTriggerInventory::PickUp()
{
	// Reach의 결과를 저장
	FHitResult HR;
	if (Reach(HR))
	{
		AActor *HitActor = HR.GetActor();
		// Pick up Trigger
		if (HitActor && HitActor->IsA(ATrigger::StaticClass()))
		{
			if (CurrentInventorySize >= MaxInventorySize)
			{
				UE_LOG(LogTemp, Log, TEXT("Inventory is Full![%d/%d]"), Inventory.Num(), MaxInventorySize);
				return;
			}
			else
			{
				if (!HitActor)
					return;

				ATrigger *HitTrigger = Cast<ATrigger>(HitActor);
				int32 ValidIndex = Inventory.Find(ETriggerName::None);

                if (ValidIndex != -1) // None이 있는지 확인
                {
                    Inventory[ValidIndex] = HitTrigger->TriggerName;
                    TriggerIDs[ValidIndex] = HitTrigger->TriggerInfo->object_id(); // 임시로 트리거 ID 저장
                }
					

				UE_LOG(LogTemp, Log, TEXT("You got %s! [%d/%d]"), *HitActor->GetName(), ++CurrentInventorySize, MaxInventorySize);
				if (HitTrigger->TriggerSpawnPoint)
					HitTrigger->TriggerSpawnPoint->bCanSpawn = true;

				HitActor->Destroy();
			}
		}
		// Activate Trigger
		else if (Inventory[SelectedIndex] != ETriggerName::None && HitActor->IsA(ATriggerMirror::StaticClass()))
		{
			ATriggerMirror *HitMirror = Cast<ATriggerMirror>(HitActor);
			if (HitMirror->ActivateTrigger(Inventory[SelectedIndex]))
			{
				Inventory[SelectedIndex] = ETriggerName::None;
				CurrentInventorySize--;
			}
		}
        else if (Inventory[SelectedIndex] != ETriggerName::None && HitActor->IsA(ANetworkMirror::StaticClass()))
        {
            ANetworkMirror* HitMirror = Cast<ANetworkMirror>(HitActor);
            if (HitMirror->ActivateTrigger(TriggerIDs[SelectedIndex]))
            {
                Inventory[SelectedIndex] = ETriggerName::None;
                CurrentInventorySize--;
            }
        }
	}
}
// 인벤토리 사이즈를 확인하여 Reach로 받은 Trigger를 인벤토리에 추가
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