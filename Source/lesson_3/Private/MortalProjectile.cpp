// Fill out your copyright notice in the Description page of Project Settings.


#include "MortalProjectile.h"

AMortalProjectile::AMortalProjectile()
{
	// подключение физики
	Collision->SetSimulatePhysics(true); // коллизия из базового класса 

	// настроим коллизию что бы генерировались хит эвенты при столкновении
	//тут он решил собрать проект и идти дальше, что-то не получилось

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent"); // аттачить его ни к чему не нужно это просто компонент
	
}