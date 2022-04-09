// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTrigger.h"
#include "Kismet/GameplayStatics.h"
#include "UnitPawn.h"
#include "BaseFactory.h"

// Sets default values
ALevelTrigger::ALevelTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>("TriggerBox");
	RootComponent = TriggerBox;
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelTrigger::OnBeginOverlap);

	InactiveLight = CreateDefaultSubobject<USpotLightComponent>("InactiveLight");
	InactiveLight->SetupAttachment(RootComponent);

	ActiveLight = CreateDefaultSubobject<USpotLightComponent>("ActiveLight");
	ActiveLight->SetupAttachment(RootComponent);

	PointActiveLight = CreateDefaultSubobject<UPointLightComponent>("PointActiveLight");
	PointActiveLight->SetupAttachment(RootComponent);

	AudioEffect = CreateDefaultSubobject<UAudioComponent>("AudioEffect");
	AudioEffect->SetupAttachment(RootComponent);

	VisualEffect = CreateDefaultSubobject<UParticleSystemComponent>("VisualEffect");
	VisualEffect->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ALevelTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	VisualEffect->ActivateSystem();
	PointActiveLight->Activate();

	/*TArray<AActor*> ActorsEnemy;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnitPawn::StaticClass(), ActorsEnemy);

	TArray<AActor*> ActorsFactory;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseFactory::StaticClass(), ActorsFactory);

	AActor* TempActor;
	for (auto Actor : ActorsEnemy)
	{
		if (Actor == GetWorld()->GetFirstPlayerController()->GetPawn())
			TempActor = Actor;
	}
	ActorsEnemy.Remove(TempActor);

	Quantity += (ActorsEnemy.GetTypeSize() + ActorsFactory.GetTypeSize()) / 4;
	GEngine->AddOnScreenDebugMessage(425623, 123, FColor::Blue, FString::Printf(TEXT("%d"), Quantity), false);*/
}

// Called every frame
void ALevelTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bActiveAudio)
	{
		AudioEffect->Play();
		bActiveAudio = false;
	}

	FindEnemy();

	if (bAllEnemyDestroyed)
		if(Quantity == 0)
			SetActive(true);
}

void ALevelTrigger::FindEnemy()
{
	TArray<AActor*> ActorsEnemy;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnitPawn::StaticClass(), ActorsEnemy);

	TArray<AActor*> ActorsFactory;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseFactory::StaticClass(), ActorsFactory);

	AActor* TempActor;
	for (auto Actor : ActorsEnemy)
	{
		if (Actor == GetWorld()->GetFirstPlayerController()->GetPawn())
			TempActor = Actor;
	}
	ActorsEnemy.Remove(TempActor);

	Quantity = ActorsEnemy.Num() + ActorsFactory.Num();
	//GEngine->AddOnScreenDebugMessage(425623, 123, FColor::Blue, FString::Printf(TEXT("%d"), Quantity), false);
}

// переводит нас на новый уровень
void ALevelTrigger::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsActive /*&& !LevelName.IsNone()*/) // если активен тригер и задано имя
	{
		APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn(); // это получение игрока
		if (OtherActor == PlayerPawn) // если заехал игрок - запустить
			if(!LevelName.IsNone())
				UGameplayStatics::OpenLevel(this, LevelName);
			else
				UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
	}
}

void ALevelTrigger::SetActive(bool Active)
{
	InactiveLight->SetHiddenInGame(Active); // прячем неактивный свет если активно
	ActiveLight->SetHiddenInGame(!Active);	// и прячем активный свет если неактивно
	IsActive = Active;
	bActiveAudio = Active;
}