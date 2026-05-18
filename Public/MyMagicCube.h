// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCube.h"
#include "GameFramework/Actor.h"
#include "MyMagicCube.generated.h"

UCLASS()
class ABC_API AMyMagicCube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyMagicCube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// 定义函数
	UFUNCTION()
	void SpawnMyCube();

	UFUNCTION()
	void MyCubeDestory();

	//更新方块相对于魔方的坐标
	UFUNCTION()
	void UpdateComponentLocatedSurface();

	UFUNCTION()
	void GetColor();

	//判断一个组件的tag值是不是颜色有关的
	UFUNCTION()
	bool CheckTagIsColor(FString tag);

	//获得多个方块的中心
	UFUNCTION()
	FVector GetPivot(TArray<AMyCube*> Cubes);

	//魔方旋转
	UFUNCTION()
	float MagicCubeRotate(FString CurrentMoveDirectionOfMouse,FString DraggedComponentLocatedSurface,AMyCube* DraggedCube,float value);

	//旋转运动器Yaw
	UFUNCTION()
	static void YawMotionDevice(FVector Pivot, AActor* TargetActor, float RotationDegree);

	//旋转运动器Roll
	UFUNCTION()
	static void RollMotionDevice(FVector Pivot, AActor* TargetActor, float RotationDegree);

	//旋转运动器Pitch
	UFUNCTION()
	static void PitchMotionDevice(FVector Pivot, AActor* TargetActor, float RotationDegree);

	//参数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MagicCube")
	bool IsAbleRotate;

	UPROPERTY(EditAnywhere, Category = "MagicCube")
	int32 Order = 3;  //阶数

	UPROPERTY(EditAnywhere, Category = "MagicCube")
	int32 SpaceSize = 110;

	UPROPERTY(EditAnywhere, Category = "MagicCube")
	TArray<AMyCube*> MyCubeArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MagicCube")
	TSubclassOf<AMyCube>MyCube;
	
#pragma region FaceColor
	//6个面的颜色
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FaceColor")
	TArray<FString> ForwardFaceColorArray;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FaceColor")
	TArray<FString> BackFaceColorArray;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FaceColor")
	TArray<FString> LeftFaceColorArray;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FaceColor")
	TArray<FString> RightFaceColorArray;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FaceColor")
	TArray<FString> DownFaceColorArray;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FaceColor")
	TArray<FString> UpFaceColorArray;

	UFUNCTION()
	void PrintFaceColor();
#pragma endregion

	//行为身份验证相关
	double PressTime;
	double ReleaseTime;
	TArray<double> PressTimes;
	TArray<double> ReleaseTimes;
	TArray<double> TimeInterval;
	TArray<double> Rotations;
	TArray<double> RotationsSpeed;
	TArray<FString> Faces;
	TArray<FString> Directions;
	TArray<FString> Preferences;
};
