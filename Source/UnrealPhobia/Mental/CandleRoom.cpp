#include "CandleRoom.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
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

	// 추가: 시각화용 메시 생성
	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(RootComponent);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VisualMesh->SetVisibility(false); // 기본 숨김
	VisualMesh->SetRenderCustomDepth(true); // 커스텀 뎁스 아웃라인 효과용
	VisualMesh->CustomDepthStencilValue = 1; // 스텐실 값 → PostProcess에서 설정 필요
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
    ASurvivor* Survivor = Cast<ASurvivor>(OtherActor);
    if (Survivor)
    {
        // 현재 트리거가 활성화 상태인지 확인 (Active 태그가 붙어 있는지)
        if (Tags.Contains(FName("Active")))
        {
            Survivor->StartMentalRegen(10.f);
        }
    }
}

void ACandleRoom::OnOverlapEnd(UPrimitiveComponent *OverlappedComp, AActor *OtherActor,
							   UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	ASurvivor* Survivor = Cast<ASurvivor>(OtherActor);
	if (Survivor && Tags.Contains(FName("Active")))
	{
		Tags.Remove(FName("Active"));
		Survivor->StopMentalRegen();
	}
}

// void ACandleRoom::SetActive(bool bActive)
// {
// 	if (bActive)
// 	{
// 		Tags.AddUnique(FName("Active"));
// 		VisualMesh->SetVisibility(true);
// 	}
// 	else
// 	{
// 		Tags.Remove(FName("Active"));
// 		VisualMesh->SetVisibility(false);
// 	}
// }
