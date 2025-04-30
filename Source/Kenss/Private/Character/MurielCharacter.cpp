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

    }

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
    }

}

