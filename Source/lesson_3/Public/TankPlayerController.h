// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TargetController.h"
#include "TankPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LESSON_3_API ATankPlayerController : public APlayerController, public ITargetController
{
	GENERATED_BODY()
	
public:

	virtual void SetupInputComponent() override;

	void OnMoveForward(float Value);

	void OnMoveRight(float Value);

	void OnRotationRight(float Value);

	void OnlastPush(float Value);

	void OnShoot();

	void OnShootTrace();

	void OnFlameShoot(float Value);

	void OnStopSoundFlame();

	void OnRotationTurretScale(float Value);
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual FVector GetShootTarget() const override { return MouseWorldPosition; }

private:

	UPROPERTY()
		class ATankPawn* TankPawn;

	FVector MouseWorldPosition;

	bool bShootTrace = false;
};
