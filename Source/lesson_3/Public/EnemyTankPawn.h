// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankPawn.h"
#include "Engine/TargetPoint.h"
#include "EnemyTankPawn.generated.h"

/**
 * 
 */
UCLASS()
class LESSON_3_API AEnemyTankPawn : public ATankPawn
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UParticleSystemComponent* DeathEffectt;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioDeathEffectt;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffect_FlameDeath;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UParticleSystemComponent* DeathEffect_Flame;

	//набор вейпоинтов когда аи контроллер дойдет до фун possess то он запомнит их для себя
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move")
		TArray<ATargetPoint*> Waypoints;

	AEnemyTankPawn();

	// сюда будут передаваться параметры при спавне танка из класса BaseFactory
	virtual void OnConstrution(const FTransform& Transform) override;

	virtual void SetupCannon(const TSubclassOf<ACannon>& CannonClass) override;

	// это для примера, ни на что не влияет
	int Number = -1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	//UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	//UFUNCTION()
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void OnHealthChanged(float CurrentHealth) override;
	virtual void OnDeath() override;
	virtual void SelfDestruction() override;
};
