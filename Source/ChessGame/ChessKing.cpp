// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessKing.h"
#include "ChessGameState.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/Actor.h"

AChessKing::AChessKing()
{
	PrimaryActorTick.bCanEverTick = true;


	bool AllSuccessfull = true;
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshLoader(TEXT("StaticMesh'/Game/King.King'"));

	if (!MeshLoader.Succeeded())
		AllSuccessfull = false;

	if (AllSuccessfull)
	{
		Mesh = MeshLoader.Object;
	}
}

void AChessKing::BeginPlay()
{
	Super::BeginPlay();

	InitializeAllowedMoves();

	InitMeshAndMaterial();

	// Make Object Clickable
	AChessGameState* GameState = Cast<AChessGameState>(GetWorld()->GetGameState());

	GameState->ClickableActors.Add(this);
}

void AChessKing::InitializeAllowedMoves()
{
	AllowedMoves.Add(FVector2D(0, 1));
	AllowedMoves.Add(FVector2D(0, -1));
	AllowedMoves.Add(FVector2D(1, 0));
	AllowedMoves.Add(FVector2D(-1, 0));
	AllowedMoves.Add(FVector2D(1, 1));
	AllowedMoves.Add(FVector2D(1, -1));
	AllowedMoves.Add(FVector2D(-1, 1));
	AllowedMoves.Add(FVector2D(-1, -1));
}

void AChessKing::GetPossibleMoveHighlight(TArray<int>& indexes)
{
	indexes.Empty();
	for (int i = 0; i < AllowedMoves.Num(); i++)
	{
		int NextI = CurrentSlotI + AllowedMoves[i].Y;
		int NextJ = CurrentSlotJ + AllowedMoves[i].X;

		if (NextI >= 0 && NextI < 8 && NextJ >= 0 && NextJ < 8)
		{
			// Check if theres a piece occupying the slot already

			AChessPiece* ChessPiece = Cast<AChessPiece>(GetActorFromSlot(NextI, NextJ));

			if (ChessPiece != nullptr)
			{
				if (ChessPiece->type != this->type)
					indexes.Add(ToOneDimentional(NextI, NextJ));
			}
			else
			{
				indexes.Add(ToOneDimentional(NextI, NextJ));
			}
		}
	}
}

void AChessKing::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	Position = GetPositionFromSlot(CurrentSlotI, CurrentSlotJ);

	SetActorLocation(Position);

	UpdateHighlight();
}

bool AChessKing::isValidMove(int IndexToMoveToI, int IndexToMoveToJ)
{
	bool MoveAllowed = false;
	for (int i = 0; i < AllowedMoves.Num(); i++)
	{
		float DeltaY = IndexToMoveToI - CurrentSlotI;
		float DeltaX = IndexToMoveToJ - CurrentSlotJ;

		if (DeltaY == AllowedMoves[i].Y && DeltaX == AllowedMoves[i].X)
		{
			AChessPiece* ChessPiece = Cast<AChessPiece>(GetActorFromSlot(IndexToMoveToI, IndexToMoveToJ));

			if (!isPieceInSlot(IndexToMoveToI, IndexToMoveToJ))
				MoveAllowed = true;
			else if (ChessPiece != nullptr)
			{
				if (ChessPiece->type != this->type)
				{
					MoveAllowed = true;
				}
			}
		}
	}

	return MoveAllowed;
}
