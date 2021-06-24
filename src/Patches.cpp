#include "Patches.h"

void Patches::Patch()
{
	const auto settings = Settings::GetSingleton();

	logger::info("{:*^30}", "PATCHES"sv);

	if (settings->factionStealing) {
		FactionStealing::Patch();
		logger::info("Installed faction stealing patch"sv);
	}
	if (settings->aiFadeOut) {
		AIFadeOut::Patch();
		logger::info("Installed load door fade out patch"sv);
	}
	if (settings->voiceModulationValue != 1.0f) {
		VoiceModulation::Patch();
		logger::info("Installed voice modulation patch"sv);
	}
	if (settings->dopplerShift) {
		DopplerShift::Patch();
		logger::info("Installed sound - time sync patch"sv);
	}
	if (settings->dynamicSnowMat) {
		DynamicSnowMaterial::Patch();
		logger::info("Installed dynamic snow patch"sv);
	}
	if (settings->noWaterPhysicsOnHover) {
		NoRipplesOnHover::Patch();
		logger::info("Installed no ripples on hover patch"sv);
	}
	if (settings->screenshotToConsole) {
		ScreenshotToConsole::Patch();
		logger::info("Installed screenshot to console patch"sv);
	}		
	if (settings->noCritSneakMsg != 0) {
		NoCritSneakMessage::Patch(settings->noCritSneakMsg);
		logger::info("Installed crit/sneak message patch"sv);
	}	
	if (settings->sitToWait) {
		SitToWait::Patch();
		logger::info("Installed sit to wait patch"sv);
	}
	if (settings->noCheatMode != 0) {
		NoCheatMode::Patch(settings->noCheatMode);
	}
	
	//PSB::Patch();

	if (settings->fastScripts) {
		const auto papyrus = SKSE::GetPapyrusInterface();
		papyrus->Register(Script::Speedup);
	}
}
