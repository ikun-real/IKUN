// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCube.h"
#include "GameFramework/Actor.h"
#include "MySphere.generated.h"

UCLASS()
class ABC_API AMySphere : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMySphere();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "666")
	TSubclassOf<AMyCube>Cube;

	UFUNCTION()
	void Spawn();

	UFUNCTION()
	static void Rotate();
};
