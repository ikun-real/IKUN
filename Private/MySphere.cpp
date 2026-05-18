// Fill out your copyright notice in the Description page of Project Settings.


#include "MySphere.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
// Sets default values
AMySphere::AMySphere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AMySphere::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMySphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMySphere::Spawn()
{
	FVector Pivot = GetActorLocation();
	
	//生成两个方块
	FVector Position1(Pivot.X+100,Pivot.Y,Pivot.Z); 
	AMyCube* cube1 = GetWorld()->SpawnActor<AMyCube>(Cube, Position1, FRotator(0.0f, 0.0f, 0.0f));

	FVector Position2(Pivot.X - 100, Pivot.Y, Pivot.Z);
	AMyCube* cube2 = GetWorld()->SpawnActor<AMyCube>(Cube, Position2, FRotator(0.0f, 0.0f, 0.0f));
}

void AMySphere::Rotate()
{
}

