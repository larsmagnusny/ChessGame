// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "Engine/StaticMeshActor.h"
#include "ConstructorHelpers.h"
#include "ChessPiece.generated.h"

class UMaterialInstanceConstant;
class UMaterialInstanceDynamic;
class UStaticMesh;

UCLASS()
class CHESSGAME_API AChessPiece : public AStaticMeshActor
{
	GENERATED_BODY()
public:
	AChessPiece();

	void InitMeshAndMaterial();

	virtual void SetVariables(int type);

	void MultiplayerSafeToggleHighlight();
	void MultiplayerSafeRemoveHighlight();
	void MultiplayerSafeToggleSelected();
	void MultiplayerSafeRemoveSelected();

	//UFUNCTION(Server, Reliable, WithValidation)
	virtual void ToggleHighlight();

	//UFUNCTION(Server, Reliable, WithValidation)
	virtual void RemoveHighlight();

	//UFUNCTION(Server, Reliable, WithValidation)
	virtual void ToggleSelected();

	//UFUNCTION(Server, Reliable, WithValidation)
	virtual void RemoveSelected();

	void UpdateHighlight();

	virtual void GetPossibleMoveHighlight(TArray<int> &indexes);
	virtual bool isValidMove(int IndexToMoveToI, int IndexToMoveToJ);

	virtual void InitializeAllowedMoves();

	int ToOneDimentional(int i, int j);

	FVector GetPositionFromSlot(int i, int j);
	AActor* GetActorFromSlot(int i, int j);
	
	bool isPieceInSlot(int i, int j);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	//UPROPERTY(Replicated)
	bool IsHighlighted = false;

	//UPROPERTY(Replicated)
	bool Selected = false;

	bool MovesAreHighlighted = false;

	//UPROPERTY(Replicated)
	bool isOnStartingSquare = true;

	//UPROPERTY(Replicated)
	int type = 0;

	UStaticMesh* Mesh = nullptr;

	UMaterialInstanceConstant* WhiteMaterial = nullptr;

	UMaterialInstanceConstant* BlackMaterial = nullptr;

	UMaterialInstanceDynamic* MaterialToUse = nullptr;

	//UPROPERTY(Replicated)
	TArray<FVector2D> AllowedMoves;

	//UPROPERTY(Replicated)
	FVector Position;

	//UPROPERTY(Replicated)
	int CurrentSlotI = 0;

	//UPROPERTY(Replicated)
	int CurrentSlotJ = 0;


	bool MeshAndMaterialInitialized = false;
	
};
