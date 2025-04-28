// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MurielAnimInstance.h"
#include "Character/MurielCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UMurielAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Muriel = Cast<AMurielCharacter>(TryGetPawnOwner());
	if (Muriel)
	{
		MurielCharacterMovement = Muriel->GetCharacterMovement();
	}
}

void UMurielAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (MurielCharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(MurielCharacterMovement->Velocity);

	}
}

