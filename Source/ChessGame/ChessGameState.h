// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChessBoard.h"
#include "GameFramework/GameState.h"
#include "GameFramework/Actor.h"
#include "ChessGameState.generated.h"

class AChessGameGameModeBase;

UCLASS()
class CHESSGAME_API AChessGameState : public AGameState
{
	GENERATED_BODY()
public:
	AChessGameState();

	virtual void BeginPlay() override;
	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	bool IsClickable(AActor* Actor);

	void SpawnPieces();

	void SpawnPawnAtSlot(int i, int j, int type);
	void SpawnRookAtSlot(int i, int j, int type);
	void SpawnKnightAtSlot(int i, int j, int type);
	void SpawnBishopAtSlot(int i, int j, int type);
	void SpawnKingAtSlot(int i, int j, int type);
	void SpawnQueenAtSlot(int i, int j, int type);

	UPROPERTY(Replicated)
	float RandomFloat = 0.f;

	UPROPERTY(Replicated)
	int CurrentPlayers = 0;

	UPROPERTY(Replicated)
	bool NextPlayer = false;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Run Camera Turn Animation?")
	bool RunCameraAnimation = false;

	TArray<AActor*> ClickableActors;

	AActor* CurrentlySelectedActor = nullptr;

	ChessBoard BoardInstance;

	AActor* HighlightController = nullptr;

	AChessGameGameModeBase* GameMode = nullptr;

	FVector StartPosition = FVector(-18.f, 18.f, 1.f);
	float Delta = 5.13f;
};
