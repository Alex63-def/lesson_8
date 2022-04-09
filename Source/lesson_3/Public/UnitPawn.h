// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "TankPlayerController.h"
#include "Components/ArrowComponent.h"
#include "Cannon.h"
#include "TraceCannon.h"
#include "FlameCannon.h"
#include "HealthComponent.h"
#include "DamageTarget.h"
#include "IScorable.h"
#include "LevelTrigger.h"
#include "UnitPawn.generated.h"

UCLASS(Abstract)
class LESSON_3_API AUnitPawn : public APawn, public IDamageTarget, public IIScorable
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AUnitPawn();

	// при смерти мен€ет material
	UPROPERTY(EditAnywhere)
		class UMaterial* DeathMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USphereComponent* RangeSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UCapsuleComponent* Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UBoxComponent* Box;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USpringArmComponent* Arm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UArrowComponent* CannonPosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UArrowComponent* CannonTracePosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UArrowComponent* CannonFlamePosition1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UArrowComponent* CannonFlamePosition2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
		TSubclassOf<ACannon> CannonType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
		TSubclassOf<ATraceCannon> TraceCannonClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
		TSubclassOf<AFlameCannon> FlameCannonClass1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
		TSubclassOf<AFlameCannon> FlameCannonClass2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UHealthComponent* HealthComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UParticleSystemComponent* DeathEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioDeathEffect;

	// активируем переход на левел если враг убит
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory")
		ALevelTrigger* LevelTrigger;

	UPROPERTY()
		ACannon* Cannon;

	UPROPERTY()
		ATraceCannon* TraceCannon;

	UPROPERTY()
		AFlameCannon* FlameCannon1;

	UPROPERTY()
		AFlameCannon* FlameCannon2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float CamLagSpeed = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float MovementSpeed = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float MovementAcceleration = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float RotationSpeed = 90;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float RotationAcceleration = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float TurretAcceleration = 0.02;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float lastPushSpeed = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		int QuantityEnergy = 5000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		int QuantityFuel = 500;

	//специальное свойство - будем искать на карте все объекты акторы у которых есть определенный Tag и использовать их в качестве вейпоинтов 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move")
		FName WaypointTag; // дл€ этого сделали пустой класс Actor "WaypointActor" это сделает немного удобней реализацию - на сколько € понимаю мы хотим сделать точки движени€ »» видимыми или что-то типа того
				// позволит отфильтровать вейпоинты что бы иметь несколько разных маршрутов
			// FName - это специализированный тип строка

	virtual void MoveForward(float Value)PURE_VIRTUAL(MoveForward);

	virtual void MoveRight(float Value) PURE_VIRTUAL(MoveRight);

	virtual void RotationRight(float Value) PURE_VIRTUAL(RotationRight);

	virtual void lastPush(float Value) PURE_VIRTUAL(lastPush);

	virtual void Shoot() PURE_VIRTUAL(Shoot);

	virtual void ShootTrace() PURE_VIRTUAL(ShootTrace);

	virtual void FlameShoot(float Value) PURE_VIRTUAL(FlameShoot);

	virtual void StopSoundFlame() PURE_VIRTUAL(StopSoundFlame);

	virtual void SetupCannon(const TSubclassOf<ACannon>& CannonClass) PURE_VIRTUAL(SetupCannon);

	virtual void RechargeFlameTimer() PURE_VIRTUAL(RechargeFlameTimer);

	virtual void StopFlameTimer() PURE_VIRTUAL(StopFlameTimer);

	virtual void RechargeFlame() PURE_VIRTUAL(RechargeFlame);

	virtual void ChargingStationTimer() PURE_VIRTUAL(ChargingStationTimer);

	virtual void StopStationTimer() PURE_VIRTUAL(StopStationTimer);

	virtual void ChargingStation() PURE_VIRTUAL(ChargingStation);

	virtual void TakeDamage(FDamageData Damage) PURE_VIRTUAL(TakeDamage);
	virtual void TakeExp(FExpData ExpData) PURE_VIRTUAL(TakeExp);

	virtual void MoveTank(float DeltaTime) PURE_VIRTUAL(MoveTank);
	virtual void RotationTank(float DeltaTime) PURE_VIRTUAL(RotationTank);
	virtual void RotationCannon(float DeltaTime) PURE_VIRTUAL(RotationCannon);

	virtual void OnDeath() PURE_VIRTUAL(OnDeath);
	virtual void OnHealthChanged(float CurrentHealth) PURE_VIRTUAL(OnHealthChanged);

	virtual void SelfDestruction() PURE_VIRTUAL(SelfDestruction);
	
	// дл€ того что бы работало в танке враге, тут оно так как этот класс наследуетс€ от павна, а дочерние от него
	virtual void OnConstrution(const FTransform& Transform) PURE_VIRTUAL(OnConstrution);

	float MoveSForwardScaleTarget = 0;
	float MoveSForwardScaleCurrent = 0;
	float lastPushForward = 0;

	float MoveRightScaleTarget = 0;
	float MoveRightScaleCurrent = 0;

	float RotationRightScaleTarget = 0;
	float RotationRightScaleCurrent = 0;

	float TurretRightScaleTarget = 0;
	float TurretRightScaleCurrent = 0;

	float Experience = 0;

	int OldTrurretRotation;
	int NewTrurretRotation;
	bool IsTurretRotation = false;

	UPROPERTY()
		class ATankPlayerController* TankController;

		class ITargetController* TargetController;

	int Energy{};
	int Fuel{};

	FTimerHandle TimerFlame;
	FTimerHandle TimerStation;
	FTimerHandle TimerDestruction;

	bool bMove = false;

	bool bDeath = false;

	bool bDeath2 = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire Params")
		float TargetingSpeed = 0.1f;

	//принцип стрельбы по таймеру который отвечает за прицеливание и стрельбу - тут период тика таймера
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire Params")
		float TargetingRate = 0.005f;

	// это угол меньше которого пушка начнет стрел€ть - это в функции Targeting
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire Params")
		float AimSlack = 15;

	UFUNCTION()
		virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) PURE_VIRTUAL(OnBeginOverlap);
	
	UFUNCTION()
		virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) PURE_VIRTUAL(OnEndOverlap);

	virtual void FindNextTarget()PURE_VIRTUAL(FindNextTarget);
	virtual void Targeting()PURE_VIRTUAL(Targeting);
	virtual void Fire()PURE_VIRTUAL(Fire);

	// то где будем хранить врагов башни
	UPROPERTY()
		TArray<AActor*> OtherActors;

	TWeakObjectPtr<AActor> Target;
};
