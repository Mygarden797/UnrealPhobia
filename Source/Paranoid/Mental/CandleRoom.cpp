#include "Mental/CandleRoom.h"
#include "Components/BoxComponent.h"
#include "Network/Contents/NetworkPlayer.h"

ACandleRoom::ACandleRoom()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
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

void ACandleRoom::OnOverlapBegin(UPrimitiveComponent *OverlappedComp, AActor *OtherActor,
								 UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
								 const FHitResult &SweepResult)
{
    ANetworkPlayer* NetworkPlayer = Cast<ANetworkPlayer>(OtherActor);
    if (NetworkPlayer)
    {
        NetworkPlayer->TouchingCandleRoom = this;
        if (Tags.Contains(FName("Active")))
        {
            NetworkPlayer->StartMentalRegen(10.f);
        }
    }
}

void ACandleRoom::OnOverlapEnd(UPrimitiveComponent *OverlappedComp, AActor *OtherActor,
							   UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	ANetworkPlayer* NetworkPlayer = Cast<ANetworkPlayer>(OtherActor);
    if(NetworkPlayer){
        if (Tags.Contains(FName("Active")))
        {
            NetworkPlayer->StopMentalRegen();
        }
        NetworkPlayer->TouchingCandleRoom = nullptr;
        NetworkPlayer->HideForceActivateProgress();
    }
}

void ACandleRoom::TurnOnEffects()
{
    for (const auto& Candle : AssociatedCandles)
    {
        if (Candle)
        {
            Candle->SetFlameActive(true);
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("'%s' effects turned ON."), *GetName());
}

void ACandleRoom::TurnOffEffects()
{
    for (const auto& Candle : AssociatedCandles)
    {
        if (Candle)
        {
            Candle->SetFlameActive(false);
        }
    }
    UE_LOG(LogTemp, Log, TEXT("'%s' effects turned OFF."), *GetName());
}
