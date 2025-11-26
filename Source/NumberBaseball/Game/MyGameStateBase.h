// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotification, const FString&, NewNotice);

UCLASS()
class NUMBERBASEBALL_API AMyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMyGameStateBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));

	UPROPERTY(BlueprintAssignable)
	FOnNotification OnNotification;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Winner)
	FString Notification;

	UFUNCTION()
	void OnRep_Winner();

	UPROPERTY(Replicated)
	int32 TurnIndex;
	int32 PlayerNum;
};
