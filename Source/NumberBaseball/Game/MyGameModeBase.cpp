#include "Game/MyGameModeBase.h"
#include "Game/MyGameStateBase.h"
#include "Player/MyPlayerController.h"
#include "Player/MyPlayerState.h"

void AMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMyGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	AMyPlayerController* NewMyPlayerController = Cast<AMyPlayerController>(NewPlayer);
	if (IsValid(NewMyPlayerController))
	{
		AllPlayerControllers.Add(NewMyPlayerController);

		AMyPlayerState* MyPlayerState = NewMyPlayerController->GetPlayerState<AMyPlayerState>();
		if (IsValid(MyPlayerState))
		{
			MyPlayerState->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		AMyGameStateBase* MyGameStateBase = GetGameState<AMyGameStateBase>();
		if (IsValid(MyGameStateBase))
		{
			MyGameStateBase->MulticastRPCBroadcastLoginMessage(MyPlayerState->PlayerNameString);
		}
	}

	if (!bIsAlreadyStart)
	{
		bIsAlreadyStart = true;
		StartGame();
	}
}

FString AMyGameModeBase::GenerateSecreteNumber()
{
	TArray<int> Numbers;
	for (int32 i = 1; i <= 9; i++)
	{
		Numbers.Add(i);
	}

	Numbers = Numbers.FilterByPredicate([](int32 Num) {return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; i++)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool AMyGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	if (InNumberString.Len() != 3)
		return false;

	for (const TCHAR C : InNumberString)
	{
		if (!FChar::IsDigit(C) || C == '0')
			return false;
	}

	TSet<TCHAR> UniqueDigits;
	for (const TCHAR C : InNumberString)
	{
		if (UniqueDigits.Contains(C))
			return false;
		UniqueDigits.Add(C);
	}

	return true;
}

FString AMyGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	TSet<TCHAR> UniqueDigits;
	for (const TCHAR C : InSecretNumberString)
	{
		UniqueDigits.Add(C);
	}

	int32 Strike = 0, Ball = 0;
	for (int32 i = 0; i < InGuessNumberString.Len(); i++)
	{
		if (InGuessNumberString[i] == InSecretNumberString[i])
		{
			Strike++;
		}
		else if (UniqueDigits.Contains(InGuessNumberString[i]))
		{
			Ball++;
		}
	}

	if (Strike == 0 && Ball == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), Strike, Ball);
}

void AMyGameModeBase::PrintChatMessageString(AMyPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	AMyPlayerState* MyPlayerState = InChattingPlayerController->GetPlayerState<AMyPlayerState>();
	AMyGameStateBase* MyGS = GetGameState<AMyGameStateBase>();
	if (IsValid(MyPlayerState) && IsValid(MyGS))
	{
		FString Message;
		if (AllPlayerControllers.IsValidIndex(MyGS->TurnIndex) && (AllPlayerControllers[MyGS->TurnIndex] == InChattingPlayerController) && IsGuessNumberString(InChatMessageString))
		{
			MyPlayerState->EndTurn();
			Message = MyPlayerState->GetPlayerInfoString();
			Message.Append(TEXT(" : ")).Append(InChatMessageString);
			FString Result = JudgeResult(SecretNumberString, InChatMessageString);
			Message.Append(" -> ").Append(Result);

			JudgeGame(MyPlayerState->PlayerNameString, Result[0] - '0');
		}
		
		else
		{
			Message = MyPlayerState->GetPlayerInfoString();
			Message.Append(TEXT(" : ")).Append(InChatMessageString);
		}

		for (const TObjectPtr<AMyPlayerController> MyPlayerController : AllPlayerControllers)
		{
			MyPlayerController->ClientRPCPrintChatMessageString(Message);
		}
	}
}

void AMyGameModeBase::ResetGame()
{
	AMyGameStateBase* MyGS = GetGameState<AMyGameStateBase>();
	if (IsValid(MyGS))
	{
		MyGS->TurnIndex = -1;
	}

	GetWorldTimerManager().SetTimer(
		WaitHandle,
		this,
		&AMyGameModeBase::StartGame,
		3.0f,
		false
	);
}

void AMyGameModeBase::StartGame()
{
	for (const TObjectPtr<AMyPlayerController> MyPlayerController : AllPlayerControllers)
	{
		AMyPlayerState* MyPlayerState = MyPlayerController->GetPlayerState<AMyPlayerState>();
		if (IsValid(MyPlayerState))
		{
			MyPlayerState->SetCurrentGuessCount(0);
		}
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	SecretNumberString = GenerateSecreteNumber();
	UE_LOG(LogTemp, Error, TEXT("%s"), *SecretNumberString);

	AMyGameStateBase* MyGS = GetGameState<AMyGameStateBase>();
	if (IsValid(MyGS))
	{
		MyGS->TurnIndex = GetRandomControllerIndex();
		MyGS->Notification = FString("Player") + FString::FromInt(MyGS->TurnIndex + 1) + FString(" can answer");
		MyGS->PlayerNum = AllPlayerControllers.Num();
		AMyPlayerController* PC = AllPlayerControllers[MyGS->TurnIndex];

		if (IsValid(PC))
		{
			AMyPlayerState* PS = PC->GetPlayerState<AMyPlayerState>();
			if (IsValid(PS))
			{
				PS->StartTurn();
			}
		}
	}
}

//void AMyGameModeBase::NextTurn()
//{
//	JudgeGame("", 0);
//}

void AMyGameModeBase::JudgeGame(FString PlayerName, int InStrikeCount)
{
	if (InStrikeCount == 3)
	{
		AMyGameStateBase* MyGameState = GetGameState<AMyGameStateBase>();
		if (IsValid(MyGameState))
		{
			MyGameState->Notification = PlayerName.Append(" has won the game");
		}
		ResetGame();
	}
	else
	{
		AMyGameStateBase* MyGS = GetGameState<AMyGameStateBase>();
		if (IsValid(MyGS))
		{
			MyGS->TurnIndex = (MyGS->TurnIndex + 1) % MyGS->PlayerNum;
			AMyPlayerState* MyPS = AllPlayerControllers[MyGS->TurnIndex]->GetPlayerState<AMyPlayerState>();
			if (IsValid(MyPS))
			{
				if (MyPS->GetCurrentGuessCount() >= MyPS->MaxGuessCount)
				{
					MyGS->Notification = "Draw";
					ResetGame();
					return;
				}

				MyGS->Notification = FString("Player") + FString::FromInt(MyGS->TurnIndex + 1) + FString(" can answer");
				MyPS->StartTurn();
			}
		}
		//for (const TObjectPtr<AMyPlayerController> MyPlayerController : AllPlayerControllers)
		//{
		//	AMyPlayerState* MyPlayerState = MyPlayerController->GetPlayerState<AMyPlayerState>();
		//	if (!IsValid(MyPlayerState))
		//		return;
		//	if (MyPlayerState->CurrentGuessCount < MyPlayerState->MaxGuessCount)
		//	{
		//		NextTurn();
		//		return;
		//	}
		//}
		//AMyGameStateBase* MyGameState = GetGameState<AMyGameStateBase>();
		//if (IsValid(MyGameState))
		//{
		//	MyGameState->Notification = "Draw";
		//	ResetGame();
		//}
	}
}

bool AMyGameModeBase::IsChanceLeft(AMyPlayerState* MyPlayerState)
{
	return MyPlayerState->GetCurrentGuessCount() < MyPlayerState->MaxGuessCount;
}

int32 AMyGameModeBase::GetRandomControllerIndex()
{
	return FMath::RandRange(0, AllPlayerControllers.Num() - 1);
}
