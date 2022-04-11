// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h" 
#include "MortalProjectile.generated.h"

// ���� ������ ����� ����������� ������ � ������ �� �������� 

/**
 * 
 */
UCLASS()
class LESSON_3_API AMortalProjectile : public ABaseProjectile
{
	GENERATED_BODY()
	
public:

	AMortalProjectile();

	// �������� ��� �� ���� ������ ������ ������� - ��� ��� � ����� � ������� �� ����� �����
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UProjectileMovementComponent* MovementComponent;

	// ��� ����� �� �������
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USphereComponent* BlastRegion; 

	 // ��� ������������ � ���-���� ����� ����������� ����� � ����� ������������ ������������
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
};
