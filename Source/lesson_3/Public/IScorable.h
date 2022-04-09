// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/Actor.h"
#include "IScorable.generated.h"

USTRUCT(BlueprintType)
struct FExpData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ExperienceValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* Enemy;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIScorable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LESSON_3_API IIScorable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//virtual void TakeExp(FExpData ExpData) = 0;
};
