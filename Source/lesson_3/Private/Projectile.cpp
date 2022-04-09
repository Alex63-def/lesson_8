// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "DamageTarget.h"
#include "UnitPawn.h"
#include "lesson_3/lesson_3.h"
#include "BaseFactory.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBeginOverlap);

	AudioEffect = CreateDefaultSubobject<UAudioComponent>("AudioEffect");
	AudioEffect->SetupAttachment(Mesh);

	ShootEffect = CreateDefaultSubobject<UParticleSystemComponent>("ShootEffect");
	ShootEffect->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AProjectile::MoveTick, MoveRate, true); 

	GetWorld()->GetTimerManager().SetTimer(TimerDestruction, this, &AProjectile::SelfDestruction, SelfDestructionSec, false);
}
													
void AProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this)
		return;
	GetWorld()->GetTimerManager().ClearTimer(Timer);
	
	// сделаем что бы уничтожение было только при достижении хп = 0
	auto Target = Cast<IDamageTarget>(OtherActor);
	if (Target) // если может получить урон
	{
		FDamageData DamageData;
		DamageData.DamageValue = Damage;
		DamageData.Instigator = this;
		Target->TakeDamage(DamageData);
	}

	if (auto Unit = Cast<AUnitPawn>(OtherActor))
	{
		if (Unit->bDeath)
		{
			Unit->bDeath = false;

			FExpData ExpData;
			ExpData.ExperienceValue = Unit->Experience;
			ExpData.Enemy = OtherActor;

			if (OnExpEventProjectile.IsBound())
				OnExpEventProjectile.Broadcast(ExpData);

			if (OnKillEvent.IsBound())
				OnKillEvent.Broadcast();
		}
	}
	else if (auto Unitt = Cast<ABaseFactory>(OtherActor))
	{
		if (Unitt->bDeath)
		{
			Unitt->bDeath = false;

			FExpData ExpData;
			ExpData.ExperienceValue = Unitt->Experience;
			ExpData.Enemy = OtherActor;

			if (OnExpEventProjectile.IsBound())
				OnExpEventProjectile.Broadcast(ExpData);

			if (OnKillEvent.IsBound())
				OnKillEvent.Broadcast();
		}
	}

	auto Temp = GetActorLocation();
	SetActorLocation({-100, -100, -100});
	ShootEffect->SetWorldLocation(Temp);
	AudioEffect->SetWorldLocation(Temp);
	AudioEffect->Play();
	ShootEffect->ActivateSystem();
	//Destroy(); 
}

void AProjectile::MoveTick()
{
	const auto Location = GetActorLocation();
	
	SetActorLocation(Location + GetActorForwardVector() * MoveRate * MoveSpeed, true); 
}

void AProjectile::SelfDestruction()
{
	/*AudioEffect->Play();
	ShootEffect->ActivateSystem();*/
	Destroy();
}