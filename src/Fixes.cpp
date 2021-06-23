#include "Fixes.h"
#include "Settings.h"

void Fixes::Fix()
{
	const auto settings = Settings::GetSingleton();

	logger::info("{:*^30}", "FIXES"sv);

	if (settings->queuedRefCrash) {
		QueuedRefCrash::Fix();
		logger::info("Installed queued ref crash fix"sv);
	}
	if (settings->mapMarker) {
		MapMarker::Fix();
		logger::info("Installed map marker placement fix"sv);
	}
	if (settings->dontTakeBookFlag) {
		CantTakeBook::Fix();
		logger::info("Installed 'Can't Be Taken' book flag fix"sv);
	}
	if (settings->projectileRange) {
		ProjectileRange::Fix();
		logger::info("Installed projectile range fix"sv);
	}
	if (settings->combatDialogue) {
		CombatDialogue::Fix();
		logger::info("Installed combat dialogue fix"sv);
	}
	if (settings->addedSpell) {
		Spells::ReapplyAdded::Fix();
		Spells::DispelAdded::Fix();
		logger::info("Installed added spell reapply fix"sv);
	}
	if (settings->deathSpell) {
		Spells::ReapplyOnDeath::Fix();
		logger::info("Installed no death dispel spell reapply fix"sv);
	}
	if (settings->furnitureAnimType) {
		IsFurnitureAnimType::Fix();
		logger::info("Installed IsFurnitureAnimType fix"sv);
	}
	if (settings->lightAttachCrash) {
		AttachLightCrash::Fix();
		logger::info("Installed light attach crash fix"sv);
	}	
}
