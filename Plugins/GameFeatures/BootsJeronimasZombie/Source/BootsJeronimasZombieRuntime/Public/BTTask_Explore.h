#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Explore.generated.h"

UCLASS()
class BOOTSJERONIMASZOMBIERUNTIME_API UBTTask_Explore : public UBTTaskNode
{
    GENERATED_BODY()
public:
    UBTTask_Explore();
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    // Tracks last known explored direction to avoid going back
    FVector LastExploreTarget = FVector::ZeroVector;
};