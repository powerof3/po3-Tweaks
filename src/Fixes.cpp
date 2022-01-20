#include "Fixes.h"

void Fixes::Install(std::uint32_t skse_version)
{
	const auto fixes = Settings::GetSingleton()->fixes;

	logger::info("{:*^30}", "FIXES"sv);

	if (fixes.queuedRefCrash) {
		QueuedRefCrash::Install();
	}
	if (fixes.mapMarker) {
		if (GetModuleHandle(L"DisableFastTravel")) {
			logger::info("Detected DisableFastTravel, skipping mapMarker fix."sv);
		} else {
			MapMarker::Install();
		}
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
	if (fixes.effectShaderZBuffer) {
		EffectShaderZBufferFix::Install();
	}
	if (fixes.collisionToggleFix) {
		ToggleCollisionFix::Install();
	}
	if (fixes.skinnedDecalDelete) {
		SkinnedDecalDeleteFix::Install();
	}
	if (fixes.loadEditorIDs) {
		LoadFormEditorIDs::Install();
	}
#ifdef SKYRIMVR
	if (fixes.fixVRCrosshairRefEvent) {
		FixCrosshairRefEvent::Install(skse_version);
	}
#endif
}
