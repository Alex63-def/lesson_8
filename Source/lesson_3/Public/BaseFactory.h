// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "HealthComponent.h"
#include "DamageTarget.h"
#include "IScorable.h"
#include "Engine/TargetPoint.h"
#include "EnemyTankPawn.h"
#include "LevelTrigger.h"
#include "BaseFactory.generated.h"

UCLASS()
class LESSON_3_API ABaseFactory : public AActor, public IDamageTarget, public IIScorable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseFactory();

	// ��� ������ ������ ���
	UStaticMesh* Asset;

	// ��� ������ ������ material
	UPROPERTY(EditAnywhere)
		class UMaterial* DeathMaterial;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* BuildingMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UBoxComponent* HitCollider;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UArrowComponent* SpawnPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UHealthComponent* HealthComponent;

	// ��� ��� �� ��� ������ ������ �� ���� - ��� ����� ������ � ��������� �� � ������ ����� ��� ������ - ����� � 47 ������ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory")
		TArray<ATargetPoint*> Waypoints;
	
	// ��� �� ��� ����� ���������� 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Factory")
		TSubclassOf<AEnemyTankPawn> TankClass;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffect_1;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffect_2;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffect_3;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UParticleSystemComponent* DeathEffect_1;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UParticleSystemComponent* DeathEffect_2;

	// ��� ������� ��� ��������
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Factory")
		int MaxTanks = 100;

	// �������� ������ � ��������
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Factory")
		float SpawnInterval = 20;

	// ���������� ������� �� ����� ���� ������� �����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory")
		ALevelTrigger* LevelTrigger; 

	virtual void TakeDamage(FDamageData Damage) override;

	void OnHealthChanged(float CurrentHealth);

	void OnDeath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	// ������� 
	void OnTankSpawnTick();

	void SelfDestruction();

	// ��� ��� ������� ������� ������ ����������
	int TanksSpawned = 0;

	FTimerHandle Timer;

public:

	float Experience = 100;

	bool bDeath = false;
	bool bDeath2 = false;
};

