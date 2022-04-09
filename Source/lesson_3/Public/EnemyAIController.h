// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
//#include "TankPawn.h"
#include "EnemyTankPawn.h"
#include "TargetController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class LESSON_3_API AEnemyAIController : public AAIController, public ITargetController
{
	GENERATED_BODY()
	
public:

	// WaypointTag ����� ��� ���� ��������� � � �����
	//����������� �������� - ����� ������ �� ����� ��� ������� ������ � ������� ���� ������������ Tag � ������������ �� � �������� ���������� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move")
		FName WaypointTag; // ��� ����� ������� ������ ����� Actor "WaypointActor" ��� ������� ������� ������� ���������� - �� ������� � ������� �� ����� ������� ����� �������� �� �������� ��� ���-�� ���� ����
				// �������� ������������� ��������� ��� �� ����� ��������� ������ ���������
			// FName - ��� ������������������ ��� ������

	//������� ����� ��� �������������� - ��� ������ ��������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move", Meta = (MakeEditWidget = true)) // Meta - �������� ����������� ����� ������ ��������� �� �� �� ����� ����� �� ����� ����
		TArray<FVector> PatrollingPoints;

	// �������� ��������� � ������ - ���� ������ ����� ����� ����� � ���������� � ��� ��������� ����� ����������� ��� ����� �� ����� �������� �� ����� ��-�� ����� �� ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Move")
		float MovementAccuracy = 10; // ��� ���� �������� ����� ��������� ��� ����� ����������

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire Params")
		float AimSlack = 15;

	// � ���� ����� ������������� �������� �����
	virtual void Tick(float DeltaTime) override;

	// � ����� ����� �������� ���� ����� ��������������
	virtual void BeginPlay() override;

	// ����������� ����� ������� ���������� ����� ���������� ���������� ����������� � ����� ������
	virtual void OnPossess(APawn* InPawn) override;

	virtual FVector GetShootTarget() const override;

	void AddTarget(AActor* Target);

	void RemoveTarget(AActor* Target);

	void FindNextTarget();

	void Fire();

private:
	int32 CurrentPatrolPointIndex = 0; // ������ � �������� � ������ ������ ���� ��� ���� c �� 

	/*UPROPERTY()
		class ATankPawn* TankPawn;*/

	// ���������� ���� ���� ���� ����������� ��� ��� ���������� � possess
	UPROPERTY()
		class AEnemyTankPawn* TankPawn;

	UPROPERTY()
		TArray<AActor*> OtherActors;

	// ����� ����� ��������� ����
	TWeakObjectPtr<AActor> Target;
};
