#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_UseConsumableBootsJeronimas.generated.h"

UCLASS()
class BOOTSJERONIMASZOMBIERUNTIME_API UBTTask_UseConsumable : public UBTTaskNode
{
    GENERATED_BODY()
public:
    UBTTask_UseConsumable();
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};