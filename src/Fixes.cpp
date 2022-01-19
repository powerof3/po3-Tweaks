#include "Fixes.h"

void Fixes::Install(std::uint32_t skse_version)
{
	const auto fixes = Settings::GetSingleton()->fixes;

	logger::info("{:*^30}", "FIXES"sv);

	if (fixes.queuedRefCrash.value) {
		QueuedRefCrash::Install();
	}
	if (fixes.mapMarker.value) {
		if (GetModuleHandle(L"DisableFastTravel"))
			logger::info("Detected DisableFastTravel, skipping mapMarker fix."sv);
		else
			MapMarker::Install();
	}
	if (fixes.dontTakeBookFlag.value) {
		CantTakeBook::Install();
	}
	if (fixes.projectileRange.value) {
		ProjectileRange::Install();
	}
	if (fixes.combatDialogue.value) {
		CombatDialogue::Install();
	}
	if (fixes.addedSpell.value) {
		Spells::ReapplyAdded::Install();
		Spells::DispelAdded::Install();
	}
	if (fixes.deathSpell.value) {
		Spells::ReapplyOnDeath::Install();
	}
	if (fixes.furnitureAnimType.value) {
		IsFurnitureAnimTypeFix::Install();
	}
	if (fixes.lightAttachCrash.value) {
		AttachLightCrash::Install();
	}
	if (fixes.effectShaderZBuffer.value) {
		EffectShaderZBufferFix::Install();
	}
	if (fixes.collisionToggleFix.value) {
		ToggleCollisionFix::Install();
	}
	if (fixes.loadEditorIDs.value) {
		LoadFormEditorIDs::Install();
	}
#ifdef SKYRIMVR
	if (fixes.fixVRCrosshairRefEvent.value) {
		FixCrosshairRefEvent::Install(skse_version);
	}
#endif
}
