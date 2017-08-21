// Fill out your copyright notice in the Description page of Project Settings.
#include "ChessPiece.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ChessGameState.h"
#include "ChessPlayerController.h"

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
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	bOnlyRelevantToOwner = false;
	bReplicates = true;
}

void AChessPiece::StaticLoadMaterials()
{

}

void AChessPiece::StaticLoadMesh()
{

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
		IsHighlighted = true;
	}
	else if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Error, TEXT("Should not be able to toggle as a Dedicated Server."));
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
		UE_LOG(LogTemp, Error, TEXT("Should not be able to toggle as a Dedicated Server."));
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
		UE_LOG(LogTemp, Error, TEXT("Should not be able to toggle as a Dedicated Server."));
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
		UE_LOG(LogTemp, Error, TEXT("Should not be able to toggle as a Dedicated Server."));
	}
}

void AChessPiece::MultiplayerSafeMoveSelected(int ToI, int ToJ)
{
	if (GetNetMode() == ENetMode::NM_Client)
	{
		MoveSelected(ToI, ToJ);
	}
	else if (GetNetMode() == ENetMode::NM_Standalone)
	{
		CurrentSlotI = ToI;
		CurrentSlotJ = ToJ;
	}
	else if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Error, TEXT("Should not be able to toggle as a Dedicated Server."));
	}
}

void AChessPiece::ToggleHighlight_Implementation()
{
	IsHighlighted = true;
	UE_LOG(LogTemp, Warning, TEXT("Highlight has been sendt to server"));
}

bool AChessPiece::ToggleHighlight_Validate()
{
	return true;
}

void AChessPiece::RemoveHighlight_Implementation()
{
	IsHighlighted = false;
	UE_LOG(LogTemp, Warning, TEXT("Highlight has been sendt to server"));
}

bool AChessPiece::RemoveHighlight_Validate()
{
	return true;
}

void AChessPiece::ToggleSelected_Implementation()
{
	Selected = true;
}

bool AChessPiece::ToggleSelected_Validate()
{
	return true;
}

void AChessPiece::RemoveSelected_Implementation()
{
	Selected = false;
}

bool AChessPiece::RemoveSelected_Validate()
{
	return true;
}

void AChessPiece::MoveSelected_Implementation(int ToI, int ToJ)
{
	// Runs on Server
	CurrentSlotI = ToI;
	CurrentSlotJ = ToJ;
}

bool AChessPiece::MoveSelected_Validate(int ToI, int ToJ)
{
	return true;
}

void AChessPiece::UpdateHighlight()
{
	float CurrentValue;
		
	MaterialToUse->GetScalarParameterValue("Emissive", CurrentValue);

	if (IsHighlighted && LastIsHighlighted != IsHighlighted || (Selected && LastIsSelected != Selected))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has been highlighted"), *GetName());
		MaterialToUse->SetScalarParameterValue("Emissive", 1.f);
	}
	else if (!IsHighlighted && LastIsHighlighted != IsHighlighted || (!Selected && LastIsSelected != Selected))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has had highlighting removed"), *GetName());
		MaterialToUse->SetScalarParameterValue("Emissive", 0.f);
		
	}
	LastIsHighlighted = IsHighlighted;
	LastIsSelected = Selected;
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

	return GameState->Slots[i].Position[j];
}

AActor * AChessPiece::GetActorFromSlot(int i, int j)
{
	if (i >= 8 || i < 0 || j >= 8 || j < 0)
		return nullptr;

	AChessPlayerController* PlayerController = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());

	return PlayerController->OCData[i].Occupant[j];
}

bool AChessPiece::isPieceInSlot(int i, int j)
{
	if (i >= 8 || i < 0 || j >= 8 || j < 0)
		return true;

	AChessPlayerController* PlayerController = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());

	if (PlayerController->OCData[i].Occupant[j] == nullptr)
	{
		return false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Piece in next spot: %s"), *PlayerController->OCData[i].Occupant[j]->GetName());
		return true;
	}
}

void AChessPiece::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AChessPiece, IsHighlighted, COND_None);
	DOREPLIFETIME_CONDITION(AChessPiece, Selected, COND_None);
	DOREPLIFETIME_CONDITION(AChessPiece, CurrentSlotI, COND_None);
	DOREPLIFETIME_CONDITION(AChessPiece, CurrentSlotJ, COND_None);
	DOREPLIFETIME_CONDITION(AChessPiece, type, COND_None);
}

void AChessPiece::ClientInit_Implementation()
{
	if (GetNetMode() == ENetMode::NM_Client)
	{
		//InitMeshAndMaterial();
		//AChessGameState* GameState = Cast<AChessGameState>(GetWorld()->GetGameState());

		//GameState->InitCallback(this);

		UE_LOG(LogTemp, Warning, TEXT("Ran ClientInit on Client"));
	}
	if (GetNetMode() == ENetMode::NM_DedicatedServer)
		UE_LOG(LogTemp, Warning, TEXT("Ran ClientInit on Server"));
}