#include "BTTask_UseItem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Items/BaseItem.h"
#include "Survivor/SurvivorPawn.h"

UBTTask_UseItem::UBTTask_UseItem()
{
	NodeName = TEXT("Use Target Item");
}

EBTNodeResult::Type UBTTask_UseItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();

	if (!AIController || !BBComp) return EBTNodeResult::Failed;

	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(AIController->GetPawn());
	ABaseItem* TargetItem = Cast<ABaseItem>(BBComp->GetValueAsObject(FName("TargetItem")));

	if (Survivor && TargetItem)
	{
		// Use the item via the base implementation in GameAI_Zombie.
		TargetItem->UseItem(*Survivor);

		// Clear it from our memory so we don't try to reuse it endlessly
		BBComp->ClearValue(FName("TargetItem"));
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}