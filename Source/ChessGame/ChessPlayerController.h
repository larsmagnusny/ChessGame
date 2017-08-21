// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "ChessPlayerController.generated.h"

class AChessGameState;
class AChessPiece;
class APlayerController;
class ACameraActor;


USTRUCT()
struct FOccupantData
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Occupant[8];
};

UCLASS()
class CHESSGAME_API AChessPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AChessPlayerController();

	virtual void BeginPlay() override;
	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction) override;
	virtual void BeginPlayingState() override;

	void UpdateHighlight(FHitResult &Hit);
	void UpdateBoardHighlight(FHitResult &Hit);
	int GetSlotIndexFromWorld(FVector HitPoint, int& i, int &j);
	//int ToOneDimentional(int i, int j);

	void MouseLeftClick();

	void RemoveHighlighting(AChessPiece * LastChessPiece);

	// Run on Server
	void SpawnPieces(AActor* OwnerController, int type); 
	void SpawnPawnAtSlot(int i, int j, int type, AActor* Owner);
	void SpawnRookAtSlot(int i, int j, int type, AActor* Owner);
	void SpawnKnightAtSlot(int i, int j, int type, AActor* Owner);
	void SpawnBishopAtSlot(int i, int j, int type, AActor* Owner);
	void SpawnKingAtSlot(int i, int j, int type, AActor* Owner);
	void SpawnQueenAtSlot(int i, int j, int type, AActor* Owner);

	AChessGameState* GameState = nullptr;

	AActor* LastHighlightedActor = nullptr;
	AActor* LastSelectedActor = nullptr;
	AActor* LastMovesHighlightedActor = nullptr;

	int SlotPointedAtI = 0;
	int SlotPointedAtJ = 0;
	bool HoveringOverSlot = false;

	bool CameraViewSet = false;

	ACameraActor* Camera = nullptr;

	UPROPERTY(Replicated)
	int PlayerType;

	UPROPERTY(Replicated)
	FOccupantData OCData[8];

	UFUNCTION(Server, Reliable, WithValidation)
	void UpdateSlots();
	virtual void UpdateSlots_Implementation();
	virtual bool UpdateSlots_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
	void SetOccupant(int I, int J, AActor* Actor);
	virtual void SetOccupant_Implementation(int I, int J, AActor* Actor);
	virtual bool SetOccupant_Validate(int I, int J, AActor* Actor);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
