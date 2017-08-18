// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "ConstructorHelpers.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ChessGameGameModeBase.h"
#include "ChessPiece.generated.h"

/**
 * 
 */
UCLASS()
class CHESSGAME_API AChessPiece : public AStaticMeshActor
{
	GENERATED_BODY()
public:
	AChessPiece();
	virtual void SetVariables(int type);

	virtual void ToggleHighlight();
	virtual void RemoveHighlight();
	virtual void ToggleSelected();
	virtual void RemoveSelected();

	virtual void GetPossibleMoveHighlight(TArray<int> &indexes);
	virtual bool isValidMove(int IndexToMoveToI, int IndexToMoveToJ);

	int ToOneDimentional(int i, int j);

	FVector GetPositionFromSlot(int i, int j);
	AActor* GetActorFromSlot(int i, int j);
	
	bool isPieceInSlot(int i, int j);

	bool IsHighlighted = false;
	bool Selected = false;

	bool MovesAreHighlighted = false;
	bool isOnStartingSquare = true;

	int type = 0;
	UStaticMesh* Mesh = nullptr;
	UMaterialInstanceConstant* WhiteMaterial = nullptr;
	UMaterialInstanceConstant* BlackMaterial = nullptr;
	UMaterialInstanceDynamic* MaterialToUse = nullptr;

	TArray<FVector2D> AllowedMoves;
	FVector Position;
	int CurrentSlotI = 0;
	int CurrentSlotJ = 0;
	
};
