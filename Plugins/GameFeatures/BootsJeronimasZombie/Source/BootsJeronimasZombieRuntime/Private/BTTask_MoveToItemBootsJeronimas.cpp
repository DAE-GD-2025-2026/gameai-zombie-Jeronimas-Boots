#include "BTTask_MoveToItemBootsJeronimas.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Items/BaseItem.h"
#include "Common/InventoryComponent.h"

UBTTask_MoveToItem::UBTTask_MoveToItem()
{
    NodeName = TEXT("Move To Item");
    bNotifyTick = true;
}

// BTTask_MoveToItemBootsJeronimas.cpp

EBTNodeResult::Type UBTTask_MoveToItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
    if (!AIController || !BBComp) 
        return EBTNodeResult::Failed;

    ABaseItem* TargetItem = Cast<ABaseItem>(BBComp->GetValueAsObject(FName("TargetItem")));
    if (!TargetItem) 
        return EBTNodeResult::Failed;

    APawn* Pawn = AIController->GetPawn();
    UInventoryComponent* Inventory = Pawn ? Pawn->GetComponentByClass<UInventoryComponent>() : nullptr;
    
    float AcceptanceRadius = Inventory ? Inventory->GetPickupRange() : 100.0f;

    UE_LOG(LogTemp, Warning, TEXT("MoveToItem: START -> %s (acceptance=%.1f)"), 
        *TargetItem->GetName(), AcceptanceRadius);

    AIController->MoveToActor(TargetItem, AcceptanceRadius);
    return EBTNodeResult::InProgress;
}

void UBTTask_MoveToItem::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
    if (!AIController || !BBComp)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // Always succeed when movement stops — GrabItem handles the rest
    if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
    {
        UE_LOG(LogTemp, Warning, TEXT("MoveToItem: DONE"));
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}

EBTNodeResult::Type UBTTask_MoveToItem::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController) AIController->StopMovement();
    return EBTNodeResult::Aborted;
}