#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AIPerceptionTypes.h"
#include "Items/BaseItem.h"
#include "Village/House/House.h"
#include "Zombies/BaseZombie.h"
#include "PurgeZones/PurgeZone.h"
#include "StudentPerceptorBootsJeronimas.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BOOTSJERONIMASZOMBIERUNTIME_API UStudentPerceptor : public UActorComponent
{
	GENERATED_BODY()

public:
	UStudentPerceptor();
    
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	void RefreshBestItem(UBlackboardComponent* BBComp, APawn* OwnerPawn);
	void RemoveKnownItem(ABaseItem* Item) { KnownItems.Remove(Item); }
private:
	const FName Key_TargetZombie = FName("TargetZombie");
	const FName Key_NearestHouse = FName("NearestHouse");
	const FName Key_TargetItem   = FName("TargetItem");
	const FName Key_PurgeZone    = FName("PurgeZone");

	// Track all currently visible items
	UPROPERTY()
	TSet<ABaseItem*> KnownItems;

	//void UpdateClosestItem(UBlackboardComponent* BBComp) const;
	
	int32 GetItemPriority(ABaseItem* Item, float CurrentHealth);
};