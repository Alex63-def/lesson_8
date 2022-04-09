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

	// WaypointTag также это надо прописать и в танке
	//специальное свойство - будем искать на карте все объекты акторы у которых есть определенный Tag и использовать их в качестве вейпоинтов 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move")
		FName WaypointTag; // для этого сделали пустой класс Actor "WaypointActor" это сделает немного удобней реализацию - на сколько я понимаю мы хотим сделать точки движения ИИ видимыми или что-то типа того
				// позволит отфильтровать вейпоинты что бы иметь несколько разных маршрутов
			// FName - это специализированный тип строка

	//зададим точки для патрулирования - это массив векторов
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move", Meta = (MakeEditWidget = true)) // Meta - позволит настраивать точки внутри блюпринта но их не будет видно на карте мира
		TArray<FVector> PatrollingPoints;

	// точность сближения с точкой - танк скорей всего будет ехать с умкорением и при сближении будет замедляться или может не будет доехжать до точки из-за помех на пути
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Move")
		float MovementAccuracy = 10; // при этой точности будет считаться что точка достигнута

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire Params")
		float AimSlack = 15;

	// в тике будет расчитываться движение врага
	virtual void Tick(float DeltaTime) override;

	// в бегин будем задавать наши точки патрулирования
	virtual void BeginPlay() override;

	// специальный метод который вызывается когда происходит связывание контроллера с нашим павном
	virtual void OnPossess(APawn* InPawn) override;

	virtual FVector GetShootTarget() const override;

	void AddTarget(AActor* Target);

	void RemoveTarget(AActor* Target);

	void FindNextTarget();

	void Fire();

private:
	int32 CurrentPatrolPointIndex = 0; // индекс к которому в данный момент едет наш танк c ИИ 

	/*UPROPERTY()
		class ATankPawn* TankPawn;*/

	// получается танк выше надо закоментить так как переделали в possess
	UPROPERTY()
		class AEnemyTankPawn* TankPawn;

	UPROPERTY()
		TArray<AActor*> OtherActors;

	// целью будет вражеский павн
	TWeakObjectPtr<AActor> Target;
};
