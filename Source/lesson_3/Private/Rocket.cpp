// Fill out your copyright notice in the Description page of Project Settings.


#include "Rocket.h"
#include "DamageTarget.h"
#include "UnitPawn.h"
#include "BaseFactory.h"

// Sets default values
ARocket::ARocket()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	RootComponent = Collision;

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ARocket::OnBeginOverlap);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

	AudioEffect = CreateDefaultSubobject<UAudioComponent>("AudioEffect");
	AudioEffect->SetupAttachment(Mesh);

	AudioEffect_2 = CreateDefaultSubobject<UAudioComponent>("AudioEffect_2");
	AudioEffect_2->SetupAttachment(Mesh);

	ShootEffect = CreateDefaultSubobject<UParticleSystemComponent>("ShootEffect");
	ShootEffect->SetupAttachment(Mesh);

	ShootEffect_2 = CreateDefaultSubobject<UParticleSystemComponent>("ShootEffect_2");
	ShootEffect_2->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void ARocket::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &ARocket::MoveTick, MoveRate, true);

	GetWorld()->GetTimerManager().SetTimer(TimerDestruction, this, &ARocket::SelfDestruction, SelfDestructionSec, false);

	AudioEffect->Play();
	ShootEffect->ActivateSystem();
}

void ARocket::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

			if (OnExpEventRocket.IsBound())
				OnExpEventRocket.Broadcast(ExpData);

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

			if (OnExpEventRocket.IsBound())
				OnExpEventRocket.Broadcast(ExpData);

			if (OnKillEvent.IsBound())
				OnKillEvent.Broadcast();
		}
	}

	AudioEffect->Stop();
	ShootEffect->DeactivateSystem();

	auto Temp = GetActorLocation();
	SetActorLocation({ -500, -500, -500 });
	ShootEffect_2->SetWorldLocation(Temp);
	AudioEffect_2->SetWorldLocation(Temp);
	AudioEffect_2->Play();
	ShootEffect_2->ActivateSystem();
	//Destroy();
}

void ARocket::MoveTick()
{
	const auto Location = GetActorLocation();

	SetActorLocation(Location + GetActorForwardVector() * MoveRate * MoveSpeed, true);
}

void ARocket::SelfDestruction()
{
	Destroy();
}