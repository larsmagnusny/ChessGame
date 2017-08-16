// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessPiece.h"
#include "ChessGameGameModeBase.h"

AChessPiece::AChessPiece()
{
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> WhiteMaterialLoader(TEXT("MaterialInstanceConstant'/Game/White_Inst.White_Inst'"));
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> BlackMaterialLoader(TEXT("MaterialInstanceConstant'/Game/Black_Inst.Black_Inst'"));

	if (WhiteMaterialLoader.Succeeded() && BlackMaterialLoader.Succeeded())
	{
		WhiteMaterial = WhiteMaterialLoader.Object;
		BlackMaterial = BlackMaterialLoader.Object;
	}
}

void AChessPiece::SetVariables(int type)
{
	this->type = type;
}

void AChessPiece::ToggleHighlight()
{
	IsHighlighted = true;

	if (!Selected && MaterialToUse != nullptr)
		MaterialToUse->SetScalarParameterValue("Emissive", 1.f);
}

void AChessPiece::RemoveHighlight()
{
	IsHighlighted = false;

	if (!Selected && MaterialToUse != nullptr)
		MaterialToUse->SetScalarParameterValue("Emissive", 0.f);
}

void AChessPiece::ToggleSelected()
{
	if (!MaterialToUse)
		return;

	Selected = true;
	if (MaterialToUse != nullptr)
		MaterialToUse->SetScalarParameterValue("Emissive", 1.f);
}

void AChessPiece::RemoveSelected()
{
	Selected = false;
	
	if (MaterialToUse != nullptr)
		MaterialToUse->SetScalarParameterValue("Emissive", 0.f);
}

void AChessPiece::GetPossibleMoveHighlight(TArray<int>& indexes)
{
	
}

bool AChessPiece::isValidMove(int IndexToMoveToI, int IndexToMoveToJ)
{
	return false;
}

int AChessPiece::ToOneDimentional(int i, int j)
{
	if (i >= 0 && i <= 8 && j >= 0 && j < 8)
		return i * 8 + (7 - j);
	else
		return 0;
}

FVector AChessPiece::GetPositionFromSlot(int i, int j)
{
	if (i >= 8 || i < 0 || j >= 8 || j < 0)
		return FVector(0, 0, 0);

	AChessGameGameModeBase* GameModeBase = Cast<AChessGameGameModeBase>(GetWorld()->GetAuthGameMode());

	return GameModeBase->BoardInstance.Slots[i][j].Position;
}

AActor * AChessPiece::GetActorFromSlot(int i, int j)
{
	if (i >= 8 || i < 0 || j >= 8 || j < 0)
		return nullptr;

	AChessGameGameModeBase* GameModeBase = Cast<AChessGameGameModeBase>(GetWorld()->GetAuthGameMode());

	return GameModeBase->BoardInstance.Slots[i][j].Occupant;
}

bool AChessPiece::isPieceInSlot(int i, int j)
{
	if (i >= 8 || i < 0 || j >= 8 || j < 0)
		return true;

	AChessGameGameModeBase* GameModeBase = Cast<AChessGameGameModeBase>(GetWorld()->GetAuthGameMode());

	if (GameModeBase->BoardInstance.Slots[i][j].Occupant == nullptr)
	{
		return false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Piece in next spot: %s"), *GameModeBase->BoardInstance.Slots[i][j].Occupant->GetName());
		return true;
	}
}
