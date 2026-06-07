#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "BTTask_ExploreBootsJeronimas.generated.h"

UCLASS()
class BOOTSJERONIMASZOMBIERUNTIME_API UBTTask_Explore : public UBTTaskNode
{
    GENERATED_BODY()
public:
    UBTTask_Explore();
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    FVector CalculateNextSpiralPoint() const;

    FVector SpiralOrigin = FVector::ZeroVector;
    float CurrentRadius = 0.0f;
    float CurrentAngle = 0.0f;
    bool bSpiralInitialized = false;

    // Every N steps, take a step back
    int32 StepCount = 0;

    UPROPERTY(EditAnywhere, Category="Explore")
    float RadiusStep = 400.0f;
    UPROPERTY(EditAnywhere, Category="Explore")
    float AngleStep = 60.0f;
    UPROPERTY(EditAnywhere, Category="Explore")
    float MaxRadius = 5000.0f;
    UPROPERTY(EditAnywhere, Category="Explore")
    float BacktrackDistance = 120.0f;
    UPROPERTY(EditAnywhere, Category="Explore")
    int32 BacktrackEveryNSteps = 3;
};