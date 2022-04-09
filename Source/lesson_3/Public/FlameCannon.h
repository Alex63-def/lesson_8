// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "IScorable.h"
#include "FlameCannon.generated.h"

UCLASS()
class LESSON_3_API AFlameCannon : public AActor
{
	GENERATED_BODY()

		DECLARE_EVENT_OneParam(AFlameCannon, FExpEvent, FExpData);
	
public:	
	// Sets default values for this actor's properties
	AFlameCannon();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UArrowComponent* FlameSpawnPoint;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UParticleSystemComponent* ShootEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffect_1;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffect_2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
		float FlameDamage = 0.05;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		float FireRate = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		float FireRange = 100;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		float CanCol = 0.3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire params")
		float Overheat = 300;*/

	FExpEvent OnExpEventFlame;

	void Shoot(float Value, float Overheat);

	void OnKill();

	void StopSoundFlame();

	/*void OverheatCannon();*/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	/*bool bOverheat = false;
	FTimerHandle TimerFlame;*/
};
