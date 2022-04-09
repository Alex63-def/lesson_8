// Fill out your copyright notice in the Description page of Project Settings.


#include "Station.h"
#include "TankPawn.h"

// Sets default values
AStation::AStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionBegin = CreateDefaultSubobject <UBoxComponent>("CollisionBegin");
	RootComponent = CollisionBegin;

	CollisionEnd = CreateDefaultSubobject <UBoxComponent>("CollisionEnd");
	CollisionEnd->SetupAttachment(RootComponent);

	CollisionBegin->OnComponentBeginOverlap.AddDynamic(this, &AStation::OnBeginOverlap);
	CollisionEnd->OnComponentEndOverlap.AddDynamic(this, &AStation::OnEndOverlap);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

	AudioEffect = CreateDefaultSubobject<UAudioComponent>("AudioEffect");
	AudioEffect->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void AStation::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStation::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Tank = Cast<ATankPawn>(OtherActor);

	if (Tank)
	{
		Tank->ChargingStationTimer();

		if (!AudioEffect->IsActive())
			AudioEffect->Play();
	}
}

void AStation::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto Tank = Cast<ATankPawn>(OtherActor);

	if (Tank)
	{
		Tank->StopStationTimer();

		AudioEffect->Stop();
	}
}