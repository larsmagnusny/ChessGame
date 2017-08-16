// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessGameGameModeBase.h"
#include "ChessPawn.h"
#include "Kismet/GameplayStatics.h"
#include "ChessPlayerController.h"

AChessGameGameModeBase::AChessGameGameModeBase()
{
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
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			FTransform SpawnTransform(FRotator(0, 0, 0), BoardInstance.Slots[i][j].Position);

			AChessPawn* DeferredChessPawn = Cast<AChessPawn>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessPawn::StaticClass(), SpawnTransform));

			if (DeferredChessPawn != nullptr)
			{
				DeferredChessPawn->SetVariables(0);
				DeferredChessPawn->CurrentSlotI = i;
				DeferredChessPawn->CurrentSlotJ = j;

				BoardInstance.Slots[i][j].Occupant = Cast<AActor>(DeferredChessPawn);
			}
		}
	}

	for (int i = 7; i > 5; i--)
	{
		for (int j = 0; j < 8; j++)
		{
			FTransform SpawnTransform(FRotator(0, 0, 0), BoardInstance.Slots[i][j].Position);

			AChessPawn* DeferredChessPawn = Cast<AChessPawn>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessPawn::StaticClass(), SpawnTransform));

			if (DeferredChessPawn != nullptr)
			{
				DeferredChessPawn->SetVariables(1);
				DeferredChessPawn->CurrentSlotI = i;
				DeferredChessPawn->CurrentSlotJ = j;

				BoardInstance.Slots[i][j].Occupant = Cast<AActor>(DeferredChessPawn);
			}
		}
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
