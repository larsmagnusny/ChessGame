// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessGameState.h"
#include "Net/UnrealNetwork.h"
#include "ChessGameGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
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
			Slots[i].Position[j] = StartPosition + FVector(j*Delta, -i*Delta, 0);
			Slots[i].Left[j] = Delta*0.5f;
			Slots[i].Right[j] = Delta*0.5f;

			Slots[i].Top[j] = Delta*0.5f;
			Slots[i].Bottom[j] = Delta*0.5f;
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

bool AChessGameState::IsClickable(AActor* Actor)
{
	int index;
	return ClickableActors.Find(Actor, index);
}

void AChessGameState::DebugData()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			UE_LOG(LogTemp, Warning, TEXT("i: %s, j: %s, Position: %s"), *FString::FromInt(i), *FString::FromInt(j), *Slots[i].Position[j].ToString())
		}
	}
}
