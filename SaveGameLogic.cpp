// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGameLogic.h"

USaveGameLogic::USaveGameLogic() {
	PlayerName = TEXT("Default");
	UserIndex = 0;

	CharacterStats.WeaponName = TEXT("");
	CharacterStats.LevelName = TEXT("");
}