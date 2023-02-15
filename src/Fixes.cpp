#include "Fixes.h"
#include "Settings.h"

void Fixes::PostLoad::Install()
{
	const auto fixes = Settings::GetSingleton()->fixes;

	logger::info("{:*^30}", "FIXES"sv);

	if (fixes.distantRefLoadCrash) {
		DistantRefLoadCrash::Install();
	}
	if (fixes.mapMarker) {
		if (GetModuleHandle(L"DisableFastTravel")) {
			logger::info("Detected DisableFastTravel, skipping mapMarker fix."sv);
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
	if (fixes.addedSpell) {
		ReapplySpellsOnLoad::Added::Install();
	}
	if (fixes.deathSpell) {
		ReapplySpellsOnLoad::OnDeath::Install();
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
	if (fixes.offensiveSpellAI) {
		OffensiveSpellAI::Install();
	}
	if (fixes.loadEditorIDs) {
		CacheFormEditorIDs::Install();
	}
	//UnderWaterCamera::Install(); tbd
}

void Fixes::PreLoad::Install([[maybe_unused]] std::uint32_t a_skse_version)
{
#ifdef SKYRIMVR
	const auto fixes = Settings::GetSingleton()->fixes;
	if (fixes.fixVRCrosshairRefEvent) {
		CrosshairRefEventVR::Install(a_skse_version);
	}
#endif
}

void Fixes::DataLoaded::Install()
{
	FlagSpellsAsNoAbsorb::Install();

	const auto& fixes = Settings::GetSingleton()->fixes;
	if (fixes.breathingSounds) {
		BreathingSounds::Install();
	}
}
