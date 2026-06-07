#include "BTTask_ExploreBootsJeronimas.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/Pawn.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_Explore::UBTTask_Explore()
{
    NodeName = TEXT("Explore Spiral");
    bNotifyTick = true;
}

FVector UBTTask_Explore::CalculateNextSpiralPoint() const
{
    // Convert polar coordinates to cartesian
    float RadAngle = FMath::DegreesToRadians(CurrentAngle);
    return SpiralOrigin + FVector(
        CurrentRadius * FMath::Cos(RadAngle),
        CurrentRadius * FMath::Sin(RadAngle),
        0.0f
    );
}

EBTNodeResult::Type UBTTask_Explore::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn) return EBTNodeResult::Failed;

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Pawn->GetWorld());
    if (!NavSys) return EBTNodeResult::Failed;

    // Initialize spiral origin at first run from agent's starting position
    if (!bSpiralInitialized)
    {
        SpiralOrigin = Pawn->GetActorLocation();
        CurrentRadius = RadiusStep;
        CurrentAngle = 0.0f;
        bSpiralInitialized = true;
    }

    // Advance the spiral
    CurrentAngle += AngleStep;

    // Once we've done a full circle, expand the radius
    if (CurrentAngle >= 360.0f)
    {
        CurrentAngle = 0.0f;
        CurrentRadius += RadiusStep;
    }

    // Reset spiral from current position if we've gone too far out
    if (CurrentRadius > MaxRadius)
    {
        SpiralOrigin = Pawn->GetActorLocation();
        CurrentRadius = RadiusStep;
        CurrentAngle = 0.0f;
    }

    // Calculate the ideal spiral point
    FVector TargetPoint = CalculateNextSpiralPoint();

    // Snap to navmesh — the ideal point may not be walkable
    FNavLocation NavLocation;
    bool bFound = NavSys->ProjectPointToNavigation(
        TargetPoint,
        NavLocation,
        FVector(200.0f, 200.0f, 200.0f)  // search extent
    );

    // Fall back to random reachable point if spiral point is off navmesh
    if (!bFound)
    {
        bFound = NavSys->GetRandomReachablePointInRadius(
            Pawn->GetActorLocation(),
            CurrentRadius,
            NavLocation
        );
    }

    if (!bFound) return EBTNodeResult::Failed;

    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalLocation(NavLocation.Location);
    MoveRequest.SetAcceptanceRadius(100.0f);

    FPathFollowingRequestResult Result = AIController->MoveTo(MoveRequest);

    if (Result.Code == EPathFollowingRequestResult::RequestSuccessful ||
        Result.Code == EPathFollowingRequestResult::AlreadyAtGoal)
    {
        return EBTNodeResult::InProgress;
    }

    return EBTNodeResult::Failed;
}

void UBTTask_Explore::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // When movement finishes, let BT restart the task to pick next spiral point
    EPathFollowingStatus::Type MoveStatus = AIController->GetMoveStatus();
    if (MoveStatus == EPathFollowingStatus::Idle)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}