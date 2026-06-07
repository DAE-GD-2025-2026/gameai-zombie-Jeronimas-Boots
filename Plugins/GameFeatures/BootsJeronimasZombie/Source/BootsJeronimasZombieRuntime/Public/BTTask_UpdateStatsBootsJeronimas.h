#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_UpdateStatsBootsJeronimas.generated.h"

UCLASS()
class BOOTSJERONIMASZOMBIERUNTIME_API UBTTask_UpdateStats : public UBTTaskNode
{
    GENERATED_BODY()
public:
    UBTTask_UpdateStats();
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};