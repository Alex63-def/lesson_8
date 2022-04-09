// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyTankPawn.h"
#include "EnemyAIController.h"

AEnemyTankPawn::AEnemyTankPawn()/* : ATankPawn()*/
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DeathMaterial = CreateDefaultSubobject<UMaterial>(TEXT("DeathMaterial"));

	RangeSphere = CreateDefaultSubobject<USphereComponent>("RangeSphere");
	RangeSphere->SetupAttachment(RootComponent);

	// подписываемся на события этой сферы
	// на то что зашел какой-то юнит в ее область
	RangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyTankPawn::OnBeginOverlap);

	// на - не сказал - но думаю на выходе из области перестанет стрелять 
	RangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyTankPawn::OnEndOverlap);

	AudioDeathEffectt = CreateDefaultSubobject<UAudioComponent>("AudioDeathEffectt");
	AudioDeathEffectt->SetupAttachment(BodyMesh);

	DeathEffectt = CreateDefaultSubobject<UParticleSystemComponent>("DeathEffectt");
	DeathEffectt->SetupAttachment(BodyMesh);

	AudioEffect_FlameDeath = CreateDefaultSubobject<UAudioComponent>("AudioEffect_FlameDeath");
	AudioEffect_FlameDeath->SetupAttachment(BodyMesh);

	DeathEffect_Flame = CreateDefaultSubobject<UParticleSystemComponent>("DeathEffect_Flame");
	DeathEffect_Flame->SetupAttachment(BodyMesh);
}

void AEnemyTankPawn::BeginPlay()
{
	Super::BeginPlay();

	Experience = 100;
}

void AEnemyTankPawn::OnConstrution(const FTransform& Transform)
{
	Super::OnConstrution(Transform);

	// это для примера выводим сколько танков спавнилось
	GEngine->AddOnScreenDebugMessage(987457, 3, FColor::Red, FString::Printf(TEXT("%d - Tank created"), Number));
}

void AEnemyTankPawn::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto AiController = Cast<AEnemyAIController>(GetController());
	if (AiController)
		if (!Cast<AEnemyTankPawn>(OtherActor))
			AiController->AddTarget(OtherActor);
}

void AEnemyTankPawn::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto AiController = Cast<AEnemyAIController>(GetController());
	if (AiController)
		AiController->RemoveTarget(OtherActor);
}

void AEnemyTankPawn::OnHealthChanged(float CurrentHealthTank)
{
	// когда изменится здоровьк выведем на экран сообщение 
	GEngine->AddOnScreenDebugMessage(103456341, 3, FColor::Red, FString::Printf(TEXT("Health Enemy Tank: %f"), CurrentHealthTank));
}

void AEnemyTankPawn::SetupCannon(const TSubclassOf<ACannon>& CannonClass)
{
	if (Cannon)
		Cannon->Destroy();

	CannonType = CannonClass;

	if (CannonType)
	{
		auto Transform = CannonPosition->GetComponentTransform();

		Cannon = GetWorld()->SpawnActor<ACannon>(CannonType, Transform);

		Cannon->TickInfo(false);

		Cannon->AttachToComponent(CannonPosition, FAttachmentTransformRules::SnapToTargetIncludingScale);
		Cannon->OnExpEventCannon.AddUObject(this, &ATankPawn::TakeExp);
	}
}

void AEnemyTankPawn::OnDeath()
{
	/*auto Temp = GetActorLocation();
	AudioDeathEffectt->Play();
	DeathEffectt->ActivateSystem();
	SetActorLocation({ -1000, -1000, -1000 });
	DeathEffectt->SetWorldLocation(Temp);
	AudioDeathEffectt->SetWorldLocation(Temp);*/

	HealthComponent->OnDeath.Clear();

	bDeath = true;
	bDeath2 = true;
	
	TurretMesh->DestroyComponent();
	//CannonPosition->DestroyComponent();
	//CannonTracePosition->DestroyComponent();
	//CannonFlamePosition1->DestroyComponent();
	Cannon->Destroy();
	TraceCannon->Destroy();
	FlameCannon1->Destroy();
	FlameCannon2->Destroy();
	//CannonFlamePosition2->DestroyComponent();
	RangeSphere->DestroyComponent();

	BodyMesh->SetMaterial(0, DeathMaterial);
	
	AudioDeathEffectt->Play();
	DeathEffectt->ActivateSystem();
	AudioEffect_FlameDeath->Play();
	DeathEffect_Flame->ActivateSystem();

	// активируем триггер
	if (LevelTrigger)
	{
		LevelTrigger->SetActive(true);
	}

	GetWorld()->GetTimerManager().SetTimer(TimerDestruction, this, &AEnemyTankPawn::SelfDestruction, 20, false);
}

void AEnemyTankPawn::SelfDestruction()
{
	Destroy();
}
