// Fill out your copyright notice in the Description page of Project Settings.


#include "CreatureController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Creature/PatrolManager.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Survivor.h"
#include "Network/Contents/ProtoPlayer.h"
#include "Network/Contents/NetworkPlayer.h"




/* 블랙 보드에 쓰이면서, 코드와 연결될 변수들 정리 */
const FName ACreatureController::StartLocation(TEXT("StartLocation"));
const FName ACreatureController::PatrolLocation(TEXT("PatrolLocation"));
const FName ACreatureController::Target(TEXT("Target"));
const FName ACreatureController::CreatureState(TEXT("CreatureState"));
const FName ACreatureController::LastFoundLocation(TEXT("LastFoundLocation"));
const FName ACreatureController::Friend(TEXT("Friend"));


ACreatureController::ACreatureController()
{
    //AIPerceptionCOmponent 오브젝트 생성
    UAIPerceptionComponent* CreaturePerception = CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
        SetPerceptionComponent(*CreaturePerception);

    //UAISenseConfig_sight 오브젝트 생성

    UAISenseConfig_Sight * CreatureSightConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    CreatureSightConfig->DetectionByAffiliation.bDetectEnemies = true;
    CreatureSightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    CreatureSightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    

    UAISenseConfig_Hearing* CreatureHearingConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    CreatureHearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    CreatureHearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    CreatureHearingConfig->DetectionByAffiliation.bDetectFriendlies = true;


    CreatureSightConfig->SetMaxAge(2.0f);

    //주요 감각 설정
    CreaturePerception->SetDominantSense(*CreatureSightConfig->GetSenseImplementation());
    CreaturePerception->ConfigureSense(*CreatureSightConfig);
    CreaturePerception->ConfigureSense(*CreatureHearingConfig);


    CreaturePerception->OnTargetPerceptionUpdated.Clear(); 
    CreaturePerception->OnTargetPerceptionUpdated.AddDynamic(this,&ACreatureController::OnFriendDetected);
    CreaturePerception->OnTargetPerceptionUpdated.AddDynamic(this,&ACreatureController::OnTargetDetected);




}

void ACreatureController::OnPossess(APawn * PawnToPossess)
{
    Super::OnPossess(PawnToPossess);

    UBlackboardComponent* CreatureBlackboardComp = Blackboard.Get();
    //블랙보드컴포넌트를 불러온다.
    //블랙보드는 오브젝트로 감싸져 있으므로, Get을 통해서 직접 들고 올 수 있다.

    if(UseBlackboard(BBAsset,CreatureBlackboardComp))
    { 
        //블랙 보드의 위치를 저장해준다.
        Blackboard = CreatureBlackboardComp;
        Blackboard->SetValueAsVector(StartLocation,PawnToPossess->GetActorLocation());

        //Sense 설정을 위한 위치 찾기
        FAISenseID AISenseID = UAISense::GetSenseID(UAISense_Sight::StaticClass());
        FAISenseID AIHearingID = UAISense::GetSenseID(UAISense_Hearing::StaticClass());

        UAIPerceptionComponent* CreaturePerceptionComp = GetPerceptionComponent();
        RETURN_IF_NULL(CreaturePerceptionComp);

        UAISenseConfig_Sight* CreatureSenseConfig = Cast<UAISenseConfig_Sight>(CreaturePerceptionComp->GetSenseConfig(AISenseID));
        RETURN_IF_NULL(CreatureSenseConfig);

        UAISenseConfig_Hearing* CreatureHearingConfig = Cast<UAISenseConfig_Hearing>(CreaturePerceptionComp->GetSenseConfig(AIHearingID));
        RETURN_IF_NULL(CreatureHearingConfig);
        //시각 설정 조정
        CreatureSenseConfig->SightRadius = CreatureSightRadius;
        CreatureSenseConfig->LoseSightRadius = CreatureLoseSightRadius;
        CreatureSenseConfig->PeripheralVisionAngleDegrees = CreaturePeripheralVisionAngleDegrees;


        //청각 설정 조정
        CreatureHearingConfig->HearingRange = CreatureHearingRange;

        //시각 정보를 업데이트
        CreaturePerceptionComp->RequestStimuliListenerUpdate();
        if(RunBehaviorTree(BTAsset) == false)
        {
            UE_LOG(LogTemp,Error,TEXT("BehaviorTree is False."));
        }
        else
        {
            UE_LOG(LogTemp, Display, TEXT("CreatureBT is Activate"));
        }



    }
}

void ACreatureController::PostInitializeComponents()
{
        Super::PostInitializeComponents();

}

void ACreatureController::BeginPlay()
{
    Super::BeginPlay();

}

void ACreatureController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
     if (!IsValid(Actor)) return;
    //ASurvivor* PlayerCheck = Cast<ASurvivor>(Actor);
    //RETURN_IF_NULL(PlayerCheck);
UE_LOG(LogTemp, Warning, TEXT("Detected Actor: %s (%s)"),
    *Actor->GetName(),
    *Actor->GetClass()->GetName());
    
    //Network캐릭터 추가    
    if (AProtoPlayer* ProtoPlayer = Cast<AProtoPlayer>(Actor))
    {
        RETURN_IF_NULL(ProtoPlayer);

        if (Stimulus.WasSuccessfullySensed())
        {
            Blackboard->SetValueAsObject(Target, ProtoPlayer);
            bIsDetected = true;

        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("DeACtivate"));
            bIsDetected = false;
        }
    }
    else if (ANetworkPlayer* NetworkPlayer = Cast<ANetworkPlayer>(Actor))
    {
        RETURN_IF_NULL(NetworkPlayer);

        if (Stimulus.WasSuccessfullySensed())
        {
            Blackboard->SetValueAsObject(Target, NetworkPlayer);
            bIsDetected = true;

        }
        else
        {
            bIsDetected = false;
        }
    }
    
    else if (ASurvivor* Player = Cast<ASurvivor>(Actor))
    {
        RETURN_IF_NULL(Player);

        if (Stimulus.WasSuccessfullySensed())
        {
            Blackboard->SetValueAsObject(Target, Player);
            bIsDetected = true;

        }
        else
        {
            bIsDetected = false;
        }
    }

    ACreatureBase* Creature = Cast<ACreatureBase>(GetPawn());
    RETURN_IF_NULL(Creature);

    if(bIsDetected)
    {
        Creature->SetState(ECreatureState::Chase);

    }
    else
    {
        if(Creature->GetState() != ECreatureState::Check)
        {
            Creature->SetState(ECreatureState::Check);
        }
        UE_LOG(LogTemp, Display, TEXT("bIsDetected is disabled"));
        Blackboard->SetValueAsObject(Target,nullptr);
    }


}

void ACreatureController::OnFriendDetected(AActor* Actor, FAIStimulus Stimulus)
{

    // UE_LOG(LogTemp, Warning, TEXT("Creaure Friend is Detected"));
    ACreatureBase* FriendCreature = Cast<ACreatureBase>(Actor);
    RETURN_IF_NULL(FriendCreature);

    ACreatureController* FriendCreatureController = Cast<ACreatureController>(FriendCreature->GetController());
    RETURN_IF_NULL(FriendCreatureController)

    ACreatureBase* MySelf = Cast<ACreatureBase>(GetPawn());
    RETURN_IF_NULL(MySelf)

    if(FriendList.Contains(FriendCreature) == false)
    {
        bIsDetected = true;
        Blackboard->SetValueAsObject(Friend,FriendCreature);
        FriendList.Add(FriendCreature);
        MySelf->SetState(ECreatureState::Communicate);


        FAIStimulus FriendCall;
        FriendCreatureController->OnFriendDetected(MySelf, FriendCall);
        
    }
    else
    {
        bIsDetected = false;
        Blackboard->SetValueAsObject(Friend,nullptr);
    }
}



