#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EvaluateState.generated.h"

UCLASS()
class BOOTSJERONIMASZOMBIERUNTIME_API UBTTask_EvaluateState : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_EvaluateState();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};