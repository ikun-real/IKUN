// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "MySphere.h"
#include "GameFramework/Actor.h"
#include "MyActorSpawner.generated.h"

UCLASS()
class ABC_API AMyActorSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActorSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SpwanActor();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UBoxComponent* SpawnVolume;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<AMySphere> SphereClass;
};
