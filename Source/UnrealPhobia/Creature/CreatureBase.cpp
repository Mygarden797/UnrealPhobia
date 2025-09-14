// Fill out your copyright notice in the Description page of Project Settings.

#include "CreatureBase.h"
#include "Creature/CreatureController.h"
#include "Creature/Animation/CreatureAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SpotLightComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Network/Contents/ProtoPlayer.h"
#include "Network/NetworkBase.h"
#include "Network/Contents/NetworkPlayer.h"
#include "Sound/SoundBase.h"
#include "GameFramework/CharacterMovementComponent.h"

// 정적 델리게이트 정의
// FCreatureAttackCameraDelegate ACreatureBase::OnCreatureAttackCamera;

// Sets default values
ACreatureBase::ACreatureBase()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    // 폰의 이름으로 프리셋 설정
    GetCapsuleComponent()->SetCollisionProfileName("Pawn");
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    // 레벨에 직접 배치되거나 런타임에 스폰 시에 자동 소유
    State = ECreatureState::Idle;

    /*캐릭터 간의 회피 기능 설정*/
    GetCharacterMovement()->bUseRVOAvoidance = true;
    GetCharacterMovement()->AvoidanceConsiderationRadius = 300.f;
    GetCharacterMovement()->AvoidanceWeight = 0.5f;

    // 카메라 관련
    //  카메라 붐 컴포넌트 생성
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;

    // 크리쳐의 회전을 따라가도록 설정
    CameraBoom->bUsePawnControlRotation = true; // false에서 true로 변경
    CameraBoom->bInheritPitch = true;           // false에서 true로 변경
    CameraBoom->bInheritYaw = true;             // false에서 true로 변경
    CameraBoom->bInheritRoll = false;           // 롤은 그대로 false

    // 크리쳐 뒤쪽에 카메라 위치 설정 (Y축 회전 180도)
    CameraBoom->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));

    // AttackCamera 설정
    AttackCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("AttackCamera"));
    AttackCamera->SetupAttachment(CameraBoom);
    AttackCamera->bCaptureEveryFrame = false;
    AttackCamera->bCaptureOnMovement = false;

    // 렌더 타겟 생성 (블루프린트에서 설정하거나 코드에서 동적 생성)
    AttackCameraRenderTarget = nullptr;

    CurrentInfo = new Protocol::PosInfo();
    DestInfo = new Protocol::PosInfo();
}

ACreatureBase::~ACreatureBase()
{
    delete CurrentInfo;
    delete DestInfo;
    CurrentInfo = nullptr;
    DestInfo = nullptr;
}

// Called when the game starts or when spawned
void ACreatureBase::BeginPlay()
{
    Super::BeginPlay();
    SetState(State);

    //{
    //    FVector Location = GetActorLocation();
    //    CurrentInfo->set_x(Location.X);
    //    CurrentInfo->set_y(Location.Y);
    //    CurrentInfo->set_z(Location.Z);
    //    CurrentInfo->set_yaw(GetControlRotation().Yaw);
    //}
    FaceLight = FindComponentByClass<USpotLightComponent>();
    if (FaceLight)
    {
        FaceLight->SetVisibility(false);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed To Find Face Light Component"));
    }

    CameraComp = FindComponentByClass<UCameraComponent>();

    if (!CameraComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("CameraComp not found on %s"), *GetName());
    }
}

// Called every frame
void ACreatureBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    {
        FVector Location = GetActorLocation();
        CurrentInfo->set_x(Location.X);
        CurrentInfo->set_y(Location.Y);
        CurrentInfo->set_z(Location.Z);
        CurrentInfo->set_yaw(GetActorRotation().Yaw);
    }

    if (bHaveAIController == true) // AI컨트롤러 있으면 패킷 보내고 없으면 DestInfo로 이동
    {
        // Send 판정
        bool ForceSendPacket = false;

        MovePacketSendTimer -= DeltaTime;

        if (MovePacketSendTimer <= 0 || ForceSendPacket)
        {
            MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

            Protocol::C_CREATURE_BEHAVIOR MovePkt;

            // 현재 위치 정보
            {
                Protocol::PosInfo *Info = MovePkt.mutable_info();
                Info->CopyFrom(*CurrentInfo);
                Info->set_object_id(ICreatureObjectId);
                // 크리쳐 스테이트 지정.
                ECreatureState CreatureState = this->GetState();

                if (CreatureState == ECreatureState::Attack)
                {
                    Info->set_creature_state(Protocol::CreatureState::CREATURE_STATE_ATTACK);
                }
                else if (CreatureState == ECreatureState::Chase)
                {
                    Info->set_creature_state(Protocol::CreatureState::CREATURE_STATE_CHASE);
                    Info->set_speed(CreatureData->RunSpeed);
                }
                else
                {
                    Info->set_creature_state(Protocol::CreatureState::CREATURE_STATE_MOVE);
                    Info->set_speed(CreatureData->WalkSpeed);
                }
            }

            SEND_PACKET(MovePkt);
        }
    }
    else
    {
        {
            /*SetActorRotation(FRotator(0, DestInfo->yaw(), 0));
            AddMovementInput(GetActorForwardVector());*/

            FVector Location = GetActorLocation();
            FVector DestLocation = FVector(DestInfo->x(), DestInfo->y(), DestInfo->z());

            FVector MoveDir = (DestLocation - Location);
            const float DistToDest = MoveDir.Length();
            MoveDir.Normalize();

            SetActorRotation(FRotator(0, DestInfo->yaw(), 0));
            if (DestInfo->creature_state() == Protocol::CreatureState::CREATURE_STATE_ATTACK)
            {
                UCreatureAnimInstance *CreatureAnimInstance = Cast<UCreatureAnimInstance>(GetMesh()->GetAnimInstance());
                RETURN_IF_NULL(CreatureAnimInstance)
                CreatureAnimInstance->PlayAttackMontage();
            }
            else if (DistToDest >= 140.0f)
            {
                SetActorLocation(DestLocation);
            }
            else
            {
                float speed = DestInfo->speed();
                float MoveDist = (MoveDir * speed * DeltaTime).Length();
                MoveDist = FMath::Min(MoveDist, DistToDest);
                FVector NextLocation = Location + MoveDir * MoveDist;
                SetActorLocation(NextLocation);
            }
        }
    }

    if (!bIsFOVChanging || !CameraComp)
        return;

    FOVElapsed += DeltaTime;
    float Alpha = FMath::Clamp(FOVElapsed / FOVDuration, 0.f, 1.f);
    float SmoothAlpha = Alpha * Alpha * (3.f - 2.f * Alpha);

    float NewFOV = FMath::Lerp(FOVStart, FOVTarget, SmoothAlpha);
    CameraComp->SetFieldOfView(NewFOV);

    if (Alpha >= 1.f)
    {
        // 완료
        bIsFOVChanging = false;
        FOVElapsed = 0.f;
    }
}

// Called to bind functionality to input
void ACreatureBase::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// 새로운 상태로 설정하는 함수
void ACreatureBase::SetState(ECreatureState NewState)
{
    // UE_LOG(LogTemp, Warning, TEXT("SetState called with %d"), (uint8)NewState);
    // 새 상태와 예전 상태를 저장
    ECreatureState OldState = State;
    State = NewState;

    ACreatureController *CreatureController = Cast<ACreatureController>(GetController());
    // 블랙보드 함수 설정
    if (CreatureController && CreatureController->GetBlackboardComponent())
    {
        // UE_LOG(LogTemp, Warning, TEXT("Setting Blackboard State to %d"), uint8(State));
        CreatureController->GetBlackboardComponent()->SetValueAsEnum(ACreatureController::CreatureState, uint8(State));
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("CreatureController or BlackboardComponent is null!"));
    }
    // 델리게이트 함수 발동
    OnCreatureStateChanged.Broadcast(OldState, NewState);
}

void ACreatureBase::Attack()
{
    UCreatureAnimInstance *CreatureAnimInstance = Cast<UCreatureAnimInstance>(GetMesh()->GetAnimInstance());
    RETURN_IF_NULL(CreatureAnimInstance)
    // UE_LOG(LogTemp, Display, TEXT("ACreatureBase is Activate"));
    ACreatureController *CreatureController = Cast<ACreatureController>(GetController());
    RETURN_IF_NULL(CreatureController)

    UBlackboardComponent *Blackboard = CreatureController->GetBlackboardComponent();
    RETURN_IF_NULL(Blackboard)

    AttackTarget = Cast<AActor>(Blackboard->GetValueAsObject(ACreatureController::Target));
    RETURN_IF_NULL(AttackTarget)

    LocateTargetCamera();
    UGameplayStatics::ApplyDamage(AttackTarget, AttackDamage, CreatureController, this, UDamageType::StaticClass());
    CreatureAnimInstance->PlayAttackMontage();

    // 기준 위치와 앞쪽 방향 벡터 가져오기
    const FVector BaseLocation = this->GetActorLocation();
    const FVector ForwardVector = this->GetActorForwardVector();
    const FVector NewLocation = BaseLocation + ForwardVector * 150;
    AttackTarget->SetActorLocation(NewLocation);

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,
        this,
        &ACreatureBase::RevertTargetCamera, // 몽타주 끝났을 때 실행할 함수
        1.5f,                               // 카메라 전환 지속 시간(초)
        false);

    Protocol::C_CREATURE_ATTACK AttackPkt;

    Protocol::ObjectInfo *Info = AttackPkt.mutable_creature_info();
    Protocol::CreatureInfo *CreatureInfo = Info->mutable_creature_info();
    Info->set_object_id(ICreatureObjectId);
    CreatureInfo->set_target_id(Cast<AProtoPlayer>(AttackTarget)->MyPlayerId);

    SEND_PACKET(AttackPkt);
}

void ACreatureBase::LocateTargetCamera()
{
    if (!AttackTarget)
    {
        return;
    }

    // 플레이어 컨트롤러 가져오기
    APlayerController *PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("LocateTargetCamera: PlayerController not found"));
        return;
    }

    if (FaceLight)
    {
        FaceLight->SetVisibility(true);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("LocateTargetCamera: FaceLightComp not found on %s"), *GetName());
    }

    if (AttackSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            AttackSound,
            GetActorLocation());
    }

    // 카메라 컴포넌트를 가진 액터를 뷰 타겟으로 블렌딩
    AActor *CameraOwner = CameraComp->GetOwner();
    CameraComp->FieldOfView = 190;
    StartChangeFOV(80.0f, 0.2f);
    AttackTarget->DisableInput(PC);
    PC->SetViewTargetWithBlend(CameraOwner, 0.1f /*Blend Time*/,
                               EViewTargetBlendFunction::VTBlend_Linear,
                               0.1f /*BlendExp*/,
                               false /*bLockOutgoing*/);
}
void ACreatureBase::RevertTargetCamera()
{
    if (!AttackTarget)
    {
        return;
    }
    // 플레이어 컨트롤러 가져오기
    APlayerController *PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("LocateTargetCamera: PlayerController not found"));
        return;
    }

    if (FaceLight)
    {
        FaceLight->SetVisibility(false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("LocateTargetCamera: FaceLightComp not found on %s"), *GetName());
    }

    AttackTarget->EnableInput(PC);
    PC->SetViewTargetWithBlend(AttackTarget, 0.1f /*Blend Time*/,
                               EViewTargetBlendFunction::VTBlend_Linear,
                               0.0f /*BlendExp*/,
                               false /*bLockOutgoing*/);
}

void ACreatureBase::Communicate()
{
    UCreatureAnimInstance *CreatureAnimInstance = Cast<UCreatureAnimInstance>(GetMesh()->GetAnimInstance());
    RETURN_IF_NULL(CreatureAnimInstance)
    // UE_LOG(LogTemp, Display, TEXT("ACreatureBase is Activate"));
    ACreatureController *CreatureController = Cast<ACreatureController>(GetController());
    RETURN_IF_NULL(CreatureController)

    UBlackboardComponent *Blackboard = CreatureController->GetBlackboardComponent();
    RETURN_IF_NULL(Blackboard)

    CreatureAnimInstance->PlayCommunicateMontage();
}

/* void ACreatureBase::ActivateAttackCamera(AProtoPlayer *AttackedPlayer)
{
    if (AttackCamera && AttackCameraRenderTarget)
    {
        // 카메라 활성화
        AttackCamera->bCaptureEveryFrame = true;
        AttackCamera->CaptureScene();

        // 델리게이트 브로드캐스트 - 다른 플레이어에게 알림
        // OnCreatureAttackCamera.Broadcast(this, AttackCameraRenderTarget);

        // 타이머 설정 - 일정 시간 후 비활성화
        GetWorld()->GetTimerManager().SetTimer(
            AttackCameraTimerHandle,
            this,
            &ACreatureBase::OnAttackCameraTimerEnd,
            AttackCameraShowTime,
            false);
void ACreatureBase::ActivateAttackCamera(AProtoPlayer *AttackedPlayer)
        UE_LOG(LogTemp, Log, TEXT("Attack Camera Activated for %s"), *GetName());
    }
} */

void ACreatureBase::ActivateAttackCamera_packet()
{
    if (AttackCamera && AttackCameraRenderTarget)
    {
        // 카메라 활성화
        AttackCamera->bCaptureEveryFrame = true;
        AttackCamera->CaptureScene();

        // 타이머 설정 - 일정 시간 후 비활성화
        GetWorld()->GetTimerManager().SetTimer(
            AttackCameraTimerHandle,
            this,
            &ACreatureBase::OnAttackCameraTimerEnd,
            AttackCameraShowTime,
            false);
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

void ACreatureBase::MultiBehaveior(const Protocol::PosInfo &PosInfo)
{
    // AI 컨트롤러가 없을 때 작동
    if (this->AIControllerClass != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("AIControllerClass is not nullptr"));
        return;
    }

    SetDestInfo(PosInfo);
}

void ACreatureBase::SetCreatureState(Protocol::CreatureState CreatureState)
{
    if (CurrentInfo->creature_state() == CreatureState)
        return;

    CurrentInfo->set_creature_state(CreatureState);

    // TODO 각 애니메이션 지정
}

void ACreatureBase::SetObjectId(uint64 ObjectId)
{
    if (ICreatureObjectId != 0)
    {
        assert(ICreatureObjectId == ObjectId);
    }
    ICreatureObjectId = ObjectId;
}

void ACreatureBase::SetCurrentInfo(const Protocol::PosInfo &Info)
{
    if (CurrentInfo->object_id() != 0)
    {
        assert(PlayerInfo->object_id() == Info.object_id());
    }

    CurrentInfo->CopyFrom(Info);

    FVector Location(Info.x(), Info.y(), Info.z());
    SetActorLocation(Location);
}

void ACreatureBase::SetDestInfo(const Protocol::PosInfo &Info)
{
    if (CurrentInfo->object_id() != 0)
    {
        assert(PlayerInfo->object_id() == Info.object_id());
    }

    // Dest에 최종 상태 복사.
    DestInfo->CopyFrom(Info);

    // 크리쳐 상태 지정하는 코드
    SetCreatureState(Info.creature_state());
}

void ACreatureBase::OnAttackCameraTimerEnd()
{
    DeactivateAttackCamera();
}

void ACreatureBase::StartChangeFOV(float NewFOV, float Duration)
{
    if (!CameraComp)
        return;

    FOVStart = CameraComp->FieldOfView;
    FOVTarget = NewFOV;
    FOVDuration = FMath::Max(0.01f, Duration);
    FOVElapsed = 0.f;
    bIsFOVChanging = true;
}