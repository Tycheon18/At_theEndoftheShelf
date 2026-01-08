// Fill out your copyright notice in the Description page of Project Settings.


#include "BookshelfManager.h"
#include "Actor/BookActor.h"

// Sets default values
ABookshelfManager::ABookshelfManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = Root;
}

// Called when the game starts or when spawned
void ABookshelfManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABookshelfManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABookshelfManager::AddBook(const FBookInfo& BookInfo)
{
    //if (!BookActorClass)
    //{
    //    UE_LOG(LogTemp, Error, TEXT("BookshelfManager: BookActorClass not set!"));
    //    return nullptr;
    //}

    //UWorld* World = GetWorld();

    //if (!World) return nullptr;

    //int32 BookIndex = SpawnedBooks.Num();
    //FVector Location = CalculateBookPosition(BookIndex);
    //FRotator Rotation = CalculateBookRotation(BookIndex);

    //ABookActor* NewBook = World->SpawnActor<ABookActor>(BookActorClass, Location, Rotation);
    //if (NewBook)
    //{
    //    NewBook->SetBookData(BookInfo);
    //    SpawnedBooks.Add(NewBook);

    //    UE_LOG(LogTemp, Log, TEXT("BookshelfManager: Added book '%s' at position %d"),
    //        *BookInfo.Title, BookIndex);
    //}

    //return NewBook;

    AllBooks.Add(BookInfo);
    UpdateTotalPages();

    if (CurrentPage == 0)
    {
        RefreshCurrentPage();
    }
}

//void ABookshelfManager::RemoveBook(ABookActor* BookActor)
//{
//    if (!BookActor) return;
//
//    SpawnedBooks.Remove(BookActor);
//    BookActor->Destroy();
//
//    RearrangeBooks();
//
//    UE_LOG(LogTemp, Log, TEXT("BookshelfManager: Removed book"));
//}

void ABookshelfManager::RemoveBook(int32 Index)
{
    if (AllBooks.IsValidIndex(Index))
    {
        AllBooks.RemoveAt(Index);
        UpdateTotalPages();
        RefreshCurrentPage();
    }
}

void ABookshelfManager::ClearAllBooks()
{
    for (AActor* BookActor : CurrentPageBooks)
    {
        if (BookActor && !BookActor->IsPendingKillPending())
        {
            BookActor->Destroy();
        }
    }

    CurrentPageBooks.Empty();

    AllBooks.Empty();
    ClearCurrentPageBooks();
    CurrentPage = 0;
    TotalPages = 0;
}

void ABookshelfManager::RearrangeBooks()
{
    RefreshCurrentPage();

}

void ABookshelfManager::SetPage(int32 PageIndex)
{
    if (PageIndex < 0 || PageIndex >= TotalPages)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid page index: %d (Total: %d)"), PageIndex, TotalPages);
        return;
    }

    CurrentPage = PageIndex;
    RefreshCurrentPage();
    UE_LOG(LogTemp, Log, TEXT("BookshelfManager: Set to page %d"), CurrentPage);
}

void ABookshelfManager::RefreshCurrentPage()
{
    if(!BookActorClass)
    {
        UE_LOG(LogTemp, Error, TEXT("BookshelfManager: BookActorClass not set!"));
        return;
	}

    ClearCurrentPageBooks();

    int32 StartIndex = CurrentPage * BooksPerPage;
    int32 EndIndex = FMath::Min(StartIndex + BooksPerPage, AllBooks.Num());

    for (int32 i = StartIndex; i < EndIndex; ++i)
    {
        FVector SpawnLocation = GetActorLocation() + CalculateBookPosition(i - StartIndex);
        FRotator SpawnRotation = GetActorRotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        AActor* NewBookActor = GetWorld()->SpawnActor<AActor>(BookActorClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (NewBookActor)
        {
            if (ABookActor* BookActor = Cast<ABookActor>(NewBookActor))
            {
                BookActor->SetBookData(AllBooks[i]);
            }

            CurrentPageBooks.Add(NewBookActor);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Page %d/%d loaded (%d books)"),
        CurrentPage + 1, TotalPages, CurrentPageBooks.Num());
}

void ABookshelfManager::NextPage()
{
    if(CurrentPage + 1 < TotalPages)
    {
        SetPage(CurrentPage + 1);
    }
}

void ABookshelfManager::PreviousPage()
{
	if (CurrentPage > 0)
    {
        SetPage(CurrentPage - 1);
    }
}

FVector ABookshelfManager::CalculateBookPosition(int32 Index) const
{
    FVector BaseLocation = GetActorLocation() + ShelfStartLocation;

    switch (LayoutType)
    {
    case EShelfLayout::Grid:
    {
        int32 Row = Index / BooksPerRow;
        int32 Col = Index % BooksPerRow;

        return BaseLocation + FVector(
            0,
            Col * BookSpacing,
            -Row * RowSpacing
        );
    }

    case EShelfLayout::Line:
    {
        return BaseLocation + FVector(0, Index * BookSpacing, 0);
    }

    case EShelfLayout::Circle:
    {
        float Angle = (360.0f / FMath::Max(1, SpawnedBooks.Num())) * Index;
        float Radius = BookSpacing * 5.0f;

        return BaseLocation + FVector(
            FMath::Cos(FMath::DegreesToRadians(Angle)) * Radius,
            FMath::Sin(FMath::DegreesToRadians(Angle)) * Radius,
            0
        );
    }

    default:
        return BaseLocation;
    }
}

FRotator ABookshelfManager::CalculateBookRotation(int32 Index) const
{
    switch (LayoutType)
    {
    case EShelfLayout::Circle:
    {
        float Angle = (360.0f / FMath::Max(1, SpawnedBooks.Num())) * Index;
        return FRotator(0, Angle + 90.0f, 0); 
    }

    default:
        return FRotator::ZeroRotator;
    }
}

void ABookshelfManager::UpdateTotalPages()
{
    if (BooksPerPage <= 0)
    {
        TotalPages = 0;
        return;
    }

    TotalPages = FMath::CeilToInt(static_cast<float>(AllBooks.Num()) / BooksPerPage);

    if (CurrentPage >= TotalPages && TotalPages > 0)
    {
		CurrentPage = TotalPages - 1;
    }
}

void ABookshelfManager::ClearCurrentPageBooks()
{
    for (AActor* BookActor : CurrentPageBooks)
    {
        if (BookActor)
        {
            BookActor->Destroy();
        }
    }
    CurrentPageBooks.Empty();
}