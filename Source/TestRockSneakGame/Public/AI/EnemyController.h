// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class TESTROCKSNEAKGAME_API AEnemyController : public AAIController
{
	GENERATED_BODY()
public:
	AEnemyController();
	virtual void OnPossess(APawn* InPawn) override;

private:
	/** Blackboard component for this enemy */
    UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
    UBlackboardComponent* BlackboardComponent;
    
    /** Behavior tree component for this enemy */
    UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
    UBehaviorTreeComponent* BehaviorTreeComponent;
    
public:
    FORCEINLINE UBlackboardComponent* GetBlackboardComponentFromEnemyController() const  {return BlackboardComponent; }
};
