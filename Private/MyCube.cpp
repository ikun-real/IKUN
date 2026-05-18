// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCube.h"
#include "Engine/World.h"
// Sets default values
AMyCube::AMyCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	for (int32 i = 0; i < 6; i++)
	{
		UStaticMeshComponent* FaceMesh = CreateDefaultSubobject<UStaticMeshComponent>(*FString::Printf(TEXT("FaceMesh_%s"), *Colors[i]));
		FaceMesh->ComponentTags.Add(*Colors[i]);
		FaceMesh->SetupAttachment(RootComponent);
		FaceMesh->SetRelativeLocation(Positions[i]);
		FaceMesh->SetRelativeRotation(Rotations[i]);
		FaceMeshes.Add(FaceMesh);
	}
}

// Called when the game starts or when spawned
void AMyCube::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}