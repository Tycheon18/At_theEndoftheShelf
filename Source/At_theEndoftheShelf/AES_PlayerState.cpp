// Fill out your copyright notice in the Description page of Project Settings.


#include "AES_PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

AAES_PlayerState::AAES_PlayerState()
{
	bReplicates = true; 
}

void AAES_PlayerState::BeginPlay()
{
	Super::BeginPlay();

}

void AAES_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// Replicate properties here if needed

	DOREPLIFETIME(AAES_PlayerState, BookLibrary);
	DOREPLIFETIME(AAES_PlayerState, ReadingLogs);
}

void AAES_PlayerState::AddBookToLibrary(const FBookInfo& BookInfo)
{
	for(int32 i = 0; i < BookLibrary.Num(); ++i)
	{
		if(BookLibrary[i].ISBN == BookInfo.ISBN)
		{
			// Book already exists, update it
			BookLibrary[i] = BookInfo;
			OnBookAdded.Broadcast(BookInfo);
			return;
		}
	}

	BookLibrary.Add(BookInfo);
	OnBookAdded.Broadcast(BookInfo);
}

void AAES_PlayerState::RemoveBookFromLibrary(const FString& ISBN)
{
	for(int32 i = 0; i < BookLibrary.Num(); ++i)
	{
		if(BookLibrary[i].ISBN == ISBN)
		{
			BookLibrary.RemoveAt(i);
			OnBookRemoved.Broadcast(ISBN);
			return;
		}
	}
}

bool AAES_PlayerState::IsBookInLibrary(const FString& ISBN) const
{
	for (const auto& Book : BookLibrary)
	{
		if (Book.ISBN == ISBN)
		{
			return true;
		}
	}

	return false;
}

FBookInfo AAES_PlayerState::GetBookFromLibrary(const FString& ISBN) const
{
	for(const auto& Book : BookLibrary)
	{
		if (Book.ISBN == ISBN)
		{
			return Book;
		}
	}

	return FBookInfo();
}

TArray<FBookInfo> AAES_PlayerState::GetAllBooks() const
{
	return BookLibrary;
}

int32 AAES_PlayerState::GetLibraryBookCount() const
{
	return BookLibrary.Num();
}

void AAES_PlayerState::AddReadingLog(const FReadingLog& ReadingLog)
{
	for(int32 i = 0; i < ReadingLogs.Num(); ++i)
	{
		if(ReadingLogs[i].Id == ReadingLog.Id)
		{
			// Log already exists, update it
			ReadingLogs[i] = ReadingLog;
			OnReadingLogUpdated.Broadcast(ReadingLog);
			return;
		}
	}

	ReadingLogs.Add(ReadingLog);
	OnReadingLogAdded.Broadcast(ReadingLog);
}

void AAES_PlayerState::UpdateReadingLog(const FString& LogId, const FString& Review, int32 Rating)
{
	for(int32 i = 0; i < ReadingLogs.Num(); ++i)
	{
		if(ReadingLogs[i].Id == LogId)
		{
			ReadingLogs[i].Review = Review;
			ReadingLogs[i].Rating = Rating;
			ReadingLogs[i].UpdatedAt = GetCurrentDateTime();

			OnReadingLogUpdated.Broadcast(ReadingLogs[i]);
			return;
		}
	}
}

void AAES_PlayerState::RemoveReadingLog(const FString& LogId)
{
	for(int32 i = 0; i < ReadingLogs.Num(); ++i)
	{
		if(ReadingLogs[i].Id == LogId)
		{
			ReadingLogs.RemoveAt(i);
			OnReadingLogRemoved.Broadcast(LogId);
			return;
		}
	}
}

FReadingLog AAES_PlayerState::GetReadingLog(const FString& LogId) const
{
	for(const auto& Log : ReadingLogs)
	{
		if(Log.Id == LogId)
		{
			return Log;
		}
	}

	return FReadingLog();
}

TArray<FReadingLog> AAES_PlayerState::GetAllReadingLogs() const
{
	return ReadingLogs;
}

int32 AAES_PlayerState::GetReadingLogCount() const
{
	return ReadingLogs.Num();
}

void AAES_PlayerState::CacheSearchResult(const FString& Query, const TArray<FBookInfo>& Results)
{
	SearchCache.Add(Query, Results);
	CacheTimestamps.Add(Query, GetCurrentDateTime());
}

TArray<FBookInfo> AAES_PlayerState::GetCachedSearchResult(const FString& Query) const
{
	if (SearchCache.Contains(Query) && IsCacheValid(Query))
	{
		return SearchCache[Query];
	}

	return TArray<FBookInfo>();
}

void AAES_PlayerState::ClearSearchCache()
{
	SearchCache.Empty();
	CacheTimestamps.Empty();
}

bool AAES_PlayerState::HasCachedResults(const FString& Query) const
{
	return SearchCache.Contains(Query) && IsCacheValid(Query);
}

float AAES_PlayerState::GetAverageRating() const
{
	if (ReadingLogs.Num() == 0)
	{
		return 0.0f;
	}

	float TotalRating = 0.0f;
	for (const auto& Log : ReadingLogs)
	{
		TotalRating += Log.Rating;
	}

	return TotalRating / ReadingLogs.Num();

}

TArray<FString> AAES_PlayerState::GetMostReadAuthors() const
{
	TMap<FString, int32> AuthorCounts;

	for (const auto& Log : ReadingLogs)
	{
		for (const auto& Author : Log.BookAuthors)
		{
			if (AuthorCounts.Contains(Author))
			{
				AuthorCounts[Author]++;
			}
			else
			{
				AuthorCounts.Add(Author, 1);
			}
		}
	}

	TArray<TPair<FString, int32>> SortedAuthors;
	for(const auto& Pair : AuthorCounts)
	{
		SortedAuthors.Add(Pair);
	}

	SortedAuthors.Sort([](const TPair<FString, int32>& A, const TPair<FString, int32>& B)
	{
		return A.Value > B.Value; // Sort by count descending
	});

	TArray<FString> Result;
	for (const auto& Pair : SortedAuthors)
	{
		Result.Add(Pair.Key);
	}

	return Result;
}

int32 AAES_PlayerState::GetBooksReadThisMonth() const
{
	FDateTime Now = GetCurrentDateTime();
	FDateTime StartOfMonth(Now.GetYear(), Now.GetMonth(), 1);

	int32 Count = 0;
	for(const auto& Log : ReadingLogs)
	{
		if (Log.ReadDate >= StartOfMonth)
		{
			Count++;
		}
	}

	return Count;
}

TArray<FBookInfo> AAES_PlayerState::GetRecentlyReadBooks(int32 Count) const
{
	TArray<FReadingLog> SortedLogs = ReadingLogs;

	SortedLogs.Sort([](const FReadingLog& A, const FReadingLog& B)
		{
			return A.ReadDate > B.ReadDate; // Sort by read date descending
		});

	TArray<FBookInfo> Result;
	for(int32 i = 0; i < FMath::Min(Count, SortedLogs.Num()); ++i)
	{

		FBookInfo BookInfo = GetBookFromLibrary(SortedLogs[i].ISBN);
		if (!BookInfo.Title.IsEmpty()) // Ensure book exists in library
		{
			Result.Add(BookInfo);
		}
	}

	return Result;
}

void AAES_PlayerState::OnRep_BookLibrary()
{
	// Handle book library replication
}

void AAES_PlayerState::OnRep_ReadingLogs()
{
	// Handle reading logs replication
}

FString AAES_PlayerState::GenerateLogId() const
{
	// Generate a unique log ID, e.g., using a GUID or timestamp
	return FGuid::NewGuid().ToString();
}

FDateTime AAES_PlayerState::GetCurrentDateTime() const
{
	// Get the current date and time
	return FDateTime::Now();
}

bool AAES_PlayerState::IsCacheValid(const FString& Query) const
{
		// Check if the cache for the given query is still valid
	if (CacheTimestamps.Contains(Query))
	{
		return false;
	}

	FDateTime Now = GetCurrentDateTime();
	FDateTime CacheTime = CacheTimestamps[Query];

	return (Now - CacheTime).GetTotalHours() < 1.0f;
}

void AAES_PlayerState::CleanupOldCache()
{
	// Remove cache entries older than a certain threshold
	FDateTime Now = GetCurrentDateTime();
	TArray<FString> KeysToRemove;

	for (const auto& Pair : CacheTimestamps)
	{
		if ((Now - Pair.Value).GetTotalHours() > 24.0f) // Example: remove if older than 60 minutes
		{
			KeysToRemove.Add(Pair.Key);
		}
	}

	for(const auto& Key : KeysToRemove)
	{
		SearchCache.Remove(Key);
		CacheTimestamps.Remove(Key);
	}
}