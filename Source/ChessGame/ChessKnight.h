// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChessPiece.h"
#include "ChessKnight.generated.h"

/**
 * 
 */
UCLASS()
class CHESSGAME_API AChessKnight : public AChessPiece
{
	GENERATED_BODY()
public:
	AChessKnight();
	virtual void BeginPlay() override;
	virtual void InitializeAllowedMoves() override;
	virtual void GetPossibleMoveHighlight(TArray<int> &indexes) override;
	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction) override;
	virtual bool isValidMove(int IndexToMoveToI, int IndexToMoveToJ) override;
	
	
};
