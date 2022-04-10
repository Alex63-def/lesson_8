// Fill out your copyright notice in the Description page of Project Settings.


#include "MortalProjectile.h"

AMortalProjectile::AMortalProjectile()
{
	// подключение физики
	Collision->SetSimulatePhysics(true); // коллизия из базового класса 

	// из документации Должны ли срабатывать события «Попадание», когда этот объект сталкивается во время моделирования физики.
	Collision->BodyInstance.bNotifyRigidBodyCollision = true; // видимо это для того чтобы обект

	Mesh->BodyInstance.bNotifyRigidBodyCollision = true;

	// настроим коллизию что бы генерировались хит эвенты при столкновении
	//тут он решил собрать проект и идти дальше, что-то не получилось

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent"); // аттачить его ни к чему не нужно это просто компонент
	
}

void AMortalProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	Destroy();
}
