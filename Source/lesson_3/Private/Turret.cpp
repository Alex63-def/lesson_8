// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "lesson_3/lesson_3.h"
#include "Projectile.h"
#include "EnemyTankPawn.h"

// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DeathMaterial = CreateDefaultSubobject<UMaterial>(TEXT("DeathMaterial"));

	Collision = CreateDefaultSubobject<UCapsuleComponent>("Collision");
	RootComponent = Collision;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>("BodyMesh");
	BodyMesh->SetupAttachment(RootComponent);

	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SceneComponent->SetupAttachment(BodyMesh);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>("TurretMesh");
	TurretMesh->SetupAttachment(SceneComponent);

	CannonPosition = CreateDefaultSubobject<UArrowComponent>("CannonPosition");
	CannonPosition->SetupAttachment(TurretMesh);

	RangeSphere = CreateDefaultSubobject<USphereComponent>("RangeSphere");
	RangeSphere->SetupAttachment(RootComponent);

	// ������������� �� ������� ���� �����
	// �� �� ��� ����� �����-�� ���� � �� �������
	RangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ATurret::OnBeginOverlap);

	// �� - �� ������ - �� ����� �� ������ �� ������� ���������� �������� 
	RangeSphere->OnComponentEndOverlap.AddDynamic(this, &ATurret::OnEndOverlap);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	// ������������� �� ������� ������ ��� ������
	HealthComponent->OnDeath.AddUObject(this, &ATurret::OnDeath);
	HealthComponent->OnHealthChanged.AddUObject(this, &ATurret::OnHealthChanged);

	AudioDeathEffect = CreateDefaultSubobject<UAudioComponent>("AudioDeathEffect");
	AudioDeathEffect->SetupAttachment(BodyMesh);

	DeathEffect = CreateDefaultSubobject<UParticleSystemComponent>("DeathEffect");
	DeathEffect->SetupAttachment(BodyMesh);

	AudioEffectRotateTurret = CreateDefaultSubobject<UAudioComponent>("AudioEffectRotateTurret");
	AudioEffectRotateTurret->SetupAttachment(TurretMesh);

	AudioEffectStopRotateTurret = CreateDefaultSubobject<UAudioComponent>("AudioEffectStopRotateTurret");
	AudioEffectStopRotateTurret->SetupAttachment(TurretMesh);

	AudioEffect_FlameDeath = CreateDefaultSubobject<UAudioComponent>("AudioEffect_FlameDeath");
	AudioEffect_FlameDeath->SetupAttachment(BodyMesh);

	DeathEffect_Flame = CreateDefaultSubobject<UParticleSystemComponent>("DeathEffect_Flame");
	DeathEffect_Flame->SetupAttachment(BodyMesh);
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();

	Experience = 100;

	// ������ ��� ������������ 
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &ATurret::Targeting, TargetingRate, true);

	// ������� ����� 
	if (CannonType)
	{ 
		auto Transform = CannonPosition->GetComponentTransform();

		Cannon = GetWorld()->SpawnActor<ACannon>(CannonType, Transform);

		Cannon->AttachToComponent(CannonPosition, FAttachmentTransformRules::SnapToTargetIncludingScale);
	}
}

void ATurret::Destroyed()
{
	Super::Destroyed();

	if (Cannon)
	{
		Cannon->Destroy();
	}
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATurret::TakeDamage(FDamageData Damage)
{
	if(HealthComponent)
		HealthComponent->TakeDamageTurret(Damage);
}

void ATurret::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!Cast<AEnemyTankPawn>(OtherActor))
	// �������� ����� ������� � ���� ��������� 
		OtherActors.Add(OtherActor);

	// ��������� ���� �� ���� - ���� �� ���, �� �������� ����� ������ ����
	if (!Target.IsValid())
	{
		FindNextTarget();
	}
}

void ATurret::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// ������ ����� ������� � ���� ��������� 
	OtherActors.Remove(OtherActor);

	// ���� ����� ����� �� ������� ��������� �� ���� �� ������ �� ���� �������� �� ����� 
	if (OtherActor == Target)
	{
		FindNextTarget();
	}
}

void ATurret::FindNextTarget()
{
	// ��� ����������� ���� Target , ������� �� ���
	float MinRange = 100000;
	Target = nullptr; // c ����� ��� ��� ����� �������� ��� � �������� ����������� 
	for (auto Actor : OtherActors)
	{
		float Range = FVector::Distance(Actor->GetActorLocation(), GetActorLocation());
		if (Range < MinRange)
		{
			MinRange = Range;
			Target = Actor;
		}
	}
}

void ATurret::Targeting()
{
	
	if (!Target.IsValid()) 
	{
		return;
	}
	
	auto TargetRotation = UKismetMathLibrary::FindLookAtRotation(SceneComponent->GetComponentLocation(), Target->GetActorLocation()); // TurretMesh

	SceneComponent->SetWorldRotation(FMath::Lerp(SceneComponent->GetComponentRotation(), TargetRotation, TargetingSpeed));

	int TargetRotationY = TargetRotation.Yaw * 10;
	int TargetRotationR = TargetRotation.Roll * 10;
	int TargetRotationP = TargetRotation.Pitch * 10;

	int SceneComponentY = SceneComponent->GetComponentRotation().Yaw * 10;
	int SceneComponentR = SceneComponent->GetComponentRotation().Roll * 10;
	int SceneComponentP = SceneComponent->GetComponentRotation().Pitch * 10;
	
	if (TargetRotationY != SceneComponentY || TargetRotationR != SceneComponentR || TargetRotationP != SceneComponentP)
	{
		if (!AudioEffectRotateTurret->IsActive())
		{
			AudioEffectRotateTurret->Play();
			IsTurretRotation = true;
		}
	}

	if (TargetRotationY == SceneComponentY && TargetRotationR == SceneComponentR && TargetRotationP == SceneComponentP)
	{
		AudioEffectRotateTurret->Stop();

		if (!AudioEffectStopRotateTurret->IsActive() && IsTurretRotation)
		{
			AudioEffectStopRotateTurret->Play();
			IsTurretRotation = false;
		}
	}
	
	auto TargetingDirection = TurretMesh->GetForwardVector();

	auto PlayerDirection = Target->GetActorLocation() - GetActorLocation();
	
	PlayerDirection.Normalize(); // �������� ��� ��� ����� �� ������ 

	auto Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(TargetingDirection, PlayerDirection))); // ��� �������� ��������� ������ ������� �� ������ � �� ������� ��������� ����� ������ ���� ����� ���������

	FHitResult Result;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Cannon);
	Params.bTraceComplex = true;
	auto Start = CannonPosition->GetComponentLocation();
	auto End = CannonPosition->GetForwardVector() * 650 + Start;
	bool bHasHit = GetWorld()->LineTraceSingleByChannel(Result, TurretMesh->GetComponentLocation(), Target->GetActorLocation() +10, ECollisionChannel::ECC_Visibility, Params);
	
	auto Tank = OtherActors[0]->GetActorLocation();

	if (bHasHit)
	{
		End = Result.Location;
	}
	else
		End = Tank;

	if (End == Tank)
	{
		if (Angle < AimSlack)
		{
			Fire();
		}
	}
	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.05, 0, 1);
}

void ATurret::Fire()
{
	if (Cannon)
		Cannon->ShootTurret();
} 

void ATurret::FireAnyway()
{
	if (Cannon)
		Cannon->ShootTurret();
}

void ATurret::OnDeath()
{
	// ��� � ������������� � ������ �����
	/*auto Temp = GetActorLocation();
	AudioDeathEffect->Play();
	DeathEffect->ActivateSystem();
	SetActorLocation({ -1000, -1000, -1000 });
	DeathEffect->SetWorldLocation(Temp);
	AudioDeathEffect->SetWorldLocation(Temp);*/

	bDeath = true;

	HealthComponent->OnDeath.Clear();

	TurretMesh->DestroyComponent();
	//CannonPosition->DestroyComponent();
	Cannon->Destroy();
	RangeSphere->DestroyComponent();
	
	BodyMesh->SetMaterial(0, DeathMaterial);

	AudioDeathEffect->Play();
	DeathEffect->ActivateSystem();
	AudioEffect_FlameDeath->Play();
	DeathEffect_Flame->ActivateSystem();

	// ���������� �������
	if (LevelTrigger)
	{
		LevelTrigger->SetActive(true);
	}

	GetWorld()->GetTimerManager().SetTimer(TimerDestruction, this, &ATurret::SelfDestruction, 20, false);	
}

void ATurret::OnHealthChanged(float CurrentHealthTurret)
{
	// ����� ��������� �������� ������� �� ����� ��������� 
	GEngine->AddOnScreenDebugMessage(141231312, 2, FColor::Red, FString::Printf(TEXT("Health Enemy Turret : %f"), CurrentHealthTurret));
}

void ATurret::SelfDestruction()
{
	Destroy();
}