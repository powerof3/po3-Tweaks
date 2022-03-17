#include "Tweaks.h"

void Tweaks::Install()
{
	const auto& tweaks = Settings::GetSingleton()->tweaks;

	logger::info("{:*^30}", "TWEAKS"sv);

	if (tweaks.factionStealing) {
		FactionStealing::Install();
	}
	if (tweaks.voiceModulationValue != 1.0f) {
		VoiceModulation::Install();
	}
	if (tweaks.dopplerShift) {
		DopplerShift::Install();
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
		NoCritSneakMessage::Install(tweaks.noCritSneakMsg);
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
		SilentSneakPowerAttack::Install();
	}
#ifdef SKYRIMVR
	if (tweaks.rememberLockPickAngle) {
		RememberLockPickAngle::Install();
	}
#endif
}
