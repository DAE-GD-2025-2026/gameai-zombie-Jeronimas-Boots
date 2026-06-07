#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_NeedsConsumableBootsJeronimas.generated.h"

UCLASS()
class BOOTSJERONIMASZOMBIERUNTIME_API UBTDecorator_NeedsConsumable : public UBTDecorator
{
    GENERATED_BODY()
public:
    UBTDecorator_NeedsConsumable();

protected:
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

private:
    UPROPERTY(EditAnywhere, Category = "Condition")
    float HealthThreshold = 40.0f;

    UPROPERTY(EditAnywhere, Category = "Condition")
    float StaminaThreshold = 30.0f;
};