// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trigger.h"
#include "TriggerMirror.generated.h"

/*
 - Name        : ATriggerMirror
 - Description : Mirror Actor for activating Triggers
*/
class ATriggerSpawnManager;
UCLASS()
class PARANOID_API ATriggerMirror : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATriggerMirror();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh")
	UStaticMeshComponent *BaseMeshComponent; // Default mesh component

	bool ActivateTrigger(ETriggerName TriggerName); // Activate Trigger with starting Mirror cooldown, Return success or not
	FTimerHandle CooldownTimerHandle;				// Timer Handle

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bCanActivate = true;
	float ActivateCoolTime = 10.0f;

	void SetupMirrorMesh();

	void StartCooldown();
	void EndCooldown();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
