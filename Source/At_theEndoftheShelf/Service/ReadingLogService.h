// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../NetworkManager.h"
#include "ReadingLogService.generated.h"

USTRUCT(BlueprintType)
struct FReadingLog 
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Reading Log")
	FString Id;

	UPROPERTY(BlueprintReadWrite, Category = "Reading Log")
	FString BookTitle;

	UPROPERTY(BlueprintReadWrite, Category = "Reading Log")
	TArray<FString> BookAuthors;

	UPROPERTY(BlueprintReadWrite, Category = "Reading Log")
	FString BookPublisher;

	UPROPERTY(BlueprintReadWrite, Category = "Reading Log")
	FString Review;

	UPROPERTY(BlueprintReadWrite, Category = "Reading Log")
	int32 Rating;

	UPROPERTY(BlueprintReadWrite, Category = "Reading Log")
	FDateTime ReadDate;

	UPROPERTY(BlueprintReadWrite, Category = "Reading Log")
	FString BookCoverURL;

	UPROPERTY(BlueprintReadWrite, Category = "Reading Log")
	FString ISBN;

	UPROPERTY(BlueprintReadWrite, Category = "Reading Log")
	FDateTime CreatedAt;

	UPROPERTY(BlueprintReadWrite, Category = "Reading Log")
	FDateTime UpdatedAt;
};

USTRUCT(BlueprintType)
struct FReadingLogListResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Reading Log")
	TArray<FReadingLog> Logs;

	UPROPERTY(BlueprintReadWrite, Category = "Reading Log")
	int32 TotalCount;

	UPROPERTY(BlueprintReadWrite, Category = "Reading Log")
	int32 Page;

	UPROPERTY(BlueprintReadWrite, Category = "Reading Log")
	int32 Size;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLogsListReceived, const FReadingLogListResponse&, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadingLogReceived, const FReadingLog&, Log);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadingLogCreated, const FReadingLog&, Log);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadingLogUpdated, const FReadingLog&, Log);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadingLogDeleted, const FString&, LogId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadingLogError, const FString&, ErrorMessage);
/**
 * 
 */
UCLASS()
class AT_THEENDOFTHESHELF_API UReadingLogService : public UObject
{
	GENERATED_BODY()
	
public:

	UReadingLogService();

	UFUNCTION(BlueprintCallable, Category = "Reading Log")
	void CreatedReadingLog(const FReadingLog& Log);

	UFUNCTION(BlueprintCallable, Category = "Reading Log")
	void GetReadingLogs(int32 Page = 1, int32 Size = 10);

	UFUNCTION(BlueprintCallable, Category = "Reading Log")
	void GetReadingLog(const FString& LogId);

	UFUNCTION(BlueprintCallable, Category = "Reading Log")
	void UpdateReadingLog(const FString& LogId, const FReadingLog& Log);

	UFUNCTION(BlueprintCallable, Category = "Reading Log")
	void DeleteReadingLog(const FString& LogId);

	UFUNCTION(BlueprintCallable, Category = "Reading Log")
	void SearchReadingLogs(const FString& Query, int32 Page = 1, int32 Size = 10);

	UPROPERTY(BlueprintAssignable, Category = "Reading Log")	
	FOnLogsListReceived OnLogsListReceived;

	UPROPERTY(BlueprintAssignable, Category = "Reading Log")
	FOnReadingLogReceived OnReadingLogReceived;

	UPROPERTY(BlueprintAssignable, Category = "Reading Log")
	FOnReadingLogCreated OnReadingLogCreated;

	UPROPERTY(BlueprintAssignable, Category = "Reading Log")
	FOnReadingLogUpdated OnReadingLogUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Reading Log")
	FOnReadingLogDeleted OnReadingLogDeleted;

	UPROPERTY(BlueprintAssignable, Category = "Reading Log")
	FOnReadingLogError OnReadingLogError;

	UFUNCTION(BlueprintCallable, Category = "Reading Log")
	void SetNetworkManager(UNetworkManager* InNetworkManager)
	{
		NetworkManager = InNetworkManager;
	}

	UFUNCTION(BlueprintCallable, Category = "Reading Log")
	void SetBaseURL(const FString& URL)
	{
		BaseURL = URL;
	}

protected:

	void OnResponse(const FString& Response);
	void OnError(const FString& Error);
	bool ParseReadingLog(const FString& JsonString, FReadingLog& OutLog);
	bool ParseReadingLogFromObject(const TSharedPtr<FJsonObject>& JsonObject, FReadingLog& OutLog);
	bool ParseReadingLogList(const FString& JsonString, FReadingLogListResponse& OutResponse);
	FString ReadingLogToJson(const FReadingLog& Log);

	UPROPERTY()
	UNetworkManager* NetworkManager;

	UPROPERTY()
	FString BaseURL;

	UPROPERTY()
	FString CurrentRequestType;
};
