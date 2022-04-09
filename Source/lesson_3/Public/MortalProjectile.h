// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h" 
#include "MortalProjectile.generated.h"

// этот снаряд будет имитировать физику и лететь по навесной 

/**
 * 
 */
UCLASS()
class LESSON_3_API AMortalProjectile : public ABaseProjectile
{
	GENERATED_BODY()
	
public:

	AMortalProjectile();

	// отвечает уже за саму физику полета снаряда - это как я делал в принтах до этого урока
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UProjectileMovementComponent* MovementComponent;
};
