// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessBoard.h"
#include "ChessBoardSlot.h"

ChessBoard::ChessBoard()
{
	Slots = new ChessBoardSlot*[8];

	for (int i = 0; i < 8; i++)
	{
		Slots[i] = new ChessBoardSlot[8];
	}
}

ChessBoard::~ChessBoard()
{
}
