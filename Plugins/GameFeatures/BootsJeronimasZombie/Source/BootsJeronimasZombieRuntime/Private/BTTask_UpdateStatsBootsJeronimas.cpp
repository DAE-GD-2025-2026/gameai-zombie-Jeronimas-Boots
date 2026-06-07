#include "BTTask_UpdateStatsBootsJeronimas.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "Common/HealthComponent.h"
#include "Common/StaminaComponent.h"
#include "Items/BaseItem.h"
#include "Common/InventoryComponent.h"

UBTTask_UpdateStats::UBTTask_UpdateStats()
{
    NodeName = TEXT("Update Stats");
}

EBTNodeResult::Type UBTTask_UpdateStats::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
    if (!AIController || !BBComp) return EBTNodeResult::Failed;

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn) return EBTNodeResult::Failed;

    UHealthComponent* Health = Pawn->GetComponentByClass<UHealthComponent>();
    UStaminaComponent* Stamina = Pawn->GetComponentByClass<UStaminaComponent>();
    UInventoryComponent* Inventory = Pawn->GetComponentByClass<UInventoryComponent>();

    if (Health)
    {
        float HealthPct = (float)Health->GetHealth() / (float)Health->GetMaxHealth() * 100.0f;
        BBComp->SetValueAsFloat(FName("Health"), HealthPct);
    }

    if (Stamina)
    {
        float StaminaPct = Stamina->GetCurrentStamina() / Stamina->GetMaxStamina() * 100.0f;
        BBComp->SetValueAsFloat(FName("Stamina"), StaminaPct);
    }

    if (Inventory) // guard it like the others
    {
        const TArray<ABaseItem*>& Items = Inventory->GetInventory();
        bool bHasWeapon = false;
        for (int32 i = 0; i < Items.Num(); ++i)
        {
            if (!IsValid(Items[i])) continue;
            EItemType Type = Items[i]->GetItemType();
            if ((Type == EItemType::Pistol || Type == EItemType::Shotgun)
                && Items[i]->GetValue() > 0)
            {
                bHasWeapon = true;
                break;
            }
        }
        BBComp->SetValueAsBool(FName("HasWeapon"), bHasWeapon);
    }

    return EBTNodeResult::Succeeded;
}