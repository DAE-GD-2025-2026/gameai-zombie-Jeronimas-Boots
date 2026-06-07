#include "BTTask_FleeFromZombieBootsJeronimas.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Zombies/BaseZombie.h"

UBTTask_FleeFromZombie::UBTTask_FleeFromZombie()
{
    NodeName = TEXT("Flee From Zombie");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_FleeFromZombie::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
    if (!AIController || !BBComp) return EBTNodeResult::Failed;

    APawn* Pawn = AIController->GetPawn();
    ABaseZombie* TargetZombie = Cast<ABaseZombie>(BBComp->GetValueAsObject(FName("TargetZombie")));
    if (!Pawn || !TargetZombie) return EBTNodeResult::Failed;

    // Calculate flee destination directly away from zombie
    FVector AwayFromZombie = Pawn->GetActorLocation() - TargetZombie->GetActorLocation();
    AwayFromZombie.Z = 0.f;
    AwayFromZombie.Normalize();

    FVector FleeDestination = Pawn->GetActorLocation() + AwayFromZombie * FleeDistance;

    // Project onto navmesh so we don't flee into a wall
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Pawn->GetWorld());
    if (NavSys)
    {
        FNavLocation ProjectedLocation;
        if (NavSys->ProjectPointToNavigation(FleeDestination, ProjectedLocation, FVector(200.f, 200.f, 200.f)))
        {
            FleeDestination = ProjectedLocation.Location;
        }
    }

    AIController->MoveToLocation(FleeDestination, 50.f);
    return EBTNodeResult::InProgress;
}

void UBTTask_FleeFromZombie::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
    if (!AIController || !BBComp)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    APawn* Pawn = AIController->GetPawn();
    ABaseZombie* TargetZombie = Cast<ABaseZombie>(BBComp->GetValueAsObject(FName("TargetZombie")));

    // Zombie gone done fleeing
    if (!IsValid(TargetZombie))
    {
        BBComp->ClearValue(FName("TargetZombie"));
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    // Check if we're far enough away
    float DistToZombie = FVector::Dist(Pawn->GetActorLocation(), TargetZombie->GetActorLocation());
    if (DistToZombie >= SafeDistance)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    // Movement finished but not safe yet recalculate flee direction
    if (AIController->GetMoveStatus() == EPathFollowingStatus::Type::Idle)
    {
        FVector AwayFromZombie = Pawn->GetActorLocation() - TargetZombie->GetActorLocation();
        AwayFromZombie.Z = 0.f;
        AwayFromZombie.Normalize();

        FVector FleeDestination = Pawn->GetActorLocation() + AwayFromZombie * FleeDistance;

        UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Pawn->GetWorld());
        if (NavSys)
        {
            FNavLocation ProjectedLocation;
            if (NavSys->ProjectPointToNavigation(FleeDestination, ProjectedLocation, FVector(200.f, 200.f, 200.f)))
            {
                FleeDestination = ProjectedLocation.Location;
            }
        }

        AIController->MoveToLocation(FleeDestination, 50.f);
    }
}

EBTNodeResult::Type UBTTask_FleeFromZombie::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController) AIController->StopMovement();
    return EBTNodeResult::Aborted;
}