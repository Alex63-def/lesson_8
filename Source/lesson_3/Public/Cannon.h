// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "BaseProjectile.h"
#include "Projectile.h"
#include "Rocket.h"
#include "Particles/ParticleSystemComponent.h" // ��� ����������� �������
#include "Components/AudioComponent.h" // ��� �����
#include "Cannon.generated.h"

UENUM()
enum class ECannonType : uint8
{
	FireProjectile = 0 UMETA(DisplayName = "Use projectile"), 
	FireRocket = 1 UMETA(DisplayName = "Use rocket"),
};

UCLASS()
class LESSON_3_API ACannon : public AActor
{
	GENERATED_BODY()

		DECLARE_EVENT_OneParam(ACannon, FExpEvent, FExpData);
	
public:	
	// Sets default values for this actor's properties
	ACannon();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UArrowComponent* ProjectileSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		float FireRate = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		float FireRateTurret = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		float FireRateTurretEnemy = 0.3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		float FireRange = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		ECannonType Type = ECannonType::FireProjectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		TSubclassOf<ABaseProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		TSubclassOf<ARocket> RocketClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		int QuantityProjectile = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		int QuantityRocket = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		int QuantityEnemyTurretProjectile = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		int QuantityEnemyTankProjectile = 5;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		ABaseProjectile* ProjectileKill;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		ARocket* RocketKill;

	// ��������� ���������� �� ���������� ������� // ���� ��� ������� ��� ������� � ����� �������� ������� ����� ������
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UParticleSystemComponent* ShootEffectProjectile_1;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UParticleSystemComponent* ShootEffectProjectile_2;

	// �������� ���������
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffectProjectile;

	// �������� ���������
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffectRocket;

	// ������ ������ ������
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UForceFeedbackEffect* ForceFeedbackEffect;

	// ��� ���-�� ����������� �� ������� ��������� - ���������� �������� � ��, ������ � ������� 
	// ��� ��� ��������� (�� ����� � �����)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		TSubclassOf<UCameraShakeBase> CameraEffect;

	FExpEvent OnExpEventCannon;

	void Shoot();

	void ShootTurret();

	void ShootEnemyTank();

	void OnKillEvent();

	void OnExpEvent(FExpData ExpData);

	void RechargeCannon();

	void TickInfo(bool bTickInfo);

	int Projectile{};
	int Rocket{};
	int QuantityEnemyTurret{};
	int QuantityEnemyTank{};

	bool bTickInfoCannon = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	void ResetShootState();

	FTimerHandle TimerHandleShoot;
	
	FTimerHandle TimerEnemyTurret;
	FTimerHandle TimerEnemyTank;

	bool bReadyTOShoot = true;
	bool bReadyRecharge = true;
	
	bool bEmptyProjectile = false;
	bool bEmptyRocket = false;
};
