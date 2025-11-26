#include "UI/NotificationText.h"
#include "Components/TextBlock.h"
#include "Game/MyGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"
#include "Player/MyPlayerController.h"
#include "Player/MyPlayerState.h"

void UNotificationText::NativeConstruct()
{
	AGameStateBase* GS = UGameplayStatics::GetGameState(this);
	if (IsValid(GS))
	{
		AMyGameStateBase* MyGS = Cast<AMyGameStateBase>(GS);
		if (IsValid(MyGS))
		{
			MyGS->OnNotification.AddDynamic(this, &UNotificationText::HandleNotification);
		}
	}
}

void UNotificationText::NativeDestruct()
{
	AGameStateBase* GS = UGameplayStatics::GetGameState(this);
	if (IsValid(GS))
	{
		AMyGameStateBase* MyGS = Cast<AMyGameStateBase>(GS);
		if (IsValid(MyGS))
		{
			MyGS->OnNotification.RemoveDynamic(this, &UNotificationText::HandleNotification);
		}
	}

	AMyPlayerController* MyPC = GetOwningLocalPlayer<AMyPlayerController>();
	if (IsValid(MyPC))
	{
		AMyPlayerState* MyPS = MyPC->GetPlayerState<AMyPlayerState>();
		if (IsValid(MyPS))
		{
			MyPS->OnRemainingTime.RemoveDynamic(this, &UNotificationText::HandleRemainingTime);
		}
	}
}

void UNotificationText::HandleNotification(const FString& NotificationString)
{
	TextBlock_ChatInput->SetText(FText::FromString(NotificationString));
}

void UNotificationText::HandleRemainingTime(float RemainingTime, float MaxTime)
{
	TimeProgressBar->SetPercent(RemainingTime / MaxTime);
}

void UNotificationText::HandleGuessCount(int32 CurrentGuessCount, int32 MaxGuessCount)
{
	GuessCountBlock->SetText(FText::FromString(FString::Printf(TEXT("Guess Count : %d / %d"), CurrentGuessCount, MaxGuessCount)));
}

void UNotificationText::AddDelegate()
{
	AMyPlayerController* MyPC = GetOwningPlayer<AMyPlayerController>();
	if (IsValid(MyPC))
	{
		AMyPlayerState* MyPS = MyPC->GetPlayerState<AMyPlayerState>();
		if (IsValid(MyPS))
		{
			MyPS->OnRemainingTime.AddDynamic(this, &UNotificationText::HandleRemainingTime);
			MyPS->OnGuessCount.AddDynamic(this, &UNotificationText::HandleGuessCount);
		}
	}
}
