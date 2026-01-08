// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../LibraryStruct.h"
#include "BookActor.generated.h"

UCLASS()
class AT_THEENDOFTHESHELF_API ABookActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABookActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BookMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Book Data")
	FBookInfo BookData;

public:

	UFUNCTION(BlueprintCallable, Category = "Book")
	void SetBookData(const FBookInfo& InBookData);

	UFUNCTION(BlueprintCallable, Category = "Book")
	void SetBookMaterial(UMaterialInterface* Material);

	UFUNCTION(BlueprintCallable, Category = "Book")
	FBookInfo GetBookData() const { return BookData; }

protected:

	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;
	virtual void NotifyActorBeginCursorOver() override;
	virtual void NotifyActorEndCursorOver() override;

	UFUNCTION(BlueprintCallable, Category = "Book")
	void UpdateBookAppearance();

protected:

	FLinearColor GenGenreColor() const;
};
