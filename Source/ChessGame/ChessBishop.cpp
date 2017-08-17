// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessBishop.h"
#include "ChessGameGameModeBase.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/Actor.h"

AChessBishop::AChessBishop()
{
	PrimaryActorTick.bCanEverTick = true;

	bool AllSuccessfull = true;
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshLoader(TEXT("StaticMesh'/Game/Bishop.Bishop'"));

	if (!MeshLoader.Succeeded())
		AllSuccessfull = false;

	if (AllSuccessfull)
	{
		Mesh = MeshLoader.Object;
	}
}

void AChessBishop::BeginPlay()
{
	Super::BeginPlay();
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

void AChessBishop::GetPossibleMoveHighlight(TArray<int>& indexes)
{
	indexes.Empty();
	for (int i = 0; i < AllowedMoves.Num(); i++)
	{
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

void AChessBishop::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	Position = GetPositionFromSlot(CurrentSlotI, CurrentSlotJ);

	SetActorLocation(Position);
}

bool AChessBishop::isValidMove(int IndexToMoveToI, int IndexToMoveToJ)
{
	int DeltaI = IndexToMoveToI - CurrentSlotI;
	int DeltaJ = IndexToMoveToJ - CurrentSlotJ;

	bool UpDown = FMath::Abs<int>(DeltaI);
	bool LeftRight = FMath::Abs<int>(DeltaJ);

	bool MovingSideways = UpDown && LeftRight;
	
	if (MovingSideways)
	{
		UE_LOG(LogTemp, Warning, TEXT("Moving sideways!"));
		int DeltaISign = FMath::Sign<int>(DeltaI);
		int DeltaJSign = FMath::Sign<int>(DeltaJ);

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
				{
					UE_LOG(LogTemp, Warning, TEXT("Space already occupied by your piece"));
					return false;
				}
				else
				{
					if (NextI != IndexToMoveToI || NextI != IndexToMoveToI)
					{
						UE_LOG(LogTemp, Warning, TEXT("Can't jump over other pieces"));
						return false;
					}
				}
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}
