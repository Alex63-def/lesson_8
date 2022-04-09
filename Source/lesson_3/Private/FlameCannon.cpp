// Fill out your copyright notice in the Description page of Project Settings.


#include "FlameCannon.h"
#include "DrawDebugHelpers.h"
#include "DamageTarget.h"
#include "UnitPawn.h"
#include "BaseFactory.h"

// Sets default values
AFlameCannon::AFlameCannon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

	FlameSpawnPoint = CreateDefaultSubobject<UArrowComponent>("FlameSpawnPoint");
	FlameSpawnPoint->SetupAttachment(RootComponent);

	AudioEffect_1 = CreateDefaultSubobject<UAudioComponent>("AudioEffect_1");
	AudioEffect_1->SetupAttachment(Mesh);

	AudioEffect_2 = CreateDefaultSubobject<UAudioComponent>("AudioEffect_2");
	AudioEffect_2->SetupAttachment(Mesh);

	ShootEffect = CreateDefaultSubobject<UParticleSystemComponent>("ShootEffect");
	ShootEffect->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void AFlameCannon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFlameCannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFlameCannon::Shoot(float Value, float Overheat)
{
	/*if (bOverheat)
		return;*/
	
	if (!FMath::IsNearlyEqual(Value, 0.0f))
	{
		FHitResult Result;
		FCollisionObjectQueryParams Params;
		Params.AddObjectTypesToQuery(ECollisionChannel::ECC_Vehicle);
		Params.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
		Params.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
		auto Start = FlameSpawnPoint->GetComponentLocation();
		auto End = FlameSpawnPoint->GetForwardVector() * FireRange + Start;
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
						DamageData.DamageValue = FlameDamage;
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

					if (OnExpEventFlame.IsBound())
						OnExpEventFlame.Broadcast(ExpData);

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
						DamageData.DamageValue = FlameDamage;
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

					if (OnExpEventFlame.IsBound())
						OnExpEventFlame.Broadcast(ExpData);

					OnKill();
				}
			}
		}

		if (!AudioEffect_1->IsActive())
			AudioEffect_1->Play();
		ShootEffect->ActivateSystem();
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.1, 0, 0.001);

		/*--Overheat;
		if (Overheat == 0)
		{
			bOverheat = true;
			GetWorld()->GetTimerManager().SetTimer(TimerFlame, FTimerDelegate::CreateUObject(this, &AFlameCannon::OverheatCannon), 1 / CanCol, false);
		}*/

		if (Overheat == 0)
		{
			--Overheat;
			AudioEffect_2->Play();
			AudioEffect_1->Stop();
		}
	}
}

void AFlameCannon::OnKill()
{
	GEngine->AddOnScreenDebugMessage(112025, 3, FColor::Red, FString::Printf(TEXT("Kill")));
}

void AFlameCannon::StopSoundFlame()
{
	AudioEffect_2->Play();
	AudioEffect_1->Stop();
}

//void AFlameCannon::OverheatCannon()
//{
//	bOverheat = false;
//}

