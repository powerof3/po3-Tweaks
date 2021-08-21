#include "Fixes.h"

void Fixes::Install()
{
	const auto fixes = Settings::GetSingleton()->fixes;

	logger::info("{:*^30}", "FIXES"sv);

	if (fixes.queuedRefCrash) {
		QueuedRefCrash::Install();
	}
	if (fixes.mapMarker) {
		MapMarker::Install();
	}
	if (fixes.dontTakeBookFlag) {
		CantTakeBook::Install();
	}
	if (fixes.projectileRange) {
		ProjectileRange::Install();
	}
	if (fixes.combatDialogue) {
		CombatDialogue::Install();
	}
	if (fixes.addedSpell) {
		Spells::ReapplyAdded::Install();
		Spells::DispelAdded::Install();
	}
	if (fixes.deathSpell) {
		Spells::ReapplyOnDeath::Install();
	}
	if (fixes.furnitureAnimType) {
		IsFurnitureAnimTypeFix::Install();
	}
	if (fixes.lightAttachCrash) {
		AttachLightCrash::Install();
	}	
}
