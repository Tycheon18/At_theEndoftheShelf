// Fill out your copyright notice in the Description page of Project Settings.


#include "BookReviewManager.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "GenericPlatform/GenericPlatformHttp.h"

void UBookReviewManager::UploadReviewWithCover(const FString& Title, const FString& Author, const FString& Content, const FBookInfo& BookInfo, const FString& CoverImagePath)
{
	FString Boundary = "----UnrealBoundary" + FGuid::NewGuid().ToString();

	TArray<uint8> Payload;

	auto AppendString = [&](const FString& Str) {
		FTCHARToUTF8 Converter(*Str);
		Payload.Append((uint8*)Converter.Get(), Converter.Length());
		};

	auto AddField = [&](const FString& Name, const FString& Value) {
		AppendString(FString::Printf(TEXT("--%s\r\n"), *Boundary));
		AppendString(FString::Printf(
			TEXT("Content-Disposition: form-data; name=\"%s\"\r\n\r\n"),
			*Name
		));
		AppendString(Value + TEXT("\r\n"));
		};

	AddField("title", Title);
	AddField("author", Author);
	AddField("content", Content);

	FString BookInfoJson;
	if (FJsonObjectConverter::UStructToJsonObjectString(BookInfo, BookInfoJson, 0, 0))
	{
		AddField("bookInfo", BookInfoJson);
	}

	if (FPaths::FileExists(CoverImagePath))
	{
		TArray<uint8> FileData;
		FFileHelper::LoadFileToArray(FileData, *CoverImagePath);

		FString FileName = FPaths::GetCleanFilename(CoverImagePath);
		AppendString(FString::Printf(TEXT("--%s\r\n"), *Boundary));
		AppendString(FString::Printf(TEXT("Content-Disposition: form-data; name=\"coverImage\"; filename=\"%s\"\r\n"), *FileName));
		AppendString(TEXT("Context-Type: application/octet-stream\r\n\r\n"));
	
		Payload.Append(FileData);
		AppendString(TEXT("\r\n"));
	}

	AppendString(FString::Printf(TEXT("--%s--\r\n"), *Boundary));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL("http://localhost:4000/reviews");
	Request->SetVerb("POST");
	Request->SetHeader(
		TEXT("Content-Type"),
		FString::Printf(TEXT("multipart/form-data; boundary=%s"), *Boundary)
	);
	Request->SetContent(Payload);
	Request->OnProcessRequestComplete().BindUObject(this, &UBookReviewManager::OnUploadResponse);
	Request->ProcessRequest();
}

void UBookReviewManager::OnUploadResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	bool bSuccess = bWasSuccessful && Response.IsValid() && (Response->GetResponseCode() == 201);

	OnReviewUploaded.Broadcast(bSuccess);
}
