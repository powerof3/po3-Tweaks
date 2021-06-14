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

		fastGetPlayer = ini.GetBoolValue("Experimental", "Fast GetPlayer()", false);
		ini.SetBoolValue("Experimental", "Fast GetPlayer()", fastGetPlayer, ";Speeds up Game.GetPlayer calls.", true);

		fastRandomInt = ini.GetBoolValue("Experimental", "Fast RandomInt()", false);
		ini.SetBoolValue("Experimental", "Fast RandomInt()", fastRandomInt, ";Speeds up Utility.RandomInt calls.", true);

		fastRandomFloat = ini.GetBoolValue("Experimental", "Fast RandomFloat()", false);
		ini.SetBoolValue("Experimental", "Fast RandomFloat()", fastRandomFloat, ";Speeds up Utility.RandomFloat calls.", true);

		fastScripts = fastGetPlayer || fastRandomInt || fastRandomFloat;

		ini.SaveFile(path);
	}

	void LoadSnowyRegions()
	{
		auto dataHandler = RE::TESDataHandler::GetSingleton();
		if (dataHandler) {
			auto is_snowy_region = [&](RE::TESRegion* a_region) {
				if (auto list = a_region->dataList; list) {
					for (const auto& data : list->regionDataList) {
						if (auto weatherData = data && data->GetType() == RE::TESRegionData::Type::kWeather ?
                                                   static_cast<RE::TESRegionDataWeather*>(data) :
                                                   nullptr;
							weatherData) {
							for (auto& weatherType : weatherData->weatherTypes) {
								if (auto weather = weatherType->weather; weather && weather->data.flags.any(RE::TESWeather::WeatherDataFlag::kSnow)) {
									return true;
								}
							}
						}
					}
				}
				return false;
			};

			for (auto& region : dataHandler->GetFormArray<RE::TESRegion>()) {
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

	bool factionStealing;
	bool aiFadeOut;
	float voiceModulationValue;
	bool dopplerShift;
	bool dynamicSnowMat;
	bool noWaterPhysicsOnHover;

	bool fastScripts;
	bool fastGetPlayer;
	bool fastRandomInt;
	bool fastRandomFloat;

private:
	std::vector<RE::TESRegion*> snowRegions;
};
