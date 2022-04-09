// Fill out your copyright notice in the Description page of Project Settings.


#include "TraceCannon.h"
#include "DrawDebugHelpers.h"
#include "DamageTarget.h"
#include "UnitPawn.h"
#include "BaseFactory.h"

// Sets default values
ATraceCannon::ATraceCannon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

	TraceSpawnPoint = CreateDefaultSubobject<UArrowComponent>("TraceSpawnPoint");
	TraceSpawnPoint->SetupAttachment(RootComponent);

	AudioEffect = CreateDefaultSubobject<UAudioComponent>("AudioEffect");
	AudioEffect->SetupAttachment(Mesh);

	ShootEffect = CreateDefaultSubobject<UParticleSystemComponent>("ShootEffect");
	ShootEffect->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void ATraceCannon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATraceCannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATraceCannon::Shoot()
{
	if (!bReadyTOShoot)
		return;

	AudioEffect->Play();
	ShootEffect->ActivateSystem();

	FHitResult Result;
	FCollisionObjectQueryParams Params;
	Params.AddObjectTypesToQuery(ECollisionChannel::ECC_Vehicle);
	Params.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
	Params.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	auto Start = TraceSpawnPoint->GetComponentLocation();
	auto End = TraceSpawnPoint->GetForwardVector() * FireRange + Start;
	bool bHasHit = GetWorld()->LineTraceSingleByObjectType(Result, Start, End, Params);

	if (bHasHit)
	{ 
		End = Result.Location;
		if (auto Unit = Cast<AUnitPawn>(Result.GetActor()))
		{
			if (Result.Actor.IsValid())
			{
				auto Target = Cast<IDamageTarget>(Result.Actor);
				if (Target)
				{
					FDamageData DamageData;
					DamageData.DamageValue = LaserDamage;
					DamageData.Instigator = this;
					Target->TakeDamage(DamageData);
				}
			}
			//if (!Result.Actor.IsValid())
			if (Unit->bDeath)
			{
				Unit->bDeath = false;

				FExpData ExpData;
				ExpData.ExperienceValue = Unit->Experience;
				ExpData.Enemy = Unit;

				if (OnExpEventTrace.IsBound())
					OnExpEventTrace.Broadcast(ExpData);

				OnKill();
			}
		}
		else if (auto Unitt = Cast<ABaseFactory>(Result.GetActor()))
		{
			if (Result.Actor.IsValid())
			{
				auto Target = Cast<IDamageTarget>(Result.Actor);
				if (Target)
				{
					FDamageData DamageData;
					DamageData.DamageValue = LaserDamage;
					DamageData.Instigator = this;
					Target->TakeDamage(DamageData);
				}
			}
			//if (!Result.Actor.IsValid())
			if (Unitt->bDeath)
			{
				Unitt->bDeath = false;

				FExpData ExpData;
				ExpData.ExperienceValue = Unitt->Experience;
				ExpData.Enemy = Unitt;

				if (OnExpEventTrace.IsBound())
					OnExpEventTrace.Broadcast(ExpData);

				OnKill();
			}
		}
	}
	DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, false, 0.3, 0, 1);

	bReadyTOShoot = false;

	GetWorld()->GetTimerManager().SetTimer(TimerHandleShoot, FTimerDelegate::CreateUObject(this, &ATraceCannon::ResetShootState), 1 / FireRate, false);
}

void ATraceCannon::ResetShootState()
{
	bReadyTOShoot = true;
}

void ATraceCannon::OnKill()
{
	GEngine->AddOnScreenDebugMessage(11202, 3, FColor::Red, FString::Printf(TEXT("Kill")));
}

