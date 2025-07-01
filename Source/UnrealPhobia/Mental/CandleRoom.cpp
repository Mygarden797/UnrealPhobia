#include "CandleRoom.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NetworkPlayer.h"

ACandleRoom::ACandleRoom()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	//RootComponent = TriggerBox;
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// 루트 컴포넌트 생성 및 지정
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// CubeMesh 생성 및 루트에 붙임
	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	CubeMesh->SetupAttachment(RootComponent);

}

void ACandleRoom::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACandleRoom::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ACandleRoom::OnOverlapEnd);
}

void ACandleRoom::OnOverlapBegin(UPrimitiveComponent *OverlappedComp, AActor *OtherActor,
								 UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
								 const FHitResult &SweepResult)
{
    ANetworkPlayer* NetworkPlayer = Cast<ANetworkPlayer>(OtherActor);
    if (NetworkPlayer)
    {
        // 현재 트리거가 활성화 상태인지 확인 (Active 태그가 붙어 있는지)
        if (Tags.Contains(FName("Active")))
        {
			NetworkPlayer->CurrentTrigger = this;
            NetworkPlayer->StartMentalRegen(10.f);
        }
    }
}

void ACandleRoom::OnOverlapEnd(UPrimitiveComponent *OverlappedComp, AActor *OtherActor,
							   UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	ANetworkPlayer* NetworkPlayer = Cast<ANetworkPlayer>(OtherActor);
	if (NetworkPlayer && Tags.Contains(FName("Active")))
	{
		Tags.Remove(FName("Active"));
		NetworkPlayer->StopMentalRegen();
	}
}
