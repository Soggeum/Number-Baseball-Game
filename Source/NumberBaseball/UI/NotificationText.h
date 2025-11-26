// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NotificationText.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class NUMBERBASEBALL_API UNotificationText : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_ChatInput;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> TimeProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GuessCountBlock;

	UFUNCTION()
	void HandleNotification(const FString& NotificationString);
	UFUNCTION()
	void HandleRemainingTime(float RemainingTime, float MaxTime);
	UFUNCTION()
	void HandleGuessCount(int32 CurrentGuessCount, int32 MaxGuessCount);

	void AddDelegate();
};