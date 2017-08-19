// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChessPiece.h"
#include "ChessRook.generated.h"

/**
 * 
 */
UCLASS()
class CHESSGAME_API AChessRook : public AChessPiece
{
	GENERATED_BODY()
public:
	AChessRook();
	virtual void BeginPlay() override;
	virtual void InitializeAllowedMoves() override;
	virtual void GetPossibleMoveHighlight(TArray<int> &indexes) override;
	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction) override;
	virtual bool isValidMove(int IndexToMoveToI, int IndexToMoveToJ) override;
	
	
};
