// Fill out your copyright notice in the Description page of Project Settings.


#include "Item_Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"

AItem_Weapon::AItem_Weapon() {
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());

	bWeaponParticle = false;

	WeaponState = EWeaponState::EWS_Pickup;

	Damage = 25.f;
}

// Called when the game starts or when spawned
void AItem_Weapon::BeginPlay()
{
	Super::BeginPlay();

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AItem_Weapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AItem_Weapon::CombatOnOverlapEnd);

}

void AItem_Weapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if ((WeaponState == EWeaponState::EWS_Pickup) && OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			Main->SetActiveOverlappingItem(this);
		}
	}

}

void AItem_Weapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (OtherActor) {
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main) {
			Main->SetActiveOverlappingItem(nullptr);
		}
	}
}


void AItem_Weapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	if (OtherActor) {
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy) {
			if (Enemy->HitParticles) {
				const USkeletalMeshSocket* Weapon = SkeletalMesh->GetSocketByName("WeaponSocket");
				if (Weapon) {
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, Weapon->GetSocketLocation(SkeletalMesh));
				}
			}
			if (Enemy->OnHitSound) {
				UGameplayStatics::PlaySound2D(this, Enemy->OnHitSound);
			}
			if (DamageTypeClass) {
				UGameplayStatics::ApplyDamage(Enemy, Damage, Enemy->GetController(), this, DamageTypeClass);
			}
		}
	}
}


void AItem_Weapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

}

void AItem_Weapon::Equip(AMain* Char) {
	if (Char) {
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		SkeletalMesh->SetSimulatePhysics(false);

		// Fetching the created socket
		const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");

		if (RightHandSocket) {
			RightHandSocket->AttachActor(this, Char->GetMesh());
			bRotate = false;
			Char->SetEquippedWeapon(this);
			Char->SetActiveOverlappingItem(nullptr);
			if (OnEquipSound && !Char->EquippedWeapon) {
				UGameplayStatics::PlaySound2D(this, OnEquipSound);
			}
		}
		if (!bWeaponParticle) {
			IdleParticlesComponent->Deactivate();
		}
	}
}