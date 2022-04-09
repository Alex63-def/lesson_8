// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "WaypointActor.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ��� ������� ����� �������� ��� � ��� �������� ����� �������������� - ����� ��� �������� � ����� � ������, �� ���������� ��� �� ������� �� �� ������
	/*for (int index = 1; index < PatrollingPoints.Num(); index++)
	{
		DrawDebugLine(GetWorld(), PatrollingPoints[index - 1], PatrollingPoints[index], FColor::Green);
	}*/

	// �������� ��� ���� ��������� 
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

		// � ������ ������������� ������� ��������� ��� �� ��� ������� ����������� ��� �������� ��������
		// �� � ��� ������ ���������� ������� �� �� ����� ������ ������������ �� ��� ����� ��� ��������� ������ ������������

		// ��������� ���������� �� ������ ������������ � ����� ����� 
		if (FVector::Distance(CurrentPoint, PawnLocation) < MovementAccuracy)
		{
			CurrentPatrolPointIndex++;
			// ���� ��������� ����� ������� ���� ����� �������������� �� ����� ������� �� ����
			if (CurrentPatrolPointIndex >= PatrollingPoints.Num())
			{
				CurrentPatrolPointIndex = 0;
			}
		}

		// ����� ��������� � �������� ����� � ����� ������� � ������������� ��������� �����
		FVector Direction = CurrentPoint - PawnLocation;
		Direction.Normalize();

		// ������ ���� ���� ������ ������� � ���� ���� ����� �����
		auto ForwardVector = TankPawn->GetActorForwardVector();
		auto RightVector = TankPawn->GetActorRightVector();
		float ForwardAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardVector, Direction))); // RadiansToDegrees ������� ������ �������, ������� ��������� � �������

		// ��� �� ��� �������� � ������ �������
		float RightAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(RightVector, Direction)));

		int RotationValue = 0;

		// ��� ��� ���������� ������������� ���� ��� ���������� ����� ����� �� ��������������� � ���� ������, � ���� ��� ���� �� � ������� ��� ��� ��� 
		// ���� �� �������� �� ����� � ��� ����� �������� ��������� ������������ ������ ������ ������� ������ ����-���� ������ ����������
		//if (ForwardAngle > 5) // 5 ��� �������
		//{
		//	if (RightAngle > 90) // ���� ���� ������ 90 �� ������� ����� , � ���� ������ �� ������
		//		RotationValue = -1;
		//	else
		//		RotationValue = 1;
		//}

		//if (RotationValue != 0) // �� ������� ��� ����
		//{
		//	TankPawn->RotationRight(RotationValue); // ���� ��� �� ���� ������
		//	TankPawn->MoveForward(0); // ��������� �������� ��� �� ����� �� ��������
		//}
		//else
		//{
		//	TankPawn->RotationRight(0); // ��� ���� �������� ��� �� ���� ����, � �� �������� �� �����
		//	TankPawn->MoveForward(1); // ���� ��� �� ���� ������
		//}

		// ��� ���� ������ ��������� � ������ �������������, �� ���� ������� ������� ������, �� �������� ��� ������������ ��������� � ����� ����� 

		if (ForwardAngle > 5) // 5 ��� �������
		{
			if (ForwardAngle > 45)
				TankPawn->MoveForward(0); // ���� ������ 45 �� ����� �� �����
			else
				TankPawn->MoveForward(1); // ���� ���� ������ 5 �� ���� ������ 

			if (RightAngle > 90) // ���� ���� ������ 90 �� ������� ����� , � ���� ������ �� ������
				RotationValue = -1;
			else
				RotationValue = 1;
		}

		if (RotationValue != 0) // �� ������� ��� ����
		{
			TankPawn->RotationRight(RotationValue); // ���� ��� �� ���� ������
		}
		else
		{
			TankPawn->RotationRight(0); // ��� ���� �������� ��� �� ���� ����, � �� �������� �� �����
		}
	}

	// ��� �������� 
	if (Target.IsValid())
	{
		auto TargetingDirection = TankPawn->TurretMesh->GetForwardVector();

		// ��� ����������� � ������� ������ 
		auto PlayerDirection = Target->GetActorLocation() - TankPawn->GetActorLocation();

		// ���� ������ ����� �����-�� ������������� - �� ������ ������
		PlayerDirection.Normalize(); // �������� ��� ��� ����� �� ������ 

		// ���� ����� ����� ��������� 
		auto Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(TargetingDirection, PlayerDirection))); // ��� �������� ��������� ������ ������� �� ������ � �� ������� ��������� ����� ������ ���� ����� ���������

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
	
	// ������� ���� ��� ��� ������� ��� �� ��� ��� ��� ����� ����������� ������ ��� �������� ����, ��������� ��� ���������� ������ ����� ���������� -- ��� ��� ���� 
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// ������ ��� ����
	//TankPawn = Cast<ATankPawn>(InPawn);
	// ���������� 
	TankPawn = Cast<AEnemyTankPawn>(InPawn);
	// ��������� ��� ��� ������ ����
	if (!TankPawn)
	{
		return;
	}

	// ����� ����� �������� � �������� � ��� ���
	WaypointTag = TankPawn->WaypointTag;

	// ����� �� ���������� ������ �� ��� possess �� �� �������� �� ��� ����
	if (!WaypointTag.IsNone())
	{
		// ��������� ���� 
		// ������� ��� ������
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaypointActor::StaticClass(), Actors);

		// ��������� �� �� ���������
		TArray<AWaypointActor*> Waypoints;
		for (auto Actor : Actors)
		{
			auto Waypoint = CastChecked<AWaypointActor>(Actor);
			if (Waypoint->Tags.Contains(WaypointTag)) // � ������� ������ ���� ����� ����� - ��� ������ ������ ���� ����� �������� ����� ���������� �� ������� ����� ����� �������������
				Waypoints.Add(Waypoint); // �� ������� � ����� ��� ��� ����� ��� ������ �������� ������ �����, � ���� �������� ����� �������� ������ � ������� � ���������
		}
		Waypoints.Sort([](const AWaypointActor& a, const AWaypointActor& b)
			{
				return a.WaypointIndex > b.WaypointIndex;
			});

		// ����� ���� ��� ������������� �� ����� ��������� ����� ��������������
		for (auto Waypoint : Waypoints)
		{
			PatrollingPoints.Add(Waypoint->GetActorLocation());
		}
	}
	else if (TankPawn->Waypoints.Num() > 0)
	{
		// ����� ���� ��� ������������� �� ����� ��������� ����� ��������������
		for (auto Waypoint : TankPawn->Waypoints)
		{
			PatrollingPoints.Add(Waypoint->GetActorLocation());
		}
	}

	// �������� ���� ���� � ������� 
	//// ������� ��� ������
	//TArray<AActor*> Actors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaypointActor::StaticClass(), Actors);

	//// ��������� �� �� ���������
	//TArray<AWaypointActor*> Waypoints;
	//for (auto Actor : Actors)
	//{
	//	auto Waypoint = CastChecked<AWaypointActor>(Actor);
	//	if (Waypoint->Tags.Contains(WaypointTag)) // � ������� ������ ���� ����� ����� - ��� ������ ������ ���� ����� �������� ����� ���������� �� ������� ����� ����� �������������
	//		Waypoints.Add(Waypoint); // �� ������� � ����� ��� ��� ����� ��� ������ �������� ������ �����, � ���� �������� ����� �������� ������ � ������� � ���������
	//}
	//Waypoints.Sort([](const AWaypointActor& a, const AWaypointActor& b)
	//	{
	//		return a.WaypointIndex > b.WaypointIndex;
	//	});

	//// ����� ���� ��� ������������� �� ����� ��������� ����� ��������������
	//for (auto Waypoint : Waypoints)
	//{
	//	PatrollingPoints.Add(Waypoint->GetActorLocation());
	//}

	// ��� ��� ����� ��������
	//for (int index = 1; index < PatrollingPoints.Num(); index++)
	//{
	//	DrawDebugLine(GetWorld(), PatrollingPoints[index - 1], PatrollingPoints[index], FColor::Green, true); // ��������� ��� ��� �� ������ ���� ����������
	//}
	//if (PatrollingPoints.Num() > 0)
	//	DrawDebugLine(GetWorld(), PatrollingPoints[0], PatrollingPoints[PatrollingPoints.Num() - 1], FColor::Green, true); // ��� ��������� ���� ���������
}

FVector AEnemyAIController::GetShootTarget() const
{
	// ���� ���� ���� �� ����� ���������� �� ��������� 
	if (Target.IsValid())
		return Target->GetActorLocation();

	// ���� ���� ��� �� ����� ����� �������� �� ���� ���� ������� ����� �����
	if (TankPawn)
		return TankPawn->TurretMesh->GetComponentLocation() + TankPawn->BodyMesh->GetForwardVector();
	
	return FVector::ZeroVector; // ���� ������ ���� ������ ������ ������
}

void AEnemyAIController::AddTarget(AActor* OtherActor)
{
	if (OtherActor == GetPawn())
		return;
	
	// �������� ����� ������� � ���� ��������� 
	OtherActors.Add(OtherActor);

	// ��������� ���� �� ���� - ���� �� ���, �� �������� ����� ������ ����
	if (!Target.IsValid())
	{
		FindNextTarget();
	}
}

void AEnemyAIController::RemoveTarget(AActor* OtherActor)
{
	// ������ ����� ������� � ���� ��������� 
	OtherActors.Remove(OtherActor);

	// ���� ����� ����� �� ������� ��������� �� ���� �� ������ �� ���� �������� �� ����� 
	if (OtherActor == Target)
	{
		FindNextTarget();
	}
}

void AEnemyAIController::FindNextTarget()
{
	if (!TankPawn)
		return;
	
	// ��� ����������� ���� Target , ������� �� ���
	float MinRange = 100000;
	Target = nullptr; // c ����� ��� ��� ����� �������� ��� � �������� ����������� 
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