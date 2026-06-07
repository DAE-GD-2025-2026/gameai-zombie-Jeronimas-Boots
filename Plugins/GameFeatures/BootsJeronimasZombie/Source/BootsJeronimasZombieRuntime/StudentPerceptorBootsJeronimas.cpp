#include "StudentPerceptorBootsJeronimas.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Items/BaseItem.h"
#include "Village/House/House.h"
#include "Zombies/BaseZombie.h"
#include "PurgeZones/PurgeZone.h"
#include "Common/InventoryComponent.h"

UStudentPerceptor::UStudentPerceptor()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UStudentPerceptor::BeginPlay()
{
    Super::BeginPlay();
    
    if (auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>())
    {
        PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptor::OnPerceptionUpdated);
    }
}

void UStudentPerceptor::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor) return;

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn) return;

    AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
    if (!AIController) return;

    UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
    if (!BBComp) return;

    bool bIsSensed = Stimulus.WasSuccessfullySensed();

    if (Actor->IsA<ABaseZombie>())
    {
        if (bIsSensed)
            BBComp->SetValueAsObject(Key_TargetZombie, Actor);
        else if (BBComp->GetValueAsObject(Key_TargetZombie) == Actor)
            BBComp->ClearValue(Key_TargetZombie);
    }
    else if (Actor->IsA<AHouse>())
    {
        if (bIsSensed)
            BBComp->SetValueAsObject(Key_NearestHouse, Actor);
        else if (BBComp->GetValueAsObject(Key_NearestHouse) == Actor)
            BBComp->ClearValue(Key_NearestHouse);
    }
    else if (Actor->IsA<ABaseItem>())
    {
        ABaseItem* Item = Cast<ABaseItem>(Actor);
        if (bIsSensed)
        {
            KnownItems.Add(Item);
            RefreshBestItem(BBComp, OwnerPawn);
        }
        else
        {
            // Only forget if the actor is actually destroyed
            // losing perception sight on a static item means nothing
            if (!IsValid(Item))
            {
                KnownItems.Remove(Item);

                if (BBComp->GetValueAsObject(Key_TargetItem) == Actor)
                {
                    BBComp->ClearValue(Key_TargetItem);
                    RefreshBestItem(BBComp, OwnerPawn);
                }
            }
        }
    }
    else if (Actor->IsA<APurgeZone>())
    {
        if (bIsSensed)
            BBComp->SetValueAsObject(Key_PurgeZone, Actor);
        else if (BBComp->GetValueAsObject(Key_PurgeZone) == Actor)
            BBComp->ClearValue(Key_PurgeZone);
    }
}

void UStudentPerceptor::RefreshBestItem(UBlackboardComponent* BBComp, APawn* OwnerPawn)
{
    // Check if inventory has any free or garbage slots worth targeting
    UInventoryComponent* Inventory = OwnerPawn->GetComponentByClass<UInventoryComponent>();
    if (Inventory)
    {
        const TArray<ABaseItem*>& InvItems = Inventory->GetInventory();
        bool bHasFreeSlot = false;
        for (int32 i = 0; i < InvItems.Num(); ++i)
        {
            if (!IsValid(InvItems[i]) || InvItems[i]->GetItemType() == EItemType::Garbage)
            {
                bHasFreeSlot = true;
                break;
            }
        }

        if (!bHasFreeSlot)
        {
            // Inventory full with useful items — no point targeting anything
            BBComp->ClearValue(Key_TargetItem);
            return;
        }
    }

    // Purge any items that have been picked up / destroyed
    TArray<ABaseItem*> ToRemove;
    for (ABaseItem* Item : KnownItems)
    {
        if (!IsValid(Item)) ToRemove.Add(Item);
    }
    for (ABaseItem* Item : ToRemove) KnownItems.Remove(Item);

    ABaseItem* CurrentTarget = Cast<ABaseItem>(BBComp->GetValueAsObject(Key_TargetItem));
    
    if (IsValid(CurrentTarget) 
        && KnownItems.Contains(CurrentTarget)
        && CurrentTarget->GetItemType() != EItemType::Garbage)
    {
        return;
    }

    ABaseItem* BestItem = nullptr;
    int32 BestPriority = -1;
    float BestDist = FLT_MAX;

    float CurrentHealth = BBComp->GetValueAsFloat(FName("Health"));

    for (ABaseItem* Item : KnownItems)
    {
        if (!IsValid(Item)) continue;

        int32 Priority = GetItemPriority(Item, CurrentHealth);
        if (Priority < 0) continue;

        float Dist = FVector::Dist(OwnerPawn->GetActorLocation(), Item->GetActorLocation());

        if (Priority > BestPriority || (Priority == BestPriority && Dist < BestDist))
        {
            BestPriority = Priority;
            BestDist = Dist;
            BestItem = Item;
        }
    }

    AActor* CurrentBB = Cast<AActor>(BBComp->GetValueAsObject(Key_TargetItem));
    if (BestItem != CurrentBB)
    {
        if (BestItem)
            BBComp->SetValueAsObject(Key_TargetItem, BestItem);
        else
            BBComp->ClearValue(Key_TargetItem);
    }
}

int32 UStudentPerceptor::GetItemPriority(ABaseItem* Item, float CurrentHealth)
{
    switch (Item->GetItemType())
    {
    case EItemType::Medkit:
        return (CurrentHealth < 40.f) ? 4 : 2;
    case EItemType::Pistol:
    case EItemType::Shotgun:
        return 3;
    case EItemType::Food:
        return 1;
    case EItemType::Garbage:
        return 0;  // lowest but still valid — need to clear spawn slots
    default:
        return 0;
    }
}