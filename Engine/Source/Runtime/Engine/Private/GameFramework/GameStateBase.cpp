#include "GameFramework/GameStateBase.h"
#include "Engine/World.h"
#include "Engine/Level.h"

void AGameStateBase::HandleBeginPlay()
{
	bReplicatedHasBegunPlay = true;

	// GetWorldSettings()->NotifyBeginPlay();
	// GetWorldSettings()->NotifyMatchStarted();
	// 위 로직 대체
	UWorld* World = GetWorld();
	if (!World->GetBegunPlay())
	{
		for (TEnginePtr<AActor> It : World->PersistentLevel->Actors)
		{
			It->DispatchBeginPlay();
		}

		World->SetBegunPlay(true);
	}
}
