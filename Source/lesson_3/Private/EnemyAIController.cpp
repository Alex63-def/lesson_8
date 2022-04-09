// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "WaypointActor.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// для красоты можем показать где у нас проходят линии патрулирования - можно тут рисовать а можно в бегине, но проставить что бы никогда их не стирал
	/*for (int index = 1; index < PatrollingPoints.Num(); index++)
	{
		DrawDebugLine(GetWorld(), PatrollingPoints[index - 1], PatrollingPoints[index], FColor::Green);
	}*/

	// заставим наш танк двигаться 
	if (!TankPawn || TankPawn->bDeath2)
	{
		TankPawn->MoveForward(0);
		TankPawn->RotationRight(0);
		return;
	}

	if (PatrollingPoints.Num() > 0)
	{
		FVector CurrentPoint = PatrollingPoints[CurrentPatrolPointIndex];
		FVector PawnLocation = GetPawn()->GetActorLocation();

		// в идеале использховать мувмент компонент что бы при встрече препятствий они обходили стороной
		// но у нас другая реализация которую мы не будем больше использовать но она нужна для понимания работы контроллеров

		// проверяем достаточно ли близко приблизились к нашей точке 
		if (FVector::Distance(CurrentPoint, PawnLocation) < MovementAccuracy)
		{
			CurrentPatrolPointIndex++;
			// если достигнла конца массива этих точек патрулирования то нужно скинуть на ноль
			if (CurrentPatrolPointIndex >= PatrollingPoints.Num())
			{
				CurrentPatrolPointIndex = 0;
			}
		}

		// берем начальную и конечную точки и будем двигать с фиксированной скоростью врага
		FVector Direction = CurrentPoint - PawnLocation;
		Direction.Normalize();

		// узнаем куда танк сейчас смотрит и куда надо будет ехать
		auto ForwardVector = TankPawn->GetActorForwardVector();
		auto RightVector = TankPawn->GetActorRightVector();
		float ForwardAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardVector, Direction))); // RadiansToDegrees косинус вернет радианы, поэтому переводим в градусы

		// что бы мог вращатся в разные стороны
		float RightAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(RightVector, Direction)));

		int RotationValue = 0;

		// вот эта реализация останавлявает танк при достижении точки потом он разворачивается и едет дальше, у меня все норм но у препода был баг что 
		// танк не доезжает до точки и все время пытается повернуть одновременно влевао вправо поэтому дальше чуть-чуть другая реализация
		//if (ForwardAngle > 5) // 5 это градусы
		//{
		//	if (RightAngle > 90) // если угол больше 90 то поворот влево , а если меньше то вправо
		//		RotationValue = -1;
		//	else
		//		RotationValue = 1;
		//}

		//if (RotationValue != 0) // то вращаем наш танк
		//{
		//	TankPawn->RotationRight(RotationValue); // если нет то едет вперед
		//	TankPawn->MoveForward(0); // отключаем движение что бы ыечно не двигался
		//}
		//else
		//{
		//	TankPawn->RotationRight(0); // тут надо обнулять что бы танк ехал, а не крутился на месте
		//	TankPawn->MoveForward(1); // если нет то едет вперед
		//}

		// тут танк всегда двигается и находу повораччивает, но если поворот слишком крутой, то заставим его остановиться повернуть и потом ехать 

		if (ForwardAngle > 5) // 5 это градусы
		{
			if (ForwardAngle > 45)
				TankPawn->MoveForward(0); // если больше 45 то стоит на месте
			else
				TankPawn->MoveForward(1); // если угол больше 5 то едет вперед 

			if (RightAngle > 90) // если угол больше 90 то поворот влево , а если меньше то вправо
				RotationValue = -1;
			else
				RotationValue = 1;
		}

		if (RotationValue != 0) // то вращаем наш танк
		{
			TankPawn->RotationRight(RotationValue); // если нет то едет вперед
		}
		else
		{
			TankPawn->RotationRight(0); // тут надо обнулять что бы танк ехал, а не крутился на месте
		}
	}

	// это стрельба 
	if (Target.IsValid())
	{
		auto TargetingDirection = TankPawn->TurretMesh->GetForwardVector();

		// это направление в сторону игрока 
		auto PlayerDirection = Target->GetActorLocation() - TankPawn->GetActorLocation();

		// этот вектор нужно зачем-то нормализовать - не сказал почему
		PlayerDirection.Normalize(); // наверное это как число по модулю 

		// угол между двумя векторами 
		auto Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(TargetingDirection, PlayerDirection))); // это операция умножения одного вектора на другой и по теореме косинусов вроде найдет угол между векторами

		FHitResult Result;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(TankPawn->Cannon);
		Params.AddIgnoredActor(TankPawn);
		Params.bTraceComplex = true;
		auto Start = TankPawn->CannonPosition->GetComponentLocation();
		auto End = TankPawn->CannonPosition->GetForwardVector() * 650 + Start;
		bool bHasHit = GetWorld()->LineTraceSingleByChannel(Result, TankPawn->TurretMesh->GetComponentLocation(), Target->GetActorLocation(), ECollisionChannel::ECC_Visibility, Params);

		auto Tank = OtherActors[0]->GetActorLocation();

		if (bHasHit)
		{
			End = Result.Location;
		}
		else
			End = Tank;

		if (End == Tank)
		{
			if (Angle < AimSlack)
			{
				Fire();
			}
		}

		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0, 0, 0.0001);
	}
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	
	// сначала весь код был написан тут но так как код может выполниться раньше чем появится танк, перенесли его исполнение только после связывания -- это все ниже 
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// кастим наш танк
	//TankPawn = Cast<ATankPawn>(InPawn);
	// переделали 
	TankPawn = Cast<AEnemyTankPawn>(InPawn);
	// проверяем что это именно танк
	if (!TankPawn)
	{
		return;
	}

	// можно взять вейпоинт и закинуть в наш таг
	WaypointTag = TankPawn->WaypointTag;

	// когда аи контроллер дойдет до фун possess то он запомнит их для себя
	if (!WaypointTag.IsNone())
	{
		// перенесли сюда 
		// находим все акторы
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaypointActor::StaticClass(), Actors);

		// сортируем их по предикату
		TArray<AWaypointActor*> Waypoints;
		for (auto Actor : Actors)
		{
			auto Waypoint = CastChecked<AWaypointActor>(Actor);
			if (Waypoint->Tags.Contains(WaypointTag)) // у каждого актора есть набор тагов - это просто строка куда можно записать любую информацию по которой потом можно отфильтровать
				Waypoints.Add(Waypoint); // на сколько я понял все это нужно для выбора маршрута нашему врагу, а само название этого маршрута задаем в принтах у вейпоинта
		}
		Waypoints.Sort([](const AWaypointActor& a, const AWaypointActor& b)
			{
				return a.WaypointIndex > b.WaypointIndex;
			});

		// после того как отсортировали мы можем заполнить точки патрулирования
		for (auto Waypoint : Waypoints)
		{
			PatrollingPoints.Add(Waypoint->GetActorLocation());
		}
	}
	else if (TankPawn->Waypoints.Num() > 0)
	{
		// после того как отсортировали мы можем заполнить точки патрулирования
		for (auto Waypoint : TankPawn->Waypoints)
		{
			PatrollingPoints.Add(Waypoint->GetActorLocation());
		}
	}

	// перенсли чуть выше в условие 
	//// находим все акторы
	//TArray<AActor*> Actors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaypointActor::StaticClass(), Actors);

	//// сортируем их по предикату
	//TArray<AWaypointActor*> Waypoints;
	//for (auto Actor : Actors)
	//{
	//	auto Waypoint = CastChecked<AWaypointActor>(Actor);
	//	if (Waypoint->Tags.Contains(WaypointTag)) // у каждого актора есть набор тагов - это просто строка куда можно записать любую информацию по которой потом можно отфильтровать
	//		Waypoints.Add(Waypoint); // на сколько я понял все это нужно для выбора маршрута нашему врагу, а само название этого маршрута задаем в принтах у вейпоинта
	//}
	//Waypoints.Sort([](const AWaypointActor& a, const AWaypointActor& b)
	//	{
	//		return a.WaypointIndex > b.WaypointIndex;
	//	});

	//// после того как отсортировали мы можем заполнить точки патрулирования
	//for (auto Waypoint : Waypoints)
	//{
	//	PatrollingPoints.Add(Waypoint->GetActorLocation());
	//}

	// вот тут можно рисовать
	//for (int index = 1; index < PatrollingPoints.Num(); index++)
	//{
	//	DrawDebugLine(GetWorld(), PatrollingPoints[index - 1], PatrollingPoints[index], FColor::Green, true); // последнее тру что бы всегда были нарисованы
	//}
	//if (PatrollingPoints.Num() > 0)
	//	DrawDebugLine(GetWorld(), PatrollingPoints[0], PatrollingPoints[PatrollingPoints.Num() - 1], FColor::Green, true); // это зациклили круг отрисовки
}

FVector AEnemyAIController::GetShootTarget() const
{
	// если есть цель то будем возвращать ее положение 
	if (Target.IsValid())
		return Target->GetActorLocation();

	// если цели нет то пушка будет смотреть на один метр впереди перед собой
	if (TankPawn)
		return TankPawn->TurretMesh->GetComponentLocation() + TankPawn->BodyMesh->GetForwardVector();
	
	return FVector::ZeroVector; // если ничего нету вернем пустой вектор
}

void AEnemyAIController::AddTarget(AActor* OtherActor)
{
	if (OtherActor == GetPawn())
		return;
	
	// запомним актор который с нами пересекся 
	OtherActors.Add(OtherActor);

	// проверяем есть ли цель - если ее нет, то вызываем поиск другой цели
	if (!Target.IsValid())
	{
		FindNextTarget();
	}
}

void AEnemyAIController::RemoveTarget(AActor* OtherActor)
{
	// удалим актор который с нами пересекся 
	OtherActors.Remove(OtherActor);

	// если актор вышел из области попадания по нему то больше по нему стрелять не можем 
	if (OtherActor == Target)
	{
		FindNextTarget();
	}
}

void AEnemyAIController::FindNextTarget()
{
	if (!TankPawn)
		return;
	
	// тут присваиваем цель Target , сначала ее нет
	float MinRange = 100000;
	Target = nullptr; // c типом вик птр можно работать как с обычными указателями 
	for (auto Actor : OtherActors)
	{
		float Range = FVector::Distance(Actor->GetActorLocation(), TankPawn->GetActorLocation());
		if (Range < MinRange)
		{
			MinRange = Range;
			Target = Actor;
		}
	}
}

void AEnemyAIController::Fire()
{
	if (TankPawn)
		TankPawn->ShootEnemyTank();
}