#pragma once

class Settings
{
public:
	[[nodiscard]] static Settings* GetSingleton()
	{
		static Settings singleton;
		return std::addressof(singleton);
	}

	void Load()
	{
		constexpr auto path = L"Data/SKSE/Plugins/po3_Tweaks.ini";

		CSimpleIniA ini;
		ini.SetUnicode();

		ini.LoadFile(path);

		//FIXES

		queuedRefCrash = ini.GetBoolValue("Fixes", "Queued Ref Crash", true);
		ini.SetBoolValue("Fixes", "Queued Ref Crash", queuedRefCrash, ";Fixes crash caused by faulty ref loading.", true);

		mapMarker = ini.GetBoolValue("Fixes", "Map Marker Placement Fix", true);
		ini.SetBoolValue("Fixes", "Map Marker Placement Fix", mapMarker, ";Allows placing map markers near fast travel destinations when fast travel is disabled", true);

		dontTakeBookFlag = ini.GetBoolValue("Fixes", "Restore 'Can't Be Taken Book' Flag", true);
		ini.SetBoolValue("Fixes", "Restore 'Can't Be Taken Book' Flag", dontTakeBookFlag, ";Enables 'Can't be taken' book flag functionality.", true);

		projectileRange = ini.GetBoolValue("Fixes", "Projectile Range Fix", true);
		ini.SetBoolValue("Fixes", "Projectile Range Fix", projectileRange, ";Adjusts range of projectile fired while moving for consistent lifetime.", true);

		combatDialogue = ini.GetBoolValue("Fixes", "CombatToNormal Dialogue Fix", true);
		ini.SetBoolValue("Fixes", "CombatToNormal Dialogue Fix", combatDialogue, ";Fixes bug where NPCs were using LostToNormal dialogue in place of CombatToNormal.", true);

		addedSpell = ini.GetBoolValue("Fixes", "Cast Added Spells on Load", true);
		ini.SetBoolValue("Fixes", "Cast Added Spells on Load", addedSpell, ";Recasts added spell effects on actors.", true);

		deathSpell = ini.GetBoolValue("Fixes", "Cast No-Death-Dispel Spells on Load", true);
		ini.SetBoolValue("Fixes", "Cast No-Death-Dispel Spells on Load", deathSpell, ";Recasts no-death-dispel spell effects on dead actors.", true);

		furnitureAnimType = ini.GetBoolValue("Fixes", "IsFurnitureAnimType Fix", true);
		ini.SetBoolValue("Fixes", "IsFurnitureAnimType Fix", furnitureAnimType, ";Patches IsFurnitureAnimType condition/console function to work on furniture references", true);

		lightAttachCrash = ini.GetBoolValue("Fixes", "Light Attach Crash", true);
		ini.SetBoolValue("Fixes", "Light Attach Crash", lightAttachCrash, ";Fixes crash caused by lights attaching on unloaded characters", true);

		noConjurationAbsorb = ini.GetBoolValue("Fixes", "No Conjuration Spell Absorb", true);
		ini.SetBoolValue("Fixes", "No Conjuration Spell Absorb", noConjurationAbsorb, ";Adds NoAbsorb flag to all conjuration spells missing this flag", true);

		//TWEAKS

		factionStealing = ini.GetBoolValue("Patches", "Faction Stealing", false);
		ini.SetBoolValue("Patches", "Faction Stealing", factionStealing, ";Items will be marked stolen until player is friendly with all present members of faction.", false);

		aiFadeOut = ini.GetBoolValue("Patches", "Load Door Fade Out", false);
		ini.SetBoolValue("Patches", "Load Door Fade Out", aiFadeOut, ";Stops NPCs from fading out when using load doors.", true);

		voiceModulationValue = static_cast<float>(ini.GetDoubleValue("Patches", "Voice Modulation", 1.0));
		ini.SetDoubleValue("Patches", "Voice Modulation", static_cast<double>(voiceModulationValue), ";Applies voice distortion effect on NPCs wearing face covering helmets. A value of 1.0 has no effect.\n;Pitch is directly proportional to value. Recommended setting (0.85-0.90).", true);

		dopplerShift = ini.GetBoolValue("Patches", "Game Time Affects Sounds", false);
		ini.SetBoolValue("Patches", "Game Time Affects Sounds", dopplerShift, ";Scales sound pitch with time speed, eg. Slow Time will massively decrease pitch of all sounds", true);

		dynamicSnowMat = ini.GetBoolValue("Patches", "Dynamic Snow Material", false);
		ini.SetBoolValue("Patches", "Dynamic Snow Material", dynamicSnowMat, ";Applies snow material to all statics with directional snow", true);

		noWaterPhysicsOnHover = ini.GetBoolValue("Patches", "Disable Water Ripples On Hover", false);
		ini.SetBoolValue("Patches", "Disable Water Ripples On Hover", noWaterPhysicsOnHover, ";Hovering NPCs will not trigger water ripples", true);

		screenshotToConsole = ini.GetBoolValue("Patches", "Screenshot Notification To Console", false);
		ini.SetBoolValue("Patches", "Screenshot Notification To Console", screenshotToConsole, ";Displays screenshot notification as a console message", true);

		try {
			noCritSneakMsg = string::lexical_cast<std::uint32_t>(ini.GetValue("Patches", "No Attack Messages", "0"));
			ini.SetValue("Patches", "No Attack Messages", std::to_string(noCritSneakMsg).c_str(), ";Disables critical and sneak hit messages.\n;0 - off, 1 - only crit, 2 - only sneak, 3 - both", true);
		}
		catch (...) {
			noCritSneakMsg = 0;
		}

		sitToWait = ini.GetBoolValue("Patches", "Sit To Wait", false);
		ini.SetBoolValue("Patches", "Sit To Wait", sitToWait, ";Player can only wait when sitting down", true);

		sitToWaitMessage = ini.GetValue("Patches", "Sit To Wait Message", "You cannot wait while standing.");
		ini.SetValue("Patches", "Sit To Wait Message", sitToWaitMessage.c_str(), "", true);

		try {
			noCheatMode = string::lexical_cast<std::uint32_t>(ini.GetValue("Patches", "Disable God Mode", "0"));
			ini.SetValue("Patches", "Disable God Mode", std::to_string(noCheatMode).c_str(), ";Disables god/immortal mod.\n;0 - off, 1 - only god mode, 2 - only immortal mode, 3 - both", true);
		} catch (...) {
			noCheatMode = 0;
		}

		//EXPERIMENTAL

		fastRandomInt = ini.GetBoolValue("Experimental", "Fast RandomInt()", false);
		ini.SetBoolValue("Experimental", "Fast RandomInt()", fastRandomInt, ";Speeds up Utility.RandomInt calls.", true);

		fastRandomFloat = ini.GetBoolValue("Experimental", "Fast RandomFloat()", false);
		ini.SetBoolValue("Experimental", "Fast RandomFloat()", fastRandomFloat, ";Speeds up Utility.RandomFloat calls.", true);

		//1.1 - remove GetPlayer()
		ini.Delete("Experimental", "Fast GetPlayer()", true);

		fastScripts = fastRandomInt || fastRandomFloat;

		ini.SaveFile(path);
	}

	void LoadSnowyRegions()
	{
		const auto dataHandler = RE::TESDataHandler::GetSingleton();
		if (dataHandler) {
			const auto is_snowy_region = [&](RE::TESRegion* a_region) {
				if (const auto list = a_region->dataList; list) {
					for (const auto& data : list->regionDataList) {
						if (const auto weatherData = data && data->GetType() == RE::TESRegionData::Type::kWeather ?
                                                   static_cast<RE::TESRegionDataWeather*>(data) :
                                                   nullptr;
							weatherData) {
							for (const auto& weatherType : weatherData->weatherTypes) {
								if (const auto weather = weatherType->weather; weather && weather->data.flags.any(RE::TESWeather::WeatherDataFlag::kSnow)) {
									return true;
								}
							}
						}
					}
				}
				return false;
			};

			for (const auto& region : dataHandler->GetFormArray<RE::TESRegion>()) {
				if (region && is_snowy_region(region)) {
					snowRegions.push_back(region);
				}
			}
		}
	}

	std::vector<RE::TESRegion*>& getRegions()
	{
		return snowRegions;
	}

	//members
	bool queuedRefCrash;
	bool mapMarker;
	bool dontTakeBookFlag;
	bool projectileRange;
	bool combatDialogue;
	bool addedSpell;
	bool deathSpell;
	bool furnitureAnimType;
	bool lightAttachCrash;
	bool noConjurationAbsorb;

	bool factionStealing;
	bool aiFadeOut;
	float voiceModulationValue;
	bool dopplerShift;
	bool dynamicSnowMat;
	bool noWaterPhysicsOnHover;
	bool screenshotToConsole;
	std::uint32_t noCritSneakMsg;
	bool sitToWait;
	std::string sitToWaitMessage;
	std::uint32_t noCheatMode;

	bool fastScripts;
	bool fastRandomInt;
	bool fastRandomFloat;

private:
	std::vector<RE::TESRegion*> snowRegions;
};
