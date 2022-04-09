// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/Actor.h"
#include "DamageTarget.generated.h"

// это интерфейс который ьудет описывать любой объект который может получить урон

// типы урона
USTRUCT(BlueprintType)
struct FDamageData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DamageValue;

	// это тот кто наносит урон
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* Instigator;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageTarget : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LESSON_3_API IDamageTarget
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// все что хочу добавить в интерфейс нужно добавить вот сюда 
	// интерфейс добавляет какие-то новые фун которые мы хотим что бы были у всех его наследников

	// вывод инфы о получениии урона
	virtual void TakeDamage(FDamageData Damage) = 0;
};
