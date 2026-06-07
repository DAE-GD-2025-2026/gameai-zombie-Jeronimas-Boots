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

private:
    // Spiral state
    FVector SpiralOrigin = FVector::ZeroVector;
    float CurrentRadius = 0.0f;
    float CurrentAngle = 0.0f;

    // How much to grow the radius each step
    UPROPERTY(EditAnywhere, Category = "Explore")
    float RadiusStep = 400.0f;

    // How many points per ring before expanding
    UPROPERTY(EditAnywhere, Category = "Explore")
    float AngleStep = 60.0f;

    // Max radius before resetting spiral from new origin
    UPROPERTY(EditAnywhere, Category = "Explore")
    float MaxRadius = 5000.0f;

    bool bSpiralInitialized = false;

    FVector CalculateNextSpiralPoint() const;
};