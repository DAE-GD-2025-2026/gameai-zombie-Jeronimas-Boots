#include "BTTask_AttackZombie.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Survivor/SurvivorPawn.h"

UBTTask_AttackZombie::UBTTask_AttackZombie()
{
	NodeName = TEXT("Attack Target Zombie");
}

EBTNodeResult::Type UBTTask_AttackZombie::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();

	if (!AIController || !BBComp) return EBTNodeResult::Failed;

	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(AIController->GetPawn());
	AActor* TargetZombie = Cast<AActor>(BBComp->GetValueAsObject(FName("TargetZombie")));

	if (Survivor && TargetZombie)
	{
		// 1. Calculate direction to TargetZombie
		FVector Direction = TargetZombie->GetActorLocation() - Survivor->GetActorLocation();
		Direction.Normalize();

		// Set the rotation of the AI to face the zombie
		Survivor->SetActorRotation(Direction.Rotation());

		// 2. Here you would normally tell your Inventory or Weapon to fire.
		// For example if you had a function: Survivor->ShootWeapon(Direction);
		// Since we don't have the exact Inventory fire method in view, you can stub this:
		// UE_LOG(LogTemp, Warning, TEXT("Pew Pew at Zombie!"));

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}