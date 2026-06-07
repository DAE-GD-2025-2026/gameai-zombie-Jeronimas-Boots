#include "BTTask_UseConsumableBootsJeronimas.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"

UBTTask_UseConsumable::UBTTask_UseConsumable()
{
    NodeName = TEXT("Use Consumable");
}

EBTNodeResult::Type UBTTask_UseConsumable::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
    if (!AIController || !BBComp) return EBTNodeResult::Failed;

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn) return EBTNodeResult::Failed;

    UInventoryComponent* Inventory = Pawn->GetComponentByClass<UInventoryComponent>();
    if (!Inventory) return EBTNodeResult::Failed;

    float HealthPct = BBComp->GetValueAsFloat(FName("Health"));
    float StaminaPct = BBComp->GetValueAsFloat(FName("Stamina"));

    const TArray<ABaseItem*>& Items = Inventory->GetInventory();

    // Priority: use medkit if health low, food if stamina low
    // Medkit first since health is more critical than stamina
    if (HealthPct < 40.0f)
    {
        for (int32 i = 0; i < Items.Num(); ++i)
        {
            if (!Items[i]) continue;
            if (Items[i]->GetItemType() == EItemType::Medkit)
            {
                Inventory->UseItem(i);
                Inventory->RemoveItem(i);
                return EBTNodeResult::Succeeded;
            }
        }
    }

    if (StaminaPct < 30.0f)
    {
        for (int32 i = 0; i < Items.Num(); ++i)
        {
            if (!Items[i]) continue;
            if (Items[i]->GetItemType() == EItemType::Food)
            {
                Inventory->UseItem(i);
                Inventory->RemoveItem(i);
                return EBTNodeResult::Succeeded;
            }
        }
    }

    // Nothing to use or not needed
    return EBTNodeResult::Failed;
}