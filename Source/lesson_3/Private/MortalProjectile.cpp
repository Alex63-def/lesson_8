// Fill out your copyright notice in the Description page of Project Settings.


#include "MortalProjectile.h"
#include "DamageTarget.h"
#include "UnitPawn.h"
#include "BaseFactory.h"

AMortalProjectile::AMortalProjectile()
{
	// ����������� ������
	Collision->SetSimulatePhysics(true); // �������� �� �������� ������ 

	// �� ������������ ������ �� ����������� ������� ����������, ����� ���� ������ ������������ �� ����� ������������� ������.
	Collision->BodyInstance.bNotifyRigidBodyCollision = true; // ������ ��� ��� ���� ����� �����
	Mesh->BodyInstance.bNotifyRigidBodyCollision = true;

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent"); // �������� ��� �� � ���� �� ����� ��� ������ ���������
	
	BlastRegion = CreateDefaultSubobject<USphereComponent>("BlastRegion");
	BlastRegion->SetupAttachment(RootComponent);
}

void AMortalProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// ��� ��������������� � ������������ ����� ������ ���� �� �������
	TArray<FHitResult> Targets; // ��� ���� ����									�� ������� � ����� ��� ������� ���� � ��� ���������� ������ �����
	GetWorld()->SweepMultiByChannel(Targets, GetActorLocation(), GetActorLocation(), GetActorRotation().Quaternion(), ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(BlastRegion->GetScaledSphereRadius()));

	for (auto& HitTarget : Targets)
	{
		// ������� ��� �� ����������� ���� ������ ��� ���������� �� = 0
		auto Target = Cast<IDamageTarget>(HitTarget.Actor);
		if (Target) // ���� ����� �������� ����
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

		// ���������� ����� ������ ������ ������ �� ���������
		if (Cast<ABaseProjectile>(HitTarget.Actor))
		{
			continue;
		}

		//// ��� ������ ����� - �������� ����������� ������
		//const auto Root = Cast<UPrimitiveComponent>(HitTarget.Actor->GetRootComponent());
		//if (Root && Root->IsSimulatingPhysics())
		//{																// ��� �������� ������� �� ������������ � ����������� �� ������ ������
		//	Root->AddImpulseAtLocation(GetActorForwardVector()*1000/* / FVector::Distance(HitTarget.Location, GetActorLocation())*/, HitTarget.Location);
		//}														// ������� ��� ��� ������-�� �� ��������� ������� � �� �������� ��� ����

		// ���� � �� �� , ������ ���� ������������ �������
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
