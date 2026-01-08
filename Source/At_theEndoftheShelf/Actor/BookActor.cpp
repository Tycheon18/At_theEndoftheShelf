// Fill out your copyright notice in the Description page of Project Settings.


#include "BookActor.h"
#include "Components/StaticMeshComponent.h"
#include "../LibraryStruct.h"

// Sets default values
ABookActor::ABookActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootSceneComponent;

	BookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BookMesh"));
	BookMesh->SetupAttachment(RootComponent);

	// Enable mouse interaction
	BookMesh->SetNotifyRigidBodyCollision(true);
	BookMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BookMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BookMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	// Enable click events
	EnableInput(nullptr);
	SetActorEnableCollision(true);
}

// Called when the game starts or when spawned
void ABookActor::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateBookAppearance();
}

// Called every frame
void ABookActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABookActor::SetBookData(const FBookInfo& InBookData)
{
	BookData = InBookData;
	UpdateBookAppearance();

	UE_LOG(LogTemp, Log, TEXT("BookActor: Set book data for '%s'"), *BookData.Title);
}

void ABookActor::SetBookMaterial(UMaterialInterface* Material)
{
	if (BookMesh && Material)
	{
		BookMesh->SetMaterial(0, Material);
	}
}

void ABookActor::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

	if (ButtonPressed == EKeys::LeftMouseButton)
	{
		UE_LOG(LogTemp, Log, TEXT("BookActor clicked: %s"), *BookData.Title);

	}
}

void ABookActor::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();

	UE_LOG(LogTemp, Log, TEXT("BookActor hover begin: %s"), *BookData.Title);
}

void ABookActor::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();

	UE_LOG(LogTemp, Log, TEXT("BookActor hover end: %s"), *BookData.Title);
}

void ABookActor::UpdateBookAppearance()
{
	if (!BookMesh) return;

	// Load basic cube mesh
	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr,
		TEXT("/Engine/BasicShapes/Cube.Cube"));

	if (CubeMesh)
	{
		BookMesh->SetStaticMesh(CubeMesh);

		// 책 비율로 스케일 조정 (두께 0.15, 너비 1.0, 높이 1.3)
		BookMesh->SetRelativeScale3D(FVector(0.15f, 1.0f, 1.3f));

		// 기본 색상 머티리얼 생성 (장르별로 나중에 구분 가능)
		UMaterialInterface* DefaultMaterial = LoadObject<UMaterialInterface>(nullptr,
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));

		if (DefaultMaterial)
		{
			BookMesh->SetMaterial(0, DefaultMaterial);
		}

		UE_LOG(LogTemp, Log, TEXT("BookActor: Updated appearance for '%s'"), *BookData.Title);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BookActor: Failed to load cube mesh"));
	}
}

FLinearColor ABookActor::GenGenreColor() const
{
	// TODO: 나중에 장르 분류 구현 시 사용
	// 기본적인 색상 구분 예시
	if (BookData.Title.Contains(TEXT("소설")) || BookData.Title.Contains(TEXT("문학")))
	{
		return FLinearColor::Blue; // 파란색 - 문학
	}
	else if (BookData.Title.Contains(TEXT("경제")) || BookData.Title.Contains(TEXT("경영")))
	{
		return FLinearColor::Green; // 초록색 - 경제/경영
	}
	else if (BookData.Title.Contains(TEXT("과학")) || BookData.Title.Contains(TEXT("기술")))
	{
		return FLinearColor::Red; // 빨간색 - 과학/기술
	}

	return FLinearColor::Gray; // 회색 - 기본
}

