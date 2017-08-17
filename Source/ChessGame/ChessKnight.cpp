// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessKnight.h"
#include "ChessGameGameModeBase.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/Actor.h"

AChessKnight::AChessKnight()
{
	PrimaryActorTick.bCanEverTick = true;

	bool AllSuccessfull = true;
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshLoader(TEXT("StaticMesh'/Game/Knight.Knight'"));

	if (!MeshLoader.Succeeded())
		AllSuccessfull = false;

	if (AllSuccessfull)
	{
		Mesh = MeshLoader.Object;
	}
}

void AChessKnight::BeginPlay()
{
	Super::BeginPlay();

	AllowedMoves.Add(FVector2D(2, 1));
	AllowedMoves.Add(FVector2D(2, -1));

	AllowedMoves.Add(FVector2D(-2, 1));
	AllowedMoves.Add(FVector2D(-2, -1));

	AllowedMoves.Add(FVector2D(1, 2));
	AllowedMoves.Add(FVector2D(-1, 2));

	AllowedMoves.Add(FVector2D(1, -2));
	AllowedMoves.Add(FVector2D(-1, -2));

	UStaticMeshComponent* MeshComponent = GetStaticMeshComponent();
	MeshComponent->SetStaticMesh(Mesh);

	switch (type)
	{
	case 0:
		MaterialToUse = UMaterialInstanceDynamic::Create(WhiteMaterial, this);
		MaterialToUse->SetScalarParameterValue("Emissive", 0);
		break;
	case 1:
		MaterialToUse = UMaterialInstanceDynamic::Create(BlackMaterial, this);
		MaterialToUse->SetScalarParameterValue("Emissive", 0);
		break;
	default:
		MaterialToUse = UMaterialInstanceDynamic::Create(WhiteMaterial, this);
		UE_LOG(LogTemp, Warning, TEXT("Color on pawn not compatible!"));
		break;
	}


	MeshComponent->SetMaterial(0, MaterialToUse);
	// Make Object Movable
	MeshComponent->SetMobility(EComponentMobility::Movable);

	// Make Object Clickable
	AChessGameGameModeBase* GameMode = Cast<AChessGameGameModeBase>(GetWorld()->GetAuthGameMode());

	GameMode->ClickableActors.Add(this);
}

void AChessKnight::GetPossibleMoveHighlight(TArray<int>& indexes)
{
	for (int i = 0; i < AllowedMoves.Num(); i++)
	{
		int NextI = CurrentSlotI + AllowedMoves[i].Y;
		int NextJ = CurrentSlotJ + AllowedMoves[i].X;

		if (NextI >= 0 && NextI < 8 && NextJ >= 0 && NextJ < 8)
		{
			AChessPiece* ChessPiece = Cast<AChessPiece>(GetActorFromSlot(NextI, NextJ));

			if (ChessPiece != nullptr)
			{
				if (ChessPiece->type != this->type)
				{
					indexes.Add(ToOneDimentional(NextI, NextJ));
				}
			}
			else
			{
				indexes.Add(ToOneDimentional(NextI, NextJ));
			}
		}
	}
}

void AChessKnight::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	Position = GetPositionFromSlot(CurrentSlotI, CurrentSlotJ);

	SetActorLocation(Position);
}

bool AChessKnight::isValidMove(int IndexToMoveToI, int IndexToMoveToJ)
{
	bool MoveAllowed = false;

	for (int i = 0; i < AllowedMoves.Num(); i++)
	{
		float DeltaY = IndexToMoveToI - CurrentSlotI;
		float DeltaX = IndexToMoveToJ - CurrentSlotJ;

		if (DeltaY == AllowedMoves[i].Y && DeltaX == AllowedMoves[i].X)
		{
			AChessPiece* ChessPiece = Cast<AChessPiece>(GetActorFromSlot(IndexToMoveToI, IndexToMoveToJ));

			if (ChessPiece != nullptr)
			{
				if (ChessPiece->type != this->type)
				{
					MoveAllowed = true;
				}
			}
			else
			{
				MoveAllowed = true;
			}
		}
	}

	return MoveAllowed;
}
