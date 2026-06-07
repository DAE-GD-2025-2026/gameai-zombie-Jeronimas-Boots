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
        BBComp->SetValueAsBool(FName("HasWeapon"), false);
        return EBTNodeResult::Failed;
    }

    // Check weapon-specific range before shooting
    EItemType WeaponType = Items[WeaponSlot]->GetItemType();
    float DistToZombie = FVector::Dist(Pawn->GetActorLocation(), TargetZombie->GetActorLocation());

    float MaxRange = (WeaponType == EItemType::Shotgun) ? ShotgunRange : PistolRange;

    if (DistToZombie > MaxRange)
    {
        // Too far — not a failure, just not ready to shoot yet
        // BT will re-evaluate next tick via FaceZombie + ShootZombie loop
        return EBTNodeResult::Failed;
    }

    bool bShot = Inventory->UseItem(WeaponSlot);

    if (!IsValid(Items[WeaponSlot]) || Items[WeaponSlot]->GetValue() <= 0)
    {
        Inventory->RemoveItem(WeaponSlot);
        BBComp->SetValueAsBool(FName("HasWeapon"), false);
    }

    return bShot ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}