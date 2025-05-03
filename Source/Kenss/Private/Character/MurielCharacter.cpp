// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MurielCharacter.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Item/Item.h"
#include "Item/Weapons/Weapon.h"

#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"

// Sets default values
AMurielCharacter::AMurielCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 350.f, 0.f);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = 300.f;

    ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
    ViewCamera->SetupAttachment(CameraBoom);

}

// Called when the game starts or when spawned
void AMurielCharacter::BeginPlay()
{
	Super::BeginPlay();

    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(MurielContext, 0);
        }
    }
	
}

void AMurielCharacter::Move(const FInputActionValue& Value)
{

    if (ActionState != EActionState::EAS_Unoccupied) return;
    const FVector2D MovementVector = Value.Get<FVector2D>();

    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    AddMovementInput(ForwardDirection, MovementVector.Y);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
    AddMovementInput(RightDirection, MovementVector.X);

}

void AMurielCharacter::Look(const FInputActionValue& Value)
{

    const FVector2D LookAxisvector = Value.Get<FVector2D>();

    AddControllerPitchInput(LookAxisvector.Y);
    AddControllerYawInput(LookAxisvector.X);
}

void AMurielCharacter::EKeyPressed()
{
    AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
    if (OverlappingWeapon)
    {
        OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"));
        CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
        OverlappingItem = nullptr;
		EquippedWeapon = OverlappingWeapon;

    }

    else
    {
        if (CanUnequip())
        {
            PlayEquipMontage(FName("UnEuip"));
            CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;
        }
        else if (CanEquip())
        {
            PlayEquipMontage(FName("Equip"));
            CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
            ActionState = EActionState::EAS_EquippingWeapon;
        }


    }

}

void AMurielCharacter::Attack()
{
    if (CanAttack())
    {
        PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
    }

}

bool AMurielCharacter::CanAttack()
{
    return ActionState == EActionState::EAS_Unoccupied &&
           CharacterState != ECharacterState::ECS_Unequipped;
}



bool AMurielCharacter::CanUnequip()
{
    return ActionState == EActionState::EAS_Unoccupied &&
        CharacterState != ECharacterState::ECS_Unequipped;;
}

bool AMurielCharacter::CanEquip()
{
    return ActionState == EActionState::EAS_Unoccupied &&
        CharacterState == ECharacterState::ECS_Unequipped &&
        EquippedWeapon;
}

void AMurielCharacter::UnEquip()
{
    if (EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("Spine_Socket"));
    }

}

void AMurielCharacter::Equip()
{
    if (EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
    }

}

void AMurielCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;

}



void AMurielCharacter::PlayAttackMontage()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && AttackMontage)
    {
        AnimInstance->Montage_Play(AttackMontage);
        const int32 Selection = FMath::RandRange(0, 1);
        FName SectioName = FName();
        switch (Selection)
        {
        case 0:
            SectioName = FName("Attack1");
            break;
        case 1:
            SectioName = FName("Attack2");
            break;
        default:
            break;
        }
        AnimInstance->Montage_JumpToSection(SectioName, AttackMontage);
    }

}

void AMurielCharacter::PlayEquipMontage(FName SectionName)
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && EquipMontage)
    {
        AnimInstance->Montage_Play(EquipMontage);
        AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);

    }

}

void AMurielCharacter::AttackEnd()
{
    ActionState = EActionState::EAS_Unoccupied;
}




// Called every frame
void AMurielCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMurielCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputcomponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputcomponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AMurielCharacter::Move);
        EnhancedInputcomponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMurielCharacter::Look);

        PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
        PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &AMurielCharacter::EKeyPressed);
        PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &AMurielCharacter::Attack);
    }

}

void AMurielCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
    if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
    {
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
    }
}

