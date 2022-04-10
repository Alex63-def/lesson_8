// Fill out your copyright notice in the Description page of Project Settings.


#include "MortalProjectile.h"

AMortalProjectile::AMortalProjectile()
{
	// ����������� ������
	Collision->SetSimulatePhysics(true); // �������� �� �������� ������ 

	// �� ������������ ������ �� ����������� ������� ����������, ����� ���� ������ ������������ �� ����� ������������� ������.
	Collision->BodyInstance.bNotifyRigidBodyCollision = true; // ������ ��� ��� ���� ����� �����

	Mesh->BodyInstance.bNotifyRigidBodyCollision = true;

	// �������� �������� ��� �� �������������� ��� ������ ��� ������������
	//��� �� ����� ������� ������ � ���� ������, ���-�� �� ����������

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent"); // �������� ��� �� � ���� �� ����� ��� ������ ���������
	
}

void AMortalProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	Destroy();
}
