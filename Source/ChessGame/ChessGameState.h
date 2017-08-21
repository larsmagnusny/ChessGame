// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameFramework/Actor.h"
#include "ChessGameState.generated.h"

class AChessGameGameModeBase;

USTRUCT()
struct FSlotData
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Position[8];
	UPROPERTY()
	float Left[8];
	UPROPERTY()
	float Right[8];
	UPROPERTY()
	float Top[8];
	UPROPERTY()
	float Bottom[8];
	UPROPERTY()
	AActor* Occupant[8];
};

UCLASS()
class CHESSGAME_API AChessGameState : public AGameState
{
	GENERATED_BODY()
public:
	AChessGameState();

	virtual void BeginPlay() override;
	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction) override;

	bool IsClickable(AActor* Actor);

	void DebugData();
	int CurrentPlayers = 0;
	bool NextPlayer = false;

	TArray<AActor*> ClickableActors;
	AActor* CurrentlySelectedActor = nullptr;
	AActor* HighlightController = nullptr;
	FSlotData Slots[8];
	FVector StartPosition = FVector(-18.f, 18.f, 1.f);
	float Delta = 5.13f;
};
