#include "BTTask_FaceZombieBootsJeronimas.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "Zombies/BaseZombie.h"

UBTTask_FaceZombie::UBTTask_FaceZombie()
{
    NodeName = TEXT("Face Zombie");
}

EBTNodeResult::Type UBTTask_FaceZombie::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
    if (!AIController || !BBComp) return EBTNodeResult::Failed;

    APawn* Pawn = AIController->GetPawn();
    ABaseZombie* TargetZombie = Cast<ABaseZombie>(BBComp->GetValueAsObject(FName("TargetZombie")));
    if (!Pawn || !TargetZombie) return EBTNodeResult::Failed;

    // Get direction to zombie on the horizontal plane only
    FVector ToZombie = TargetZombie->GetActorLocation() - Pawn->GetActorLocation();
    ToZombie.Z = 0.f;
    ToZombie.Normalize();

    FRotator NewRotation = ToZombie.Rotation();
    Pawn->SetActorRotation(NewRotation);

    return EBTNodeResult::Succeeded;
}