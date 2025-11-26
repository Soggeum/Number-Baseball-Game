#include "Player/MyPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Game/MyGameModeBase.h"
#include "Kismet/GameplayStatics.h"

AMyPlayerState::AMyPlayerState()
{
	bReplicates = true;
	CurrentGuessCount = 0;
	MaxGuessCount = 3;

	MaxTime = 5.0f;
	RemainingTime = MaxTime;


	SetNetUpdateFrequency(NetUpdateFrequency);
}

void AMyPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyPlayerState, PlayerNameString);
	DOREPLIFETIME(AMyPlayerState, CurrentGuessCount);

	DOREPLIFETIME(AMyPlayerState, RemainingTime);
}

FString AMyPlayerState::GetPlayerInfoString()
{
	FString PlayerInfoString = PlayerNameString + TEXT("(") + FString::FromInt(CurrentGuessCount) + TEXT("/") + FString::FromInt(MaxGuessCount) + TEXT(")");
	return PlayerInfoString;
}

void AMyPlayerState::OnRep_GuessCount()
{
	OnGuessCount.Broadcast(CurrentGuessCount, MaxGuessCount);
}

void AMyPlayerState::OnRep_RemainingTime()
{
	//UE_LOG(LogTemp, Error, TEXT("OnRep"));


	OnRemainingTime.Broadcast(RemainingTime, MaxTime);
}

void AMyPlayerState::StartTurn()
{
	RemainingTime = MaxTime;

	GetWorldTimerManager().SetTimer(
		TimerHandler,
		this,
		&AMyPlayerState::TickTurn,
		0.1f,
		true
	);
}

void AMyPlayerState::EndTurn()
{
	GetWorldTimerManager().ClearTimer(TimerHandler);
	CurrentGuessCount++;
	RemainingTime = 0;
}

void AMyPlayerState::TickTurn()
{
	RemainingTime -= 0.1f;

	if (RemainingTime <= 0)
	{
		EndTurn();
		AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
		if (IsValid(GM))
		{
			AMyGameModeBase* MyGM = Cast<AMyGameModeBase>(GM);
			if (IsValid(MyGM))
			{
				MyGM->JudgeGame("", 0);
			}
		}
	}
}
