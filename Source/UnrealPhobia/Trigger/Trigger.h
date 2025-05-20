// Trigger.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trigger.generated.h"

/*
 - Name        : ETriggerName
 - Description : 트리거 종류 지정하는 열거형 클래스
*/
UENUM(BlueprintType)
enum class ETriggerName : uint8
{
	Grey UMETA(DisplayName = "트리거 타입 : Grey"),
	Ghost UMETA(DisplayName = "트리거 타입 : Ghost"),
	None UMETA(DisplayName = "지정된 트리거 타입이 없음"),
};

/*
 - Name        : ATrigger
 - Description : 트리거 오브젝트의 액터 클래스
*/
UCLASS()
class UNREALPHOBIA_API ATrigger : public AActor
{
	GENERATED_BODY()

public:
	ATrigger();
	void SetTriggerName(ETriggerName Name); // 트리거 타입 지정 함수

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh")
	UStaticMeshComponent *BaseMeshComponent; // 기본 메시 컴포넌트

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	ETriggerName TriggerName = ETriggerName::Grey; // 디폴트 트리거 종류

	UPROPERTY(EditAnywhere, Category = "StaticMesh|Assets")
	UStaticMesh *TriggerMesh; // 트리거 메시

protected:
	virtual void BeginPlay() override;
	void SetupTriggerMesh(); // 메시 설정

public:
	virtual void Tick(float DeltaTime) override;
};
