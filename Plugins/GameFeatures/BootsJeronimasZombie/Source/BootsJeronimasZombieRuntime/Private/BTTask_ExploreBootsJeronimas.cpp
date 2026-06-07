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

    if (!bSpiralInitialized)
    {
        SpiralOrigin = Pawn->GetActorLocation();
        CurrentRadius = RadiusStep;
        CurrentAngle = 0.0f;
        StepCount = 0;
        bSpiralInitialized = true;
    }

    StepCount++;

    FVector TargetPoint;

    // Every N steps walk back behind the survivor briefly
    if (StepCount % BacktrackEveryNSteps == 0)
    {
        FVector Backward = -Pawn->GetActorForwardVector();
        TargetPoint = Pawn->GetActorLocation() + Backward * BacktrackDistance;
    }
    else
    {
        // Normal spiral advance
        CurrentAngle += AngleStep;
        if (CurrentAngle >= 360.0f)
        {
            CurrentAngle = 0.0f;
            CurrentRadius += RadiusStep;
        }

        if (CurrentRadius > MaxRadius)
        {
            SpiralOrigin = Pawn->GetActorLocation();
            CurrentRadius = RadiusStep;
            CurrentAngle = 0.0f;
        }

        TargetPoint = CalculateNextSpiralPoint();
    }

    FNavLocation NavLocation;
    bool bFound = NavSys->ProjectPointToNavigation(
        TargetPoint,
        NavLocation,
        FVector(200.0f, 200.0f, 200.0f)
    );

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

    if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}

EBTNodeResult::Type UBTTask_Explore::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController) AIController->StopMovement();
    return EBTNodeResult::Aborted;
}