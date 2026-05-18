// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMagicCube.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
// Sets default values
AMyMagicCube::AMyMagicCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	IsAbleRotate = true;
	Order = 3; //阶数
}

// Called when the game starts or when spawned
void AMyMagicCube::BeginPlay()
{
	Super::BeginPlay();
	SpawnMyCube();
	GetColor();
}

//初始化魔方
void AMyMagicCube::SpawnMyCube()
{	
	FVector BasicPosition = GetActorLocation();

	//生成魔方
	for (int x = 1; x <= Order; x++)
	{
		for (int y = 1; y <= Order; y++)
		{
			for (int z = 1; z <= Order; z++)
			{
				//生成魔方
				FVector Position(BasicPosition.X + x * SpaceSize, BasicPosition.Y + y * SpaceSize, BasicPosition.Z + z * SpaceSize);
				AMyCube* Cube=GetWorld()->SpawnActor<AMyCube>(MyCube, Position,FRotator(0.0f,0.0f,0.0f));

				Cube->Parent = this;
				//存储魔方数据

				//单一方块相对于魔方的坐标
				Cube->X = x;
				Cube->Y = y;
				Cube->Z = z;

				MyCubeArray.Add(Cube);
			}
		}
	}
}

void AMyMagicCube::MyCubeDestory()
{
	for (AMyCube* Cube : MyCubeArray)
	{
		if (IsValid(Cube))
		{
			Cube->Destroy();
		}
	}
	MyCubeArray.Empty();
	SpawnMyCube();
}

//更新每个面的颜色序列和63个小面各自所处于的大面
void AMyMagicCube::GetColor()
{
	//清空数组原有颜色序列
	ForwardFaceColorArray.Empty();
	BackFaceColorArray.Empty();
	LeftFaceColorArray.Empty();
	RightFaceColorArray.Empty();
	DownFaceColorArray.Empty();
	UpFaceColorArray.Empty();

	//获得该类对象的坐标相当于(0,0,0)
	FVector BasicPosition = GetActorLocation();

	//获得魔方各个面左下角那个方块的坐标(下面和左面是同一个)
	FVector ForWardCubePosition = FVector(BasicPosition.X + SpaceSize, BasicPosition.Y + SpaceSize, BasicPosition.Z + SpaceSize);
	FVector BackCubePosition = FVector(BasicPosition.X + Order * SpaceSize, BasicPosition.Y + Order * SpaceSize, BasicPosition.Z + SpaceSize);
	FVector LeftCubePosition = FVector(BasicPosition.X + Order * SpaceSize, BasicPosition.Y + SpaceSize, BasicPosition.Z + SpaceSize);
	FVector RightCubePosition = FVector(BasicPosition.X + SpaceSize, BasicPosition.Y + Order * SpaceSize, BasicPosition.Z + SpaceSize);
	FVector DownCubePosition = FVector(BasicPosition.X + Order * SpaceSize, BasicPosition.Y + SpaceSize, BasicPosition.Z + SpaceSize);
	FVector UpCubePosition = FVector(BasicPosition.X + SpaceSize, BasicPosition.Y + SpaceSize, BasicPosition.Z + Order * SpaceSize);

	//获得魔方各个面左下角那个方块的垂线上的一个点
	FVector ForwardPointPosition = FVector(ForWardCubePosition.X - 1 * SpaceSize, ForWardCubePosition.Y, ForWardCubePosition.Z);
	FVector BackPointPosition = FVector(BackCubePosition.X + 1* SpaceSize, BackCubePosition.Y, BackCubePosition.Z);
	FVector LeftPointPosition = FVector(LeftCubePosition.X, LeftCubePosition.Y - 1 * SpaceSize, LeftCubePosition.Z);
	FVector RightPointPosition = FVector(RightCubePosition.X, RightCubePosition.Y + 1 * SpaceSize, RightCubePosition.Z);
	FVector DownPointPosition = FVector(DownCubePosition.X, DownCubePosition.Y, DownCubePosition.Z - 1 * SpaceSize);
	FVector UpPointPosition = FVector(UpCubePosition.X, UpCubePosition.Y, UpCubePosition.Z + 1 * SpaceSize);

#pragma region Forward
	//Forward
	for (int z = 0; z <= Order - 1; z++)
	{
		for (int y = 0; y <= Order - 1; y++)
		{
			//射线起点和终点
			FVector Start = FVector(ForwardPointPosition.X, ForwardPointPosition.Y + y * SpaceSize, ForwardPointPosition.Z + z * SpaceSize);
			FVector End = FVector(Start.X + 1 * SpaceSize, Start.Y, Start.Z);

			FHitResult HitResult;
			bool BHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Start,
				End,
				ECC_Visibility
			);
			if (BHit)
			{
				//获得碰撞到的小面的自定义值
				UPrimitiveComponent* HitComponent = HitResult.GetComponent();
				if (HitComponent)
				{
					TArray<FName>TagsArray = HitComponent->ComponentTags;
					for (FName tag : TagsArray)
					{
						//判断该自定义值是否是颜色有关的,不是就删除
						if (CheckTagIsColor(tag.ToString()))
						{
							ForwardFaceColorArray.Add(tag.ToString());
						}
						else
						{
							HitComponent->ComponentTags.Remove(tag);
						}
					}
					//再添加小面位于得的大面
					HitComponent->ComponentTags.Add("Forward");
				}
				//DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f, FColor::Red, false, 100.0f);
				//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 100.0f);
			}
		}
	}
	//打印
	FString ForwardString = "Forward:";
	for (int i = 0; i < ForwardFaceColorArray.Num(); i++)
	{
		if (i != Order * Order - 1)
		{
			ForwardString += (ForwardFaceColorArray[i] + "_");
		}
		else
		{
			ForwardString += ForwardFaceColorArray[i];
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s"), *ForwardString));
#pragma endregion

#pragma region Back
	//Back
	for (int z = 0; z <= Order - 1; z++)
	{
		for (int y = 0; y <= Order - 1; y++)
		{
			FVector Start = FVector(BackPointPosition.X, BackPointPosition.Y - y * SpaceSize, BackPointPosition.Z + z * SpaceSize);//!
			FVector End = FVector(Start.X - 1 * SpaceSize, Start.Y, Start.Z);//!

			FHitResult HitResult;
			bool BHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Start,
				End,
				ECC_Visibility
			);
			if (BHit)
			{
				UPrimitiveComponent* HitComponent = HitResult.GetComponent();
				if (HitComponent)
				{
					TArray<FName>TagsArray = HitComponent->ComponentTags;
					for (FName tag : TagsArray)
					{
						//判断该自定义值是否是颜色有关的,不是就删除
						if (CheckTagIsColor(tag.ToString()))
						{
							BackFaceColorArray.Add(tag.ToString());//!
						} 
						else
						{
							HitComponent->ComponentTags.Remove(tag);
						}
					}
					//再添加小面位于得的大面
					HitComponent->ComponentTags.Add("Back"); //!
				}
				//DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f, FColor::Red, false, 100.0f);
				//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 100.0f);
			}
		}
	}
	//打印
	FString BackString = "Back:";//!
	for (int i = 0; i < BackFaceColorArray.Num(); i++)
	{
		if (i != Order * Order - 1)
		{
			BackString += (BackFaceColorArray[i] + "_");//!
		}
		else
		{
			BackString += BackFaceColorArray[i];//!
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s"), *BackString));//!
#pragma endregion

#pragma region Left
	//Left
	for (int z = 0; z <= Order - 1; z++)
	{
		for (int x = 0; x <= Order - 1; x++)
		{
			FVector Start = FVector(LeftPointPosition.X-x*SpaceSize, LeftPointPosition.Y , LeftPointPosition.Z + z * SpaceSize);//!
			FVector End = FVector(Start.X , Start.Y+1*SpaceSize, Start.Z);//!

			FHitResult HitResult;
			bool BHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Start,
				End,
				ECC_Visibility
			);
			if (BHit)
			{
				UPrimitiveComponent* HitComponent = HitResult.GetComponent();
				if (HitComponent)
				{
					TArray<FName>TagsArray = HitComponent->ComponentTags;
					for (FName tag : TagsArray)
					{
						//判断该自定义值是否是颜色有关的,不是就删除
						if (CheckTagIsColor(tag.ToString()))

						{
							LeftFaceColorArray.Add(tag.ToString());//!
						}
						else
						{
							HitComponent->ComponentTags.Remove(tag);
						}
					}
					//再添加小面位于得的大面
					HitComponent->ComponentTags.Add("Left"); //!
				}
				//DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f, FColor::Red, false, 100.0f);
				//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 100.0f);
			}
		}
	}
	//打印
	FString LeftString = "Left:";//!
	for (int i = 0; i < LeftFaceColorArray.Num(); i++)//!
	{
		if (i != Order * Order - 1)
		{
			LeftString += (LeftFaceColorArray[i] + "_");//!
		}
		else
		{
			LeftString += LeftFaceColorArray[i];//!
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s"), *LeftString));//!
#pragma endregion

#pragma region Right
	//Right
	for (int z = 0; z <= Order - 1; z++)
	{
		for (int x = 0; x <= Order - 1; x++)
		{
			FVector Start = FVector(RightPointPosition.X + x * SpaceSize, RightPointPosition.Y, RightPointPosition.Z + z * SpaceSize);//!
			FVector End = FVector(Start.X, Start.Y - 1 * SpaceSize, Start.Z);//!

			FHitResult HitResult;
			bool BHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Start,
				End,
				ECC_Visibility
			);
			if (BHit)
			{
				UPrimitiveComponent* HitComponent = HitResult.GetComponent();
				if (HitComponent)
				{
					TArray<FName>TagsArray = HitComponent->ComponentTags;
					for (FName tag : TagsArray)
					{
						//判断该自定义值是否是颜色有关的,不是就删除
						if (CheckTagIsColor(tag.ToString()))
						{
							RightFaceColorArray.Add(tag.ToString());//!
						}
						else
						{
							HitComponent->ComponentTags.Remove(tag);
						}
					}
					//再添加小面位于得的大面
					HitComponent->ComponentTags.Add("Right"); //!
				}
				//DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f, FColor::Red, false, 100.0f);
				//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 100.0f);
			}
		}
	}
	//打印
	FString RightString = "Right:";//!
	for (int i = 0; i < RightFaceColorArray.Num(); i++)//!
	{
		if (i != Order * Order - 1)
		{
			RightString += (RightFaceColorArray[i] + "_");//!
		}
		else
		{
			RightString += RightFaceColorArray[i];//!
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s"), *RightString));//!
#pragma endregion

#pragma region Down
	//Down
	for (int x = 0; x <= Order - 1; x++)
	{
		for (int y = 0; y <= Order - 1; y++)
		{
			FVector Start = FVector(DownPointPosition.X-x*SpaceSize , DownPointPosition.Y+y*SpaceSize, DownPointPosition.Z);//!
			FVector End = FVector(Start.X, Start.Y, Start.Z+1*SpaceSize);//!

			FHitResult HitResult;
			bool BHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Start,
				End,
				ECC_Visibility
			);
			if (BHit)
			{
				UPrimitiveComponent* HitComponent = HitResult.GetComponent();
				if (HitComponent)
				{
					TArray<FName>TagsArray = HitComponent->ComponentTags;
					for (FName tag : TagsArray)
					{
						//判断该自定义值是否是颜色有关的,不是就删除
						if (CheckTagIsColor(tag.ToString()))
						{
							DownFaceColorArray.Add(tag.ToString());//!
						} 
						else
						{
							HitComponent->ComponentTags.Remove(tag);
						}
					}
					//再添加小面位于得的大面
					HitComponent->ComponentTags.Add("Down"); //!
				}
				//DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f, FColor::Red, false, 100.0f);
				//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 100.0f);
			}
		}
	}
	//打印
	FString DownString = "Down:";//!
	for (int i = 0; i < DownFaceColorArray.Num(); i++)//!
	{
		if (i != Order * Order - 1)
		{
			DownString += (DownFaceColorArray[i] + "_");//!
		}
		else
		{
			DownString += DownFaceColorArray[i];//!
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s"), *DownString));//!
#pragma endregion

#pragma region Up
	//Up
	for (int x = 0; x <= Order - 1; x++)
	{
		for (int y = 0; y <= Order - 1; y++)
		{
			FVector Start = FVector(UpPointPosition.X + x * SpaceSize, UpPointPosition.Y + y * SpaceSize, UpPointPosition.Z);//!
			FVector End = FVector(Start.X, Start.Y, Start.Z + 1 - SpaceSize);//!

			FHitResult HitResult;
			bool BHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Start,
				End,
				ECC_Visibility
			);
			if (BHit)
			{
				UPrimitiveComponent* HitComponent = HitResult.GetComponent();
				if (HitComponent)
				{
					TArray<FName>TagsArray = HitComponent->ComponentTags;
					for (FName tag : TagsArray)
					{
						//判断该自定义值是否是颜色有关的,不是就删除
						if (CheckTagIsColor(tag.ToString()))
						{
							UpFaceColorArray.Add(tag.ToString());//!
						} 
						else
						{
							HitComponent->ComponentTags.Remove(tag);
						}
					}
					//再添加小面位于得的大面
					HitComponent->ComponentTags.Add("Up"); //!
				}
				//DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f, FColor::Red, false, 100.0f);
				//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 100.0f);
			}
		}
	}
	//打印
	FString UpString = "Up:";//!
	for (int i = 0; i < UpFaceColorArray.Num(); i++)//!
	{
		if (i != Order * Order - 1)
		{
			UpString += (UpFaceColorArray[i] + "_");//!
		}
		else
		{
			UpString += UpFaceColorArray[i];//!
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s"), *UpString));//!
#pragma endregion

}

bool AMyMagicCube::CheckTagIsColor(FString tag)
{
	if (tag == "Red" || tag == "Blue" || tag == "Green" || tag == "Yellow" || tag == "Purple" || tag == "White")
	{
		return true;
	}
	else
	{
		return false;
	}
}

FVector AMyMagicCube::GetPivot(TArray<AMyCube*> Cubes)
{
	FVector Pivot{};
	for (AMyCube* cube : Cubes)
	{
		Pivot += cube->GetActorLocation();
	}
	return Pivot/Cubes.Num();
}

void AMyMagicCube::UpdateComponentLocatedSurface()
{
	FVector BasicLocation = GetActorLocation();
	for (AMyCube* cube:MyCubeArray)
	{
		FVector CubeLocation = cube->GetActorLocation();
	    cube->X = (CubeLocation.X - BasicLocation.X) / SpaceSize+0.5f;
		cube->Y = (CubeLocation.Y - BasicLocation.Y) / SpaceSize+0.5f;
		cube->Z = (CubeLocation.Z - BasicLocation.Z) / SpaceSize+0.5f;

		//修正位置
		cube->SetActorLocation(FVector(cube->X * SpaceSize + BasicLocation.X, cube->Y * SpaceSize + BasicLocation.Y, cube->Z * SpaceSize + BasicLocation.Z));
	}
}

void AMyMagicCube::YawMotionDevice(FVector Pivot, AActor* TargetActor, float RotationDegree)
{
	//首先获得运动单位相相对于中心点的度数
	float CurrentAngle = 0.0f;
	FVector ToTarget = TargetActor->GetActorLocation() - Pivot;
	ToTarget.Z = 0;
	if (!ToTarget.IsNearlyZero())
	{
		CurrentAngle = FMath::RadiansToDegrees(FMath::Atan2(ToTarget.Y, ToTarget.X));
	}

	//更新旋转角度
	CurrentAngle += RotationDegree;
	CurrentAngle = FMath::Fmod(CurrentAngle, 360.0f);

	//新位置
	float RotationRadius = FVector::Distance(Pivot, TargetActor->GetActorLocation());
	float Radians = FMath::DegreesToRadians(CurrentAngle);
	FVector Offset(FMath::Cos(Radians) * RotationRadius, FMath::Sin(Radians) * RotationRadius, 0);

	//设置目标对象位置
	TargetActor->SetActorLocation(Pivot + Offset);

	//同时自身也要旋转(FRotator(Pitch,Yaw,Roll))
	FRotator NewRotation(0, RotationDegree, 0);
	TargetActor->AddActorWorldRotation(NewRotation);
}

void AMyMagicCube::RollMotionDevice(FVector Pivot, AActor* TargetActor, float RotationDegree)
{
	//首先获得运动单位相相对于中心点的度数
	float CurrentAngle = 0.0f;
	FVector ToTarget = TargetActor->GetActorLocation() - Pivot;
	ToTarget.X = 0;//!
	if (!ToTarget.IsNearlyZero())
	{
		CurrentAngle = FMath::RadiansToDegrees(FMath::Atan2(ToTarget.Z, ToTarget.Y));//!
	}

	//更新旋转角度
	CurrentAngle += RotationDegree;
	CurrentAngle = FMath::Fmod(CurrentAngle, 360.0f);

	//新位置
	float RotationRadius = FVector::Distance(Pivot, TargetActor->GetActorLocation());
	float Radians = FMath::DegreesToRadians(CurrentAngle);
	FVector Offset(0, FMath::Cos(Radians) * RotationRadius, FMath::Sin(Radians) * RotationRadius);//!

	//设置目标对象位置
	TargetActor->SetActorLocation(Pivot + Offset);

	//同时自身也要旋转(FRotator(Pitch,Yaw,Roll))
	FRotator NewRotation(0, 0, -RotationDegree);//不知道为什么必须要负，不负不行，转不起来 0·0
	TargetActor->AddActorWorldRotation(NewRotation);
}

void AMyMagicCube::PitchMotionDevice(FVector Pivot, AActor* TargetActor, float RotationDegree)
{
	//首先获得运动单位相相对于中心点的度数
	float CurrentAngle = 0.0f;
	FVector ToTarget = TargetActor->GetActorLocation() - Pivot;
	ToTarget.Y = 0;//!
	if (!ToTarget.IsNearlyZero())
	{
		CurrentAngle = FMath::RadiansToDegrees(FMath::Atan2(ToTarget.Z, ToTarget.X));//!
	}

	//更新旋转角度
	CurrentAngle += RotationDegree;
	CurrentAngle = FMath::Fmod(CurrentAngle, 360.0f);

	//新位置
	float RotationRadius = FVector::Distance(Pivot, TargetActor->GetActorLocation());
	float Radians = FMath::DegreesToRadians(CurrentAngle);
	FVector Offset(FMath::Cos(Radians) * RotationRadius,0, FMath::Sin(Radians) * RotationRadius);//!

	//设置目标对象位置
	TargetActor->SetActorLocation(Pivot + Offset);

	//同时自身也要旋转(FRotator(Pitch,Yaw,Roll))
	FRotator NewRotation(RotationDegree, 0, 0);//!
	TargetActor->AddActorWorldRotation(NewRotation);
}

//打印颜色
void AMyMagicCube::PrintFaceColor()
{
	FString ForwardString = "Forward:";
	for (int i = 0; i < ForwardFaceColorArray.Num(); i++)
	{
		if (i != Order * Order - 1)
		{
			ForwardString += (ForwardFaceColorArray[i] + "_");
		}
		else
		{
			ForwardString += ForwardFaceColorArray[i];
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s"), *ForwardString));

	FString BackString = "Back:";//!
	for (int i = 0; i < BackFaceColorArray.Num(); i++)
	{
		if (i != Order * Order - 1)
		{
			BackString += (BackFaceColorArray[i] + "_");//!
		}
		else
		{
			BackString += BackFaceColorArray[i];//!
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s"), *BackString));//!

	FString LeftString = "Left:";//!
	for (int i = 0; i < LeftFaceColorArray.Num(); i++)//!
	{
		if (i != Order * Order - 1)
		{
			LeftString += (LeftFaceColorArray[i] + "_");//!
		}
		else
		{
			LeftString += LeftFaceColorArray[i];//!
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s"), *LeftString));//!

	FString RightString = "Right:";//!
	for (int i = 0; i < RightFaceColorArray.Num(); i++)//!
	{
		if (i != Order * Order - 1)
		{
			RightString += (RightFaceColorArray[i] + "_");//!
		}
		else
		{
			RightString += RightFaceColorArray[i];//!
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s"), *RightString));//!

	FString DownString = "Down:";//!
	for (int i = 0; i < DownFaceColorArray.Num(); i++)//!
	{
		if (i != Order * Order - 1)
		{
			DownString += (DownFaceColorArray[i] + "_");//!
		}
		else
		{
			DownString += DownFaceColorArray[i];//!
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s"), *DownString));//!

	FString UpString = "Up:";//!
	for (int i = 0; i < UpFaceColorArray.Num(); i++)//!
	{
		if (i != Order * Order - 1)
		{
			UpString += (UpFaceColorArray[i] + "_");//!
		}
		else
		{
			UpString += UpFaceColorArray[i];//!
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s"), *UpString));//!
}

float AMyMagicCube::MagicCubeRotate(FString CurrentMoveDirectionOfMouse, FString DraggedComponentLocatedSurface, AMyCube* DraggedCube, float value)
{
	//需要旋转的方块
	TArray<AMyCube*>NeedRotateCubes;

	//碰撞的方块的坐标
	int32 X = DraggedCube->X;
	int32 Y = DraggedCube->Y;
	int32 Z = DraggedCube->Z;

	//判断鼠标移动的方向

	//竖直
	if (CurrentMoveDirectionOfMouse == "Vertical") 
	{
		//判断碰撞组件所属的面
		if (DraggedComponentLocatedSurface == "Forward"|| DraggedComponentLocatedSurface == "Back")
		{
			//获取要旋转的方块
			for (AMyCube* cube : MyCubeArray)
			{
				if (cube->Y == Y)
				{
					NeedRotateCubes.Add(cube);
				}
			}

			//获得中心坐标
			FVector Pivot=GetPivot(NeedRotateCubes);
			
			
			//开始旋转
			for (AMyCube* cube : NeedRotateCubes)
			{
				PitchMotionDevice(Pivot, cube, value);
			}

		}
		else if (DraggedComponentLocatedSurface == "Left"|| DraggedComponentLocatedSurface == "Right")
		{
			//获取要旋转的方块
			for (AMyCube* cube : MyCubeArray)
			{
				if (cube->X == X)
				{
					NeedRotateCubes.Add(cube);
				}
			}

			//获得中心坐标
			FVector Pivot = GetPivot(NeedRotateCubes);

			//开始旋转
			for (AMyCube* cube : NeedRotateCubes)
			{
				RollMotionDevice(Pivot, cube, value);
			}
		}
		else if (DraggedComponentLocatedSurface == "Down"|| DraggedComponentLocatedSurface == "Up")
		{

		}
	}
	//水平
	else if (CurrentMoveDirectionOfMouse == "Horizontal") 
	{
		//判断碰撞组件所属的面
		if (DraggedComponentLocatedSurface == "Forward" || DraggedComponentLocatedSurface == "Back"|| DraggedComponentLocatedSurface == "Left" || DraggedComponentLocatedSurface == "Right")
		{
			//获取要旋转的方块
			for (AMyCube* cube : MyCubeArray)
			{
				if (cube->Z == Z)
				{
					NeedRotateCubes.Add(cube);
				}
			}

			//获得中心坐标
			FVector Pivot = GetPivot(NeedRotateCubes);

			//开始旋转
			for (AMyCube* cube : NeedRotateCubes)
			{
				YawMotionDevice(Pivot, cube, value);
			}
		}
		else if (DraggedComponentLocatedSurface == "Down" || DraggedComponentLocatedSurface == "Up")
		{

		}
	}
	return 0.0f;
}
