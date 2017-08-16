// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChessBoardSlot.h"

/**
 * 
 */
class CHESSGAME_API ChessBoard
{
public:
	ChessBoard();
	~ChessBoard();

	ChessBoardSlot** Slots;
};
