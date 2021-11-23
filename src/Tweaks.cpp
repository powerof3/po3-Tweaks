#include "Tweaks.h"

void Tweaks::Install()
{
	const auto& tweaks = Settings::GetSingleton()->tweaks;

	logger::info("{:*^30}", "TWEAKS"sv);

	if (tweaks.factionStealing.value) {
		FactionStealing::Install();
	}
	if (tweaks.aiFadeOut.value) {
		//AIFadeOut::Install();
	}
	if (tweaks.voiceModulationValue.value != 1.0f) {
		VoiceModulation::Install();
	}
	if (tweaks.dopplerShift.value) {
		DopplerShift::Install();
	}
	if (tweaks.dynamicSnowMat.value) {
		DynamicSnowMaterial::Install();
	}
	if (tweaks.noWaterPhysicsOnHover.value) {
		NoRipplesOnHover::Install();
	}
	if (tweaks.screenshotToConsole.value) {
		ScreenshotToConsole::Install();
	}		
	if (tweaks.noCritSneakMsg.value != 0) {
		NoCritSneakMessage::Install(tweaks.noCritSneakMsg.value);
	}	
	if (tweaks.sitToWait.active.value) {
		SitToWait::Install();
	}
	if (tweaks.noCheatMode.value != 0) {
		NoCheatMode::Install(tweaks.noCheatMode.value);
	}
	if (tweaks.loadDoorPrompt.type.value != 0) {
		LoadDoorPrompt::Install();
	}
	if (tweaks.noPoisonPrompt.value != 0) {
		NoPoisonPrompt::Install(tweaks.noPoisonPrompt.value);
	}
}
