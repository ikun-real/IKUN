// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyCube.generated.h"

UCLASS()
class ABC_API AMyCube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyCube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma region MyRegion

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "CubeBlock")
	TArray<UStaticMeshComponent*> FaceMeshes;

	UPROPERTY(VisibleAnywhere, Category = "CubeBlock")
	TArray<FString>Colors{ "Red","Green","Yellow","Blue","Purple","White" };

    UPROPERTY(VisibleAnywhere, Category = "CubeBlock")

    int32 FaceSize = 50;

    TArray<FVector> Positions = {
       FVector(FaceSize, 0, 0),    // 右
       FVector(-FaceSize, 0, 0),   // 左
       FVector(0, FaceSize, 0),    // 前
       FVector(0, -FaceSize, 0),   // 后
       FVector(0, 0, FaceSize),    // 上
       FVector(0, 0, -FaceSize)    // 下
    };

    TArray<FRotator> Rotations = {
        FRotator(90, 0, 0),   // 右
        FRotator(-90, 0, 0),  // 左
        FRotator(0, 0, 90),    // 前
        FRotator(0, 0, -90),  // 后
        FRotator(0, 0, 0),  // 上
        FRotator(0, 0, 180)    // 下
    }; 
#pragma endregion

    //单一方块在魔方中的坐标
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "3D_Coordinate")
    int32 X=0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "3D_Coordinate")
    int32 Y=0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "3D_Coordinate")
    int32 Z=0;

    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "3D_Coordinate")
    AActor* Parent;
};
