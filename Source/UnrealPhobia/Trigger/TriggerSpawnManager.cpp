// Fill out your copyright notice in the Description page of Project Settings.

#include "TriggerSpawnManager.h"
#include "Kismet/GameplayStatics.h"
void ATriggerSpawnManager::BeginPlay()
{
    Super::BeginPlay();
    if (!IsOwnedBy(GetWorld()->GetFirstPlayerController())) // 서버, 클라이언트에서 두 번씩 동작됨 방지
    {
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("The number of TriggerSpawnPoints : %d"), SetTriggerSpawnPoints());

    // DefaultTriggerCount만큼 랜덤 스폰 수행
    for (int32 i = 0; i < DefaultTriggerCount; ++i)
        if (SpawnTriggerInRandom())
        {
            UE_LOG(LogTemp, Log, TEXT("Trigger Spawned"));
        }
}
ATriggerSpawnManager::ATriggerSpawnManager()
{
    UE_LOG(LogTemp, Log, TEXT("TriggerSpawnManager Started"));
}

uint8 ATriggerSpawnManager::SetTriggerSpawnPoints()
{
    SpawnPoints.Empty();

    // 월드에서 모든 ATriggerSpawnPoint 액터를 검색
    TArray<AActor *> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(),
        ATriggerSpawnPoint::StaticClass(),
        FoundActors);

    // SpawnerId == 10 인 것만 필터링
    for (AActor *Actor : FoundActors)
    {
        if (ATriggerSpawnPoint *Point = Cast<ATriggerSpawnPoint>(Actor))
        {
            if (Point->SpawnerId == 1)
            {
                SpawnPoints.Add(Point);
            }
        }
    }
    // 저장된 포인트 수 반환
    return static_cast<uint8>(SpawnPoints.Num());
}

// 현재 소환 가능한 스폰 포인트들을 업데이트
bool ATriggerSpawnManager::UpdateAvailablePoints()
{
    AvailablePoints.Empty();
    // bCanSpawn == false인 경우에만 추가
    for (ATriggerSpawnPoint *Point : SpawnPoints)
    {
        if (Point && Point->bCanSpawn)
        {
            AvailablePoints.Add(Point);
        }
    }
    if (AvailablePoints.Num() == 0)
        return false;

    return true;
}

// 임의의 스폰 포인트에 트리거를 소환
bool ATriggerSpawnManager::SpawnTriggerInRandom()
{
    if (!HasAuthority() || SpawnPoints.Num() == 0 || !UpdateAvailablePoints())
    {
        return false;
    }
    int32 Index = FMath::RandRange(0, AvailablePoints.Num() - 1); // 랜덤으로 하나 선택
    return SpawnTrigger(Index);
}

bool ATriggerSpawnManager::SpawnTrigger(int32 index)
{
    if (AvailablePoints.Num() == 0)
    {
        if (!UpdateAvailablePoints())
            return false;
    }

    ATriggerSpawnPoint *SelectedPoint = AvailablePoints[index];

    // 선택된 포인트 위치·회전
    const FVector SpawnLocation = SelectedPoint->GetActorLocation();
    const FRotator SpawnRotation = SelectedPoint->GetActorRotation();

    // 트리거 스폰
    FActorSpawnParameters Params;
    ATrigger *SpawnedTrigger = GetWorld()->SpawnActor<ATrigger>(
        ATrigger::StaticClass(),
        SpawnLocation,
        SpawnRotation,
        Params);
    if (SpawnedTrigger)
    {
        // 스폰이 완료되었으니 해당 포인트는 비활성화
        SelectedPoint->bCanSpawn = false;

        CurrentTriggerCount++;

        // 스폰된 트리거에 트리거 종류 랜덤으로 부여
        int32 RandValue = FMath::RandRange(0, static_cast<int32>(ETriggerName::None) - 1);
        SpawnedTrigger->SetTriggerName(static_cast<ETriggerName>(RandValue));
        SpawnedTrigger->TriggerSpawnPoint = SelectedPoint;
        return true;
    }
    return false;
}

void ATriggerSpawnManager::StartCooldown()
{
    bIsSpawning = true;
    GetWorld()->GetTimerManager().SetTimer(
        CooldownTimerHandle,                // 타이머 핸들
        this,                               // 호출 대상
        &ATriggerSpawnManager::EndCooldown, // 호출할 함수
        SpawnCooldown,                      // 지연 시간 (초)
        false                               // 반복 여부 (false면 단발성)
    );
}

void ATriggerSpawnManager::EndCooldown()
{
    SpawnTriggerInRandom();

    if (CurrentTriggerCount < DefaultTriggerCount)
        StartCooldown();
    else
        bIsSpawning = false;
}

void ATriggerSpawnManager::StartSpawn()
{
    CurrentTriggerCount--;
    if (bIsSpawning)
        return;
    StartCooldown();
}
