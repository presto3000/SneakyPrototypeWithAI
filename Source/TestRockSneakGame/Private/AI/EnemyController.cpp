// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnemyController.h"

#include "AI/Enemy.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyController::AEnemyController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	//check is valid
	check(BlackboardComponent);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	check(BehaviorTreeComponent);
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

		if(InPawn == nullptr) return;
    
    	const AEnemy* Enemy = Cast<AEnemy>(InPawn);
    	if(Enemy)
    	{
    		if(Enemy->GetBehaviorTree())
    		{
    			// Initialize Blackboard
    			BlackboardComponent->InitializeBlackboard(*(Enemy->GetBehaviorTree()->BlackboardAsset));
    		}
    	}
}
