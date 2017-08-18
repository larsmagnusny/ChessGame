// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessQueen.h"
#include "ChessGameGameModeBase.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/Actor.h"

AChessQueen::AChessQueen()
{
	PrimaryActorTick.bCanEverTick = true;

	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Error, TEXT("Don't need to load mesh if im a server!"));
	}
	else
	{
		bool AllSuccessfull = true;
		const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshLoader(TEXT("StaticMesh'/Game/Queen.Queen'"));

		if (!MeshLoader.Succeeded())
			AllSuccessfull = false;

		if (AllSuccessfull)
		{
			Mesh = MeshLoader.Object;
		}
	}
}

void AChessQueen::BeginPlay()
{
	Super::BeginPlay();

	AllowedMoves.Add(FVector2D(0, 7));
	AllowedMoves.Add(FVector2D(0, -7));
	AllowedMoves.Add(FVector2D(7, 0));
	AllowedMoves.Add(FVector2D(-7, 0));

	AllowedMoves.Add(FVector2D(7, 7));
	AllowedMoves.Add(FVector2D(-7, 7));
	AllowedMoves.Add(FVector2D(7, -7));
	AllowedMoves.Add(FVector2D(-7, -7));

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

void AChessQueen::GetPossibleMoveHighlight(TArray<int>& indexes)
{
	indexes.Empty();
	for (int i = 0; i < AllowedMoves.Num(); i++)
	{
		int DeltaI = FMath::Abs<float>(AllowedMoves[i].Y);
		int DeltaJ = FMath::Abs<float>(AllowedMoves[i].X);

		bool UpDownLeftRight = !(DeltaI && DeltaJ);

		if (UpDownLeftRight)
		{
			// Highlight all squares in the direction
			for (int y = 1; y <= FMath::Abs(AllowedMoves[i].Y); y++)
			{
				int NextI = CurrentSlotI + FMath::Sign<float>(AllowedMoves[i].Y)*y;
				int NextJ = CurrentSlotJ;

				AChessPiece* ChessPiece = Cast<AChessPiece>(GetActorFromSlot(NextI, NextJ));

				if (NextI >= 0 && NextI < 8 && NextJ >= 0 && NextJ < 8)
				{
					if (ChessPiece == nullptr)
					{
						indexes.Add(ToOneDimentional(NextI, NextJ));
					}
					else if (ChessPiece->type == this->type)
					{
						y = FMath::Abs(AllowedMoves[i].Y) + 1; // Exit loop
					}
					else if (ChessPiece->type != this->type)
					{
						indexes.Add(ToOneDimentional(NextI, NextJ));
						y = FMath::Abs(AllowedMoves[i].Y) + 1; // Exit loop
					}
				}
			}
		}
		else
		{

			for (int x = 1; x <= FMath::Abs(AllowedMoves[i].X); x++)
			{
				int NextI = CurrentSlotI;
				int NextJ = CurrentSlotJ + FMath::Sign<float>(AllowedMoves[i].X)*x;

				AChessPiece* ChessPiece = Cast<AChessPiece>(GetActorFromSlot(NextI, NextJ));

				if (NextI >= 0 && NextI < 8 && NextJ >= 0 && NextJ < 8)
				{
					if (ChessPiece == nullptr)
					{
						indexes.Add(ToOneDimentional(NextI, NextJ));
					}
					else if (ChessPiece->type == this->type)
					{
						x = FMath::Abs(AllowedMoves[i].X) + 1; // Exit loop
					}
					else if (ChessPiece->type != this->type)
					{
						indexes.Add(ToOneDimentional(NextI, NextJ));
						x = FMath::Abs(AllowedMoves[i].X) + 1; // Exit loop
					}
				}

				int xSign = FMath::Sign(AllowedMoves[i].X);
				int ySign = FMath::Sign(AllowedMoves[i].Y);
				for (int xy = 1; xy <= FMath::Abs(AllowedMoves[i].X); xy++)
				{
					int NextI = CurrentSlotI + xy*ySign;
					int NextJ = CurrentSlotJ + xy*xSign;

					if (NextI >= 0 && NextI < 8 && NextJ >= 0 && NextJ < 8)
					{
						AChessPiece* ChessPiece = Cast<AChessPiece>(GetActorFromSlot(NextI, NextJ));

						if (ChessPiece != nullptr)
						{
							if (ChessPiece->type != this->type)
							{
								indexes.Add(ToOneDimentional(NextI, NextJ));
								xy = FMath::Abs(AllowedMoves[i].X) + 1;
							}
							else
							{
								xy = FMath::Abs(AllowedMoves[i].X) + 1;
							}
						}
						else
						{
							indexes.Add(ToOneDimentional(NextI, NextJ));
						}
					}
				}
			}
		}
	}
}

void AChessQueen::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	Position = GetPositionFromSlot(CurrentSlotI, CurrentSlotJ);

	SetActorLocation(Position);
}

bool AChessQueen::isValidMove(int IndexToMoveToI, int IndexToMoveToJ)
{
	int DeltaI = IndexToMoveToI - CurrentSlotI;
	int DeltaJ = IndexToMoveToJ - CurrentSlotJ;

	int DeltaISign = FMath::Sign<int>(DeltaI);
	int DeltaJSign = FMath::Sign<int>(DeltaJ);

	bool MovingUpDown = FMath::Abs<int>(DeltaI);
	bool MovingLeftRight = FMath::Abs<int>(DeltaJ);

	bool MovingStraightLines = !(MovingUpDown && MovingLeftRight);
	
	if (MovingStraightLines)
	{
		// Check if the place you are moving towards has any collisions along the way...
		if (MovingUpDown)
		{
			if (DeltaISign == 1)
			{
				// Check every position up to IndexToMoveToI
				for (int y = 1; y <= DeltaI; y++)
				{
					int NextI = CurrentSlotI + y;
					int NextJ = CurrentSlotJ;

					AChessPiece* ChessPiece = Cast<AChessPiece>(GetActorFromSlot(NextI, NextJ));

					if (ChessPiece != nullptr)
					{
						UE_LOG(LogTemp, Warning, TEXT("Found a chesspiece at I: %s J: %s"), *FString::FromInt(NextI), *FString::FromInt(NextJ));
						if (ChessPiece->type == this->type)
						{
							return false;
						}
						else
						{
							if (NextI != IndexToMoveToI || NextJ != IndexToMoveToJ)
								return false;
						}
					}
				}
			}
			else if (DeltaISign == -1)
			{
				// Check every position down to IndexToMoveToJ
				for (int y = -1; y >= DeltaI; y--)
				{
					int NextI = CurrentSlotI + y;
					int NextJ = CurrentSlotJ;

					AChessPiece* ChessPiece = Cast<AChessPiece>(GetActorFromSlot(NextI, NextJ));

					if (ChessPiece != nullptr)
					{
						if (ChessPiece->type == this->type)
						{
							return false;
						}
						else
						{
							if (NextI != IndexToMoveToI || NextJ != IndexToMoveToJ)
								return false;
						}
					}
				}
			}
		}
		else if (MovingLeftRight)
		{
			if (DeltaJSign == 1)
			{
				for (int x = 1; x <= DeltaJ; x++)
				{
					int NextI = CurrentSlotI;
					int NextJ = CurrentSlotJ + x;

					AChessPiece* ChessPiece = Cast<AChessPiece>(GetActorFromSlot(NextI, NextJ));

					if (ChessPiece != nullptr)
					{
						if (ChessPiece->type == this->type)
						{
							return false;
						}
						else
						{
							if (NextI != IndexToMoveToI || NextJ != IndexToMoveToJ)
								return false;
						}
					}
				}
			}
			else if (DeltaJSign == -1)
			{
				for (int x = -1; x >= DeltaJ; x--)
				{
					int NextI = CurrentSlotI;
					int NextJ = CurrentSlotJ + x;

					AChessPiece* ChessPiece = Cast<AChessPiece>(GetActorFromSlot(NextI, NextJ));

					if (ChessPiece != nullptr)
					{
						if (ChessPiece->type == this->type)
						{
							return false;
						}
						else
						{
							if (NextI != IndexToMoveToI || NextJ != IndexToMoveToJ)
								return false;
						}
					}
				}
			}
		}
	}
	else
	{
		if (FMath::Abs<int>(DeltaJ) != FMath::Abs<int>(DeltaI))
			return false;

		for (int xy = 1; xy <= FMath::Abs(DeltaI); xy++)
		{
			int NextI = CurrentSlotI + xy*DeltaISign;
			int NextJ = CurrentSlotJ + xy*DeltaJSign;

			AChessPiece* ChessPiece = Cast<AChessPiece>(GetActorFromSlot(NextI, NextJ));

			if (ChessPiece != nullptr)
			{
				if (ChessPiece->type == this->type)
					return false;
				else
				{
					if (NextI != IndexToMoveToI || NextI != IndexToMoveToI)
						return false;
				}
			}
		}
	}

	return true;
}
