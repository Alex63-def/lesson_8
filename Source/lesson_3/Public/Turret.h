// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitPawn.h"
#include "Turret.generated.h"

UCLASS()
class LESSON_3_API ATurret : public AUnitPawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurret();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffectRotateTurret;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffectStopRotateTurret;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffect_FlameDeath;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UParticleSystemComponent* DeathEffect_Flame;

	virtual void TakeDamage(FDamageData Damage) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Destroyed() override;
		
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
		
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	//функция стрельбы в любом случае для нового типа снарядов
	// это блюпринт для нее, что бы из движка ее ставить 
	UFUNCTION(BlueprintCallable) // ее запускаем выбрав cannon position в event graph в принтах
		void FireAnyway();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
		
private:

	virtual void FindNextTarget() override;
	virtual void Targeting() override;
	virtual void Fire() override;
	virtual void OnDeath() override;
	virtual void OnHealthChanged(float CurrentHealth) override;
	virtual void SelfDestruction() override;
};
