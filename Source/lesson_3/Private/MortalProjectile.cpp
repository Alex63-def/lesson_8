// Fill out your copyright notice in the Description page of Project Settings.


#include "MortalProjectile.h"

AMortalProjectile::AMortalProjectile()
{
	// ����������� ������
	Collision->SetSimulatePhysics(true); // �������� �� �������� ������ 

	// �������� �������� ��� �� �������������� ��� ������ ��� ������������
	//��� �� ����� ������� ������ � ���� ������, ���-�� �� ����������

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent"); // �������� ��� �� � ���� �� ����� ��� ������ ���������
	
}