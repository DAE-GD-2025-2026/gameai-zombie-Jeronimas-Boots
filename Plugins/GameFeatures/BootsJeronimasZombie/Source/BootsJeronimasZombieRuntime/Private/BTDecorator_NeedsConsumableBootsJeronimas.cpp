#include "BTDecorator_NeedsConsumableBootsJeronimas.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"

UBTDecorator_NeedsConsumable::UBTDecorator_NeedsConsumable()
{
    NodeName = TEXT("Needs Consumable");
}

bool UBTDecorator_NeedsConsumable::CalculateRawConditionValue(
    UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!BBComp || !AIController) return false;

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn) return false;

    float HealthPct = BBComp->GetValueAsFloat(FName("Health"));
    float StaminaPct = BBComp->GetValueAsFloat(FName("Stamina"));

    bool bNeedsHealth = HealthPct < HealthThreshold;
    bool bNeedsStamina = StaminaPct < StaminaThreshold;

    if (!bNeedsHealth && !bNeedsStamina) return false;

    // Check we actually have something to use
    UInventoryComponent* Inventory = Pawn->GetComponentByClass<UInventoryComponent>();
    if (!Inventory) return false;

    for (ABaseItem* Item : Inventory->GetInventory())
    {
        if (!Item) continue;
        if (bNeedsHealth && Item->GetItemType() == EItemType::Medkit) return true;
        if (bNeedsStamina && Item->GetItemType() == EItemType::Food) return true;
    }

    return false;
}