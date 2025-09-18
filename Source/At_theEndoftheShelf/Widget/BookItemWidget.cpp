// Fill out your copyright notice in the Description page of Project Settings.


#include "BookItemWidget.h"
#include "../AES_GameInstance.h"
#include "../NetworkManager.h"
#include "../LibraryStruct.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UBookItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UAES_GameInstance* GameInstance = Cast<UAES_GameInstance>(GetGameInstance());
	if(GameInstance)
	{
		NetworkManager = GameInstance->GetNetworkManager();
		if(NetworkManager)
		{
			NetworkManager->OnImageDownload.AddDynamic(this, &UBookItemWidget::OnImageDownloaded);

            if (!CurrentImageUrl.IsEmpty())
            {
                NetworkManager->DownloadImage(CurrentImageUrl);
            }
		}

	}
}

void UBookItemWidget::SetBookData(const FBookInfo& InBookInfo)
{
    UE_LOG(LogTemp, Warning, TEXT("SetBookData called with thumbnail: %s"), *InBookInfo.Thumbnail);

    if (TitleText)
    {
        TitleText->SetText(FText::FromString(InBookInfo.Title));
    }

    if (AuthorsText)
    {
        FString AuthorsString = FString::Join(InBookInfo.Authors, TEXT(", "));
        AuthorsText->SetText(FText::FromString(AuthorsString));
    }

    if (PublisherText)
    {
        PublisherText->SetText(FText::FromString(InBookInfo.Publisher));
    }

    CurrentImageUrl = InBookInfo.Thumbnail;

    if (NetworkManager && !CurrentImageUrl.IsEmpty())
    {
        NetworkManager->DownloadImage(CurrentImageUrl);
    }

}

void UBookItemWidget::OnImageDownloaded(const FString& ImageUrl, UTexture2D* Texture)
{
    if (ImageUrl == CurrentImageUrl && ThumbnailImage)
    {
        if (Texture)
        {
            ThumbnailImage->SetBrushFromTexture(Texture);
        }
    }
}

