// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void UHealthComponent::TakeDamageTurret(FDamageData DamageData)
{
	float PrevValue = CurrentHealthTurret;

	CurrentHealthTurret -= DamageData.DamageValue;
	if (CurrentHealthTurret < 0)
	{
		CurrentHealthTurret = 0;
	}

	if (!FMath::IsNearlyEqual(PrevValue, CurrentHealthTurret)) // ���� ���� ��������� � �� �� ���������
		if (OnHealthChanged.IsBound())
			OnHealthChanged.Broadcast(CurrentHealthTurret);


	if (FMath::IsNearlyZero(CurrentHealthTurret)) // �������� ��� ������� �������� ����� �� ���� - ���� ����� ��������� ��� �������� ����� ��������� ����� ������ � ����
	{		
		if (OnDeath.IsBound())
			OnDeath.Broadcast();
	}
}

void UHealthComponent::TakeDamageTank(FDamageData DamageData)
{
	float PrevValue = CurrentHealthTank;

	CurrentHealthTank -= DamageData.DamageValue;
	if (CurrentHealthTank < 0)
	{
		CurrentHealthTank = 0;
	}

	if (!FMath::IsNearlyEqual(PrevValue, CurrentHealthTank)) // ���� ���� ��������� � �� �� ���������
		if (OnHealthChanged.IsBound())
			OnHealthChanged.Broadcast(CurrentHealthTank);


	if (FMath::IsNearlyZero(CurrentHealthTank)) // �������� ��� ������� �������� ����� �� ���� - ���� ����� ��������� ��� �������� ����� ��������� ����� ������ � ����
	{
		if (OnDeath.IsBound())
			OnDeath.Broadcast();
	}
}

void UHealthComponent::TakeDamageFactoty(FDamageData DamageData)
{
	float PrevValue = CurrentHealthFactory;

	CurrentHealthFactory -= DamageData.DamageValue;
	if (CurrentHealthFactory < 0)
	{
		CurrentHealthFactory = 0;
	}

	if (!FMath::IsNearlyEqual(PrevValue, CurrentHealthFactory)) // ���� ���� ��������� � �� �� ���������
		if (OnHealthChanged.IsBound())
			OnHealthChanged.Broadcast(CurrentHealthFactory);


	if (FMath::IsNearlyZero(CurrentHealthFactory)) // �������� ��� ������� �������� ����� �� ���� - ���� ����� ��������� ��� �������� ����� ��������� ����� ������ � ����
	{
		if (OnDeath.IsBound())
			OnDeath.Broadcast();
	}
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealthTurret = MaxHealthTurret;

	CurrentHealthFactory = MaxHealthFactory;

	CurrentHealthTank = MaxHealthTank;
	
}


