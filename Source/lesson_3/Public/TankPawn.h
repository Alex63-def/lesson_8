// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "UnitPawn.h"
#include "TankPawn.generated.h"

UCLASS()
class LESSON_3_API ATankPawn : public AUnitPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATankPawn();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffectMove_1;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffectMove_2;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffectRotateTurret;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffectStopRotateTurret;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		float CanCol = 0.3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		float Overheat = 300;

	virtual void MoveForward(float Value);

	virtual void MoveRight(float Value) override;

	virtual void RotationRight(float Value) override;

	void RotationTurretScale(float Value);

	virtual void lastPush(float Value) override;

	virtual void Shoot() override;

	virtual void ShootTrace() override;

	virtual void FlameShoot(float Value) override;

	virtual void StopSoundFlame() override;

	virtual void SetupCannon(const TSubclassOf<ACannon>& CannonClass) override;

	virtual void RechargeFlameTimer() override;

	virtual void StopFlameTimer() override;

	virtual void RechargeFlame() override;

	virtual void ChargingStationTimer() override;

	virtual void StopStationTimer() override;

	virtual void ChargingStation() override;

	virtual void TakeDamage(FDamageData Damage) override;

	virtual void TakeExp(FExpData ExpData) override;

	//это тольтко что бы работало  в танке враге
	virtual void OnConstrution(const FTransform& Transform) override;

	// на сколько € пон€л что если два контроллера дл€ игрока и »» то надо в ней прописывать - у нас контроллеры дл€ башни 
	virtual void PossessedBy(AController* NewController) override;

	void ShootEnemyTank();

	void OverheatCannon();

	bool bMousePosition = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:

	virtual void MoveTank(float DeltaTime) override;
	virtual void RotationTank(float DeltaTime) override;
	virtual void RotationCannon(float DeltaTime) override;
	virtual void OnDeath() override;
	virtual void OnHealthChanged(float CurrentHealth) override;

	float TempEnergy{};
	float TempOverheat{};

	bool bOverheat = false;
	FTimerHandle TimerFlame;

	/*bool bMousePosition = true;*/
};
