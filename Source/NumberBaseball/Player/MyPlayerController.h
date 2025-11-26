// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

class UChatInput;
class UNotificationText;

UCLASS()
class NUMBERBASEBALL_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override;

	void SetChatMessageString(const FString& InChatMessageString);
	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UChatInput> ChatInputWidgetClass;
	UPROPERTY()
	TObjectPtr<UChatInput> ChatInputWidgetInstance;

	FString ChatMessageString;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNotificationText> NotificationWidgetClass;
	UPROPERTY()
	TObjectPtr<UNotificationText> NotificationWidgetInstance;

};