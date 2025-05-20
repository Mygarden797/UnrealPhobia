
#include "ProtoPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MyProtoPlayer.h"


AProtoPlayer::AProtoPlayer()
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

    // Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
    // instead of recompiling to adjust them
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

    GetCharacterMovement()->bRunPhysicsWithNoController = true;

    PlayerInfo = new Protocol::PlayerInfo();
    DestInfo = new Protocol::PlayerInfo();
}

AProtoPlayer::~AProtoPlayer()
{
    delete PlayerInfo;
    delete DestInfo;
    PlayerInfo = nullptr;
    DestInfo = nullptr;
}

void AProtoPlayer::BeginPlay()
{
    Super::BeginPlay();

    {
        FVector Location = GetActorLocation();
        DestInfo->set_x(Location.X);
        DestInfo->set_y(Location.Y);
        DestInfo->set_z(Location.Z);
        DestInfo->set_yaw(GetControlRotation().Yaw);

        SetMoveState(Protocol::MOVE_STATE_IDLE);
    }
}

void AProtoPlayer::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    {
        FVector Location = GetActorLocation();
        PlayerInfo->set_x(Location.X);
        PlayerInfo->set_y(Location.Y);
        PlayerInfo->set_z(Location.Z);
        PlayerInfo->set_yaw(GetControlRotation().Yaw);
    }

    if (IsMyPlayer() == false)
    {
        /*FVector Location = GetActorLocation();
        FVector DestLocation = FVector(DestInfo->x(), DestInfo->y(), DestInfo->z());

        FVector MoveDir = (DestLocation - Location);
        const float DistToDest = MoveDir.Length();
        MoveDir.Normalize();

        float MoveDist = (MoveDir * 600.f * DeltaSeconds).Length();
        MoveDist = FMath::Min(MoveDist, DistToDest);
        FVector NextLocation = Location + MoveDir* MoveDist;

        SetActorLocation(NextLocation);*/
        const Protocol::MoveState State = PlayerInfo->state();

        if (State == Protocol::MOVE_STATE_RUN)
        {
            SetActorRotation(FRotator(0, DestInfo->yaw(), 0));
            AddMovementInput(GetActorForwardVector());
        }
        else
        {

        }
    }
}

bool AProtoPlayer::IsMyPlayer()
{
    return Cast<AMyProtoPlayer>(this) != nullptr;
}

void AProtoPlayer::SetMoveState(Protocol::MoveState State)
{
    if (PlayerInfo->state() == State)
        return;

    PlayerInfo->set_state(State);

    // TODO
}

void AProtoPlayer::SetPlayerInfo(const Protocol::PlayerInfo& Info)
{
    if (PlayerInfo->object_id() != 0)
    {
        assert(PlayerInfo->object_id() == Info.object_id());
    }

    PlayerInfo->CopyFrom(Info);

    FVector Location(Info.x(), Info.y(), Info.z());
    SetActorLocation(Location);
}

void AProtoPlayer::SetDestInfo(const Protocol::PlayerInfo& Info)
{
    if (PlayerInfo->object_id() != 0)
    {
        assert(PlayerInfo->object_id() == Info.object_id());
    }

    // Dest에 최종 상태 복사.
    DestInfo->CopyFrom(Info);

    // 상태만 바로 적용하자.
    SetMoveState(Info.state());
}

