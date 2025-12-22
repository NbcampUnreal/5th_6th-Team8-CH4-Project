// Fill out your copyright notice in the Description page of Project Settings.

#include "Interface/Door.h"
#include "Components/TimelineComponent.h"

ADoor::ADoor()
{
	//타임라인
	DoorTimeline = CreateDefaultSubobject<UTimelineComponent>("DoorTimeline");

	//처음은 문이 닫힌 상태
	bDoorOpen = false;
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	//타임라인
	UpdateFunctionFloat.BindDynamic(this, &ADoor::DoorOpenTimeLineFunc);
	if (DoorTimelineCurveFloat)
	{
		DoorTimeline->AddInterpFloat(DoorTimelineCurveFloat, UpdateFunctionFloat);
	}

	//시작부터 열린 문인가
	if (bDoorOpen)
	{
		float MinValue, MAxValue;
		DoorTimelineCurveFloat->GetValueRange(MinValue, MAxValue);
		DoorOpenTimeLineFunc(MinValue);
		StaticMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}

void ADoor::Interact_Implementation(AActor* Interactor)
{
	if (bDoorOpen)
	{
		DoorTimeline->Reverse();
		bDoorOpen = false;

		StaticMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	}
	else
	{
		//문을 열어라
		DoorTimeline->Play();
		bDoorOpen = true;

		//문을 열면 더이상 막히지 않음
		StaticMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}

}

void ADoor::DoorOpenTimeLineFunc(float Output)
{
	//문열림 연출을 위한 회전값 변경
	const FRotator Value = FRotator(0, Output, 0);
	StaticMesh->SetRelativeRotation(Value);
}
