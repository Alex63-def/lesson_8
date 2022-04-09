// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoBox.h"
#include "TankPawn.h"

// Sets default values
AAmmoBox::AAmmoBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; 

	CollisionBegin = CreateDefaultSubobject <UBoxComponent>("CollisionBegin");
	RootComponent = CollisionBegin;

	CollisionEnd = CreateDefaultSubobject <UBoxComponent>("CollisionEnd");
	CollisionEnd->SetupAttachment(RootComponent);

	CollisionBegin->OnComponentBeginOverlap.AddDynamic(this, &AAmmoBox::OnBeginOverlap);
	CollisionEnd->OnComponentEndOverlap.AddDynamic(this, &AAmmoBox::OnEndOverlap);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

	AudioEffect = CreateDefaultSubobject<UAudioComponent>("AudioEffect");
	AudioEffect->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void AAmmoBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAmmoBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAmmoBox::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Tank = Cast<ATankPawn>(OtherActor); 

	if (Tank)
	{
		if (CannonClass != Tank->CannonType)
			Tank->SetupCannon(CannonClass);

		if (Tank->Cannon->Type == ECannonType::FireProjectile && CannonClass == Tank->CannonType)
			if (Tank->Cannon->Projectile != Tank->Cannon->QuantityProjectile)
				++Tank->Cannon->Projectile;

		if (Tank->Cannon->Type == ECannonType::FireRocket && CannonClass == Tank->CannonType)
			if (Tank->Cannon->Rocket != Tank->Cannon->QuantityRocket)
				++Tank->Cannon->Rocket;

		if (FlameClass)
			Tank->RechargeFlameTimer();

		if (!AudioEffect->IsActive())
			AudioEffect->Play();
	}
	
	if (!bInfinite) 
	{
		Destroy();
	}
}

void AAmmoBox::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto Tank = Cast<ATankPawn>(OtherActor);

	if (Tank)
	{
		if (FlameClass)
			Tank->StopFlameTimer();

		AudioEffect->Stop();
	}
}