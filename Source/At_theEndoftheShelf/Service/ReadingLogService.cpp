// Fill out your copyright notice in the Description page of Project Settings.


#include "ReadingLogService.h"
#include "JsonUtilities.h"

UReadingLogService::UReadingLogService()
{
	NetworkManager = nullptr;
	BaseURL = TEXT("/api/reading-logs");
	CurrentRequestType = TEXT("");

}

void UReadingLogService::CreatedReadingLog(const FReadingLog& Log)
{
	if(!NetworkManager)
	{
		OnReadingLogError.Broadcast(TEXT("NetworkManager가 설정되지 않았습니다."));
		return;
	}

	CurrentRequestType = TEXT("CREATE");

	NetworkManager->OnResponse.AddDynamic(this, &UReadingLogService::OnResponse);
	NetworkManager->OnError.AddDynamic(this, &UReadingLogService::OnError);

	FString JsonBody = ReadingLogToJson(Log);

	TMap<FString, FString> Headers;
	Headers.Add(TEXT("Content-Type"), TEXT("application/json"));

	NetworkManager->Post(BaseURL, JsonBody,
		Headers);
}

void UReadingLogService::GetReadingLogs(int32 Page, int32 Size)
{
	if(!NetworkManager)
	{
		OnReadingLogError.Broadcast(TEXT("NetworkManager가 설정되지 않았습니다."));
		return;
	}

	CurrentRequestType = TEXT("GET_LIST");

	NetworkManager->OnResponse.AddDynamic(this, &UReadingLogService::OnResponse);
	NetworkManager->OnError.AddDynamic(this, &UReadingLogService::OnError);

	FString URL = FString::Printf(TEXT("%s?page=%d&size=%d"), *BaseURL, Page, Size);

	TMap<FString, FString> Headers;

	NetworkManager->Get(URL, Headers);
}

void UReadingLogService::GetReadingLog(const FString& LogId)
{
	if(!NetworkManager)
	{
		OnReadingLogError.Broadcast(TEXT("NetworkManager가 설정되지 않았습니다."));
		return;
	}

	CurrentRequestType = TEXT("GET_ONE");

	NetworkManager->OnResponse.AddDynamic(this, &UReadingLogService::OnResponse);
	NetworkManager->OnError.AddDynamic(this, &UReadingLogService::OnError);

	FString URL = FString::Printf(TEXT("%s/%s"), *BaseURL, *LogId);

	TMap<FString, FString> Headers;

	NetworkManager->Get(URL, Headers);
}

void UReadingLogService::UpdateReadingLog(const FString& LogId, const FReadingLog& Log)
{
	if (!NetworkManager)
	{
		OnReadingLogError.Broadcast(TEXT("NetworkManager가 설정되지 않았습니다."));
		return;
	}

	CurrentRequestType = TEXT("UPDATE");

	NetworkManager->OnResponse.AddDynamic(this, &UReadingLogService::OnResponse);
	NetworkManager->OnError.AddDynamic(this, &UReadingLogService::OnError);

	FString JsonBody = ReadingLogToJson(Log);

	FString URL = FString::Printf(TEXT("%s/%s"), *BaseURL, *LogId);

	TMap<FString, FString> Headers;
	Headers.Add("Content-Type", "application/json");

	NetworkManager->Put(URL, JsonBody, Headers);
}

void UReadingLogService::DeleteReadingLog(const FString& LogId)
{
	if(!NetworkManager)
	{
		OnReadingLogError.Broadcast(TEXT("NetworkManager가 설정되지 않았습니다."));
		return;
	}

	CurrentRequestType = TEXT("DELETE");

	NetworkManager->OnResponse.AddDynamic(this, &UReadingLogService::OnResponse);
	NetworkManager->OnError.AddDynamic(this, &UReadingLogService::OnError);

	FString URL = FString::Printf(TEXT("%s/%s"), *BaseURL, *LogId);

	TMap<FString, FString> Headers;
	NetworkManager->Delete(URL, Headers);
}

void UReadingLogService::SearchReadingLogs(const FString& Query, int32 Page, int32 Size)
{
	if(!NetworkManager)
	{
		OnReadingLogError.Broadcast(TEXT("NetworkManager가 설정되지 않았습니다."));
		return;
	}

	CurrentRequestType = TEXT("SEARCH");

	NetworkManager->OnResponse.AddDynamic(this, &UReadingLogService::OnResponse);
	NetworkManager->OnError.AddDynamic(this, &UReadingLogService::OnError);

	FString URL = FString::Printf(TEXT("%s/search?query=%s&page=%d&size=%d"), *BaseURL, *FGenericPlatformHttp::UrlEncode(Query), Page, Size);

	TMap<FString, FString> Headers;
	NetworkManager->Get(URL, Headers);
}

void UReadingLogService::OnResponse(const FString& Response)
{
	// Handle the response from the server
	NetworkManager->OnResponse.RemoveDynamic(this, &UReadingLogService::OnResponse);
	NetworkManager->OnError.RemoveDynamic(this, &UReadingLogService::OnError);

	if (CurrentRequestType == TEXT("CREATE"))
	{
		FReadingLog Log;
		if(ParseReadingLog(Response, Log))
		{
			OnReadingLogCreated.Broadcast(Log);

		}
		else
		{
			OnReadingLogError.Broadcast(TEXT("Failed to parse created reading log."));
		}
	}
	else if (CurrentRequestType == TEXT("GET_LIST") || CurrentRequestType == TEXT("SEARCH"))
	{
		FReadingLogListResponse LogListResponse;
		if(ParseReadingLogList(Response, LogListResponse))
		{
			OnLogsListReceived.Broadcast(LogListResponse);
		}
		else
		{
			OnReadingLogError.Broadcast(TEXT("Failed to parse reading log list."));
		}
	}
	else if (CurrentRequestType == TEXT("UPDATE"))
	{
		FReadingLog Log;
		if(ParseReadingLog(Response, Log))
		{
			OnReadingLogUpdated.Broadcast(Log);
		}
		else
		{
			OnReadingLogError.Broadcast(TEXT("Failed to parse updated reading log."));
		}
	}
	else if (CurrentRequestType == TEXT("DELETE"))
	{
		OnReadingLogDeleted.Broadcast("deleted");
	}
	
	CurrentRequestType = TEXT(""); // Reset the request type after handling
}

void UReadingLogService::OnError(const FString& Error)
{
	// Handle the error from the server
	NetworkManager->OnResponse.RemoveDynamic(this, &UReadingLogService::OnResponse);
	NetworkManager->OnError.RemoveDynamic(this, &UReadingLogService::OnError);

	OnReadingLogError.Broadcast(Error);
	CurrentRequestType = TEXT(""); // Reset the request type after handling
}

bool UReadingLogService::ParseReadingLog(const FString& JsonString, FReadingLog& OutLog)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("ReadingLogService: Failed to parse JSON"));
		return false;
	}
	// Parse the JSON object into FReadingLog structure
	// Example parsing logic here...

	return ParseReadingLogFromObject(JsonObject, OutLog);
}

bool UReadingLogService::ParseReadingLogFromObject(const TSharedPtr<FJsonObject>& JsonObject, FReadingLog& OutLog)
{
	if(!JsonObject.IsValid())
	{
		return false;
	}

	OutLog.Id = JsonObject->GetStringField(TEXT("id"));
	OutLog.BookTitle = JsonObject->GetStringField(TEXT("bookTitle"));
	OutLog.BookPublisher = JsonObject->GetStringField(TEXT("bookPublisher"));
	OutLog.Review = JsonObject->GetStringField(TEXT("review"));
	OutLog.Rating = JsonObject->GetNumberField(TEXT("rating"));
	OutLog.BookCoverURL = JsonObject->GetStringField(TEXT("bookCoverURL"));
	OutLog.ISBN = JsonObject->GetStringField(TEXT("isbn"));

	const TArray<TSharedPtr<FJsonValue>>* Authors;
	if(JsonObject->TryGetArrayField(TEXT("bookAuthors"), Authors))
	{
		for (const auto& AuthorValue : *Authors)
		{
			OutLog.BookAuthors.Add(AuthorValue->AsString());
		}
	}

	FString ReadDateString = JsonObject->GetStringField(TEXT("readDate"));
	FDateTime::Parse(ReadDateString, OutLog.ReadDate);

	FString CreatedAtString = JsonObject->GetStringField(TEXT("createdAt"));
	FDateTime::Parse(CreatedAtString, OutLog.CreatedAt);

	FString UpdatedAtString = JsonObject->GetStringField(TEXT("updatedAt"));
	FDateTime::Parse(UpdatedAtString, OutLog.UpdatedAt);


	return true;
}

bool UReadingLogService::ParseReadingLogList(const FString& JsonString, FReadingLogListResponse& OutResponse)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if(!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		return false;
	}

	OutResponse.TotalCount = JsonObject->GetNumberField(TEXT("totalCount"));
	OutResponse.Page = JsonObject->GetNumberField(TEXT("page"));
	OutResponse.Size = JsonObject->GetNumberField(TEXT("size"));

	const TArray<TSharedPtr<FJsonValue>>* Logs;
	if(JsonObject->TryGetArrayField(TEXT("logs"), Logs))
	{
		for (const auto& LogValue : *Logs)
		{
			FReadingLog Log;
			if (ParseReadingLogFromObject(LogValue->AsObject(), Log))
			{
				OutResponse.Logs.Add(Log);
			}
		}
	}

	return true;
}

FString UReadingLogService::ReadingLogToJson(const FReadingLog& Log)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

	JsonObject->SetStringField(TEXT("bookTitle"), Log.BookTitle);
	JsonObject->SetStringField(TEXT("bookPublisher"), Log.BookPublisher);
	JsonObject->SetStringField(TEXT("review"), Log.Review);
	JsonObject->SetNumberField(TEXT("rating"), Log.Rating);
	JsonObject->SetStringField(TEXT("bookCoverURL"), Log.BookCoverURL);
	JsonObject->SetStringField(TEXT("isbn"), Log.ISBN);

	TArray<TSharedPtr<FJsonValue>> Authors;
	for(const auto& Author : Log.BookAuthors)
	{
		Authors.Add(MakeShared<FJsonValueString>(Author));
	}
	JsonObject->SetArrayField(TEXT("bookAuthors"), Authors);

	JsonObject->SetStringField(TEXT("readDate"), Log.ReadDate.ToString());

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);

	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	return OutputString;
}