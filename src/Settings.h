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

	bool IsTweakInstalled(std::string_view a_tweak)
	{
		const auto it = settingsMap.find(stl::as_string(a_tweak));
		return it != settingsMap.end() ? it->second : false;
	}

	struct
	{
		void Load(CSimpleIniA& a_ini)
		{
			static const char* section = "Fixes";

			//1.5 - remove GetEquippedFix()
			a_ini.Delete(section, "GetEquipped Fix", true);

			//1.6 - delete QueuedRef Crash
			a_ini.Delete(section, "Queued Ref Crash", true);

			detail::get_value(a_ini, distantRefLoadCrash, section, "Distant Ref Load Crash", ";Fixes loading crash caused by missing 3D on distant references.");
			detail::get_value(a_ini, mapMarker, section, "Map Marker Placement Fix", ";Allows placing map markers near fast travel destinations when fast travel is disabled");
			detail::get_value(a_ini, dontTakeBookFlag, section, "Restore 'Can't Be Taken Book' Flag", ";Enables 'Can't be taken' book flag functionality.");
			detail::get_value(a_ini, projectileRange, section, "Projectile Range Fix", ";Adjusts range of projectile fired while moving for consistent lifetime.");
			detail::get_value(a_ini, combatDialogue, section, "CombatToNormal Dialogue Fix", ";Fixes bug where NPCs were using LostToNormal dialogue in place of CombatToNormal.");
			detail::get_value(a_ini, addedSpell, section, "Cast Added Spells on Load", ";Recasts added spell effects on actors.");
			detail::get_value(a_ini, deathSpell, section, "Cast No-Death-Dispel Spells on Load", ";Recasts no-death-dispel spell effects on dead actors.");
			detail::get_value(a_ini, furnitureAnimType, section, "IsFurnitureAnimType Fix", ";Patches IsFurnitureAnimType condition/console function to work on furniture references");
			detail::get_value(a_ini, lightAttachCrash, section, "Light Attach Crash", ";Fixes crash caused by lights attaching on unloaded characters");
			detail::get_value(a_ini, noConjurationAbsorb, section, "No Conjuration Spell Absorb", ";Adds NoAbsorb flag to all conjuration spells missing this flag");
			detail::get_value(a_ini, effectShaderZBuffer, section, "EffectShader Z-Buffer Fix", ";Fixes effect shader z-buffer rendering so particles can show through objects");
			detail::get_value(a_ini, collisionToggleFix, section, "ToggleCollision Fix", ";Patches ToggleCollision to toggle object collision if selected in console");
			detail::get_value(a_ini, skinnedDecalDelete, section, "Skinned Decal Delete", ";Immediately delete skinned decals when they're marked for removal (ie. removing bloody armor)");
			detail::get_value(a_ini, jumpingBonusFix, section, "Jumping Bonus Fix", ";Jump height is multiplied by 1% per point of JumpingBonus actor value");
			detail::get_value(a_ini, toggleAIFreezeAllFix, section, "ToggleAI Freeze All Fix", "All NPCs are frozen/unfrozen when using TAI console command/Debug.ToggleAI() function");
			detail::get_value(a_ini, loadEditorIDs, section, "Load EditorIDs", ";Loads editorIDs for skipped forms at runtime");
#ifdef SKYRIMVR
			detail::get_value(a_ini, fixVRCrosshairRefEvent, section, "VR CrosshairRefEvent Fix", "; Trigger CrossHairRefEvent with hand selection (normally requires game controller to enable crosshair events)");
#endif
		}

		bool distantRefLoadCrash{ true };
		bool mapMarker{ true };
		bool dontTakeBookFlag{ true };
		bool projectileRange{ true };
		bool combatDialogue{ true };
		bool addedSpell{ true };
		bool deathSpell{ true };
		bool furnitureAnimType{ true };
		bool lightAttachCrash{ true };
		bool noConjurationAbsorb{ true };
		bool effectShaderZBuffer{ true };
		bool collisionToggleFix{ true };
		bool skinnedDecalDelete{ true };
		bool jumpingBonusFix{ true };
		bool toggleAIFreezeAllFix{ true };
		bool loadEditorIDs{ true };
#ifdef SKYRIMVR
		bool fixVRCrosshairRefEvent{ true };
#endif

	} fixes;

	struct
	{
		void Load(CSimpleIniA& a_ini, bool a_clearOld)
		{
			const char* section = a_clearOld ? "Patches" : "Tweaks";

			//1.5 - delete AIFadeOut()
			a_ini.Delete(section, "Load Door Fade Out", true);

			detail::get_value(a_ini, factionStealing, section, "Faction Stealing", ";Items will be marked stolen until player is friendly with all present members of faction.");
			detail::get_value(a_ini, voiceModulationValue, section, "Voice Modulation", ";Applies voice distortion effect on NPCs wearing face covering helmets. A value of 1.0 has no effect.\n;Pitch is directly proportional to value. Recommended setting (0.85-0.90).");
			detail::get_value(a_ini, dopplerShift, section, "Game Time Affects Sounds", ";Scales sound pitch with time speed, eg. Slow Time will massively decrease pitch of all sounds");
			detail::get_value(a_ini, dynamicSnowMat, section, "Dynamic Snow Material", ";Applies snow collision material to all statics with directional snow");
			detail::get_value(a_ini, noWaterPhysicsOnHover, section, "Disable Water Ripples On Hover", ";Hovering NPCs will not trigger water ripples");
			detail::get_value(a_ini, screenshotToConsole, section, "Screenshot Notification To Console", ";Displays screenshot notification as a console message");
			detail::get_value(a_ini, noCritSneakMsg, section, "No Attack Messages", ";Disables critical and sneak hit messages.\n;0 - off, 1 - only crit, 2 - only sneak, 3 - both");
			detail::get_value(a_ini, sitToWait.active, section, "Sit To Wait", ";Player can only wait when sitting down");
			detail::get_value(a_ini, sitToWait.message, section, "Sit To Wait Message", nullptr);
			detail::get_value(a_ini, noCheatMode, section, "Disable God Mode", ";Disables god/immortal mod.\n;0 - off, 1 - only god mode, 2 - only immortal mode, 3 - both");
			detail::get_value(a_ini, noHostileAbsorb, section, "No Hostile Spell Absorb", ";Adds NoAbsorb flag to all non-hostile and non-detrimental spells");
			detail::get_value(a_ini, grabbingIsStealing, section, "Grabbing Is Stealing", ";Grabbing owned items will count as stealing");
			detail::get_value(a_ini, loadDoorPrompt.type, section, "Load Door Activate Prompt", ";Replaces load door activate prompts with Enter and Exit\n;0 - off, 1 - replaces prompt (Open Skyrim -> Enter Skyrim), 2 - replaces prompt and cell name when moving from interior to exterior (Open Skyrim -> Exit Sleeping Giant Inn)");
			detail::get_value(a_ini, loadDoorPrompt.enter, section, "Enter Label", nullptr);
			detail::get_value(a_ini, loadDoorPrompt.exit, section, "Exit Label", nullptr);
			detail::get_value(a_ini, noPoisonPrompt, section, "No Poison Prompt", ";Disables poison confirmation messages.\n;0 - off, 1 - disable confirmation, 2 - show other messages as notifications (may clip with inventory menu), 3 - both");
			detail::get_value(a_ini, silentSneakPowerAttack, section, "Silent Sneak Power Attacks", ";Prevent player shouting during power attacks if sneaking");
#ifdef SKYRIMVR
			detail::get_value(a_ini, rememberLockPickAngle, section, "Remember Lock Pick Angle", ";Angle is preserved after break");
#endif
			if (a_clearOld) {
				logger::info("Replacing old Patches section with Tweaks");
				a_ini.Delete("Patches", nullptr, true);
			}
		}

		bool factionStealing{ false };
		float voiceModulationValue{ 1.0f };
		bool dopplerShift{ false };
		bool dynamicSnowMat{ false };
		bool noWaterPhysicsOnHover{ false };
		bool screenshotToConsole{ false };
		std::uint32_t noCritSneakMsg{ 0 };
#ifdef SKYRIMVR
		bool rememberLockPickAngle{ false };
#endif
		struct
		{
			bool active{ false };
			std::string message{ "You cannot wait while standing." };

		} sitToWait;

		std::uint32_t noCheatMode{ 0 };
		bool noHostileAbsorb{ false };
		bool grabbingIsStealing{ false };

		struct
		{
			std::uint32_t type{ 0 };
			std::string enter{ "Enter" };
			std::string exit{ "Exit" };

		} loadDoorPrompt;

		std::uint32_t noPoisonPrompt{ 0 };
		bool silentSneakPowerAttack{ false };

	} tweaks;

	struct
	{
		void Load(CSimpleIniA& a_ini, bool a_clearOld)
		{
			const char* section = "Experimental";

			if (a_clearOld) {
				a_ini.Delete("Experimental", nullptr, true);  //delete and recreate it below tweaks section
			}

			//1.1 - remove GetPlayer()
			a_ini.Delete("Experimental", "Fast GetPlayer()", true);

			detail::get_value(a_ini, fastRandomInt, section, "Fast RandomInt()", ";Speeds up Utility.RandomInt calls.");
			detail::get_value(a_ini, fastRandomFloat, section, "Fast RandomFloat()", ";Speeds up Utility.RandomFloat calls.");
			detail::get_value(a_ini, orphanedAEFix, section, "Clean Orphaned ActiveEffects", ";Removes active effects from NPCs with missing ability perks.");
			detail::get_value(a_ini, updateGameTimers, section, "Update GameHour Timers", ";Updates game timers when advancing time using GameHour.SetValue.");
			detail::get_value(a_ini, stackDumpTimeoutModifier, section, "Stack Dump Timeout Modifier", ";How many seconds Papyrus will try to dump script stacks (vanilla : 30 seconds). Setting this to 0 will disable the timeout (warning: this may result in a locked state if Skyrim can't dump stacks).");
		}

		bool fastRandomInt{ false };
		bool fastRandomFloat{ false };
		bool orphanedAEFix{ false };
		bool updateGameTimers{ false };
		double stackDumpTimeoutModifier{ 30.0 };

	} experimental;

private:
	struct detail
	{
		template <class T>
		static void get_value(CSimpleIniA& a_ini, T& a_value, const char* a_section, const char* a_key, const char* a_comment)
		{
			if constexpr (std::is_same_v<bool, T>) {
				a_value = a_ini.GetBoolValue(a_section, a_key, a_value);
				a_ini.SetBoolValue(a_section, a_key, a_value, a_comment);

				GetSingleton()->settingsMap.emplace(a_key, a_value);
			} else if constexpr (std::is_floating_point_v<T>) {
				a_value = static_cast<T>(a_ini.GetDoubleValue(a_section, a_key, a_value));
				a_ini.SetDoubleValue(a_section, a_key, a_value, a_comment);

				GetSingleton()->settingsMap.emplace(a_key, a_value != 1.0);  //for the one setting that uses a float (Voice Modulation)
			} else if constexpr (std::is_arithmetic_v<T>) {
				a_value = string::lexical_cast<T>(a_ini.GetValue(a_section, a_key, std::to_string(a_value).c_str()));
				a_ini.SetValue(a_section, a_key, std::to_string(a_value).c_str(), a_comment);

				GetSingleton()->settingsMap.emplace(a_key, a_value != 0);
			} else {
				a_value = a_ini.GetValue(a_section, a_key, a_value.c_str());
				a_ini.SetValue(a_section, a_key, a_value.c_str(), a_comment);
			}
		}
	};

	robin_hood::unordered_flat_map<std::string, bool> settingsMap{};
};
