// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LibraryStruct.h"
#include "BookshelfManager.generated.h"

UENUM(BlueprintType)
enum class EShelfLayout : uint8
{
    Grid UMETA(DisplayName = "Grid Layout"),
    Line UMETA(DisplayName = "Line Layout"),
    Circle UMETA(DisplayName = "Circle Layout")
};

UCLASS()
class AT_THEENDOFTHESHELF_API ABookshelfManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABookshelfManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bookshelf")
    TSubclassOf<class ABookActor> BookActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bookshelf")
    EShelfLayout LayoutType = EShelfLayout::Grid;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bookshelf")
    int32 BooksPerRow = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bookshelf")
    float BookSpacing = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bookshelf")
    float RowSpacing = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bookshelf")
    FVector ShelfStartLocation = FVector(1000, 1000, 300);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pagination")
    int32 BooksPerPage = 10;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pagination")
    int32 CurrentPage = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pagination")
    int32 TotalPages = 0;

    UPROPERTY()
    TArray<FBookInfo> AllBooks;

    UPROPERTY()
    TArray<AActor*> CurrentPageBooks;

    UFUNCTION(BlueprintCallable, Category = "Bookshelf")
    void AddBook(const FBookInfo& BookInfo);

    UFUNCTION(BlueprintCallable, Category = "Bookshelf")
    void RemoveBook(int32 Index);

    UFUNCTION(BlueprintCallable, Category = "Bookshelf")
    void ClearAllBooks();

    UFUNCTION(BlueprintCallable, Category = "Bookshelf")
    void RearrangeBooks();

    UFUNCTION(BlueprintCallable, Category = "Bookshelf")
    TArray<ABookActor*> GetAllBooks() const { return SpawnedBooks; }

    UFUNCTION(BlueprintCallable, Category = "Bookshelf")
    int32 GetBookCount() const { return SpawnedBooks.Num(); }

    UFUNCTION(BlueprintCallable, Category = "Pagination")
    void SetPage(int32 PageIndex);

    UFUNCTION(BlueprintCallable, Category = "Pagination")
    void NextPage();

    UFUNCTION(BlueprintCallable, Category = "Pagination")
    void PreviousPage();

    UFUNCTION(BlueprintCallable, Category = "Pagination")
    int32 GetCurrentPage() const { return CurrentPage; }

    UFUNCTION(BlueprintCallable, Category = "Pagination")
    int32 GetTotalPages() const { return TotalPages; }

    UFUNCTION(BlueprintCallable, Category = "Pagination")
    void RefreshCurrentPage();

protected:

    UPROPERTY()
    TArray<ABookActor*> SpawnedBooks;

    FVector CalculateBookPosition(int32 Index) const;
    FRotator CalculateBookRotation(int32 Index) const;

    void UpdateTotalPages();
    void ClearCurrentPageBooks();
};
