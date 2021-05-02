// Fill out your copyright notice in the Description page of Project Settings.


#include "VolumeVisualizer.h"
#include "ProceduralMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/BillboardComponent.h"

#include "GameFramework/Volume.h"
#include "Components/BrushComponent.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/AggregateGeom.h"
#include "PhysicsEngine/ConvexElem.h"

#include "Math/TransformNonVectorized.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
AVolumeVisualizer::AVolumeVisualizer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bVisible(true)

{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SetCanBeDamaged(false);

	static FName CollisionProfileName(TEXT("NoCollision"));
	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionProfileName(CollisionProfileName);
	Mesh->Mobility = EComponentMobility::Static;
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->CastShadow = false;

	RootComponent = Mesh;

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	if (Billboard)
	{
		Billboard->bHiddenInGame = true;
		Billboard->Mobility = EComponentMobility::Static;
		Billboard->SetupAttachment(RootComponent);
	}

	TextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Label"));
	if (TextRender)
	{
		TextRender->bHiddenInGame = true;
		TextRender->Mobility = EComponentMobility::Static;
		TextRender->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
		TextRender->SetWorldSize(48.f);
		TextRender->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
		TextRender->SetupAttachment(RootComponent);
	}
}

void AVolumeVisualizer::OnConstruction(const FTransform& Transform)
{

#if WITH_EDITOR
	if (TextRender)
	{
		TextRender->SetText(FText::FromString(GetActorLabel()));
	}
#endif

	CreateVisualMesh();
}


void AppendVolumeVertices(const AVolume* Volume, const FTransform T, TArray<FVector>& Verts, TArray<int32>& Tris)
{
	if (!Volume)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target volume is NONE"));
		return;
	}

	auto Body = Volume->GetBrushComponent()->GetBodySetup();

	if (!Body) {
		UE_LOG(LogTemp, Warning, TEXT("Volume has not BodySetup, %s"), *Volume->GetName());
		return;
	}


	for (auto convex : Body->AggGeom.ConvexElems)
	{
		int32 vertsOffset = Verts.Num();
		Verts.AddUninitialized(convex.VertexData.Num());

		for (int32 i = 0; i < convex.VertexData.Num(); ++i)
		{
			Verts[i + vertsOffset] = T.InverseTransformPosition(convex.VertexData[i]);
		}

		int32 trisOffset = Tris.Num();
		Tris.AddUninitialized(convex.IndexData.Num());
		for (int32 i = 0; i < convex.IndexData.Num(); ++i)
		{
			Tris[i + trisOffset] = convex.IndexData[i] + vertsOffset;
		}
	}
}

void AVolumeVisualizer::CreateVisualMesh()
{
	Mesh->ClearAllMeshSections();

	if (!bVisible) return;

	TArray<FVector> Verts;
	TArray<int32> Tris;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FLinearColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TargetVolumeClass, ActorList);

	for (AActor* Actor : ActorList)
	{
		AVolume* Volume = Cast<AVolume>(Actor);
		if (Volume)
		{
			FTransform relativeOffset = GetActorTransform().GetRelativeTransform(Volume->GetActorTransform());
			AppendVolumeVertices(Volume, relativeOffset, Verts, Tris);
		}
	}

	//UE_LOG(LogTemp, Log, TEXT("Create visual mesh, Verts=%d"), Verts.Num());
	Mesh->CreateMeshSection_LinearColor(0, Verts, Tris, Normals, UV0, VertexColors, Tangents, false);

	if (VisualizeMaterial) {
		Mesh->SetMaterial(0, VisualizeMaterial);
	}
}

