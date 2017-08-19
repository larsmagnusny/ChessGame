// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessBoardActor.h"
#include "ConstructorHelpers.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "ChessGameState.h"

#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"

AChessBoardActor::AChessBoardActor()
{
	bReplicates = true;
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;

	ColorNames = new FString[64];

	ColorNames[0] = "HighlightColor";

	for (int i = 1; i < 64; i++)
	{
		ColorNames[i] = ColorNames[0] + "_" + FString::FromInt(i);
	}

	for (int i = 0; i < 64; i++)
		ColorValues.Add(FLinearColor(1.0f, 1.0f, 1.0f));

	OnOffNames = new FString[64];

	OnOffNames[0] = "OnOff";

	for (int i = 1; i < 64; i++)
	{
		OnOffNames[i] = OnOffNames[0] + "_" + FString::FromInt(i);
	}

	for (int i = 0; i < 64; i++)
		OnOffValues.Add(0.f);

	// Load Mesh to use
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshLoader(TEXT("StaticMesh'/Game/GameBoard.GameBoard'"));

	if (MeshLoader.Succeeded())
	{
		Mesh = MeshLoader.Object;
	}

	// Load Material Instance
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> MaterialLoader(TEXT("MaterialInstanceConstant'/Game/ChessBoard_Inst.ChessBoard_Inst'"));

	if (MaterialLoader.Succeeded())
	{
		MaterialConstantInstance = MaterialLoader.Object;
	}
}

void AChessBoardActor::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	GameState = Cast<AChessGameState>(World->GetGameState());

	GameState->HighlightController = this;

	// Change Material Of object to a dynamic instance
	UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));


	if (Mesh != nullptr)
	{
		MeshComponent->SetMobility(EComponentMobility::Movable);
		MeshComponent->SetStaticMesh(Mesh);
	}
	if (MaterialConstantInstance != nullptr)
		MaterialToUse = UMaterialInstanceDynamic::Create(MaterialConstantInstance, this);

	if (MaterialToUse != nullptr)
		MeshComponent->SetMaterial(0, MaterialToUse);
}


// Called every frame
void AChessBoardActor::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	// Update Material Paramaters

	if (GetNetMode() != ENetMode::NM_DedicatedServer)
		UpdateMaterialParameters();
}

void AChessBoardActor::UpdateMaterialParameters()
{
	// Update OnOff
	for (int i = 0; i < 64; i++)
	{
		MaterialToUse->SetScalarParameterValue(FName(*OnOffNames[i]), OnOffValues[i]);
	}


	// Update Colors
	for (int i = 0; i < 64; i++)
	{
		MaterialToUse->SetVectorParameterValue(FName(*ColorNames[i]), ColorValues[i]);
	}
}

void AChessBoardActor::SetOnOffByIndex(int i, float value)
{
	if (i < 64)
	{
		OnOffValues[i] = value;
	}
}

void AChessBoardActor::SetColorByIndex(int i, FLinearColor Color)
{
	if (i < 64)
	{
		ColorValues[i] = Color;
	}
}

void AChessBoardActor::SetAllOff()
{
	for (int i = 0; i < 64; i++)
	{
		OnOffValues[i] = 0.f;
	}
}

void AChessBoardActor::ClearAllColor()
{
	for (int i = 0; i < 64; i++)
	{
		ColorValues[i] = FLinearColor(0.f, 0.f, 0.f);
	}
}

void AChessBoardActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AChessBoardActor, ColorValues);
	DOREPLIFETIME(AChessBoardActor, OnOffValues);
}

