#include "BTTask_ExploreBootsJeronimas.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/Pawn.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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

    // Disable movement component rotation so we can rotate freely
    UCharacterMovementComponent* MoveComp = Pawn->GetComponentByClass<UCharacterMovementComponent>();
    if (MoveComp)
    {
        MoveComp->bOrientRotationToMovement = false;
        MoveComp->bUseControllerDesiredRotation = false;
    }

    if (!bSpiralInitialized)
    {
        SpiralOrigin = Pawn->GetActorLocation();
        CurrentRadius = RadiusStep;
        CurrentAngle = 0.0f;
        bSpiralInitialized = true;
    }

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

    FVector TargetPoint = CalculateNextSpiralPoint();

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

    APawn* Pawn = AIController->GetPawn();
    UCharacterMovementComponent* MoveComp = Pawn ? Pawn->GetComponentByClass<UCharacterMovementComponent>() : nullptr;

    // Rotate pawn to scan surroundings while exploring
    if (Pawn)
    {
        FRotator CurrentRotation = Pawn->GetActorRotation();
        CurrentRotation.Yaw += ScanRotationSpeed * DeltaSeconds;
        Pawn->SetActorRotation(CurrentRotation);
    }

    if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
    {
        // Restore rotation before handing control back
        if (MoveComp)
        {
            MoveComp->bOrientRotationToMovement = true;
        }
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}

EBTNodeResult::Type UBTTask_Explore::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController)
    {
        APawn* Pawn = AIController->GetPawn();
        UCharacterMovementComponent* MoveComp = Pawn ? Pawn->GetComponentByClass<UCharacterMovementComponent>() : nullptr;
        if (MoveComp)
        {
            MoveComp->bOrientRotationToMovement = true;
        }
        AIController->StopMovement();
    }
    return EBTNodeResult::Aborted;
}