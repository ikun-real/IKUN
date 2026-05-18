// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActorSpawner.h"
#include "Components/BoxComponent.h"
// Sets default values
AMyActorSpawner::AMyActorSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	SpawnVolume->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AMyActorSpawner::BeginPlay()
{
	Super::BeginPlay();
	SpwanActor();
}

void AMyActorSpawner::SpwanActor()
{
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();
	GetWorld()->SpawnActor<AMySphere>(SphereClass, SpawnLocation, SpawnRotation);
}



// Called every frame
void AMyActorSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


