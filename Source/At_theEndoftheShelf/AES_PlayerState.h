// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LibraryStruct.h"
#include "Service/ReadingLogService.h"
#include "AES_PlayerState.generated.h"

struct FReadingLog;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBookAdded, const FBookInfo&, BookInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBookRemoved, const FString&, ISBN);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadingLogAdded, const FReadingLog&, Log);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadingLogRemoved, const FString&, Log);


/**
 * 
 */
UCLASS()
class AT_THEENDOFTHESHELF_API AAES_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AAES_PlayerState();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Book Library")
	void AddBookToLibrary(const FBookInfo& BookInfo);

	UFUNCTION(BlueprintCallable, Category = "Book Library")
	void RemoveBookFromLibrary(const FString& ISBN);

	UFUNCTION(BlueprintCallable, Category = "Book Library")
	bool IsBookInLibrary(const FString& ISBN) const;

	UFUNCTION(BlueprintCallable, Category = "Book Library")
	FBookInfo GetBookFromLibrary(const FString& ISBN) const;

	UFUNCTION(BlueprintCallable, Category = "Book Library")
	TArray<FBookInfo> GetAllBooks() const;

	UFUNCTION(BlueprintCallable, Category = "Book Library")
	int32 GetLibraryBookCount() const;

	UFUNCTION(BlueprintCallable, Category = "Reading Log")
	void AddReadingLog(const FReadingLog& ReadingLog);

	UFUNCTION(BlueprintCallable, Category = "Reading Log")
	void UpdateReadingLog(const FString& LogId, const FString& Review, int32 Rating);

	UFUNCTION(BlueprintCallable, Category = "Reading Log")
	void RemoveReadingLog(const FString& LogId);

	UFUNCTION(BlueprintCallable, Category = "Reading Log")
	FReadingLog GetReadingLog(const FString& LogId) const;

	UFUNCTION(BlueprintCallable, Category = "Reading Log")
	TArray<FReadingLog> GetAllReadingLogs() const;

	UFUNCTION(BlueprintCallable, Category = "Reading Log")
	int32 GetReadingLogCount() const;

	UFUNCTION(BlueprintCallable ,Category = "Search Cache")
	void CacheSearchResult(const FString& Query, const TArray<FBookInfo>& Results);

	UFUNCTION(BlueprintCallable, Category = "Search Cache")
	TArray<FBookInfo> GetCachedSearchResult(const FString& Query) const;

	UFUNCTION(BlueprintCallable, Category = "Search Cache")
	void ClearSearchCache();

	UFUNCTION(BlueprintCallable, Category = "Search Cache")
	bool HasCachedResults(const FString& Query) const;

	UFUNCTION(BlueprintCallable, Category = "Statistics")
	float GetAverageRating() const;

	UFUNCTION(BlueprintCallable, Category = "Statistics")
	TArray<FString> GetMostReadAuthors() const;

	UFUNCTION(BlueprintCallable, Category = "Statistics")
	int32 GetBooksReadThisMonth() const;

	UFUNCTION(BlueprintCallable, Category = "Statistics")
	TArray<FBookInfo> GetRecentlyReadBooks(int32 Count = 5) const;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBookAdded OnBookAdded;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBookRemoved OnBookRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnReadingLogAdded OnReadingLogAdded;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnReadingLogUpdated OnReadingLogUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnReadingLogRemoved OnReadingLogRemoved;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Library", ReplicatedUsing = OnRep_BookLibrary)
	TArray<FBookInfo> BookLibrary;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reading Logs", ReplicatedUsing = OnRep_ReadingLogs)
	TArray<FReadingLog> ReadingLogs;

	TMap<FString, TArray<FBookInfo>> SearchCache;

	UPROPERTY()
	TMap<FString, FDateTime> CacheTimestamps;

	UFUNCTION()
	void OnRep_BookLibrary();

	UFUNCTION()
	void OnRep_ReadingLogs();

	FString GenerateLogId() const;
	FDateTime GetCurrentDateTime() const;
	bool IsCacheValid(const FString& Query) const;
	void CleanupOldCache();

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
