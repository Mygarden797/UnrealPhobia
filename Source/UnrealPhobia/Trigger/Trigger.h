// Trigger.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trigger.generated.h"

UENUM(BlueprintType)
enum class ETriggerName : uint8
{
	Grey UMETA(DisplayName = "GreyTrigger"),
	Ghost UMETA(DisplayName = "GhostTrigger"),
};

UCLASS()
class UNREALPHOBIA_API ATrigger : public AActor
{
	GENERATED_BODY()

public:
	ATrigger();

	// Base and highlight mesh components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh")
	UStaticMeshComponent *BaseMesh;

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh")
	// UStaticMeshComponent *HighlightMesh;

	// Trigger type enum
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	ETriggerName TriggerName = ETriggerName::Grey;

	UPROPERTY(EditAnywhere, Category = "StaticMesh|Assets")
	UStaticMesh *GreyTriggerMesh;

	UPROPERTY(EditAnywhere, Category = "StaticMesh|Assets")
	UStaticMesh *GhostTriggerMesh;

protected:
	virtual void OnConstruction(const FTransform &Transform) override;
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
