// Fill out your copyright notice in the Description page of Project Settings.


#include "StudentPerceptor.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UStudentPerceptor::UStudentPerceptor()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStudentPerceptor::BeginPlay()
{
	Super::BeginPlay();
	
	if (auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptor::OnPerceptionUpdated);
	}
}

void UStudentPerceptor::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor) return;

	AAIController* AIController = Cast<AAIController>(GetOwner());
	if (!AIController) return;

	UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
	if (!BBComp) return;

	bool bIsSensed = Stimulus.WasSuccessfullySensed();

	// Use Tags to identify actors dynamically without hardcoding references
	if (Actor->ActorHasTag(FName("Zombie")))
	{
		if (bIsSensed)
		{
			BBComp->SetValueAsObject(Key_TargetZombie, Actor);
		}
		else if (BBComp->GetValueAsObject(Key_TargetZombie) == Actor)
		{
			BBComp->ClearValue(Key_TargetZombie);
		}
	}
	else if (Actor->ActorHasTag(FName("House")))
	{
		if (bIsSensed) BBComp->SetValueAsObject(Key_NearestHouse, Actor);
	}
	else if (Actor->ActorHasTag(FName("Item")))
	{
		if (bIsSensed) BBComp->SetValueAsObject(Key_TargetItem, Actor);
		else if (BBComp->GetValueAsObject(Key_TargetItem) == Actor) BBComp->ClearValue(Key_TargetItem);
	}
	else if (Actor->ActorHasTag(FName("PurgeZone")))
	{
		if (bIsSensed) BBComp->SetValueAsObject(Key_PurgeZone, Actor);
		else if (BBComp->GetValueAsObject(Key_PurgeZone) == Actor) BBComp->ClearValue(Key_PurgeZone);
	}
}
