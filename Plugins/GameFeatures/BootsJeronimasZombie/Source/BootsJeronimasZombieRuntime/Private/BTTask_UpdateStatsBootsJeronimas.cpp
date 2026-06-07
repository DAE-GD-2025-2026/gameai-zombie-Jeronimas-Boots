#include "BTTask_UpdateStatsBootsJeronimas.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "Common/HealthComponent.h"
#include "Common/StaminaComponent.h"

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

    return EBTNodeResult::Succeeded;
}