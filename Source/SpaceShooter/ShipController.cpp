// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipController.h"

#include <string>

#include "BulletController.h"
#include "EnemyController.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AShipController::AShipController()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	CollisionBox->SetGenerateOverlapEvents(true);
	
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void AShipController::BeginPlay()
{
	Super::BeginPlay();
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AShipController::OnOverlap);
	
}

// Called every frame
void AShipController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!CurrentVelocity.IsZero())
	{
		const FVector NewLocation = GetActorLocation() + speed * CurrentVelocity * DeltaTime;
		SetActorLocation(NewLocation);
	}

}

// Called to bind functionality to input
void AShipController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis("MoveX", this, &AShipController::Move_XAxis);
	InputComponent->BindAxis("MoveY", this, &AShipController::Move_YAxis);
	InputComponent->BindAction("Shoot", IE_Pressed, this, &AShipController::OnShoot);
	InputComponent->BindAction("Restart", IE_Pressed, this, &AShipController::OnRestart).bExecuteWhenPaused = true;

}

void AShipController::Move_XAxis(float AxisValue)
{
	CurrentVelocity.X = AxisValue * 100.0f;
}

void AShipController::Move_YAxis(float AxisValue)
{
	CurrentVelocity.Y = AxisValue * 100.0f;
}

void AShipController::OnShoot()
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector Location = GetActorLocation();
		World->SpawnActor<ABulletController>(BulletBlueprint, Location, FRotator::ZeroRotator);
	}
}

void AShipController::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AEnemyController::StaticClass()))
	{
		Died = true;
		this->SetActorHiddenInGame(true);
		((ASpaceShooterGameMode*)GetWorld()->GetAuthGameMode())->GameOver();
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		
	}
}

void AShipController::OnRestart()
{
	if (Died)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), TEXT("OnRestart"));
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
}






