// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "IScorable.h"
#include "Rocket.generated.h"

UCLASS()
class LESSON_3_API ARocket : public AActor
{
	GENERATED_BODY()
	
		DECLARE_EVENT(ARocket, FKillEvent);
		DECLARE_EVENT_OneParam(ARocket, FExpEvent, FExpData);

public:	
	// Sets default values for this actor's properties
	ARocket();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USphereComponent* Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UParticleSystemComponent* ShootEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UParticleSystemComponent* ShootEffect_2;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffect_2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rocket")
		float MoveSpeed = 500;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rocket")
		float MoveRate = 0.005;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rocket")
		float Damage = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rocket")
		int SelfDestructionSec = 3;

	FKillEvent OnKillEvent;
	FExpEvent OnExpEventRocket;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	FTimerHandle Timer;
	FTimerHandle TimerDestruction;

	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void MoveTick();

	void SelfDestruction();
};
