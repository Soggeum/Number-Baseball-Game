#include "Game/MyGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MyPlayerController.h"
#include "Net/UnrealNetwork.h"

void AMyGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (!HasAuthority())
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

		if (IsValid(PC))
		{
			AMyPlayerController* MyPC = Cast<AMyPlayerController>(PC);
			if (IsValid(MyPC))
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				MyPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}

AMyGameStateBase::AMyGameStateBase() : TurnIndex(-1), PlayerNum(0)
{
	bReplicates = true;
}

void AMyGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyGameStateBase, Notification);
	DOREPLIFETIME(AMyGameStateBase, TurnIndex);
}

void AMyGameStateBase::OnRep_Winner()
{
	OnNotification.Broadcast(Notification);
}
