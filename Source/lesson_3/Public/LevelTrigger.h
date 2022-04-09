// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h" // ��� ����� �������� ���, ������� ��������
#include "Components/SpotLightComponent.h" // � ��� ����� �������� ������ ������������� �������
#include "Particles/ParticleSystemComponent.h" // ��� ����������� �������
#include "Components/AudioComponent.h" // ��� �����
#include "LevelTrigger.generated.h"

UCLASS()
class LESSON_3_API ALevelTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelTrigger();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UBoxComponent* TriggerBox;

	// ������� � ������� ����� ������� ����� ����������
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		USpotLightComponent* InactiveLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		USpotLightComponent* ActiveLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UPointLightComponent* PointActiveLight;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UParticleSystemComponent* VisualEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Level")
		bool IsActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AllEnemy")
		bool bAllEnemyDestroyed = false;

	// ��� ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
		FName LevelName;

	// ����� ������ ���� �����
	void SetActive(bool Active);

	// ����������� ���-�� ������ � ����
	void FindEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bActiveAudio = false;

	uint32 Quantity = 0;
};
