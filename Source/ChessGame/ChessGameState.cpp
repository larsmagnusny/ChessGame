// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessGameState.h"
#include "Net/UnrealNetwork.h"
#include "ChessGameGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "ChessPawn.h"
#include "ChessRook.h"
#include "ChessKnight.h"
#include "ChessBishop.h"
#include "ChessKing.h"
#include "ChessQueen.h"
#include "ChessBoardActor.h"

AChessGameState::AChessGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

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

void AChessGameState::BeginPlay()
{
	Super::BeginPlay();
}

void AChessGameState::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
}

void AChessGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AChessGameState, RandomFloat);
	DOREPLIFETIME(AChessGameState, CurrentPlayers);
	DOREPLIFETIME(AChessGameState, NextPlayer);
	DOREPLIFETIME(AChessGameState, RunCameraAnimation);
}

bool AChessGameState::IsClickable(AActor* Actor)
{
	int index;
	return ClickableActors.Find(Actor, index);
}

void AChessGameState::SpawnPieces()
{
	if (GetNetMode() == ENetMode::NM_DedicatedServer)
		UE_LOG(LogTemp, Warning, TEXT("SpawnPieces called on Dedicated Server"));
	if (GetNetMode() == ENetMode::NM_Client)
		UE_LOG(LogTemp, Warning, TEXT("SpawnPieces called on Client"));

	// Spawn Board
	FTransform SpawnTransform(FRotator(0, 0, 0), FVector(0, 0, 0));

	AChessBoardActor* DeferredChessPawn = Cast<AChessBoardActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessBoardActor::StaticClass(), SpawnTransform));

	if (DeferredChessPawn != nullptr)
	{
		UGameplayStatics::FinishSpawningActor(DeferredChessPawn, SpawnTransform);
	}

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

void AChessGameState::SpawnPawnAtSlot(int i, int j, int type)
{
	FTransform SpawnTransform(FRotator(0, 0, 0), BoardInstance.Slots[i][j].Position);

	AChessPawn* DeferredChessPawn = Cast<AChessPawn>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessPawn::StaticClass(), SpawnTransform));

	if (DeferredChessPawn != nullptr)
	{
		DeferredChessPawn->SetVariables(type);
		DeferredChessPawn->CurrentSlotI = i;
		DeferredChessPawn->CurrentSlotJ = j;

		BoardInstance.Slots[i][j].Occupant = Cast<AActor>(DeferredChessPawn);
		UGameplayStatics::FinishSpawningActor(DeferredChessPawn, SpawnTransform);
	}
}

void AChessGameState::SpawnRookAtSlot(int i, int j, int type)
{
	FTransform SpawnTransform(FRotator(0, 0, 0), BoardInstance.Slots[i][j].Position);

	AChessRook* DeferredChessRook = Cast<AChessRook>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessRook::StaticClass(), SpawnTransform));

	if (DeferredChessRook != nullptr)
	{
		DeferredChessRook->SetVariables(type);
		DeferredChessRook->CurrentSlotI = i;
		DeferredChessRook->CurrentSlotJ = j;

		BoardInstance.Slots[i][j].Occupant = Cast<AActor>(DeferredChessRook);
		UGameplayStatics::FinishSpawningActor(DeferredChessRook, SpawnTransform);
	}
}

void AChessGameState::SpawnKnightAtSlot(int i, int j, int type)
{
	float Yaw = 0;
	if (type == 1)
		Yaw = 180;

	FTransform SpawnTransform(FRotator(0, Yaw, 0), BoardInstance.Slots[i][j].Position);

	AChessKnight* DeferredChessKnight = Cast<AChessKnight>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessKnight::StaticClass(), SpawnTransform));

	if (DeferredChessKnight != nullptr)
	{
		DeferredChessKnight->SetVariables(type);
		DeferredChessKnight->CurrentSlotI = i;
		DeferredChessKnight->CurrentSlotJ = j;

		BoardInstance.Slots[i][j].Occupant = Cast<AActor>(DeferredChessKnight);
		UGameplayStatics::FinishSpawningActor(DeferredChessKnight, SpawnTransform);
	}
}

void AChessGameState::SpawnBishopAtSlot(int i, int j, int type)
{
	FTransform SpawnTransform(FRotator(0, 0, 0), BoardInstance.Slots[i][j].Position);

	AChessBishop* DeferredChessBishop = Cast<AChessBishop>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessBishop::StaticClass(), SpawnTransform));

	if (DeferredChessBishop != nullptr)
	{
		DeferredChessBishop->SetVariables(type);
		DeferredChessBishop->CurrentSlotI = i;
		DeferredChessBishop->CurrentSlotJ = j;

		BoardInstance.Slots[i][j].Occupant = Cast<AActor>(DeferredChessBishop);
		UGameplayStatics::FinishSpawningActor(DeferredChessBishop, SpawnTransform);
	}
}

void AChessGameState::SpawnKingAtSlot(int i, int j, int type)
{
	FTransform SpawnTransform(FRotator(0, 0, 0), BoardInstance.Slots[i][j].Position);

	AChessKing* DeferredChessKing = Cast<AChessKing>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessKing::StaticClass(), SpawnTransform));

	if (DeferredChessKing != nullptr)
	{
		DeferredChessKing->SetVariables(type);
		DeferredChessKing->CurrentSlotI = i;
		DeferredChessKing->CurrentSlotJ = j;

		BoardInstance.Slots[i][j].Occupant = Cast<AActor>(DeferredChessKing);
		UGameplayStatics::FinishSpawningActor(DeferredChessKing, SpawnTransform);
	}
}

void AChessGameState::SpawnQueenAtSlot(int i, int j, int type)
{
	FTransform SpawnTransform(FRotator(0, 0, 0), BoardInstance.Slots[i][j].Position);

	AChessQueen* DeferredChessQueen = Cast<AChessQueen>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessQueen::StaticClass(), SpawnTransform));

	if (DeferredChessQueen != nullptr)
	{
		DeferredChessQueen->SetVariables(type);
		DeferredChessQueen->CurrentSlotI = i;
		DeferredChessQueen->CurrentSlotJ = j;

		BoardInstance.Slots[i][j].Occupant = Cast<AActor>(DeferredChessQueen);
		UGameplayStatics::FinishSpawningActor(DeferredChessQueen, SpawnTransform);
	}
}