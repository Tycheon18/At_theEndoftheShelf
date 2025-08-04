// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LibraryStruct.generated.h"

USTRUCT(BlueprintType)
struct FBookReview
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Author;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Content;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime CreatedAt;
};

USTRUCT(BlueprintType)
struct FBookInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Book")
	FString Title;

	UPROPERTY(BlueprintReadWrite, Category = "Book")
	TArray<FString> Authors;

	UPROPERTY(BlueprintReadWrite, Category = "Book")
	FString Publisher;

	UPROPERTY(BlueprintReadWrite, Category = "Book")
	FString Thumbnail;

	UPROPERTY(BlueprintReadWrite, Category = "Book")
	FString Contents;

	UPROPERTY(BlueprintReadWrite, Category = "Book")
	FString ISBN;

	UPROPERTY(BlueprintReadWrite, Category = "Book")
	FString Price;

	UPROPERTY(BlueprintReadWrite, Category = "Book")
	FString SalePrice;

	UPROPERTY(BlueprintReadWrite, Category = "Book")
	FString Status;

	UPROPERTY(BlueprintReadWrite, Category = "Book")
	FString DateTime;
};

UCLASS()
class AT_THEENDOFTHESHELF_API ALibraryStruct : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALibraryStruct();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
