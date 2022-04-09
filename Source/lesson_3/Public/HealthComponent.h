// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageTarget.h"
#include "HealthComponent.generated.h"

//  этот компонент будет отслеживать здоровье всех объектов у которых он есть

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LESSON_3_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

		// нужно что бы компонент сообщал игроку что здоровье закончилось или изменилось
		// используем делегаты
		DECLARE_EVENT(UHealthComponent, FDieEvent); // это на сколько я понял если усер оповестит
		DECLARE_EVENT_OneParam(UHealthComponent, FHealthChanged, float); // а это оповестит сколько хп осталось 

public:	

	

	// Sets default values for this component's properties
	UHealthComponent();

	void TakeDamageTurret(FDamageData Damage);

	void TakeDamageTank(FDamageData Damage);

	void TakeDamageFactoty(FDamageData Damage);

	float GetHealthTurret() const { return CurrentHealthTurret; }

	float GetHealthTank() const { return CurrentHealthTank; }

	FDieEvent OnDeath; // это что бы заработал эвент
	FHealthChanged OnHealthChanged;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float MaxHealthTurret = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float MaxHealthFactory = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float MaxHealthTank = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float CurrentHealthTurret;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float CurrentHealthFactory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float CurrentHealthTank;
		
};
