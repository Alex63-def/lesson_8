// Fill out your copyright notice in the Description page of Project Settings.


#include "Cannon.h"
#include "DrawDebugHelpers.h"


// Sets default values
ACannon::ACannon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>("ProjectileSpawnPoint");
	ProjectileSpawnPoint->SetupAttachment(RootComponent);

	// визуальный компонент тоже крепится к какой-то точке и в ней выполняет эффект
	AudioEffectProjectile = CreateDefaultSubobject<UAudioComponent>("AudioEffectProjectile");
	AudioEffectProjectile->SetupAttachment(Mesh);

	AudioEffectRocket = CreateDefaultSubobject<UAudioComponent>("AudioEffectRocket");
	AudioEffectRocket->SetupAttachment(Mesh);

	// аудио компонент можно разместить будет куда угодно и оттуда будет слышен звук
	ShootEffectProjectile_1 = CreateDefaultSubobject<UParticleSystemComponent>("ShootEffectProjectile_1");
	ShootEffectProjectile_1->SetupAttachment(Mesh);

	ShootEffectProjectile_2 = CreateDefaultSubobject<UParticleSystemComponent>("ShootEffectProjectile_2");
	ShootEffectProjectile_2->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void ACannon::BeginPlay()
{
	Super::BeginPlay();
	
	Projectile = QuantityProjectile;
	Rocket = QuantityRocket;
	QuantityEnemyTurret = QuantityEnemyTurretProjectile;
	QuantityEnemyTank = QuantityEnemyTankProjectile;
}

// Called every frame
void ACannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bTickInfoCannon)
	{
		if (Type == ECannonType::FireProjectile)
			GEngine->AddOnScreenDebugMessage(3, 0.1, FColor::Blue, FString::Printf(TEXT("%f - %d / %d Projectile"), GetWorld()->GetTimerManager().GetTimerElapsed(TimerHandleShoot), Projectile, QuantityProjectile), false);
		else
			GEngine->AddOnScreenDebugMessage(3, 0.1, FColor::Blue, FString::Printf(TEXT("%f - %d / %d Rocket"), GetWorld()->GetTimerManager().GetTimerElapsed(TimerHandleShoot), Rocket, QuantityRocket), false);
	}
}

void ACannon::TickInfo(bool bTickInfo)
{
	bTickInfoCannon = bTickInfo;
}


void ACannon::Shoot()
{
	if (!bReadyTOShoot)
		return;

	if (Projectile != 0 && Rocket != 0)
	{
		switch (Type)
		{
		case ECannonType::FireProjectile:
			ProjectileKill = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
			AudioEffectProjectile->Play();
			ShootEffectProjectile_1->ActivateSystem();
			ShootEffectProjectile_2->ActivateSystem();
			// проверка на то что выстрел сделал игрок а не другие иначе камера от всех будет трястись
			/*if (GetOwner() == GetWorld()->GetFirstPlayerController()->GetPawn()) // закомментировал так как с этим не работало 
			{*/
				if (CameraEffect) // проверяем что она есть
				{
					GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(CameraEffect); // вызываем наш контроллер и прямо в нем вызываем тряску
				}
			//}
			if (ForceFeedbackEffect)
			{
				FForceFeedbackParameters Parameters; // параметры
				Parameters.bLooping = false; // не зацикливаем
				GetWorld()->GetFirstPlayerController()->ClientPlayForceFeedback(ForceFeedbackEffect, Parameters);
			}
			--Projectile;
			ProjectileKill->OnKillEvent.AddUObject(this, &ACannon::OnKillEvent);
			ProjectileKill->OnExpEventProjectile.AddUObject(this, &ACannon::OnExpEvent);
			break;
		case ECannonType::FireRocket:
			RocketKill = GetWorld()->SpawnActor<ARocket>(RocketClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
			AudioEffectRocket->Play();
			--Rocket;
			RocketKill->OnKillEvent.AddUObject(this, &ACannon::OnKillEvent);
			RocketKill->OnExpEventRocket.AddUObject(this, &ACannon::OnExpEvent);
			break;
		}

		bReadyTOShoot = false;

		GetWorld()->GetTimerManager().SetTimer(TimerHandleShoot, FTimerDelegate::CreateUObject(this, &ACannon::ResetShootState), 1 / FireRate, false);
	}
}

void ACannon::ShootTurret()
{
	if (!bReadyTOShoot)
		return;
	
	if (!bReadyRecharge)
		return;

	switch (Type)
	{
	case ECannonType::FireProjectile:
		GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
		AudioEffectProjectile->Play();
		ShootEffectProjectile_1->ActivateSystem();
		ShootEffectProjectile_2->ActivateSystem();
		break;
	case ECannonType::FireRocket:
		GetWorld()->SpawnActor<ARocket>(RocketClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
		AudioEffectRocket->Play();
		break;
	}

	bReadyTOShoot = false;

	GetWorld()->GetTimerManager().SetTimer(TimerHandleShoot, FTimerDelegate::CreateUObject(this, &ACannon::ResetShootState), 1 / FireRateTurret, false);

	--QuantityEnemyTurret;
	if (QuantityEnemyTurret == 0)
	{
		bReadyRecharge = false;
		GetWorld()->GetTimerManager().SetTimer(TimerEnemyTurret, FTimerDelegate::CreateUObject(this, &ACannon::RechargeCannon), 1 / FireRateTurretEnemy, false);
	}
}

void ACannon::ShootEnemyTank()
{
	if (!bReadyTOShoot)
		return;

	if (!bReadyRecharge)
		return;

	switch (Type)
	{
	case ECannonType::FireProjectile:
		GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
		AudioEffectProjectile->Play();
		ShootEffectProjectile_1->ActivateSystem();
		ShootEffectProjectile_2->ActivateSystem();
		break;
	case ECannonType::FireRocket:
		GetWorld()->SpawnActor<ARocket>(RocketClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
		AudioEffectRocket->Play();
		break;
	}

	bReadyTOShoot = false;

	GetWorld()->GetTimerManager().SetTimer(TimerHandleShoot, FTimerDelegate::CreateUObject(this, &ACannon::ResetShootState), 1 / FireRateTurret, false);

	--QuantityEnemyTank;
	if (QuantityEnemyTank == 0)
	{
		bReadyRecharge = false;
		GetWorld()->GetTimerManager().SetTimer(TimerEnemyTank, FTimerDelegate::CreateUObject(this, &ACannon::RechargeCannon), 1 / FireRateTurretEnemy, false);
	}
}

void ACannon::ResetShootState()
{
	bReadyTOShoot = true;
}

void ACannon::RechargeCannon()
{
	bReadyRecharge = true;

	if (QuantityEnemyTurret == 0)
		QuantityEnemyTurret = QuantityEnemyTurretProjectile;

	if (QuantityEnemyTank == 0)
		QuantityEnemyTank = QuantityEnemyTankProjectile;
}

void ACannon::OnKillEvent()
{
	GEngine->AddOnScreenDebugMessage(11202, 3, FColor::Red, FString::Printf(TEXT("Kill")));
}

void ACannon::OnExpEvent(FExpData ExpData)
{
	if (OnExpEventCannon.IsBound())
		OnExpEventCannon.Broadcast(ExpData);
}
