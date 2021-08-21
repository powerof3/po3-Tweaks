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

		CSimpleIniA::TNamesDepend patchesSection;
		ini.GetAllKeys("Patches", patchesSection);
        const bool usesOldPatches = !patchesSection.empty();

		//FIXES
		fixes.Load(ini);

		//TWEAKS
		tweaks.Load(ini, usesOldPatches);

		//EXPERIMENTAL
		experimental.Load(ini, usesOldPatches);

		ini.SaveFile(path);
	}

	//members
	struct fixes
	{
		void Load(CSimpleIniA& a_ini)
		{
			queuedRefCrash = a_ini.GetBoolValue("Fixes", "Queued Ref Crash", true);
			a_ini.SetBoolValue("Fixes", "Queued Ref Crash", queuedRefCrash, ";Fixes crash caused by faulty ref loading.", true);

			mapMarker = a_ini.GetBoolValue("Fixes", "Map Marker Placement Fix", true);
			a_ini.SetBoolValue("Fixes", "Map Marker Placement Fix", mapMarker, ";Allows placing map markers near fast travel destinations when fast travel is disabled", true);

			dontTakeBookFlag = a_ini.GetBoolValue("Fixes", "Restore 'Can't Be Taken Book' Flag", true);
			a_ini.SetBoolValue("Fixes", "Restore 'Can't Be Taken Book' Flag", dontTakeBookFlag, ";Enables 'Can't be taken' book flag functionality.", true);

			projectileRange = a_ini.GetBoolValue("Fixes", "Projectile Range Fix", true);
			a_ini.SetBoolValue("Fixes", "Projectile Range Fix", projectileRange, ";Adjusts range of projectile fired while moving for consistent lifetime.", true);

			combatDialogue = a_ini.GetBoolValue("Fixes", "CombatToNormal Dialogue Fix", true);
			a_ini.SetBoolValue("Fixes", "CombatToNormal Dialogue Fix", combatDialogue, ";Fixes bug where NPCs were using LostToNormal dialogue in place of CombatToNormal.", true);

			addedSpell = a_ini.GetBoolValue("Fixes", "Cast Added Spells on Load", true);
			a_ini.SetBoolValue("Fixes", "Cast Added Spells on Load", addedSpell, ";Recasts added spell effects on actors.", true);

			deathSpell = a_ini.GetBoolValue("Fixes", "Cast No-Death-Dispel Spells on Load", true);
			a_ini.SetBoolValue("Fixes", "Cast No-Death-Dispel Spells on Load", deathSpell, ";Recasts no-death-dispel spell effects on dead actors.", true);

			furnitureAnimType = a_ini.GetBoolValue("Fixes", "IsFurnitureAnimType Fix", true);
			a_ini.SetBoolValue("Fixes", "IsFurnitureAnimType Fix", furnitureAnimType, ";Patches IsFurnitureAnimType condition/console function to work on furniture references", true);

			lightAttachCrash = a_ini.GetBoolValue("Fixes", "Light Attach Crash", true);
			a_ini.SetBoolValue("Fixes", "Light Attach Crash", lightAttachCrash, ";Fixes crash caused by lights attaching on unloaded characters", true);

			noConjurationAbsorb = a_ini.GetBoolValue("Fixes", "No Conjuration Spell Absorb", true);
			a_ini.SetBoolValue("Fixes", "No Conjuration Spell Absorb", noConjurationAbsorb, ";Adds NoAbsorb flag to all conjuration spells missing this flag", true);
		}

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

	} fixes;

	struct tweaks
	{
		void Load(CSimpleIniA& a_ini, bool a_clearOld)
		{
			const char* section = a_clearOld ? "Patches" : "Tweaks";

			factionStealing = a_ini.GetBoolValue(section, "Faction Stealing", false);
			a_ini.SetBoolValue("Tweaks", "Faction Stealing", factionStealing, ";Items will be marked stolen until player is friendly with all present members of faction.", false);

			aiFadeOut = a_ini.GetBoolValue(section, "Load Door Fade Out", false);
			a_ini.SetBoolValue("Tweaks", "Load Door Fade Out", aiFadeOut, ";Stops NPCs from fading out when using load doors.", true);

			voiceModulationValue = static_cast<float>(a_ini.GetDoubleValue(section, "Voice Modulation", 1.0));
			a_ini.SetDoubleValue("Tweaks", "Voice Modulation", static_cast<double>(voiceModulationValue), ";Applies voice distortion effect on NPCs wearing face covering helmets. A value of 1.0 has no effect.\n;Pitch is directly proportional to value. Recommended setting (0.85-0.90).", true);

			dopplerShift = a_ini.GetBoolValue(section, "Game Time Affects Sounds", false);
			a_ini.SetBoolValue("Tweaks", "Game Time Affects Sounds", dopplerShift, ";Scales sound pitch with time speed, eg. Slow Time will massively decrease pitch of all sounds", true);

			dynamicSnowMat = a_ini.GetBoolValue(section, "Dynamic Snow Material", false);
			a_ini.SetBoolValue("Tweaks", "Dynamic Snow Material", dynamicSnowMat, ";Applies snow material to all statics with directional snow", true);

			noWaterPhysicsOnHover = a_ini.GetBoolValue(section, "Disable Water Ripples On Hover", false);
			a_ini.SetBoolValue("Tweaks", "Disable Water Ripples On Hover", noWaterPhysicsOnHover, ";Hovering NPCs will not trigger water ripples", true);

			screenshotToConsole = a_ini.GetBoolValue(section, "Screenshot Notification To Console", false);
			a_ini.SetBoolValue("Tweaks", "Screenshot Notification To Console", screenshotToConsole, ";Displays screenshot notification as a console message", true);

			try {
				noCritSneakMsg = string::lexical_cast<std::uint32_t>(a_ini.GetValue(section, "No Attack Messages", "0"));
				a_ini.SetValue("Tweaks", "No Attack Messages", std::to_string(noCritSneakMsg).c_str(), ";Disables critical and sneak hit messages.\n;0 - off, 1 - only crit, 2 - only sneak, 3 - both", true);
			} catch (...) {
				noCritSneakMsg = 0;
			}

			sitToWait.active = a_ini.GetBoolValue(section, "Sit To Wait", false);
			a_ini.SetBoolValue("Tweaks", "Sit To Wait", sitToWait.active, ";Player can only wait when sitting down", true);

			sitToWait.message = a_ini.GetValue(section, "Sit To Wait Message", "You cannot wait while standing.");
			a_ini.SetValue("Tweaks", "Sit To Wait Message", sitToWait.message.c_str(), nullptr, true);

			try {
				noCheatMode = string::lexical_cast<std::uint32_t>(a_ini.GetValue(section, "Disable God Mode", "0"));
				a_ini.SetValue("Tweaks", "Disable God Mode", std::to_string(noCheatMode).c_str(), ";Disables god/immortal mod.\n;0 - off, 1 - only god mode, 2 - only immortal mode, 3 - both", true);
			} catch (...) {
				noCheatMode = 0;
			}

			noHostileAbsorb = a_ini.GetBoolValue("Tweaks", "No Hostile Spell Absorb", false);
			a_ini.SetBoolValue("Tweaks", "No Hostile Spell Absorb", noHostileAbsorb, ";Adds NoAbsorb flag to all non-hostile and non-detrimental spells", true);

			grabbingIsStealing = a_ini.GetBoolValue("Tweaks", "Grabbing Is Stealing", false);
			a_ini.SetBoolValue("Tweaks", "Grabbing Is Stealing", grabbingIsStealing, ";Grabbing owned items will count as stealing", true);

			try {
				loadDoorPrompt.type = string::lexical_cast<std::uint32_t>(a_ini.GetValue("Tweaks", "Load Door Activate Prompt", "0"));
				a_ini.SetValue("Tweaks", "Load Door Activate Prompt", std::to_string(loadDoorPrompt.type).c_str(), ";Replaces load door activate prompts with Enter and Exit\n;0 - off, 1 - replaces prompt (Open Skyrim -> Enter Skyrim), 2 - replaces prompt and cell name when moving from interior to exterior (Open Skyrim -> Exit Sleeping Giant Inn)", true);
			} catch (...) {
				loadDoorPrompt.type = 0;
			}

			loadDoorPrompt.enter = a_ini.GetValue("Tweaks", "Enter Label", "Enter");
			a_ini.SetValue("Tweaks", "Enter Label", loadDoorPrompt.enter.c_str(), nullptr, true);

			loadDoorPrompt.exit = a_ini.GetValue("Tweaks", "Exit Label", "Exit");
			a_ini.SetValue("Tweaks", "Exit Label", loadDoorPrompt.exit.c_str(), nullptr, true);

			if (a_clearOld) {
				logger::info("Replacing old Patches section with Tweaks");
				a_ini.Delete("Patches", nullptr, true);
			}
		}

		bool factionStealing;
		bool aiFadeOut;
		float voiceModulationValue;
		bool dopplerShift;
		bool dynamicSnowMat;
		bool noWaterPhysicsOnHover;
		bool screenshotToConsole;
		std::uint32_t noCritSneakMsg;

		struct
		{
			bool active;
			std::string message;

		} sitToWait;

		std::uint32_t noCheatMode;
		bool noHostileAbsorb;
		bool grabbingIsStealing;

		struct
		{
			std::uint32_t type;
			std::string enter;
			std::string exit;

		} loadDoorPrompt;

	} tweaks;

	struct experimental
	{
		void Load(CSimpleIniA& a_ini, bool a_clearOld)
		{
			//1.1 - remove GetPlayer()
			a_ini.Delete("Experimental", "Fast GetPlayer()", true);

			fastRandomInt = a_ini.GetBoolValue("Experimental", "Fast RandomInt()", false);
			fastRandomFloat = a_ini.GetBoolValue("Experimental", "Fast RandomFloat()", false);

			if (a_clearOld) {
				a_ini.Delete("Experimental", nullptr, true);  //delete and recreate it below tweaks section
			}

			a_ini.SetBoolValue("Experimental", "Fast RandomInt()", fastRandomInt, ";Speeds up Utility.RandomInt calls.", true);
			a_ini.SetBoolValue("Experimental", "Fast RandomFloat()", fastRandomFloat, ";Speeds up Utility.RandomFloat calls.", true);
		}

		bool fastRandomInt;
		bool fastRandomFloat;

	} experimental;
};
