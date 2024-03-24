#include "Fixes.h"
#include "Settings.h"

void Fixes::PreLoad::Install([[maybe_unused]] std::uint32_t a_skse_version)
{
#ifdef SKYRIMVR
	logger::info("\t[FIXES]");
	const auto& fixes = Settings::GetSingleton()->GetFixes();
	if (fixes.fixVRCrosshairRefEvent) {
		CrosshairRefEventVR::Install(a_skse_version);
	}
#endif
}

void Fixes::PostLoad::Install()
{
	const auto& fixes = Settings::GetSingleton()->GetFixes();

	logger::info("\t[FIXES]");

	if (fixes.distantRefLoadCrash) {
		DistantRefLoadCrash::Install();
	}
	if (fixes.mapMarker) {
		if (GetModuleHandle(L"DisableFastTravel")) {
			logger::info("\t\tDetected DisableFastTravel, skipping mapMarker fix."sv);
		} else {
			MapMarkerPlacement::Install();
		}
	}
	if (fixes.dontTakeBookFlag) {
		RestoreCantTakeBook::Install();
	}
	if (fixes.projectileRange) {
		ProjectileRange::Install();
	}
	if (fixes.combatDialogue) {
		CombatDialogue::Install();
	}
	if (fixes.furnitureAnimType) {
		IsFurnitureAnimTypeForFurniture::Install();
	}
	if (fixes.lightAttachCrash) {
		AttachLightHitEffectCrash::Install();
	}
	if (fixes.effectShaderZBuffer) {
		EffectShaderZBuffer::Install();
	}
	if (fixes.collisionToggleFix) {
		ToggleCollision::Install();
	}
	if (fixes.skinnedDecalDelete) {
		SkinnedDecalDelete::Install();
	}
	if (fixes.jumpingBonusFix) {
		RestoreJumpingBonus::Install();
	}
	if (fixes.toggleGlobalAIFix) {
		ToggleGlobalAI::Install();
	}
	if (fixes.useFurnitureInCombat != 0) {
		UseFurnitureInCombat::Install();
	}
	if (fixes.loadEditorIDs) {
		CacheFormEditorIDs::Install();
	}
	if (fixes.firstPersonAlpha) {
		FirstPersonAlpha::Install();
	}
	//UnderWaterCamera::Install(); tbd
}

void Fixes::PostPostLoad::Install()
{
	logger::info("\t[FIXES]");
	const auto& fixes = Settings::GetSingleton()->GetFixes();
	if (fixes.addedSpell) {
		ReapplyAddedSpells::Install();
	}
	if (fixes.deathSpell) {
		ReapplyNoDeathDispelSpells::Install();
	}
}

void Fixes::DataLoaded::Install()
{
	logger::info("\t[FIXES]");
	const auto& fixes = Settings::GetSingleton()->GetFixes();

	FlagSpellsAsNoAbsorb::Install();

	if (fixes.breathingSounds) {
		BreathingSounds::Install();
	}

	if (fixes.validateScreenshotFolder) {
		ValidateScreenshotFolder::Install();
	}
}
