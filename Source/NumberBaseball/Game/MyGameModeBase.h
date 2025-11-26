#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"

class AMyPlayerController;
class AMyPlayerState;

UCLASS()
class NUMBERBASEBALL_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecreteNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	void PrintChatMessageString(AMyPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void ResetGame();

	void StartGame();

	//void NextTurn();

	void JudgeGame(FString WinnerName, int InStrikeCount);

	bool IsChanceLeft(AMyPlayerState* MyPlayerState);

	int32 GetRandomControllerIndex();

protected:
	FString SecretNumberString;

	TArray<TObjectPtr<AMyPlayerController>> AllPlayerControllers;

	FTimerHandle WaitHandle;

	bool bIsAlreadyStart = false;
};