// Fill out your copyright notice in the Description page of Project Settings.
#include "ChessPiece.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ChessGameState.h"

AChessPiece::AChessPiece()
{
	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChessPiece Constructor ran on Server"));
	}

	if (GetNetMode() == ENetMode::NM_Client)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChessPiece Constructor ran on Client"));
	}

	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> WhiteMaterialLoader(TEXT("MaterialInstanceConstant'/Game/White_Inst.White_Inst'"));
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> BlackMaterialLoader(TEXT("MaterialInstanceConstant'/Game/Black_Inst.Black_Inst'"));

	if (WhiteMaterialLoader.Succeeded() && BlackMaterialLoader.Succeeded())
	{
		WhiteMaterial = WhiteMaterialLoader.Object;
		BlackMaterial = BlackMaterialLoader.Object;
	}

	//SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	//bReplicates = true;
	//bAlwaysRelevant = true;
}



void AChessPiece::InitMeshAndMaterial()
{
	/*if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChessPiece Beginplay ran on Server"));
	}

	if (GetNetMode() == ENetMode::NM_Client)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChessPiece Beginplay ran on Client"));
	}*/

	UStaticMeshComponent* MeshComponent = GetStaticMeshComponent();

	// Make Object Movable
	MeshComponent->SetMobility(EComponentMobility::Movable);

	MeshComponent->SetStaticMesh(Mesh);

	//UE_LOG(LogTemp, Warning, TEXT("BEGIN PLAY CALLED for piece!"));

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
}

void AChessPiece::SetVariables(int type)
{
	this->type = type;
}

void AChessPiece::MultiplayerSafeToggleHighlight()
{
	if (GetNetMode() == ENetMode::NM_Client)
	{
		ToggleHighlight();
	}
	else if (GetNetMode() == ENetMode::NM_Standalone)
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to set highlighted"));
		IsHighlighted = true;
	}
	else if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Error, TEXT("Should not be able to toggle highlighting as a Dedicated Server."));
	}
}

void AChessPiece::MultiplayerSafeRemoveHighlight()
{
	if (GetNetMode() == ENetMode::NM_Client)
	{
		RemoveHighlight();
	}
	else if (GetNetMode() == ENetMode::NM_Standalone)
	{
		
		IsHighlighted = false;
	}
	else if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Error, TEXT("Should not be able to toggle highlighting as a Dedicated Server."));
	}
}

void AChessPiece::MultiplayerSafeToggleSelected()
{
	if (GetNetMode() == ENetMode::NM_Client)
	{
		ToggleSelected();
	}
	else if (GetNetMode() == ENetMode::NM_Standalone)
	{
		Selected = true;
	}
	else if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Error, TEXT("Should not be able to toggle selection as a Dedicated Server."));
	}
}

void AChessPiece::MultiplayerSafeRemoveSelected()
{
	if (GetNetMode() == ENetMode::NM_Client)
	{
		RemoveSelected();
	}
	else if (GetNetMode() == ENetMode::NM_Standalone)
	{
		Selected = false;
	}
	else if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Error, TEXT("Should not be able to toggle selection as a Dedicated Server."));
	}
}

void AChessPiece::ToggleHighlight()
{
	IsHighlighted = true;
}

void AChessPiece::RemoveHighlight()
{
	IsHighlighted = false;
}

void AChessPiece::ToggleSelected()
{
	if (!MaterialToUse)
		return;

	Selected = true;
}

void AChessPiece::RemoveSelected()
{
	Selected = false;
}

void AChessPiece::UpdateHighlight()
{
	if (GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		if (IsHighlighted)
			MaterialToUse->SetScalarParameterValue("Emissive", 1.f);
		else
			MaterialToUse->SetScalarParameterValue("Emissive", 0.f);

		if (Selected)
			MaterialToUse->SetScalarParameterValue("Emissive", 1.f);
		else
			MaterialToUse->SetScalarParameterValue("Emissive", 0.f);
	}
}

void AChessPiece::GetPossibleMoveHighlight(TArray<int>& indexes)
{
	
}

bool AChessPiece::isValidMove(int IndexToMoveToI, int IndexToMoveToJ)
{
	return false;
}

void AChessPiece::InitializeAllowedMoves()
{

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
	AChessGameState* GameState = Cast<AChessGameState>(GetWorld()->GetGameState());

	return GameState->BoardInstance.Slots[i][j].Position;
}

AActor * AChessPiece::GetActorFromSlot(int i, int j)
{
	if (i >= 8 || i < 0 || j >= 8 || j < 0)
		return nullptr;

	AChessGameState* GameState = Cast<AChessGameState>(GetWorld()->GetGameState());

	return GameState->BoardInstance.Slots[i][j].Occupant;
}

bool AChessPiece::isPieceInSlot(int i, int j)
{
	if (i >= 8 || i < 0 || j >= 8 || j < 0)
		return true;

	AChessGameState* GameState = Cast<AChessGameState>(GetWorld()->GetGameState());

	if (GameState->BoardInstance.Slots[i][j].Occupant == nullptr)
	{
		return false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Piece in next spot: %s"), *GameState->BoardInstance.Slots[i][j].Occupant->GetName());
		return true;
	}
}

void AChessPiece::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AChessPiece, IsHighlighted);
	DOREPLIFETIME(AChessPiece, Selected);
	DOREPLIFETIME(AChessPiece, isOnStartingSquare);
	DOREPLIFETIME(AChessPiece, type);
	DOREPLIFETIME(AChessPiece, AllowedMoves);
	DOREPLIFETIME(AChessPiece, Position);
	DOREPLIFETIME(AChessPiece, CurrentSlotI);
	DOREPLIFETIME(AChessPiece, CurrentSlotJ);
}
