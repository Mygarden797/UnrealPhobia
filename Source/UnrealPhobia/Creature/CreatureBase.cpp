// Fill out your copyright notice in the Description page of Project Settings.


#include "CreatureBase.h"
#include "Creature/CreatureController.h"
#include "Creature/Animation/CreatureAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"

#include "GameFramework/CharacterMovementComponent.h"

// 정적 델리게이트 정의
FCreatureAttackCameraDelegate ACreatureBase::OnCreatureAttackCamera;

// Sets default values
ACreatureBase::ACreatureBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//폰의 이름으로 프리셋 설정
	GetCapsuleComponent()-> SetCollisionProfileName("Pawn");
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	//레벨에 직접 배치되거나 런타임에 스폰 시에 자동 소유
	State = ECreatureState::Idle;

	    /*캐릭터 간의 회피 기능 설정*/
    GetCharacterMovement()->bUseRVOAvoidance = true;    
    GetCharacterMovement()->AvoidanceConsiderationRadius = 300.f;
    GetCharacterMovement()->AvoidanceWeight = 0.5f;


//카메라 관련
    // 카메라 붐 컴포넌트 생성
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f; // 카메라와 캐릭터 사이의 거리
    CameraBoom->bUsePawnControlRotation = false; // 고정 카메라로 설정
    CameraBoom->bInheritPitch = false;
    CameraBoom->bInheritYaw = false;
    CameraBoom->bInheritRoll = false;
    // 크리쳐 뒤쪽에 카메라 위치 설정 (Y축 회전 180도)
    CameraBoom->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));

    // 기존 코드 유지
    AttackCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("AttackCamera"));
    AttackCamera->SetupAttachment(CameraBoom);
    AttackCamera->bCaptureEveryFrame = false; // 필요할 때만 캡처
    AttackCamera->bCaptureOnMovement = false;
    AttackCamera->bCaptureEveryFrame = false; // 필요할 때만 캡처
    AttackCamera->bCaptureOnMovement = false;

    // 렌더 타겟 생성 (블루프린트에서 설정하거나 코드에서 동적 생성)
    AttackCameraRenderTarget = nullptr;
}



// Called when the game starts or when spawned
void ACreatureBase::BeginPlay()
{
	Super::BeginPlay();
	SetState(State);
	
}



// Called every frame
void ACreatureBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACreatureBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//새로운 상태로 설정하는 함수
void ACreatureBase::SetState(ECreatureState NewState)
{
	// UE_LOG(LogTemp, Warning, TEXT("SetState called with %d"), (uint8)NewState);
	//새 상태와 예전 상태를 저장
	ECreatureState OldState = State;
	State = NewState;

	ACreatureController* CreatureController = Cast<ACreatureController>(GetController());
	//블랙보드 함수 설정
	if (CreatureController && CreatureController->GetBlackboardComponent())
	{
		// UE_LOG(LogTemp, Warning, TEXT("Setting Blackboard State to %d"), uint8(State));
		CreatureController->GetBlackboardComponent()->SetValueAsEnum(ACreatureController::CreatureState, uint8(State));
	}
	else
	{
		// UE_LOG(LogTemp, Error, TEXT("CreatureController or BlackboardComponent is null!"));
	}
	//델리게이트 함수 발동
	OnCreatureStateChanged.Broadcast(OldState, NewState);

}

void ACreatureBase::Attack()
{
	UCreatureAnimInstance* CreatureAnimInstance = Cast<UCreatureAnimInstance>(GetMesh()->GetAnimInstance());
	RETURN_IF_NULL(CreatureAnimInstance)
	// UE_LOG(LogTemp, Display, TEXT("ACreatureBase is Activate"));
	ACreatureController* CreatureController = Cast<ACreatureController>(GetController());
	RETURN_IF_NULL(CreatureController)

	UBlackboardComponent* Blackboard = CreatureController->GetBlackboardComponent();
	RETURN_IF_NULL(Blackboard)

	AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(ACreatureController::Target));
	RETURN_IF_NULL(Target)

	UGameplayStatics::ApplyDamage(Target,AttackDamage,CreatureController,this,UDamageType::StaticClass());
	CreatureAnimInstance->PlayAttackMontage();

//카메라 관련
    // 공격 시 카메라 활성화
    //ActivateAttackCamera();
}

void ACreatureBase::Communicate()
{
	UCreatureAnimInstance* CreatureAnimInstance = Cast<UCreatureAnimInstance>(GetMesh()->GetAnimInstance());
	RETURN_IF_NULL(CreatureAnimInstance)
	// UE_LOG(LogTemp, Display, TEXT("ACreatureBase is Activate"));
	ACreatureController* CreatureController = Cast<ACreatureController>(GetController());
	RETURN_IF_NULL(CreatureController)

	UBlackboardComponent* Blackboard = CreatureController->GetBlackboardComponent();
	RETURN_IF_NULL(Blackboard)


	CreatureAnimInstance->PlayCommunicateMontage();


}



void ACreatureBase::ActivateAttackCamera()
{
    if (AttackCamera && AttackCameraRenderTarget)
    {
        // 카메라 활성화
        AttackCamera->bCaptureEveryFrame = true;
        AttackCamera->CaptureScene();

        // 델리게이트 브로드캐스트 - 다른 플레이어에게 알림
        OnCreatureAttackCamera.Broadcast(this, AttackCameraRenderTarget);

        // 타이머 설정 - 일정 시간 후 비활성화
        GetWorld()->GetTimerManager().SetTimer(
            AttackCameraTimerHandle,
            this,
            &ACreatureBase::OnAttackCameraTimerEnd,
            AttackCameraShowTime,
            false
        );

        UE_LOG(LogTemp, Log, TEXT("Attack Camera Activated for %s"), *GetName());
    }
}

void ACreatureBase::DeactivateAttackCamera()
{
    if (AttackCamera)
    {
        AttackCamera->bCaptureEveryFrame = false;
        UE_LOG(LogTemp, Log, TEXT("Attack Camera Deactivated for %s"), *GetName());
    }

    // 타이머 클리어
    if (GetWorld()->GetTimerManager().IsTimerActive(AttackCameraTimerHandle))
    {
        GetWorld()->GetTimerManager().ClearTimer(AttackCameraTimerHandle);
    }
}

void ACreatureBase::OnAttackCameraTimerEnd()
{
    DeactivateAttackCamera();
}