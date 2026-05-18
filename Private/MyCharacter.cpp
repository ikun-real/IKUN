// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "MyCube.h"
#include "DrawDebugHelpers.h" // 用于绘制调试信息
#include "Engine/World.h"    // 用于获取World对象
#include "Components/CapsuleComponent.h"
// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 设置碰撞胶囊体的大小
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// 为输入设置我们的旋转速度
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// 当控制器旋转时不旋转使其仅影响摄像机。
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// 创建摄像机升降臂（发生碰撞则朝着玩家推进）
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; 
	CameraBoom->bUsePawnControlRotation = true; // 基于控制器旋转升降臂

	// 创建一个跟随摄像机
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // 将摄像机连接到升降臂末端，对升降臂进行调整以便与控制器方向保持一致
	FollowCamera->bUsePawnControlRotation = false; // 摄像机不相对于升降臂旋转

}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	bIsDragging = false;
	DraggedMagicCube = nullptr;
	DraggedActor = nullptr;
	DraggedComponentLocatedSurface = "";
	CurrentMoveDirectionOfMouse = "";
	CurrentRotation = 0;

	BIsAblePressMouse = true;
	RotateSpeed = 3.0f;
}

void AMyCharacter::OpenLobby()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("666"));
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 绑定输入动作到函数
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMyCharacter::Jump);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);

		EnhancedInputComponent->BindAction(yyaw, ETriggerEvent::Triggered, this, &AMyCharacter::yaw);
		EnhancedInputComponent->BindAction(rroll, ETriggerEvent::Triggered, this, &AMyCharacter::roll);
		EnhancedInputComponent->BindAction(ppitch, ETriggerEvent::Triggered, this, &AMyCharacter::pitch);
		EnhancedInputComponent->BindAction(RightMousePressed, ETriggerEvent::Triggered, this, &AMyCharacter::OnRightMousePressed);
		EnhancedInputComponent->BindAction(RightMouseReleased, ETriggerEvent::Triggered, this, &AMyCharacter::OnRightMouseReleased);
		EnhancedInputComponent->BindAction(Jia, ETriggerEvent::Triggered, this, &AMyCharacter::OnJia);
		EnhancedInputComponent->BindAction(Jian, ETriggerEvent::Triggered, this, &AMyCharacter::OnJian);
	}

	// 添加输入映射上下文
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			Subsystem->AddMappingContext(ActorOperate, 1);
		}
	}
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 获取控制器的旋转
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 获取前进和右方向
		FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 移动角色
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();

	if (Controller != nullptr&& bIsDragging == false)
	{
		// 添加控制器旋转
		AddControllerPitchInput(LookVector.Y);
		AddControllerYawInput(LookVector.X);
	}

	//单一方块旋转
	/*
	if (bIsDragging)
	{
		FVector Start = GetActorLocation();
		FVector End = DraggedActor->GetActorLocation();
		FVector Direction = (End - Start).GetSafeNormal();
		FVector YAxis= FVector::CrossProduct(Direction, FVector(0,0,1)).GetSafeNormal();
		DraggedActor->AddActorWorldRotation(FRotator(5*YAxis.Y*LookVector.Y,5*LookVector.X,5*YAxis.X*LookVector.Y));
	}
	*/
	
	//射线碰撞到了方块
	if (bIsDragging&&DraggedActor->Parent!=nullptr&&BIsAblePressMouse)
	{
		AMyCube* DraggedCube = Cast<AMyCube>(DraggedActor);

		//如果已经开始滑动了，不能切换方向
		if (CurrentRotation != 0)
		{
				if (CurrentMoveDirectionOfMouse == "Horizontal")
				{			
					CurrentRotation += RotateSpeed * LookVector.X;
					DraggedMagicCube->MagicCubeRotate(CurrentMoveDirectionOfMouse, DraggedComponentLocatedSurface, DraggedCube, RotateSpeed * LookVector.X);
				}
				else if (CurrentMoveDirectionOfMouse == "Vertical")
				{
					CurrentRotation += RotateSpeed * LookVector.Y;
					DraggedMagicCube->MagicCubeRotate(CurrentMoveDirectionOfMouse, DraggedComponentLocatedSurface, DraggedCube, RotateSpeed * LookVector.Y);
				}		
		}
		else if(CurrentMoveDirectionOfMouse==""&& (FMath::Abs(LookVector.X)>0.2|| FMath::Abs(LookVector.Y)>0.2)) //鼠标滑动距离太小不管
		{
			//鼠标移动时水平方向的值大于竖直方向的值，说明魔方应该水平转动
			if (LookVector.X > LookVector.Y)
			{
				CurrentRotation += RotateSpeed * LookVector.X;
				CurrentMoveDirectionOfMouse = "Horizontal";
				DraggedMagicCube->MagicCubeRotate(CurrentMoveDirectionOfMouse, DraggedComponentLocatedSurface,DraggedCube, RotateSpeed*LookVector.X);
			}
			else
			{
				CurrentRotation += RotateSpeed * LookVector.Y;
				CurrentMoveDirectionOfMouse = "Vertical";
				DraggedMagicCube->MagicCubeRotate(CurrentMoveDirectionOfMouse, DraggedComponentLocatedSurface,DraggedCube, RotateSpeed*LookVector.Y);
			}
		}
	}
	
}

void AMyCharacter::Jump()
{
	ACharacter::Jump();
}

void AMyCharacter::OnRightMousePressed()
{
	// 获取World对象
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 射线的起点和终点
	FVector Start = GetActorLocation();
	FVector End = Start + (FollowCamera->GetForwardVector()*1000.0f);

	// 碰撞查询参数
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // 忽略自身

	//执行射线检测
	FHitResult HitResult;
	bool bHit = World->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		CollisionParams
	);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			//碰撞到的actor是AMyCube类
			if (AMyCube* CollidedCube=Cast<AMyCube>(HitActor))
			{
				//记录点击到的方块
				bIsDragging = true;
				DraggedActor = CollidedCube; 

				//该方块对应的魔方
				DraggedMagicCube = Cast<AMyMagicCube>(CollidedCube->Parent);

				//将该魔方加入DraggedMagicCube数组
				if (!MagicCubeCheckArray.Contains(DraggedMagicCube)&& MagicCubeCheckArray.Num()<=2)
				{
					MagicCubeCheckArray.Add(DraggedMagicCube);
				}

				DraggedMagicCube->PressTime = FPlatformTime::Seconds();//按下时间
				DraggedMagicCube->PressTimes.Add(DraggedMagicCube->PressTime); //按下时间数组
				//GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Red, FString::Printf(TEXT("PressTime:%f"), DraggedMagicCube->PressTime)); //打印按下时间

				//打印方块的名称
				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%d_%d_%d"),CollidedCube->X, CollidedCube->Y, CollidedCube->Z));
			}		
			// 在击中位置绘制一个红色的点
			DrawDebugPoint(World, HitResult.ImpactPoint, 10.0f, FColor::Red, false, 2.0f);
		}

		UPrimitiveComponent* HitComponent = HitResult.GetComponent();
		if (HitComponent)
		{
			TArray<FName>TagsArray = HitComponent->ComponentTags;
			for (FName tag : TagsArray)
			{
				//判断该自定义值是否是颜色有关的
				if(!DraggedMagicCube->CheckTagIsColor(tag.ToString()))
				{
					DraggedComponentLocatedSurface = tag.ToString();
				}
			}
		}
	}

	// 绘制射线
	DrawDebugLine(World, Start, End, FColor::Green, false, 2.0f, 0, 1.0f);
}

void AMyCharacter::OnRightMouseReleased()
{

	//碰撞到了方块
	if (bIsDragging&&DraggedActor->Parent!=nullptr)
	{
		BIsAblePressMouse = false;

		//行为特征收集

		DraggedMagicCube->ReleaseTime= FPlatformTime::Seconds(); //松开时间
		DraggedMagicCube->ReleaseTimes.Add(DraggedMagicCube->ReleaseTime); //松开时间数组
		//GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Red, FString::Printf(TEXT("ReleaseTime:%f"), DraggedMagicCube->ReleaseTime)); //打印松开时间

		//行为特征整理
		double duration = DraggedMagicCube->ReleaseTime - DraggedMagicCube->PressTime; //转动时间
		double ratatespeed = FMath::Abs(CurrentRotation) / duration; //转动速度
		FString face = DraggedComponentLocatedSurface; //小方块所处面 Forward/Back/Left/Right
		FString direction = CurrentRotation <= 0 ? "Negative" : "Positive"; //顺序正负
		DraggedMagicCube->RotationsSpeed.Add(ratatespeed); //转动速度数组
		DraggedMagicCube->Preferences.Add(face+"_" + direction); //偏好数组

		//GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Red,face+"_" + direction);
		
		//打印时间间隔数组
		/*
		for (int i = 0; i < DraggedMagicCube->HoldDurations.Num(); i++) 
		{
			double CurrentValue = DraggedMagicCube->HoldDurations[i];
			UE_LOG(LogTemp, Log, TEXT("Duration %d:%.3f"), i, CurrentValue);
			if (i == DraggedMagicCube->HoldDurations.Num() - 1)
			{
				UE_LOG(LogTemp, Log, TEXT("\n"));
			}
		}
		*/

		//打印转动速度数组
		/*
		for (int i = 0; i < DraggedMagicCube->RotationsSpeed.Num(); i++)
		{
			double CurrentValue = DraggedMagicCube->RotationsSpeed[i];
			UE_LOG(LogTemp, Log, TEXT("RotationsSpeed %d:%.3f"), i, CurrentValue);
			if (i == DraggedMagicCube->RotationsSpeed.Num() - 1)
			{
				UE_LOG(LogTemp, Log, TEXT("\n"));
			}
		}
		*/

		//打印偏好数组
		/*	
		for (int i = 0; i < DraggedMagicCube->Preferences.Num(); i++)
		{
			FString CurrentValue = DraggedMagicCube->Preferences[i];
			UE_LOG(LogTemp, Log, TEXT("Preferences %d:%s"), i, *CurrentValue);
			if (i == DraggedMagicCube->Preferences.Num() - 1)
			{
				UE_LOG(LogTemp, Log, TEXT("\n"));
			}
		}
		*/

		FTimerDelegate Lambda;
		Lambda.BindLambda([this] {BIsAblePressMouse = true; });
		GetWorldTimerManager().SetTimer(
			IsAblePressMouse_Timer,
			Lambda,
			0.25f,
			false
		);

		//优化转动(即魔方转动必须是90°的倍数)
		
		//继续转动的度数
		float Degree = FMath::Fmod(FMath::Abs(CurrentRotation), 90.0f);
		Degree = (Degree > 45.0f) ? 90.0f - Degree : -Degree;
		Degree = CurrentRotation <= 0 ? -Degree : Degree;
		AMyCube* DraggedCube = Cast<AMyCube>(DraggedActor);
		DraggedMagicCube->MagicCubeRotate(CurrentMoveDirectionOfMouse, DraggedComponentLocatedSurface, DraggedCube, Degree);

		//更新魔方数据
		AMyMagicCube* MagicCube = Cast<AMyMagicCube>(DraggedCube->Parent);
		MagicCube->GetColor();
		MagicCube->UpdateComponentLocatedSurface();

	}
	//初始化变量
	CurrentRotation = 0;
	bIsDragging = false;
	DraggedActor = nullptr;
	DraggedMagicCube = nullptr;
	DraggedComponentLocatedSurface = "";
	CurrentMoveDirectionOfMouse = "";

	//检查 
	if (MagicCubeCheckArray.Num() == 2)
	{
		if (IsSame(MagicCubeCheckArray[0], MagicCubeCheckArray[1]))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Red, TEXT("Color Is Same"));
			if (IsSame_Behavior(MagicCubeCheckArray[0], MagicCubeCheckArray[1]))
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Behavior Is Similar,Pass"));
			//else GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Behavior Is Not Similar,End"));
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Red, TEXT("Color Is Different\n"));
		}
	}
}

void AMyCharacter::OnJia()
{
	
	if (MagicCubeCheckArray[0] != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("order+1")));
		MagicCubeCheckArray[0]->Order+=1;
		MagicCubeCheckArray[0]->MyCubeDestory();
	}
	if (MagicCubeCheckArray[1] != nullptr)
	{
		MagicCubeCheckArray[1]->Order += 1;
		MagicCubeCheckArray[1]->MyCubeDestory();
	}
}

void AMyCharacter::OnJian()
{
	if (MagicCubeCheckArray[0] != nullptr&&MagicCubeCheckArray[0]->Order > 1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("order-1")));
		MagicCubeCheckArray[0]->Order -= 1;
		MagicCubeCheckArray[0]->MyCubeDestory();
	}
	if (MagicCubeCheckArray[1] != nullptr && MagicCubeCheckArray[1]->Order > 1)
	{
		MagicCubeCheckArray[1]->Order -= 1;
		MagicCubeCheckArray[1]->MyCubeDestory();
	}
}

#pragma region RotationInputAction
void AMyCharacter::yaw()
{
	if (false) //测试用的
	{
		if (IsDragged)
		{
			AMyMagicCube::YawMotionDevice(DraggedLocation, a, 10);
			//FRotator NewRotation(0, 15, 0);
			//a->AddActorWorldRotation(NewRotation);
		}
		else if (bIsDragging && !IsDragged)
		{
			IsDragged = true;
			a = DraggedActor;
			DraggedLocation = DraggedActor->GetActorLocation() + FVector(0, 0, 0);
		}
	}
}

void AMyCharacter::roll()
{
	if (false)
	{
		if (IsDragged)
		{
			AMyMagicCube::RollMotionDevice(DraggedLocation, a, 10);
			//FRotator NewRotation(0, 0, 15);
			//a->AddActorWorldRotation(NewRotation);
		}
		else if (bIsDragging && !IsDragged)
		{
			IsDragged = true;
			a = DraggedActor;
			DraggedLocation = DraggedActor->GetActorLocation() + FVector(0, 0, 0);
		}
	}
}

void AMyCharacter::pitch()
{
	if (false)
	{
		if (IsDragged)
		{
			AMyMagicCube::PitchMotionDevice(DraggedLocation, a, 10);
			//FRotator NewRotation(15, 0, 0);
			//a->AddActorWorldRotation(NewRotation);
		}
		else if (bIsDragging && !IsDragged)
		{
			IsDragged = true;
			a = DraggedActor;
			DraggedLocation = DraggedActor->GetActorLocation() + FVector(0, 0, 0);
		}
	}
}
#pragma endregion

#pragma region Check

//颜色序列比对算法
bool AMyCharacter::IsSame(AMyMagicCube* aa, AMyMagicCube* bb)
{
	//首先将两个魔方的6个面的颜色序列转换成int32类型的数组
	TArray<FString> aArray,bArray;

	//aa->PrintFaceColor();
	//bb->PrintFaceColor();
	//转换
	if (true)
	{
		aArray.Add(ColorArrayToString(aa->ForwardFaceColorArray));
		aArray.Add(ColorArrayToString(aa->BackFaceColorArray));
		aArray.Add(ColorArrayToString(aa->LeftFaceColorArray));
		aArray.Add(ColorArrayToString(aa->RightFaceColorArray));
		aArray.Add(ColorArrayToString(aa->DownFaceColorArray));
		aArray.Add(ColorArrayToString(aa->UpFaceColorArray));

		bArray.Add(ColorArrayToString(bb->ForwardFaceColorArray));
		bArray.Add(ColorArrayToString(bb->BackFaceColorArray));
		bArray.Add(ColorArrayToString(bb->LeftFaceColorArray));
		bArray.Add(ColorArrayToString(bb->RightFaceColorArray));
		bArray.Add(ColorArrayToString(bb->DownFaceColorArray));
		bArray.Add(ColorArrayToString(bb->UpFaceColorArray));
	}

	//用来保存相同的字符串的序号
	TArray<int32> Ikun;

	for (int i = 0; i < 6; i++)
	{
		for (int k = 0; k < 6; k++)
		{
			//相同且没有匹配相同的面
			if (aArray[i] == bArray[k]&&!Ikun.Contains(k))
			{
				Ikun.Add(k);
				k = 6;
			}
			//没有一样的
			else if(aArray[i] != bArray[k]&&k==5)
			{
				return false;
			}
		}
	}
	return true;
}

FString AMyCharacter::ColorArrayToString(TArray<FString> array)
{
	FString value = "";
	for (FString string : array)
	{
		if (string != "")
		{
			value += string[0];
		}
	}
	return value;
}

//行为特征比对算法
bool AMyCharacter::IsSame_Behavior(AMyMagicCube* aa, AMyMagicCube* bb)
{
	double aIntervalAvg=0, bIntervalAvg=0; //时间间隔平均值
	double aRotateSpeedAvg=0, bRotateSpeedAvg=0; //转动速度平均值
	double value1=0, value2=0, value3=0, value4=0; //匹配度，依次为时间间隔、转动速度、选择面偏好、顺序偏好
	//aIntervalAvg
	if (aa->ReleaseTimes.Num() > 1)
	{
		for (int i = 0; i < aa->ReleaseTimes.Num() - 1; i++)
		{
			aIntervalAvg += (aa->PressTimes[i + 1] - aa->ReleaseTimes[i]);
			if (i == aa->ReleaseTimes.Num() - 2)
			{
				aIntervalAvg /= (aa->ReleaseTimes.Num() - 1);
				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("aIntervalAvg:%f"), aIntervalAvg));
			}
		}
	}
	//bIntervalAvg
	if (bb->ReleaseTimes.Num() > 1)
	{
		for (int i = 0; i < bb->ReleaseTimes.Num() - 1; i++)
		{
			bIntervalAvg += (bb->PressTimes[i + 1] - bb->ReleaseTimes[i]);
			if (i == bb->ReleaseTimes.Num() - 2)
			{
				bIntervalAvg /= (bb->ReleaseTimes.Num() - 1);
				//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("bIntervalAvg:%f"), bIntervalAvg));
			}
		}
	}

	//aRotateSpeedAvg
	for (int i = 0; i < aa->RotationsSpeed.Num(); i++)
	{
		aRotateSpeedAvg += aa->RotationsSpeed[i];
		if (i == aa->RotationsSpeed.Num() - 1)
		{
			aRotateSpeedAvg /= aa->RotationsSpeed.Num();
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("aRotateSpeedAvg:%f"), aRotateSpeedAvg));
		}
	}

	//bRotateSpeedAvg
	for (int i = 0; i < bb->RotationsSpeed.Num(); i++)
	{
		bRotateSpeedAvg += bb->RotationsSpeed[i];
		if (i == bb->RotationsSpeed.Num() - 1)
		{
			bRotateSpeedAvg /= bb->RotationsSpeed.Num();
			//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("bRotateSpeedAvg:%f"), bRotateSpeedAvg));
		}
	}

	//value1
	if (0.8 * aIntervalAvg <= bIntervalAvg && bIntervalAvg <= 1.2 * aIntervalAvg)
	{
		value1 = 1;
	}
	else if (bIntervalAvg < 0.8 * aIntervalAvg)
	{
		value1 = 1 - (0.8 * aIntervalAvg - bIntervalAvg) / aIntervalAvg;
	}
	else
	{
		value1 = 1 - (bIntervalAvg - 1.2 * aIntervalAvg) / aIntervalAvg;
	}

	//value2
	if (0.8 * aRotateSpeedAvg <= bRotateSpeedAvg && bRotateSpeedAvg <= 1.2 * aRotateSpeedAvg)
	{
		value2 = 1;
	}
	else if (bRotateSpeedAvg < 0.8 * aRotateSpeedAvg)
	{
		value2 = 1 - (0.8 * aRotateSpeedAvg - bRotateSpeedAvg) / aRotateSpeedAvg;
	}
	else
	{
		value2 = 1 - (bRotateSpeedAvg - 1.2 * aRotateSpeedAvg) / aRotateSpeedAvg;
	}

	//value3,value4
	const TArray<FString>& A = aa->Preferences;
	const TArray<FString>& B = bb->Preferences;
	int n = A.Num();
	int m = B.Num();
	TArray<TArray<int32>> dp; // 动态规划表
	dp.SetNum(n);
	for (int32 i = 0; i < n; i++)
		dp[i].Init(0, m);

	int32 maxLen = 0;
	int32 endIndexA = -1;

	for (int32 i = 0; i < n; i++)
	{
		for (int32 j = 0; j < m; j++)
		{
			if (A[i] == B[j])
			{
				if (i == 0 || j == 0)
					dp[i][j] = 1;
				else
					dp[i][j] = dp[i - 1][j - 1] + 1;

				// 更新最长长度
				if (dp[i][j] > maxLen)
				{
					maxLen = dp[i][j];
					endIndexA = i;
				}
			}
			else
			{
				dp[i][j] = 0;
			}
		}
	}

	// 提取结果
	TArray<FString> result;
	if (maxLen > 0)
	{
		int32 start = endIndexA - maxLen + 1;
		for (int32 i = start; i <= endIndexA; i++)
			result.Add(A[i]);
	}

	value3 = result.Num() / static_cast<double>(m);
	value4 = value3;

	//在日志打印最大公共子数组
	
	FString msg = "BiggestSunArray: ";
	for (const FString& s : result)
		msg += s + " ";
	UE_LOG(LogTemp, Log, TEXT("%s"), *msg);
	

	double value = 0.3 * (value1 + value2) + 0.2 * (value3 + value4);

	//在日志打印各个匹配度
	UE_LOG(LogTemp, Log, TEXT("value1:%f  value2:%f  value3:%f  value4:%f value%f"), value1, value2, value3, value4,value);

	//在日志打印花费时间
	double CostTime1 = aa->ReleaseTimes[aa->ReleaseTimes.Num() - 1] - aa->PressTimes[0];
	double CostTime2 = bb->ReleaseTimes[bb->ReleaseTimes.Num() - 1] - bb->PressTimes[0];
	UE_LOG(LogTemp, Log, TEXT("CostTime1:%f CostTime2:%f"), CostTime1, CostTime2);

	//在日志打印偏好
	FString References1 = "References1: ", References2 = "References2: ";
	for (const FString& s : aa->Preferences)References1 += s + " ";
	for (const FString& s : bb->Preferences)References2 += s + " ";
	UE_LOG(LogTemp, Log, TEXT("%s"), *References1);
	UE_LOG(LogTemp, Log, TEXT("%s"), *References2);
	return value<0.8?false:true;
}

#pragma endregion
