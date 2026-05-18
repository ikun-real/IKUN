// Fill out your copyright notice in the Description page of Project Settings.


#include "MySoftActor.h"

// Sets default values
AMySoftActor::AMySoftActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMySoftActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMySoftActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

