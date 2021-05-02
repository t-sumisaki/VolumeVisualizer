// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VolumeVisualizer.generated.h"

class AVolume;

UCLASS(HideCategories=(Object, Collision, Display, Rendering, Physics, Replication, Input, Blueprint), ConversionRoot)
class HKBN_VOLUMEVISUALIZER_API AVolumeVisualizer : public AActor
{
	GENERATED_BODY()

public:
	AVolumeVisualizer(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;

	// ----- Components -----

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "VolumeVisualizer")
		class UProceduralMeshComponent* Mesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "VolumeVisualizer")
		class UBillboardComponent* Billboard;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "VolumeVisualizer")
		class UTextRenderComponent* TextRender;

	// ---- Properties ---

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VolumeVisualizer")
		TSubclassOf<AVolume> TargetVolumeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VolumeVisualizer")
		class UMaterialInterface* VisualizeMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VolumeVisualizer")
		bool bVisible;


	UFUNCTION(BlueprintCallable, Category = "VolumeVisualizer")
		void CreateVisualMesh();

};
