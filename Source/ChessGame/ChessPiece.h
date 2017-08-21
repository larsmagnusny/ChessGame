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

	void MultiplayerSafeMoveSelected(int ToI, int ToJ);

	UFUNCTION(Server, Reliable, WithValidation)
	void ToggleHighlight();
	virtual void ToggleHighlight_Implementation();
	virtual bool ToggleHighlight_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
	void RemoveHighlight();
	virtual void RemoveHighlight_Implementation();
	virtual bool RemoveHighlight_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
	void ToggleSelected();
	virtual void ToggleSelected_Implementation();
	virtual bool ToggleSelected_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
	void RemoveSelected();
	virtual void RemoveSelected_Implementation();
	virtual bool RemoveSelected_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
	void MoveSelected(int ToI, int ToJ);
	virtual void MoveSelected_Implementation(int ToI, int ToJ);
	virtual bool MoveSelected_Validate(int ToI, int ToJ);

	void UpdateHighlight();

	virtual void GetPossibleMoveHighlight(TArray<int> &indexes);
	virtual bool isValidMove(int IndexToMoveToI, int IndexToMoveToJ);

	virtual void InitializeAllowedMoves();

	int ToOneDimentional(int i, int j);

	FVector GetPositionFromSlot(int i, int j);
	AActor* GetActorFromSlot(int i, int j);
	
	bool isPieceInSlot(int i, int j);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual void StaticLoadMaterials();

	virtual void StaticLoadMesh();

	UPROPERTY(Replicated)
	bool IsHighlighted = false;
	UPROPERTY(Replicated)
	bool Selected = false;

	bool LastIsHighlighted = false;
	bool LastIsSelected = false;

	bool MovesAreHighlighted = false;

	//UPROPERTY(Replicated)
	bool isOnStartingSquare = true;

	UPROPERTY(Replicated)
	int type = 0;

	UStaticMesh* Mesh = nullptr;

	UMaterialInstanceConstant* WhiteMaterial = nullptr;

	UMaterialInstanceConstant* BlackMaterial = nullptr;

	UMaterialInstanceDynamic* MaterialToUse = nullptr;

	//UPROPERTY(Replicated)
	TArray<FVector2D> AllowedMoves;

	//UPROPERTY(Replicated)
	FVector Position;

	UPROPERTY(Replicated)
	int CurrentSlotI = 0;

	UPROPERTY(Replicated)
	int CurrentSlotJ = 0;

	bool MeshAndMaterialInitialized = false;

	bool MessageSendtToClients = false;

	UFUNCTION(NetMulticast, Reliable)
	void ClientInit();
	virtual void ClientInit_Implementation();
};
