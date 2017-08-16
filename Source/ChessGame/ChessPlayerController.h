// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChessPlayerController.generated.h"

class AChessGameGameModeBase;

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

	AChessGameGameModeBase* GameMode = nullptr;

	AActor* LastHighlightedActor = nullptr;
	AActor* LastSelectedActor = nullptr;
	AActor* LastMovesHighlightedActor = nullptr;

	int SlotPointedAtI = 0;
	int SlotPointedAtJ = 0;
	bool HoveringOverSlot = false;
};
