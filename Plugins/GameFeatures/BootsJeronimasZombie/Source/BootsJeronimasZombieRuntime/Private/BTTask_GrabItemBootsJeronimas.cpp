#include "BTTask_GrabItemBootsJeronimas.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"
#include "BootsJeronimasZombieRuntime/StudentPerceptorBootsJeronimas.h"

UBTTask_GrabItem::UBTTask_GrabItem()
{
    NodeName = TEXT("Grab Item");
}

EBTNodeResult::Type UBTTask_GrabItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
    if (!AIController || !BBComp) return EBTNodeResult::Failed;

    APawn* Pawn = AIController->GetPawn();
    ABaseItem* TargetItem = Cast<ABaseItem>(BBComp->GetValueAsObject(FName("TargetItem")));
    if (!Pawn || !TargetItem) return EBTNodeResult::Failed;

    UInventoryComponent* Inventory = Pawn->GetComponentByClass<UInventoryComponent>();
    if (!Inventory) return EBTNodeResult::Failed;

    // Find any free slot
    const TArray<ABaseItem*>& Items = Inventory->GetInventory();
    int32 FreeSlot = -1;
    for (int32 i = 0; i < Items.Num(); ++i)
    {
        if (Items[i] == nullptr) { FreeSlot = i; break; }
    }

    if (FreeSlot == -1)
    {
        BBComp->ClearValue(FName("TargetItem"));
        return EBTNodeResult::Failed;
    }

    float Dist = FVector::Dist(Pawn->GetActorLocation(), TargetItem->GetActorLocation());
    float Range = Inventory->GetPickupRange();

    bool bGrabbed = Inventory->GrabItem(FreeSlot, TargetItem);

    if (!bGrabbed)
    {
        // Don't give up — remove this item from known items and try the next one
        UStudentPerceptor* Perceptor = Pawn->GetComponentByClass<UStudentPerceptor>();
        if (Perceptor)
        {
            Perceptor->RemoveKnownItem(TargetItem);
            BBComp->ClearValue(FName("TargetItem"));
            Perceptor->RefreshBestItem(BBComp, Pawn);
        }
        else
        {
            BBComp->ClearValue(FName("TargetItem"));
        }

        return BBComp->GetValueAsObject(FName("TargetItem")) != nullptr 
            ? EBTNodeResult::Succeeded 
            : EBTNodeResult::Failed;
    }

    EItemType Type = TargetItem->GetItemType();
    if (Type == EItemType::Garbage)
        Inventory->RemoveItem(FreeSlot);

    BBComp->ClearValue(FName("TargetItem"));
    return EBTNodeResult::Succeeded;
}