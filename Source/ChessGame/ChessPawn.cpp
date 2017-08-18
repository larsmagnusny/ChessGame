// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessPawn.h"
#include "ChessGameGameModeBase.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/Actor.h"

AChessPawn::AChessPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Error, TEXT("Don't need to load mesh if im a server!"));
	}
	else
	{
		bool AllSuccessfull = true;
		const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshLoader(TEXT("StaticMesh'/Game/Pawn.Pawn'"));

		if (!MeshLoader.Succeeded())
			AllSuccessfull = false;

		if (AllSuccessfull)
		{
			Mesh = MeshLoader.Object;
		}
	}
}

void AChessPawn::BeginPlay()
{
	Super::BeginPlay();

	if (!type)
	{
		AllowedMoves.Add(FVector2D(0, 1));
		AllowedMoves.Add(FVector2D(0, 2));
		AllowedMoves.Add(FVector2D(1, 1));
		AllowedMoves.Add(FVector2D(-1, 1));
	}
	else
	{
		AllowedMoves.Add(FVector2D(0, -1));
		AllowedMoves.Add(FVector2D(0, -2));
		AllowedMoves.Add(FVector2D(1, -1));
		AllowedMoves.Add(FVector2D(-1, -1));
	}

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

void AChessPawn::GetPossibleMoveHighlight(TArray<int>& indexes)
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

			if (FMath::Abs(AllowedMoves[i].Y) == 2 && isOnStartingSquare)
			{
				for (int y = 1; y <= 2; y++)
				{
					int SlotNI = CurrentSlotI + FMath::Sign<float>(AllowedMoves[i].Y)*y;
					int SlotNJ = CurrentSlotJ;

					AChessPiece* ChessPiece1 = Cast<AChessPiece>(GetActorFromSlot(SlotNI, SlotNJ));
					if (ChessPiece1 != nullptr)
					{
						if (ChessPiece1->type != this->type)
						{
							y = 3;
						}
						else
						{
							y = 3;
						}
					}
					else
					{
						indexes.Add(ToOneDimentional(SlotNI, SlotNJ));
					}
				}
			}
			else
			{
				if (!isPieceInSlot(NextI, NextJ) && NextI - CurrentSlotI != 0 && NextJ - CurrentSlotJ == 0 && FMath::Abs(AllowedMoves[i].Y) != 2)
					indexes.Add(ToOneDimentional(NextI, NextJ));
				else if (ChessPiece != nullptr)
				{
					if (ChessPiece->type != this->type && NextI - CurrentSlotI != 0 && NextJ - CurrentSlotJ != 0 && FMath::Abs(AllowedMoves[i].Y) != 2)
					{
						indexes.Add(ToOneDimentional(NextI, NextJ));
					}
				}
			}
		}
	}
}

void AChessPawn::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	Position = GetPositionFromSlot(CurrentSlotI, CurrentSlotJ);

	SetActorLocation(Position);
}

bool AChessPawn::isValidMove(int IndexToMoveToI, int IndexToMoveToJ)
{
	bool MoveAllowed = false;
	for (int i = 0; i < AllowedMoves.Num(); i++)
	{
		float DeltaY = IndexToMoveToI - CurrentSlotI;
		float DeltaX = IndexToMoveToJ - CurrentSlotJ;

		if (DeltaY == AllowedMoves[i].Y && DeltaX == AllowedMoves[i].X)
		{
			AChessPiece* ChessPiece = Cast<AChessPiece>(GetActorFromSlot(IndexToMoveToI, IndexToMoveToJ));

			if (isOnStartingSquare && FMath::Abs<float>(DeltaY) == 2)
			{
				for (int y = 1; y <= 2; y++)
				{
					
					int SlotNI = CurrentSlotI + FMath::Sign<float>(AllowedMoves[i].Y)*y;
					int SlotNJ = CurrentSlotJ;

					AChessPiece* ChessPiece1 = Cast<AChessPiece>(GetActorFromSlot(SlotNI, SlotNJ));

					if (ChessPiece1 != nullptr)
					{
						if (ChessPiece1->type == this->type)
							return false;
						if (ChessPiece1->type != this->type)
							return false;
					}
				}
			}
			else if(FMath::Abs<float>(DeltaY) == 2)
			{
				return false;
			}

			if (!isPieceInSlot(IndexToMoveToI, IndexToMoveToJ) && IndexToMoveToI - CurrentSlotI != 0 && IndexToMoveToJ - CurrentSlotJ == 0)
				MoveAllowed = true;
			else if (ChessPiece != nullptr)
			{
				if (ChessPiece->type != this->type && IndexToMoveToI - CurrentSlotI != 0 && IndexToMoveToJ - CurrentSlotJ != 0)
				{
					MoveAllowed = true;
				}
			}
		}
	}

	return MoveAllowed;
}
