#include "CandleRoom.h"
#include "Components/BoxComponent.h"
#include "Survivor.h"

ACandleRoom::ACandleRoom()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ACandleRoom::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACandleRoom::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ACandleRoom::OnOverlapEnd);
}

void ACandleRoom::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
	ASurvivor* Survivor = Cast<ASurvivor>(OtherActor);
	if (Survivor)
	{
		Survivor->StartMentalRegen(RegenPerTick, RegenInterval, RegenTotalAmount);
	}
}

void ACandleRoom::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASurvivor* Survivor = Cast<ASurvivor>(OtherActor);
	if (Survivor)
	{
		Survivor->StopMentalRegen();
	}
}
