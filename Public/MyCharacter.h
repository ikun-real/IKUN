// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include <Kismet/GameplayStatics.h>
#include "MyActorSpawner.h"
#include "MySphere.h"
#include "MyMagicCube.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MyCharacter.generated.h"


UCLASS()
class ABC_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable)
	void OpenLobby();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region MyRegion
	/** 用于将摄像机放置在角色身后的摄像机升降臂 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** 跟随摄像机 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** 基础旋转速度，以度/秒为单位。其他单位可能会影响最终旋转速度。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float BaseTurnRate;

	/** 基础向上看/向下看速度，以度/秒为单位。其他单位可能会影响最终速度。*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float BaseLookUpRate;

	//输入映射
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpAction;

	//函数
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	//UFUNCTION() 这里不需要UFUCTION因为父类Acharater有了Jump这个函数并声明了UFUNCTION
	void Jump();
#pragma endregion


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* RightMousePressed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* RightMouseReleased;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* Jia;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* Jian;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputMappingContext* ActorOperate;



	//拖动
	AMyCube* DraggedActor;//碰撞对象
	AMyMagicCube* DraggedMagicCube;//碰撞对象所属的魔方

	bool bIsDragging = false;//是否碰撞
	
	FString DraggedComponentLocatedSurface; //小面处于的大面

	FString CurrentMoveDirectionOfMouse;//鼠标当前滑动的方向,Vetical和Horizontal

	float CurrentRotation;//当前已转动的角度

	float RotateSpeed;//旋转速度

	FTimerHandle IsAblePressMouse_Timer;//能否点击鼠标,用于优化鼠标松开后魔方还没旋转完又旋转其他面
	bool BIsAblePressMouse;

	UFUNCTION()
	void OnRightMousePressed();

	UFUNCTION()
	void OnRightMouseReleased();

	UFUNCTION()
	void OnJia();

	UFUNCTION()
	void OnJian();
	
	//测试旋转的
#pragma region RotationInputAction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* yyaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* rroll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* ppitch;

	UFUNCTION()
	void yaw();

	UFUNCTION()
	void roll();

	UFUNCTION()
	void pitch();

	bool IsDragged;
	FVector DraggedLocation;
	AActor* a;
#pragma endregion

	//检测两个魔方颜色序列是否相同
#pragma region Color_Check

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Check")
	TArray<AMyMagicCube*> MagicCubeCheckArray;

	//比对两个魔方6个面的颜色
	UFUNCTION()
	bool IsSame(AMyMagicCube* a, AMyMagicCube* b);

	//取颜色数组中每一个元素的头一个字母，然后组成一个新的字符串
	UFUNCTION()
	FString ColorArrayToString(TArray<FString> array);
#pragma endregion

	//检测两个魔方身份特征是否相同
	UFUNCTION()
	bool IsSame_Behavior(AMyMagicCube* a, AMyMagicCube* b);
};
