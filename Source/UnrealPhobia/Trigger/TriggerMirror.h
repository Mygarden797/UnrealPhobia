// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerMirror.generated.h"

/*
 - Name        : ATriggerMirror
 - Description : Mirror Actor for activating Triggers
*/
UCLASS()
class UNREALPHOBIA_API ATriggerMirror : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATriggerMirror();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh")
	UStaticMeshComponent *BaseMeshComponent; // 기본 메시 컴포넌트

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SetupMirrorMesh(); // 메시 설정

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
