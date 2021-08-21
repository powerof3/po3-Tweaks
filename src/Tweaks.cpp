#include "Tweaks.h"

void Tweaks::Install()
{
	const auto tweaks = Settings::GetSingleton()->tweaks;
	const auto experimental = Settings::GetSingleton()->experimental;

	logger::info("{:*^30}", "TWEAKS"sv);

	if (tweaks.factionStealing) {
		FactionStealing::Install();
	}
	if (tweaks.aiFadeOut) {
		AIFadeOut::Install();
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
	if (experimental.fastRandomFloat || experimental.fastRandomInt) {
		const auto papyrus = SKSE::GetPapyrusInterface();
		papyrus->Register(Script::Speedup);
	}
}
