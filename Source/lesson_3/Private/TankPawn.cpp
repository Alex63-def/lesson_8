// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "lesson_3/lesson_3.h"
#include "AmmoBox.h"
#include "DrawDebugHelpers.h"
#include "EnemyTankPawn.h"

// Sets default values
ATankPawn::ATankPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	RootComponent = Box;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>("BodyMesh");
	BodyMesh->SetupAttachment(RootComponent);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>("TurretMesh");
	TurretMesh->SetupAttachment(BodyMesh);

	CannonPosition = CreateDefaultSubobject<UArrowComponent>("CannonPosition");
	CannonPosition->SetupAttachment(TurretMesh);

	CannonTracePosition = CreateDefaultSubobject<UArrowComponent>("CannonTracePosition");
	CannonTracePosition->SetupAttachment(TurretMesh);

	CannonFlamePosition1 = CreateDefaultSubobject<UArrowComponent>("CannonFlamePosition1");
	CannonFlamePosition1->SetupAttachment(BodyMesh);

	CannonFlamePosition2 = CreateDefaultSubobject<UArrowComponent>("CannonFlamePosition2");
	CannonFlamePosition2->SetupAttachment(BodyMesh);

	Arm = CreateDefaultSubobject<USpringArmComponent>("Arm");
	Arm->SetupAttachment(RootComponent);
	/*Arm->bInheritRoll = false;
	Arm->bInheritPitch = false;
	Arm->bInheritYaw = true;*/
	Arm->bEnableCameraLag = true;
	Arm->CameraLagSpeed = CamLagSpeed;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(Arm);
	Camera->bUsePawnControlRotation = false;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	// подписываемся на событые смерть и изменение здоровья
	HealthComponent->OnDeath.AddUObject(this, &ATankPawn::OnDeath);
	HealthComponent->OnHealthChanged.AddUObject(this, &ATankPawn::OnHealthChanged);

	AudioEffect = CreateDefaultSubobject<UAudioComponent>("AudioEffect");
	AudioEffect->SetupAttachment(BodyMesh);

	AudioEffectMove_1 = CreateDefaultSubobject<UAudioComponent>("AudioEffectMove_1");
	AudioEffectMove_1->SetupAttachment(BodyMesh);

	AudioEffectMove_2 = CreateDefaultSubobject<UAudioComponent>("AudioEffectMove_2");
	AudioEffectMove_2->SetupAttachment(BodyMesh);

	AudioEffectRotateTurret = CreateDefaultSubobject<UAudioComponent>("AudioEffectRotateTurret");
	AudioEffectRotateTurret->SetupAttachment(TurretMesh);

	AudioEffectStopRotateTurret = CreateDefaultSubobject<UAudioComponent>("AudioEffectStopRotateTurret");
	AudioEffectStopRotateTurret->SetupAttachment(TurretMesh);
}

// Called when the game starts or when spawned
void ATankPawn::BeginPlay()
{
	Super::BeginPlay();

	Experience = 0;

	Energy = QuantityEnergy;
	Fuel = QuantityFuel;
	TempEnergy = Energy;
	TempOverheat = Overheat;
	
	// тут были контроллеры сейчас перенесены в PossessedBy

	SetupCannon(CannonType);

	if (TraceCannonClass && FlameCannonClass1 && FlameCannonClass2)
	{
		auto TransformTrace = CannonTracePosition->GetComponentTransform();
		auto TransformFlame = CannonTracePosition->GetComponentTransform();

		TraceCannon = GetWorld()->SpawnActor<ATraceCannon>(TraceCannonClass, TransformTrace);
		FlameCannon1 = GetWorld()->SpawnActor<AFlameCannon>(FlameCannonClass1, TransformFlame);
		FlameCannon2 = GetWorld()->SpawnActor<AFlameCannon>(FlameCannonClass2, TransformFlame);

		TraceCannon->AttachToComponent(CannonTracePosition, FAttachmentTransformRules::SnapToTargetIncludingScale);
		TraceCannon->OnExpEventTrace.AddUObject(this, &ATankPawn::TakeExp);

		FlameCannon1->AttachToComponent(CannonFlamePosition1, FAttachmentTransformRules::SnapToTargetIncludingScale);
		FlameCannon1->OnExpEventFlame.AddUObject(this, &ATankPawn::TakeExp);

		FlameCannon2->AttachToComponent(CannonFlamePosition2, FAttachmentTransformRules::SnapToTargetIncludingScale);
		FlameCannon2->OnExpEventFlame.AddUObject(this, &ATankPawn::TakeExp);
	}

	AudioEffectMove_1->Play();
}

void ATankPawn::Destroyed()
{
	Super::Destroyed();

	if (Cannon)
	{
		Cannon->Destroy();
	}

	if (TraceCannon)
	{
		TraceCannon->Destroy();
	}

	if (FlameCannon1 && FlameCannon2)
	{
		FlameCannon1->Destroy();
		FlameCannon2->Destroy();
	}
}

// Called every frame
void ATankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Energy < 100000)
	{
		if (Energy == 0)
			GEngine->AddOnScreenDebugMessage(1, 1, FColor::Red, FString::Printf(TEXT("%d / %d no energy"), Energy, QuantityEnergy));
		else if (TempEnergy * 0.2 >= Energy)
			GEngine->AddOnScreenDebugMessage(1, 1, FColor::Orange, FString::Printf(TEXT("%d / %d little energy"), Energy, QuantityEnergy));
		else 
			GEngine->AddOnScreenDebugMessage(1, 1, FColor::Blue, FString::Printf(TEXT("%d / %d tank enegry"), Energy, QuantityEnergy));
	}

	MoveTank(DeltaTime);

	RotationTank(DeltaTime);
	
	RotationCannon(DeltaTime);

	// лазерный прицел танка
	if (auto Temp = Cast<AEnemyTankPawn>(this) == nullptr)
	{
		FHitResult Result;
		FCollisionObjectQueryParams Params;
		Params.AddObjectTypesToQuery(ECollisionChannel::ECC_Vehicle);
		Params.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
		Params.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
		FVector Start = { TurretMesh->GetComponentLocation().X, TurretMesh->GetComponentLocation().Y, TurretMesh->GetComponentLocation().Z + 5 };
		auto End = TurretMesh->GetForwardVector() * 10000 + Start;
		bool bHasHit = GetWorld()->LineTraceSingleByObjectType(Result, Start, End, Params);

		if (bHasHit)
		{
			End = Result.Location;
			auto Unit = Cast<AUnitPawn>(Result.GetActor());
		}

		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, -1, 0, 1);
	}
}

void ATankPawn::TakeDamage(FDamageData Damage)
{
	if (HealthComponent)
		HealthComponent->TakeDamageTank(Damage);
}

void ATankPawn::TakeExp(FExpData ExpData)
{	
	Experience += ExpData.ExperienceValue;
	GEngine->AddOnScreenDebugMessage(1023432, 3, FColor::Green, FString::Printf(TEXT("Experience: %f"), Experience));
}

void ATankPawn::OnConstrution(const FTransform& Transform)
{
	Super::OnConstrution(Transform);
}

void ATankPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	TankController = Cast<ATankPlayerController>(NewController);

	// для контроллера танк-врага
	TargetController = Cast<ITargetController>(NewController);
}

void ATankPawn::MoveForward(float Value)
{	
	if (FMath::IsNearlyEqual(Value, 0.0f))
	{
		MoveSForwardScaleTarget = Value;
			
	}

	if (!FMath::IsNearlyEqual(Value, 0.0f))
	{
		if (Energy > 0)
			--Energy;
		MoveSForwardScaleTarget = Value;
		bMove = true;
	}
}

void ATankPawn::MoveRight(float Value)
{
	if (FMath::IsNearlyEqual(Value, 0.0f))
	{
		MoveRightScaleTarget = Value;
	}

	if (!FMath::IsNearlyEqual(Value, 0.0f))
	{
		if (Energy > 0)
			--Energy;
		MoveRightScaleTarget = Value;
		bMove = true;
	}
}

void ATankPawn::RotationRight(float Value)
{
	if (FMath::IsNearlyEqual(Value, 0.0f))
	{
		RotationRightScaleTarget = Value;
	}

	if (!FMath::IsNearlyEqual(Value, 0.0f))
	{		
		if (Energy > 0)
			--Energy;
		RotationRightScaleTarget = Value;
		bMove = true;
	}
}

void ATankPawn::RotationTurretScale(float Value)
{
	TurretRightScaleTarget = Value;
	
	if (!FMath::IsNearlyEqual(Value, 0.0f))
	{
		bMousePosition = false;
	}
}

void ATankPawn::lastPush(float Value)
{
	if (FMath::IsNearlyEqual(Value, 0.0f))
			lastPushForward = Value;
	
	if (!FMath::IsNearlyEqual(Value, 0.0f) && bMove)
	{
		if (Energy > 0)
			if (Energy < 10)
				Energy = 0;
			else
				Energy -= 10;
		lastPushForward = Value * lastPushSpeed;
		bMove = false;
	}
}

void ATankPawn::Shoot()
{
	if (Cannon)
	{
		Cannon->Shoot();
	}
}

void ATankPawn::ShootTrace()
{
	if (TraceCannon)
	{
		if (!TraceCannon->bReadyTOShoot)
			return;
		
		if (Energy <= TempEnergy * 0.3)
			GEngine->AddOnScreenDebugMessage(123579676, 3, FColor::Red, FString::Printf(TEXT("Very little enegry")));
		
		if (Energy != 0 && Energy >= TempEnergy * 0.3)
		{
			Energy -= TempEnergy * 0.2;
			TraceCannon->Shoot();
		}
	}
}

void ATankPawn::FlameShoot(float Value)
{
	if (FlameCannon1 && FlameCannon2)
	{
		if (bOverheat)
			return;
		
		if (Fuel == 0)
			return;
		
		if (!FMath::IsNearlyEqual(Value, 0.0f))
		{
			--Fuel;
			--Overheat;

			FlameCannon1->Shoot(Value, Overheat);
			FlameCannon2->Shoot(Value, Overheat);

			if (Overheat == 0)
			{
				bOverheat = true;
				GetWorld()->GetTimerManager().SetTimer(TimerFlame, FTimerDelegate::CreateUObject(this, &ATankPawn::OverheatCannon), 1 / CanCol, false);
				GEngine->AddOnScreenDebugMessage(22345262, 2, FColor::Red, FString::Printf(TEXT("Overheat!!!")));
			}
		}
	}

	if (Fuel == 0)
		GEngine->AddOnScreenDebugMessage(2, 10, FColor::Red, FString::Printf(TEXT("%d / %d no fuel"), Fuel, QuantityFuel));
	else
		GEngine->AddOnScreenDebugMessage(2, 10, FColor::Blue, FString::Printf(TEXT("%d / %d flamethrower fuel"), Fuel, QuantityFuel));
}

void ATankPawn::OverheatCannon()
{
	bOverheat = false;
	Overheat = TempOverheat;
}

void ATankPawn::StopSoundFlame()
{
	if (FlameCannon1 && FlameCannon2)
	{
			FlameCannon1->StopSoundFlame();
			FlameCannon2->StopSoundFlame();

			Overheat = TempOverheat;
	}
}

void ATankPawn::SetupCannon(const TSubclassOf<ACannon>& CannonClass)
{
	if (Cannon)
		Cannon->Destroy();
	
	CannonType = CannonClass;

	if (CannonType)
	{
		auto Transform = CannonPosition->GetComponentTransform(); 

		Cannon = GetWorld()->SpawnActor<ACannon>(CannonType, Transform);

		Cannon->TickInfo(true);

		Cannon->AttachToComponent(CannonPosition, FAttachmentTransformRules::SnapToTargetIncludingScale); 
		Cannon->OnExpEventCannon.AddUObject(this, &ATankPawn::TakeExp);
	}
}

void ATankPawn::ShootEnemyTank()
{
	if (Cannon)
		Cannon->ShootEnemyTank();
}

void ATankPawn::RechargeFlameTimer()
{
	GetWorld()->GetTimerManager().SetTimer(TimerFlame, this, &ATankPawn::RechargeFlame, 0.02, true);
}

void ATankPawn::StopFlameTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerFlame);
}

void ATankPawn::RechargeFlame()
{
	if (Fuel != QuantityFuel)
	{
		++Fuel;
	}
}

void ATankPawn::ChargingStationTimer()
{
	GetWorld()->GetTimerManager().SetTimer(TimerStation, this, &ATankPawn::ChargingStation, 0.02, true);
}

void ATankPawn::StopStationTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerStation);
}

void ATankPawn::ChargingStation()
{
	if (Energy != QuantityEnergy)
	{
		Energy += 100;

		if (Energy > QuantityEnergy)
			Energy = QuantityEnergy;
	}
}

void ATankPawn::MoveTank(float DeltaTime)
{
	if (!FMath::IsNearlyEqual(MoveSForwardScaleTarget, 0.0f) || !FMath::IsNearlyEqual(MoveRightScaleTarget, 0.0f) || !FMath::IsNearlyEqual(RotationRightScaleTarget, 0.0f))
	{
		AudioEffectMove_1->Stop();
		if (!AudioEffectMove_2->IsActive())
			AudioEffectMove_2->Play();
	}
	else
	{
		AudioEffectMove_2->Stop();
		if (!AudioEffectMove_1->IsActive())
			AudioEffectMove_1->Play();
	}

	MoveSForwardScaleCurrent = FMath::Lerp(MoveSForwardScaleCurrent, MoveSForwardScaleTarget, MovementAcceleration);
	MoveRightScaleCurrent = FMath::Lerp(MoveRightScaleCurrent, MoveRightScaleTarget, MovementAcceleration);

	const auto Location = GetActorLocation();

	if (Energy == 0)
	{
		MovementSpeed = FMath::Lerp(MovementSpeed, 0.f, MovementAcceleration);
	}

	if (lastPushForward)
		SetActorLocation(Location + ((GetActorForwardVector() * MoveSForwardScaleCurrent + GetActorRightVector() * MoveRightScaleCurrent) * lastPushForward) * MovementSpeed * DeltaTime, false);
	else 
		SetActorLocation(Location + ((GetActorForwardVector() * MoveSForwardScaleCurrent + GetActorRightVector() * MoveRightScaleCurrent)) * MovementSpeed * DeltaTime, false);

	//UE_LOG(LogTanks, Warning, TEXT("%s"), *Location.ToString());
	
	//GEngine->AddOnScreenDebugMessage(123423423, 10, FColor::Red, FString::Printf(TEXT("%f - %f - %f - %f - %f"), DeltaTime, MoveSForwardScaleCurrent, MoveRightScaleCurrent, lastPushForward, MovementSpeed));
}

void ATankPawn::RotationTank(float DeltaTime)
{
	/*if (Energy > 0)
	{*/
		if (!FMath::IsNearlyEqual(MoveSForwardScaleTarget, 0.0f) || !FMath::IsNearlyEqual(MoveRightScaleTarget, 0.0f) || !FMath::IsNearlyEqual(RotationRightScaleTarget, 0.0f))
		{
			AudioEffectMove_1->Stop();
			if (!AudioEffectMove_2->IsActive())
				AudioEffectMove_2->Play();
		}
		else
		{
			AudioEffectMove_2->Stop();
			if (!AudioEffectMove_1->IsActive())
				AudioEffectMove_1->Play();
		}

		RotationRightScaleCurrent = FMath::Lerp(RotationRightScaleCurrent, RotationRightScaleTarget, RotationAcceleration);

		if (Energy == 0)
		{
			RotationSpeed = FMath::Lerp(RotationSpeed, 0.f, MovementAcceleration);
		}

		auto Rotation = GetActorRotation();

		//Rotation.Yaw += RotationRightScaleCurrent * RotationSpeed * DeltaTime;

		if (lastPushForward)
			Rotation.Yaw += RotationRightScaleCurrent * RotationSpeed * lastPushForward * DeltaTime;
		else
			Rotation.Yaw += RotationRightScaleCurrent * RotationSpeed * DeltaTime;


		SetActorRotation(Rotation);

		//UE_LOG(LogTanks, Warning, TEXT("%s"), *Rotation.ToString());
	//}
}

void ATankPawn::RotationCannon(float DeltaTime)
{
	if (Energy > 0 && TurretMesh)
	{
		if (!TargetController)
			return;

		if (!bMousePosition)
		{
			TurretRightScaleCurrent = FMath::Lerp(TurretRightScaleCurrent, TurretRightScaleTarget, RotationAcceleration);

			auto OldRotation = TurretMesh->GetComponentRotation();
			auto BodyMeshRotation = BodyMesh->GetComponentRotation();

			auto NewRotation = OldRotation;

			NewRotation.Yaw += TurretRightScaleCurrent * RotationSpeed;

			OldTrurretRotation = OldRotation.Yaw;
			NewTrurretRotation = NewRotation.Yaw;

			if (OldTrurretRotation != NewTrurretRotation)
			{
				if (!AudioEffectRotateTurret->IsActive())
				{
					AudioEffectRotateTurret->Play();
					IsTurretRotation = true;
				}
			}

			if (OldTrurretRotation == NewTrurretRotation)
			{
				AudioEffectRotateTurret->Stop();

				if (!AudioEffectStopRotateTurret->IsActive() && IsTurretRotation)
				{
					AudioEffectStopRotateTurret->Play();
					IsTurretRotation = false;
				}
			}

			OldRotation.Pitch = BodyMeshRotation.Pitch;
			OldRotation.Roll = BodyMeshRotation.Roll;

			TurretMesh->SetWorldRotation(FMath::Lerp(OldRotation, NewRotation, TurretAcceleration));
		}
		else
		{
			auto OldRotation = TurretMesh->GetComponentRotation();
			auto BodyMeshRotation = BodyMesh->GetComponentRotation();

			FRotator TurretRotation = UKismetMathLibrary::FindLookAtRotation(TurretMesh->GetComponentLocation(), TargetController->GetShootTarget());

			OldTrurretRotation = OldRotation.Yaw;
			NewTrurretRotation = TurretRotation.Yaw;

			if (OldTrurretRotation != NewTrurretRotation)
			{
				if (!AudioEffectRotateTurret->IsActive())
				{
					AudioEffectRotateTurret->Play();
						IsTurretRotation = true;
				}
			}

			if (OldTrurretRotation == NewTrurretRotation)
			{
				AudioEffectRotateTurret->Stop();

				if (!AudioEffectStopRotateTurret->IsActive() && IsTurretRotation)
				{
					AudioEffectStopRotateTurret->Play();
					IsTurretRotation = false;
				}
			}

			OldRotation.Yaw = TurretRotation.Yaw;

			OldRotation.Pitch = BodyMeshRotation.Pitch;
			OldRotation.Roll = BodyMeshRotation.Roll;

			TurretMesh->SetWorldRotation(FMath::Lerp(TurretMesh->GetComponentRotation(), OldRotation, TurretAcceleration));
		}
	}

	//if (auto Temp = Cast<AEnemyTankPawn>(this) == nullptr)
	//{
	//	// лазерный прицел танка
	//	FHitResult Result;
	//	FCollisionObjectQueryParams Params;
	//	Params.AddObjectTypesToQuery(ECollisionChannel::ECC_Vehicle);
	//	Params.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
	//	Params.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	//	FVector Start = { TurretMesh->GetComponentLocation().X, TurretMesh->GetComponentLocation().Y, TurretMesh->GetComponentLocation().Z + 5 };
	//	auto End = TurretMesh->GetForwardVector() * 10000 + Start;
	//	bool bHasHit = GetWorld()->LineTraceSingleByObjectType(Result, Start, End, Params);

	//	if (bHasHit)
	//	{
	//		End = Result.Location;
	//		auto Unit = Cast<AUnitPawn>(Result.GetActor());
	//	}

	//	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.04, 0, 0.7);
	//}
}

void ATankPawn::OnDeath()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
}

void ATankPawn::OnHealthChanged(float CurrentHealthTank)
{
	// когда изменится здоровьк выведем на экран сообщение 
	GEngine->AddOnScreenDebugMessage(10, 10, FColor::Red, FString::Printf(TEXT("Health Player: %f"), CurrentHealthTank));

	AudioEffect->Play();
}
