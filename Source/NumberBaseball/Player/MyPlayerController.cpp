#include "Player/MyPlayerController.h"
#include "UI/ChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NumberBaseball.h"
#include "Game/MyGameModeBase.h"
#include "Player/MyPlayerState.h"
#include "UI/NotificationText.h"

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController() == false)
		return;

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationWidgetClass))
	{
		NotificationWidgetInstance = CreateWidget<UNotificationText>(this, NotificationWidgetClass);
		if (IsValid(NotificationWidgetInstance))
		{
			NotificationWidgetInstance->AddToViewport();
		}
	}
}

void AMyPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	NotificationWidgetInstance->AddDelegate();
}

void AMyPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	if (IsLocalController())
	{
		ServerRPCPrintChatMessageString(InChatMessageString);
	}
}

void AMyPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	NumberBaseballLibrary::MyPrintString(this, InChatMessageString, 5.0f, FColor::Red);
}

void AMyPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void AMyPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	AMyGameModeBase* MyGameModeBase = GetWorld()->GetAuthGameMode<AMyGameModeBase>();
	MyGameModeBase->PrintChatMessageString(this, InChatMessageString);
}
