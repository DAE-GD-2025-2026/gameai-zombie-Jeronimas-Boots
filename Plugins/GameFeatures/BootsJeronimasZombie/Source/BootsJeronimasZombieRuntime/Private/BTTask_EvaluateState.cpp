#include "BTTask_EvaluateState.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"

UBTTask_EvaluateState::UBTTask_EvaluateState()
{
	NodeName = "Evaluate Survivor State";
}

EBTNodeResult::Type UBTTask_EvaluateState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	APawn* Pawn = AIController->GetPawn();
	if (!BBComp || !Pawn) return EBTNodeResult::Failed;

	// Note: Replace these dummy bools/floats with actual calls 
	// to your player's inventory or health components
	float CurrentHealthPct = 100.0f; // Example: GetHealth() / GetMaxHealth()
	bool bHasWeapon = false;         // Example: InventoryComp->HasWeapon()

	bool bZombieSpotted = BBComp->GetValueAsObject(FName("TargetZombie")) != nullptr;

	// Rule 1: Always prioritize health
	if (CurrentHealthPct < 30.0f)
	{
		BBComp->SetValueAsBool(FName("NeedsHealing"), true);
	}
	else
	{
		BBComp->SetValueAsBool(FName("NeedsHealing"), false);
	}

	// Rule 2: Evaluate Fight or Flight based on logic
	if (bZombieSpotted)
	{
		if (bHasWeapon && CurrentHealthPct > 30.0f)
		{
			BBComp->SetValueAsBool(FName("ShouldFight"), true);
			BBComp->SetValueAsBool(FName("ShouldFlee"), false);
		}
		else
		{
			BBComp->SetValueAsBool(FName("ShouldFight"), false);
			BBComp->SetValueAsBool(FName("ShouldFlee"), true);
		}
	}
	else
	{
		BBComp->SetValueAsBool(FName("ShouldFight"), false);
		BBComp->SetValueAsBool(FName("ShouldFlee"), false);
	}

	return EBTNodeResult::Succeeded;
}