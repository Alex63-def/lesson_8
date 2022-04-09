// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPlayerController.h"
#include "TankPawn.h"
#include "DrawDebugHelpers.h" // для 3 варианта отладки

void ATankPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForvard", this, &ATankPlayerController::OnMoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATankPlayerController::OnMoveRight);
	InputComponent->BindAxis("RotationRight", this, &ATankPlayerController::OnRotationRight);
	InputComponent->BindAxis("lastPush", this, &ATankPlayerController::OnlastPush);
	InputComponent->BindAction("Shoot", IE_Pressed, this, &ATankPlayerController::OnShoot); 
	InputComponent->BindAction("ShootTrace", IE_Pressed, this, &ATankPlayerController::OnShootTrace);
	InputComponent->BindAxis("ShootFlame", this, &ATankPlayerController::OnFlameShoot);
	InputComponent->BindAction("StopSoundFlame", IE_Released, this, &ATankPlayerController::OnStopSoundFlame);
	InputComponent->BindAxis("RotationRightTurret", this, &ATankPlayerController::OnRotationTurretScale);

	bShowMouseCursor = true;
}

void ATankPlayerController::OnMoveForward(float Value)
{
	if (TankPawn)
		TankPawn->MoveForward(Value);
}

void ATankPlayerController::OnMoveRight(float Value)
{
	if (TankPawn)
		TankPawn->MoveRight(Value);
}

void ATankPlayerController::OnRotationRight(float Value)
{
	if (TankPawn)
		TankPawn->RotationRight(Value);
}

void ATankPlayerController::OnlastPush(float Value) 
{
	if (TankPawn)
		TankPawn->lastPush(Value);
}

void ATankPlayerController::OnShoot()
{
	if (TankPawn)
	{
		if (!bShootTrace)
			TankPawn->Shoot();
		else
			TankPawn->ShootTrace();
	}
}

void ATankPlayerController::OnShootTrace()
{
	if(!bShootTrace)
		bShootTrace = true;
	else
		bShootTrace = false;
}

void ATankPlayerController::OnFlameShoot(float Value)
{
	if (TankPawn)
		TankPawn->FlameShoot(Value);
}

void ATankPlayerController::OnStopSoundFlame()
{
	if (TankPawn)
	{
		TankPawn->StopSoundFlame();
	}
}

void ATankPlayerController::OnRotationTurretScale(float Value)
{
	if (TankPawn)
	{
		TankPawn->RotationTurretScale(Value);
	}

	if (FMath::IsNearlyEqual(Value, 0.0f))
		TankPawn->bMousePosition = true;
}

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TankPawn = CastChecked<ATankPawn>(GetPawn());
}

void ATankPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (TankPawn->bMousePosition == true)
	{
		FVector MousePosition, MouseDirection;
		DeprojectMousePositionToWorld(MousePosition, MouseDirection);
		auto Z = FMath::Abs(TankPawn->GetActorLocation().Z - MousePosition.Z);
		MouseWorldPosition = MousePosition - MouseDirection * Z / MouseDirection.Z;
	}
}