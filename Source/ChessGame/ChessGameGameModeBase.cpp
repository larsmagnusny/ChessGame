// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessGameGameModeBase.h"
#include "ChessPawn.h"
#include "ChessRook.h"
#include "ChessKnight.h"
#include "ChessBishop.h"
#include "ChessKing.h"
#include "ChessQueen.h"
#include "Kismet/GameplayStatics.h"
#include "ChessPlayerController.h"

AChessGameGameModeBase::AChessGameGameModeBase()
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("I AM THE SERVER!"));
	}

	// Don't use default pawn
	PrimaryActorTick.bCanEverTick = true;
	DefaultPawnClass = nullptr;
	PlayerControllerClass = AChessPlayerController::StaticClass();

	// Initialize all the positions of the board...
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			BoardInstance.Slots[i][j].Position = StartPosition + FVector(j*Delta, -i*Delta, 0);
			BoardInstance.Slots[i][j].Left = Delta*0.5f;
			BoardInstance.Slots[i][j].Right = Delta*0.5f;

			BoardInstance.Slots[i][j].Top = Delta*0.5f;
			BoardInstance.Slots[i][j].Bottom = Delta*0.5f;
		}
	}
}

void AChessGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	

	// Spawn 8*2 pawns
	int counter = 0;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (counter == 0 || counter == 7)
				SpawnRookAtSlot(i, j, 0);
			else if (counter == 1 || counter == 6)
				SpawnKnightAtSlot(i, j, 0);
			else if (counter == 2 || counter == 5)
				SpawnBishopAtSlot(i, j, 0);
			else if (counter == 3)
				SpawnQueenAtSlot(i, j, 0);
			else if (counter == 4)
				SpawnKingAtSlot(i, j, 0);
			else
				SpawnPawnAtSlot(i, j, 0);
			//UE_LOG(LogTemp, Warning, TEXT("%s"), *FString::FromInt(counter));
			counter++;
		}
	}

	counter = 0;
	for (int i = 7; i > 5; i--)
	{
		for (int j = 0; j < 8; j++)
		{
			if (counter == 0 || counter == 7)
				SpawnRookAtSlot(i, j, 1);
			else if (counter == 1 || counter == 6)
				SpawnKnightAtSlot(i, j, 1);
			else if (counter == 2 || counter == 5)
				SpawnBishopAtSlot(i, j, 1);
			else if (counter == 3)
				SpawnQueenAtSlot(i, j, 1);
			else if (counter == 4)
				SpawnKingAtSlot(i, j, 1);
			else
				SpawnPawnAtSlot(i, j, 1);

			counter++;
		}
	}
}

void AChessGameGameModeBase::SpawnPawnAtSlot(int i, int j, int type)
{
	FTransform SpawnTransform(FRotator(0, 0, 0), BoardInstance.Slots[i][j].Position);

	AChessPawn* DeferredChessPawn = Cast<AChessPawn>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessPawn::StaticClass(), SpawnTransform));

	if (DeferredChessPawn != nullptr)
	{
		DeferredChessPawn->SetVariables(type);
		DeferredChessPawn->CurrentSlotI = i;
		DeferredChessPawn->CurrentSlotJ = j;

		BoardInstance.Slots[i][j].Occupant = Cast<AActor>(DeferredChessPawn);
	}
}

void AChessGameGameModeBase::SpawnRookAtSlot(int i, int j, int type)
{
	FTransform SpawnTransform(FRotator(0, 0, 0), BoardInstance.Slots[i][j].Position);

	AChessRook* DeferredChessRook = Cast<AChessRook>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessRook::StaticClass(), SpawnTransform));

	if (DeferredChessRook != nullptr)
	{
		DeferredChessRook->SetVariables(type);
		DeferredChessRook->CurrentSlotI = i;
		DeferredChessRook->CurrentSlotJ = j;

		BoardInstance.Slots[i][j].Occupant = Cast<AActor>(DeferredChessRook);
	}
}

void AChessGameGameModeBase::SpawnKnightAtSlot(int i, int j, int type)
{
	float Yaw = 0;
	if (type == 1)
		Yaw = 180;

	FTransform SpawnTransform(FRotator(0, Yaw, 0), BoardInstance.Slots[i][j].Position);

	AChessKnight* DeferredChessRook = Cast<AChessKnight>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessKnight::StaticClass(), SpawnTransform));

	if (DeferredChessRook != nullptr)
	{
		DeferredChessRook->SetVariables(type);
		DeferredChessRook->CurrentSlotI = i;
		DeferredChessRook->CurrentSlotJ = j;

		BoardInstance.Slots[i][j].Occupant = Cast<AActor>(DeferredChessRook);
	}
}

void AChessGameGameModeBase::SpawnBishopAtSlot(int i, int j, int type)
{
	FTransform SpawnTransform(FRotator(0, 0, 0), BoardInstance.Slots[i][j].Position);

	AChessBishop* DeferredChessRook = Cast<AChessBishop>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessBishop::StaticClass(), SpawnTransform));

	if (DeferredChessRook != nullptr)
	{
		DeferredChessRook->SetVariables(type);
		DeferredChessRook->CurrentSlotI = i;
		DeferredChessRook->CurrentSlotJ = j;

		BoardInstance.Slots[i][j].Occupant = Cast<AActor>(DeferredChessRook);
	}
}

void AChessGameGameModeBase::SpawnKingAtSlot(int i, int j, int type)
{
	FTransform SpawnTransform(FRotator(0, 0, 0), BoardInstance.Slots[i][j].Position);

	AChessKing* DeferredChessRook = Cast<AChessKing>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessKing::StaticClass(), SpawnTransform));

	if (DeferredChessRook != nullptr)
	{
		DeferredChessRook->SetVariables(type);
		DeferredChessRook->CurrentSlotI = i;
		DeferredChessRook->CurrentSlotJ = j;

		BoardInstance.Slots[i][j].Occupant = Cast<AActor>(DeferredChessRook);
	}
}

void AChessGameGameModeBase::SpawnQueenAtSlot(int i, int j, int type)
{
	FTransform SpawnTransform(FRotator(0, 0, 0), BoardInstance.Slots[i][j].Position);

	AChessQueen* DeferredChessRook = Cast<AChessQueen>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessQueen::StaticClass(), SpawnTransform));

	if (DeferredChessRook != nullptr)
	{
		DeferredChessRook->SetVariables(type);
		DeferredChessRook->CurrentSlotI = i;
		DeferredChessRook->CurrentSlotJ = j;

		BoardInstance.Slots[i][j].Occupant = Cast<AActor>(DeferredChessRook);
	}
}

bool AChessGameGameModeBase::IsClickable(AActor* Actor)
{
	int index;
	return ClickableActors.Find(Actor, index);
}

void AChessGameGameModeBase::SetViewTarget(AActor * Target)
{
	APlayerController* PlayerController =  GetWorld()->GetFirstPlayerController();

	PlayerController->SetViewTarget(Target);
}
