// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessPlayerPawn.h"


// Sets default values
AChessPlayerPawn::AChessPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AChessPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Pawn Initialized!"));
}

// Called every frame
void AChessPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetActorLocation(FVector(0.f, 17.f, 36.f));
	SetActorRotation(FRotator(-70.f, -90.f, 0));
}

// Called to bind functionality to input
void AChessPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

