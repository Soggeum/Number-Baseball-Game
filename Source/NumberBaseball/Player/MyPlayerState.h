#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRemainingTime, float, RemainingTime, float, MaxTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGuessCount, int32, CurrentGuessCount, int32, MaxGuessCount);

UCLASS()
class NUMBERBASEBALL_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMyPlayerState();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FString GetPlayerInfoString();

	UPROPERTY(Replicated)
	FString PlayerNameString;

	int32 GetCurrentGuessCount() const { return CurrentGuessCount; }
	void SetCurrentGuessCount(int32 InCurrentCount) { CurrentGuessCount = InCurrentCount; }


	UPROPERTY()
	int32 MaxGuessCount;
	FOnGuessCount OnGuessCount;
	UFUNCTION()
	void OnRep_GuessCount();

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_RemainingTime)
	float RemainingTime;
	UPROPERTY(Replicated)
	float MaxTime;
	FOnRemainingTime OnRemainingTime;
	UFUNCTION()
	void OnRep_RemainingTime();


	UFUNCTION()
	void StartTurn();
	UFUNCTION()
	void EndTurn();
	UFUNCTION()
	void TickTurn();

	FTimerHandle TimerHandler;

protected:
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_GuessCount)
	int32 CurrentGuessCount;

	const float NetUpdateFrequency = 10.f;
};
