#include "Tweaks.h"
#include "Settings.h"

void Tweaks::PostLoad::Install()
{
	const auto& tweaks = Settings::GetSingleton()->GetTweaks();

	logger::info("\t[TWEAKS]");

	if (tweaks.factionStealing) {
		FactionStealing::Install();
	}
	if (tweaks.voiceModulationValue != 1.0f) {
		VoiceModulation::Install();
	}
	if (tweaks.gameTimeAffectsSounds) {
		GameTimeAffectsSounds::Install();
	}
	if (tweaks.dynamicSnowMat) {
		DynamicSnowMaterial::Install();
	}
	if (tweaks.noWaterPhysicsOnHover) {
		NoRipplesOnHover::Install();
	}
	if (tweaks.screenshotToConsole) {
		ScreenshotToConsole::Install();
	}
	if (tweaks.noCritSneakMsg != 0) {
		NoCritSneakMessages::Install(tweaks.noCritSneakMsg);
	}
	if (tweaks.sitToWait.active) {
		SitToWait::Install();
	}
	if (tweaks.noCheatMode != 0) {
		NoCheatMode::Install(tweaks.noCheatMode);
	}
	if (tweaks.loadDoorPrompt.type != 0) {
		LoadDoorPrompt::Install();
	}
	if (tweaks.noPoisonPrompt != 0) {
		NoPoisonPrompt::Install(tweaks.noPoisonPrompt);
	}
	if (tweaks.silentSneakPowerAttack) {
		SilentSneakPowerAttacks::Install();
	}
#ifdef SKYRIMVR
	if (tweaks.rememberLockPickAngle) {
		RememberLockPickAngleVR::Install();
	}
#endif
}

void Tweaks::DataLoaded::Install()
{
	logger::info("\t[TWEAKS]");

	const auto& tweaks = Settings::GetSingleton()->GetTweaks();
	if (tweaks.grabbingIsStealing) {
		GrabbingIsStealing::Install();
	}
}
