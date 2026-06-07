#include "BTTask_ShootZombieBootsJeronimas.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"
#include "Zombies/BaseZombie.h"

UBTTask_ShootZombie::UBTTask_ShootZombie()
{
    NodeName = TEXT("Shoot Zombie");
}

EBTNodeResult::Type UBTTask_ShootZombie::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
    if (!AIController || !BBComp) return EBTNodeResult::Failed;

    APawn* Pawn = AIController->GetPawn();
    ABaseZombie* TargetZombie = Cast<ABaseZombie>(BBComp->GetValueAsObject(FName("TargetZombie")));
    if (!Pawn || !TargetZombie) return EBTNodeResult::Failed;

    UInventoryComponent* Inventory = Pawn->GetComponentByClass<UInventoryComponent>();
    if (!Inventory) return EBTNodeResult::Failed;

    // Find a weapon slot with ammo
    const TArray<ABaseItem*>& Items = Inventory->GetInventory();
    int32 WeaponSlot = -1;
    for (int32 i = 0; i < Items.Num(); ++i)
    {
        if (!IsValid(Items[i])) continue;
        EItemType Type = Items[i]->GetItemType();
        if ((Type == EItemType::Pistol || Type == EItemType::Shotgun)
            && Items[i]->GetValue() > 0)
        {
            WeaponSlot = i;
            break;
        }
    }

    if (WeaponSlot == -1)
    {
        UE_LOG(LogTemp, Warning, TEXT("ShootZombie: No weapon with ammo found"));
        BBComp->SetValueAsBool(FName("HasWeapon"), false);
        return EBTNodeResult::Failed;
    }

    bool bShot = Inventory->UseItem(WeaponSlot);
    UE_LOG(LogTemp, Warning, TEXT("ShootZombie: slot=%d result=%s ammo_left=%.0f"),
        WeaponSlot,
        bShot ? TEXT("OK") : TEXT("FAIL"),
        IsValid(Items[WeaponSlot]) ? Items[WeaponSlot]->GetValue() : 0.f);

    // If weapon is now empty, update the blackboard flag
    if (!IsValid(Items[WeaponSlot]) || Items[WeaponSlot]->GetValue() <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("ShootZombie: Weapon empty"));
        BBComp->SetValueAsBool(FName("HasWeapon"), false);
    }

    return bShot ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}