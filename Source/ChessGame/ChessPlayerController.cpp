// Fill out your copyright notice in the Description page of Project Settings.
#include "ChessPlayerController.h"
#include "ChessGameState.h"
#include "ChessBoardActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "ChessPiece.h"
#include "Engine/World.h"
#include "Camera/CameraActor.h"
#include "GameFramework/Pawn.h"
#include "ChessPawn.h"
#include "ChessRook.h"
#include "ChessKnight.h"
#include "ChessBishop.h"
#include "ChessKing.h"
#include "ChessQueen.h"
//#include "ChessPlayerController.h"

AChessPlayerController::AChessPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	PrimaryActorTick.bCanEverTick = true;
	bAutoManageActiveCameraTarget = false;
}

void AChessPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!GameState)
		GameState = Cast<AChessGameState>(GetWorld()->GetGameState());

	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		if (GameState->CurrentPlayers == 0)
		{
			PlayerType = 0;
			
			UE_LOG(LogTemp, Warning, TEXT("First Player Joined"));

			GameState->CurrentPlayers++;
		}
		else
		{
			PlayerType = 1;
			UE_LOG(LogTemp, Warning, TEXT("Second Player Joined"));

			GameState->CurrentPlayers++;
		}
	}

	if (GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		InputComponent->BindAction("LeftMouseButton", IE_Released, this, &AChessPlayerController::MouseLeftClick);
	}

	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		SpawnPieces(this, PlayerType);
	}
	else
	{
		FTransform SpawnTransform(FRotator(0, 0, 0), FVector(0, 0, 0));

		AChessBoardActor* DeferredChessBoard = Cast<AChessBoardActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessBoardActor::StaticClass(), SpawnTransform));

		if (DeferredChessBoard != nullptr)
		{
			DeferredChessBoard->SetOwner(this);
			DeferredChessBoard->FinishSpawning(SpawnTransform);
			DeferredChessBoard->SetReplicates(false);
		}

		
	}
}

void AChessPlayerController::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
	
	if (GetNetMode() == ENetMode::NM_DedicatedServer)
		return;

	FHitResult Hit;

	GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), true, Hit);

	UpdateHighlight(Hit);
	UpdateBoardHighlight(Hit);

	if (!CameraViewSet || (GetViewTarget() != Camera && Camera != nullptr))
	{
		if (GetNetMode() == ENetMode::NM_Client)
		{
			UE_LOG(LogTemp, Warning, TEXT("Camera set on client: %s"), *PlayerCameraManager->GetViewTarget()->GetName());
		}
		if (GetNetMode() == ENetMode::NM_DedicatedServer)
		{
			UE_LOG(LogTemp, Warning, TEXT("Camera set on Dedicated Server"), *PlayerCameraManager->GetViewTarget()->GetName());
		}
		if (GetNetMode() == ENetMode::NM_Standalone)
		{
			UE_LOG(LogTemp, Warning, TEXT("Camera set on StandAlone"), *PlayerCameraManager->GetViewTarget()->GetName());
		}
		
		FVector Location = FVector(0, 17.f, 36.f);
		FRotator Rotation = FRotator(-70.f, -90.f, 0.f);

		if (PlayerType == 1)
		{
			Location = FVector(0.f, -17.f, 36.f);
			Rotation = FRotator(-70.f, 90.f, 0.f);
		}

		FActorSpawnParameters SpawnInfo;

		Camera = GetWorld()->SpawnActor<ACameraActor>(Location, Rotation, SpawnInfo);

		SetViewTarget(Camera);

		CameraViewSet = true;

		UpdateSlots();
	}
}

void AChessPlayerController::BeginPlayingState()
{
}

void AChessPlayerController::UpdateHighlight(FHitResult &Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerType: %s"), *FString::FromInt(PlayerType));
	if (GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		if (Hit.Actor != nullptr)
		{
			if (GameState->IsClickable(Hit.Actor.Get()))
			{
				AChessPiece* ChessPiece = Cast<AChessPiece>(Hit.Actor.Get());
				// Make the actor shine!

				if (Hit.Actor.Get() != LastHighlightedActor && LastHighlightedActor != nullptr)
				{
					AChessPiece* LastChessPiece = Cast<AChessPiece>(LastHighlightedActor);

					LastChessPiece->MultiplayerSafeRemoveHighlight();
					LastHighlightedActor = nullptr;
				}
				else if (PlayerType == ChessPiece->type && LastHighlightedActor == nullptr)
				{
					UE_LOG(LogTemp, Warning, TEXT("ToggleHighlight Called"));
					ChessPiece->MultiplayerSafeToggleHighlight();
					LastHighlightedActor = Hit.Actor.Get();
				}

				UE_LOG(LogTemp, Warning, TEXT("ChessPieceType: %s"), *FString::FromInt(ChessPiece->type));
			}
			else
			{
				if (LastHighlightedActor)
				{
					AChessPiece* ChessPiece = Cast<AChessPiece>(LastHighlightedActor);

					ChessPiece->MultiplayerSafeRemoveHighlight();
					LastHighlightedActor = nullptr;
				}
			}
		}
		else
		{
			if (LastHighlightedActor)
			{
				AChessPiece* ChessPiece = Cast<AChessPiece>(LastHighlightedActor);

				ChessPiece->MultiplayerSafeRemoveHighlight();
				LastHighlightedActor = nullptr;
			}
		}
	}
}

void AChessPlayerController::UpdateBoardHighlight(FHitResult & Hit)
{
	if (LastSelectedActor != nullptr)
	{
		AChessPiece* ChessPiece = Cast<AChessPiece>(LastSelectedActor);

		if (!ChessPiece->MovesAreHighlighted)
		{
			AChessBoardActor* HighlightController = Cast<AChessBoardActor>(GameState->HighlightController);

			TArray<int> IndexesToHighlight;
			ChessPiece->GetPossibleMoveHighlight(IndexesToHighlight);

			for (int i = 0; i < IndexesToHighlight.Num(); i++)
			{
				HighlightController->SetOnOffByIndex(63 - IndexesToHighlight[i], 1.0f);
			}

			LastMovesHighlightedActor = LastSelectedActor;

			ChessPiece->MovesAreHighlighted = true;
		}

		GetSlotIndexFromWorld(Hit.ImpactPoint, SlotPointedAtI, SlotPointedAtJ);
	}
}

int AChessPlayerController::GetSlotIndexFromWorld(FVector HitPoint, int& I, int &J)
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			// Care only about X / Y coordinates...
			if (HitPoint.X >= GameState->Slots[i].Position[j].X - GameState->Slots[i].Left[j] && HitPoint.X <= GameState->Slots[i].Position[j].X + GameState->Slots[i].Right[j])
			{
				if (HitPoint.Y >= GameState->Slots[i].Position[j].Y - GameState->Slots[i].Top[j] && HitPoint.Y <= GameState->Slots[i].Position[j].Y + GameState->Slots[i].Bottom[j])
				{
					I = i;
					J = j;
					HoveringOverSlot = true;
					return i * 8 + (7-j);
				}
			}
		}
	}

	HoveringOverSlot = false;

	return -1;
}



void AChessPlayerController::MouseLeftClick()
{

	FHitResult Hit;

	GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), false, Hit);

	// We are clicking on an actor
	if (Hit.Actor != nullptr)
	{
		AChessPiece* ChessPiece = Cast<AChessPiece>(Hit.Actor.Get());
		AChessPiece* LastChessPiece = nullptr;

		if (LastSelectedActor != nullptr)
			LastChessPiece = Cast<AChessPiece>(LastSelectedActor);

		if (GameState->IsClickable(Hit.Actor.Get()))
		{
			if (LastSelectedActor != Hit.Actor.Get() && LastSelectedActor != nullptr)
			{
				if (LastChessPiece->type == ChessPiece->type)
				{
					// Remove Selection and Board Highlighting
					RemoveHighlighting(LastChessPiece);
				}
				else
				{
					if (LastChessPiece->isValidMove(ChessPiece->CurrentSlotI, ChessPiece->CurrentSlotJ))
					{
						int SlotToMoveToI = ChessPiece->CurrentSlotI;
						int SlotToMoveToJ = ChessPiece->CurrentSlotJ;
						if (OCData[SlotToMoveToI].Occupant[SlotToMoveToJ] != nullptr)
						{
							//GameState->Slots[SlotToMoveToI].Occupant[SlotToMoveToJ]->Destroy();
							SetOccupant(SlotToMoveToI, SlotToMoveToJ, nullptr);
						}

						SetOccupant(LastChessPiece->CurrentSlotI, LastChessPiece->CurrentSlotJ, nullptr);
						LastChessPiece->MultiplayerSafeMoveSelected(SlotToMoveToI, SlotToMoveToJ);

						LastChessPiece->isOnStartingSquare = false;
						SetOccupant(SlotToMoveToI, SlotToMoveToJ, LastChessPiece);

						// Remove Selection and Board Highlighting
						RemoveHighlighting(LastChessPiece);

						// Next player to move
						UpdateSlots();

						return;
					}
					else
					{
						// Remove Selection and Board Highlighting
						RemoveHighlighting(LastChessPiece);
						return;
					}
				}
			}

			if (LastSelectedActor == Hit.Actor.Get() && LastSelectedActor != nullptr)
			{
				// Remove Selection and Board Highlighting
				RemoveHighlighting(ChessPiece);
			}
			else if(PlayerType == ChessPiece->type)
			{
				ChessPiece->MultiplayerSafeToggleSelected();
				GameState->CurrentlySelectedActor = ChessPiece;
				LastSelectedActor = ChessPiece;
			}
		}
		else
		{
			if (LastSelectedActor != nullptr)
			{
				
				AChessPiece* LastChessPiece = Cast<AChessPiece>(LastSelectedActor);

				// Check if the selected piece can move to the slot you are pointing at
				if (HoveringOverSlot)
				{
					UE_LOG(LogTemp, Warning, TEXT("Clicked a slot while a piece was selected"));
					// Check if we can actually move to the slot we clicked...

					if (LastChessPiece->isValidMove(SlotPointedAtI, SlotPointedAtJ))
					{
						// Check if there is a chesspiece in the spot you want to move, so you can take it.
						if (OCData[SlotPointedAtI].Occupant[SlotPointedAtJ] != nullptr)
						{
							//GameState->Slots[SlotPointedAtI].Occupant[SlotPointedAtJ]->Destroy();
							SetOccupant(SlotPointedAtI, SlotPointedAtJ, nullptr);
						}

						SetOccupant(LastChessPiece->CurrentSlotI, LastChessPiece->CurrentSlotJ, nullptr);
						LastChessPiece->MultiplayerSafeMoveSelected(SlotPointedAtI, SlotPointedAtJ);
						LastChessPiece->isOnStartingSquare = false;

						SetOccupant(SlotPointedAtI, SlotPointedAtJ, nullptr);

						// Next player turn...
						UpdateSlots();

						RemoveHighlighting(LastChessPiece);

						return;
					}
				}

				// Remove Selection and Board Highlighting
				RemoveHighlighting(LastChessPiece);
			}
		}
	}
	else
	{
		if (LastSelectedActor != nullptr)
		{
			AChessPiece* LastChessPiece = Cast<AChessPiece>(LastSelectedActor);

			// Remove Selection and Board Highlighting
			RemoveHighlighting(LastChessPiece);
		}
	}
}

void AChessPlayerController::RemoveHighlighting(AChessPiece * LastChessPiece)
{
	LastChessPiece->MultiplayerSafeRemoveSelected();
	LastSelectedActor = nullptr;
	GameState->CurrentlySelectedActor = nullptr;
	Cast<AChessBoardActor>(GameState->HighlightController)->SetAllOff();
	Cast<AChessPiece>(LastMovesHighlightedActor)->MovesAreHighlighted = false;
}

void AChessPlayerController::SpawnPieces(AActor * OwnerController, int type)
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnPieces called on Dedicated Server"));
	int counter = 0;
	if (type == 0)
	{
		// Spawn 8*2 pawns
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if (counter == 0 || counter == 7)
					SpawnRookAtSlot(i, j, 0, OwnerController);
				else if (counter == 1 || counter == 6)
					SpawnKnightAtSlot(i, j, 0, OwnerController);
				else if (counter == 2 || counter == 5)
					SpawnBishopAtSlot(i, j, 0, OwnerController);
				else if (counter == 3)
					SpawnQueenAtSlot(i, j, 0, OwnerController);
				else if (counter == 4)
					SpawnKingAtSlot(i, j, 0, OwnerController);
				else
					SpawnPawnAtSlot(i, j, 0, OwnerController);
				//UE_LOG(LogTemp, Warning, TEXT("%s"), *FString::FromInt(counter));
				counter++;
			}
		}
	}
	else
	{

		counter = 0;
		for (int i = 7; i > 5; i--)
		{
			for (int j = 0; j < 8; j++)
			{
				if (counter == 0 || counter == 7)
					SpawnRookAtSlot(i, j, 1, OwnerController);
				else if (counter == 1 || counter == 6)
					SpawnKnightAtSlot(i, j, 1, OwnerController);
				else if (counter == 2 || counter == 5)
					SpawnBishopAtSlot(i, j, 1, OwnerController);
				else if (counter == 3)
					SpawnQueenAtSlot(i, j, 1, OwnerController);
				else if (counter == 4)
					SpawnKingAtSlot(i, j, 1, OwnerController);
				else
					SpawnPawnAtSlot(i, j, 1, OwnerController);

				counter++;
			}
		}
	}
}

void AChessPlayerController::SpawnPawnAtSlot(int i, int j, int type, AActor* Owner)
{
	FTransform SpawnTransform(FRotator(0, 0, 0), GameState->Slots[i].Position[j]);

	AChessPawn* DeferredChessPawn = Cast<AChessPawn>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessPawn::StaticClass(), SpawnTransform));

	if (DeferredChessPawn != nullptr)
	{
		DeferredChessPawn->SetOwner(Owner);
		DeferredChessPawn->SetVariables(type);
		DeferredChessPawn->CurrentSlotI = i;
		DeferredChessPawn->CurrentSlotJ = j;

		GameState->Slots[i].Occupant[j] = Cast<AActor>(DeferredChessPawn);
		UGameplayStatics::FinishSpawningActor(DeferredChessPawn, SpawnTransform);
	}
}

void AChessPlayerController::SpawnRookAtSlot(int i, int j, int type, AActor* Owner)
{
	FTransform SpawnTransform(FRotator(0, 0, 0), GameState->Slots[i].Position[j]);

	AChessRook* DeferredChessRook = Cast<AChessRook>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessRook::StaticClass(), SpawnTransform));

	if (DeferredChessRook != nullptr)
	{
		DeferredChessRook->SetOwner(Owner);
		DeferredChessRook->SetVariables(type);
		DeferredChessRook->CurrentSlotI = i;
		DeferredChessRook->CurrentSlotJ = j;

		GameState->Slots[i].Occupant[j] = Cast<AActor>(DeferredChessRook);
		UGameplayStatics::FinishSpawningActor(DeferredChessRook, SpawnTransform);
	}
}

void AChessPlayerController::SpawnKnightAtSlot(int i, int j, int type, AActor* Owner)
{
	float Yaw = 0;
	if (type == 1)
		Yaw = 180;

	FTransform SpawnTransform(FRotator(0, Yaw, 0), GameState->Slots[i].Position[j]);

	AChessKnight* DeferredChessKnight = Cast<AChessKnight>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessKnight::StaticClass(), SpawnTransform));

	if (DeferredChessKnight != nullptr)
	{
		DeferredChessKnight->SetOwner(Owner);
		DeferredChessKnight->SetVariables(type);
		DeferredChessKnight->CurrentSlotI = i;
		DeferredChessKnight->CurrentSlotJ = j;

		GameState->Slots[i].Occupant[j] = Cast<AActor>(DeferredChessKnight);
		UGameplayStatics::FinishSpawningActor(DeferredChessKnight, SpawnTransform);
	}
}

void AChessPlayerController::SpawnBishopAtSlot(int i, int j, int type, AActor* Owner)
{
	FTransform SpawnTransform(FRotator(0, 0, 0), GameState->Slots[i].Position[j]);

	AChessBishop* DeferredChessBishop = Cast<AChessBishop>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessBishop::StaticClass(), SpawnTransform));

	if (DeferredChessBishop != nullptr)
	{
		DeferredChessBishop->SetOwner(Owner);
		DeferredChessBishop->SetVariables(type);
		DeferredChessBishop->CurrentSlotI = i;
		DeferredChessBishop->CurrentSlotJ = j;

		GameState->Slots[i].Occupant[j] = Cast<AActor>(DeferredChessBishop);
		UGameplayStatics::FinishSpawningActor(DeferredChessBishop, SpawnTransform);
	}
}

void AChessPlayerController::SpawnKingAtSlot(int i, int j, int type, AActor* Owner)
{
	FTransform SpawnTransform(FRotator(0, 0, 0), GameState->Slots[i].Position[j]);

	AChessKing* DeferredChessKing = Cast<AChessKing>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessKing::StaticClass(), SpawnTransform));

	if (DeferredChessKing != nullptr)
	{
		DeferredChessKing->SetOwner(Owner);
		DeferredChessKing->SetVariables(type);
		DeferredChessKing->CurrentSlotI = i;
		DeferredChessKing->CurrentSlotJ = j;

		GameState->Slots[i].Occupant[j] = Cast<AActor>(DeferredChessKing);
		UGameplayStatics::FinishSpawningActor(DeferredChessKing, SpawnTransform);
	}
}

void AChessPlayerController::SpawnQueenAtSlot(int i, int j, int type, AActor* Owner)
{
	FTransform SpawnTransform(FRotator(0, 0, 0), GameState->Slots[i].Position[j]);

	AChessQueen* DeferredChessQueen = Cast<AChessQueen>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, AChessQueen::StaticClass(), SpawnTransform));

	if (DeferredChessQueen != nullptr)
	{
		DeferredChessQueen->SetOwner(Owner);
		DeferredChessQueen->SetVariables(type);
		DeferredChessQueen->CurrentSlotI = i;
		DeferredChessQueen->CurrentSlotJ = j;

		GameState->Slots[i].Occupant[j] = Cast<AActor>(DeferredChessQueen);
		UGameplayStatics::FinishSpawningActor(DeferredChessQueen, SpawnTransform);
	}
}

void AChessPlayerController::UpdateSlots_Implementation()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			OCData[i].Occupant[j] = GameState->Slots[i].Occupant[j];
		}
	}
}

bool AChessPlayerController::UpdateSlots_Validate()
{
	return true;
}

void AChessPlayerController::SetOccupant_Implementation(int I, int J, AActor * Actor)
{
	GameState->Slots[I].Occupant[J] = Actor;
	UpdateSlots_Implementation();	// Update the player controller occupation slots...
}

bool AChessPlayerController::SetOccupant_Validate(int I, int J, AActor * Actor)
{
	return true;
}

void AChessPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AChessPlayerController, PlayerType);
	DOREPLIFETIME(AChessPlayerController, OCData);
}