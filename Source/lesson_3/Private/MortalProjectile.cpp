// Fill out your copyright notice in the Description page of Project Settings.


#include "MortalProjectile.h"
#include "DamageTarget.h"
#include "UnitPawn.h"
#include "BaseFactory.h"

AMortalProjectile::AMortalProjectile()
{
	// подключение физики
	Collision->SetSimulatePhysics(true); // коллизия из базового класса 

	// из документации Должны ли срабатывать события «Попадание», когда этот объект сталкивается во время моделирования физики.
	Collision->BodyInstance.bNotifyRigidBodyCollision = true; // видимо это для того чтобы обект
	Mesh->BodyInstance.bNotifyRigidBodyCollision = true;

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent"); // аттачить его ни к чему не нужно это просто компонент
	
	BlastRegion = CreateDefaultSubobject<USphereComponent>("BlastRegion");
	BlastRegion->SetupAttachment(RootComponent);
}

void AMortalProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// при соприкосновении с поверхностью будет делать урон по площади
	TArray<FHitResult> Targets; // это наши цели									на сколько я понял тут находим всех с кем столкнулся радиус урона
	GetWorld()->SweepMultiByChannel(Targets, GetActorLocation(), GetActorLocation(), GetActorRotation().Quaternion(), ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(BlastRegion->GetScaledSphereRadius()));

	for (auto& HitTarget : Targets)
	{
		// сделаем что бы уничтожение было только при достижении хп = 0
		auto Target = Cast<IDamageTarget>(HitTarget.Actor);
		if (Target) // если может получить урон
		{
			FDamageData DamageData;
			DamageData.DamageValue = Damage;
			DamageData.Instigator = this;
			Target->TakeDamage(DamageData);
		}

		if (auto Unit = Cast<AUnitPawn>(HitTarget.Actor))
		{
			if (Unit->bDeath)
			{
				Unit->bDeath = false;

				FExpData ExpData;
				ExpData.ExperienceValue = Unit->Experience;
				ExpData.Enemy = Cast<AActor>(HitTarget.Actor);

				if (OnExpEventProjectile.IsBound())
					OnExpEventProjectile.Broadcast(ExpData);

				if (OnKillEvent.IsBound())
					OnKillEvent.Broadcast();
			}
		}
		else if (auto Unitt = Cast<ABaseFactory>(HitTarget.Actor))
		{
			if (Unitt->bDeath)
			{
				Unitt->bDeath = false;

				FExpData ExpData;
				ExpData.ExperienceValue = Unitt->Experience;
				ExpData.Enemy = Cast<AActor>(HitTarget.Actor);

				if (OnExpEventProjectile.IsBound())
					OnExpEventProjectile.Broadcast(ExpData);

				if (OnKillEvent.IsBound())
					OnKillEvent.Broadcast();
			}
		}

		// исключение чтобы снаряд другой снаряд не уничтожал
		if (Cast<ABaseProjectile>(HitTarget.Actor))
		{
			continue;
		}

		//// это роняет стены - передача физического усилия
		//const auto Root = Cast<UPrimitiveComponent>(HitTarget.Actor->GetRootComponent());
		//if (Root && Root->IsSimulatingPhysics())
		//{																// тут добавили деление на интреполяцию с расстоянием от центра взрыва
		//	Root->AddImpulseAtLocation(GetActorForwardVector()*1000/* / FVector::Distance(HitTarget.Location, GetActorLocation())*/, HitTarget.Location);
		//}														// удалили так как почему-то не правильно считает и не работает как надо

		// одно и то же , просто лень переписывать коменты
		const auto Root = Cast<UPrimitiveComponent>(HitTarget.Actor->GetRootComponent());
		if (Root && Root->IsSimulatingPhysics())
		{
			auto Impulse = GetActorForwardVector() * 10000;
			Root->AddImpulseAtLocation(GetActorForwardVector() * 700, HitTarget.Location);
			//UE_LOG(LogTemp, Warning, TEXT("Force %s"), *Impulse.ToString(), FVector::Distance(HitTarget.Location, GetActorLocation()));
		}
	}

	Destroy();
}
