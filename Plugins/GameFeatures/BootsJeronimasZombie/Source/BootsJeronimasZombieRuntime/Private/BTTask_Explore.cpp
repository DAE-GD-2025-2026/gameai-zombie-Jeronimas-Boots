#include "BTTask_Explore.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/Pawn.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_Explore::UBTTask_Explore()
{
    NodeName = TEXT("Explore");
}

EBTNodeResult::Type UBTTask_Explore::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn) return EBTNodeResult::Failed;

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Pawn->GetWorld());
    if (!NavSys) return EBTNodeResult::Failed;

    // Pick a random reachable point on the navmesh to explore
    FNavLocation RandomLocation;
    bool bFound = NavSys->GetRandomReachablePointInRadius(
        Pawn->GetActorLocation(),
        2000.0f,
        RandomLocation
    );

    if (!bFound) return EBTNodeResult::Failed;

    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalLocation(RandomLocation.Location);
    MoveRequest.SetAcceptanceRadius(100.0f);

    AIController->MoveTo(MoveRequest);
    return EBTNodeResult::Succeeded;
}