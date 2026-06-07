#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ShootZombieBootsJeronimas.generated.h"

// BTTask_ShootZombieBootsJeronimas.h
UCLASS()
class BOOTSJERONIMASZOMBIERUNTIME_API UBTTask_ShootZombie : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_ShootZombie();
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    UPROPERTY(EditAnywhere, Category = "Combat")
    float PistolRange = 800.f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float ShotgunRange = 350.f;
};