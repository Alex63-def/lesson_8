// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseFactory.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseFactory::ABaseFactory()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/CSC/Meshes/SM_TANK_Base1.SM_TANK_Base1'"));
	Asset = MeshAsset.Object;

	DeathMaterial = CreateDefaultSubobject<UMaterial>(TEXT("DeathMaterial"));

	// рут сделали по умолчанию
	RootComponent = CreateDefaultSubobject<USceneComponent>("DefaultRoot");

	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>("BuildingMesh");
	BuildingMesh->SetupAttachment(RootComponent);

	HitCollider = CreateDefaultSubobject<UBoxComponent>("HitCollider");
	HitCollider->SetupAttachment(RootComponent);

	SpawnPoint = CreateDefaultSubobject<UArrowComponent>("SpawnPoint");
	SpawnPoint->SetupAttachment(RootComponent);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	HealthComponent->OnHealthChanged.AddUObject(this, &ABaseFactory::OnHealthChanged);
	HealthComponent->OnDeath.AddUObject(this, &ABaseFactory::OnDeath);

	AudioEffect_1 = CreateDefaultSubobject<UAudioComponent>("AudioEffect_1");
	AudioEffect_1->SetupAttachment(BuildingMesh);

	AudioEffect_2 = CreateDefaultSubobject<UAudioComponent>("AudioEffect_2");
	AudioEffect_2->SetupAttachment(BuildingMesh);

	AudioEffect_3 = CreateDefaultSubobject<UAudioComponent>("AudioEffect_3");
	AudioEffect_3->SetupAttachment(BuildingMesh);

	DeathEffect_1 = CreateDefaultSubobject<UParticleSystemComponent>("DeathEffect_1");
	DeathEffect_1->SetupAttachment(BuildingMesh);

	DeathEffect_2 = CreateDefaultSubobject<UParticleSystemComponent>("DeathEffect_2");
	DeathEffect_2->SetupAttachment(BuildingMesh);
}

// Called when the game starts or when spawned
void ABaseFactory::BeginPlay()
{
	Super::BeginPlay();

	// создаем таймер для спавна
	GetWorldTimerManager().SetTimer(Timer, this, &ABaseFactory::OnTankSpawnTick, SpawnInterval, true);

}

// Called every frame
void ABaseFactory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!AudioEffect_1->IsActive() && !bDeath2)
		AudioEffect_1->Play();
}

void ABaseFactory::TakeDamage(FDamageData Damage)
{
	if (HealthComponent)
		HealthComponent->TakeDamageFactoty(Damage);
}

void ABaseFactory::OnTankSpawnTick()
{	
	if (MaxTanks != 0)
	{
		// создает актор но не до конца, этим позволяя нам задать путь
		auto Tank = GetWorld()->SpawnActorDeferred<AEnemyTankPawn>(TankClass, SpawnPoint->GetComponentTransform());
																				// если фабрика имеет меньший размер то и акторы будут меньше
		// теперь можно задать ему параметры которые нужно задать - в констракт - передаем их в танк
		Tank->Waypoints = Waypoints;

		TanksSpawned++;
		Tank->Number = TanksSpawned;

		// заканчиваем создание танка, дальше он спавнится с заданными параметрами
		UGameplayStatics::FinishSpawningActor(Tank, SpawnPoint->GetComponentTransform());

		--MaxTanks;
	}
}													

void ABaseFactory::OnHealthChanged(float CurrentHealthFactory)
{
	GEngine->AddOnScreenDebugMessage(46780, 2, FColor::Red, FString::Printf(TEXT("Health Base Factory: %f"), CurrentHealthFactory));
}

void ABaseFactory::OnDeath()
{
	//Destroy();

	HealthComponent->OnDeath.Clear();

	bDeath = true;
	bDeath2 = true;

	AudioEffect_1->Stop();
	AudioEffect_2->Play();
	AudioEffect_3->Play();

	DeathEffect_1->ActivateSystem();
	DeathEffect_2->ActivateSystem();
	
	auto Temp = BuildingMesh->GetComponentTransform();

	auto Temp2 = BuildingMesh->GetComponentLocation();
	Temp2.Y -= 0.80f;

	auto Temp3 = BuildingMesh->GetComponentTransform();
	Temp3.SetScale3D(static_cast<FVector>(0.7f, 0.7f, 0.5f));
	
	//HitCollider->DestroyComponent();
	HitCollider->SetWorldTransform(Temp3);

	BuildingMesh->SetStaticMesh(Asset);
	BuildingMesh->SetWorldLocation(Temp2);
	//BuildingMesh->GetComponentTransform();
	BuildingMesh->SetMaterial(0, DeathMaterial);

	GetWorldTimerManager().ClearTimer(Timer);

	// активируем триггер
	if (LevelTrigger)
	{
		LevelTrigger->SetActive(true);
	}

	FTimerHandle TempTimer;
	GetWorld()->GetTimerManager().SetTimer(TempTimer, this, &ABaseFactory::SelfDestruction, 20, false);
}

void ABaseFactory::SelfDestruction()
{
	Destroy();
}