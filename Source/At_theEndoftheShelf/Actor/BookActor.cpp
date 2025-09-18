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
	if (BookMesh)
	{
		// Set default book mesh if not set
		if (!BookMesh->GetStaticMesh())
		{
			UE_LOG(LogTemp, Warning, TEXT("BookActor: No book mesh set for '%s'"), *BookData.Title);
		}
	}
}

