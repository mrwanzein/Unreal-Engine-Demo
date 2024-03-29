// Fill out your copyright notice in the Description page of Project Settings.


#include "ColliderPawn.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ColliderMovementComponent.h"

// Sets default values
AColliderPawn::AColliderPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(GetRootComponent());
	SphereComponent->InitSphereRadius(40.f);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComponentAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if (MeshComponentAsset.Succeeded()) {
		
		MeshComponent->SetStaticMesh(MeshComponentAsset.Object);
		MeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -40.f));
		MeshComponent->SetWorldScale3D(FVector(0.8f, 0.8f, 0.8f));

	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->RelativeRotation = FRotator(-45.f, 0.f, 0.f);
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 3.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	OurMovementComponent = CreateDefaultSubobject< UColliderMovementComponent>(TEXT("MovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;

	CameraInput = FVector2D(0.f, 0.f);


}

// Called when the game starts or when spawned
void AColliderPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AColliderPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += CameraInput.X;
	SetActorRotation(NewRotation);

	FRotator NewSpringArmRotation = SpringArm->GetComponentRotation();
	NewSpringArmRotation.Pitch = FMath::Clamp(NewSpringArmRotation.Pitch += CameraInput.Y, -80.f, 15.f);

	SpringArm->SetWorldRotation(NewSpringArmRotation);


}

// Called to bind functionality to input
void AColliderPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AColliderPawn::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AColliderPawn::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("CameraPitch"), this, &AColliderPawn::PitchCamera);
	PlayerInputComponent->BindAxis(TEXT("CameraYaw"), this, &AColliderPawn::YawCamera);

}

void AColliderPawn::MoveForward(float Value) {
	FVector Forward = GetActorForwardVector();
	if (OurMovementComponent)
	{
		OurMovementComponent->AddInputVector(Value * Forward);
	}
}

void AColliderPawn::MoveRight(float Value) {
	FVector Right = GetActorRightVector();
	if (OurMovementComponent)
	{
		OurMovementComponent->AddInputVector(Value * Right);
	}
}

void AColliderPawn::PitchCamera(float AxisValue) {
	CameraInput.Y = AxisValue;
}

void AColliderPawn::YawCamera(float AxisValue) {
	CameraInput.X = AxisValue;
}

UPawnMovementComponent* AColliderPawn::GetMovementComponent() const {
	return OurMovementComponent;
}

