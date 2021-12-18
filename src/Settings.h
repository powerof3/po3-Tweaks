#pragma once

class Settings
{
public:
	[[nodiscard]] static Settings* GetSingleton()
	{
		static Settings singleton;
		return std::addressof(singleton);
	}

	size_t Load()
	{
		size_t trampolineSpace = 0;

		constexpr auto path = L"Data/SKSE/Plugins/po3_Tweaks.ini";

		CSimpleIniA ini;
		ini.SetUnicode();

		ini.LoadFile(path);

		CSimpleIniA::TNamesDepend patchesSection;
		ini.GetAllKeys("Patches", patchesSection);
		const bool usesOldPatches = !patchesSection.empty();

		//FIXES
		fixes.Load(ini, trampolineSpace);

		//TWEAKS
		tweaks.Load(ini, trampolineSpace, usesOldPatches);

		//EXPERIMENTAL
		experimental.Load(ini, trampolineSpace, usesOldPatches);

		ini.SaveFile(path);

		return trampolineSpace;
	}

	//members
	template <class T>
	struct data
	{
		T value;
		size_t space;
	};

	struct
	{
		void Load(CSimpleIniA& a_ini, size_t& a_trampolineSpace)
		{
			static const char* section = "Fixes";

			a_trampolineSpace += detail::get_data(a_ini, queuedRefCrash, section, "Queued Ref Crash", ";Fixes crash caused by faulty ref loading.");

			a_trampolineSpace += detail::get_data(a_ini, mapMarker, section, "Map Marker Placement Fix", ";Allows placing map markers near fast travel destinations when fast travel is disabled");

			a_trampolineSpace += detail::get_data(a_ini, dontTakeBookFlag, section, "Restore 'Can't Be Taken Book' Flag", ";Enables 'Can't be taken' book flag functionality.");

			a_trampolineSpace += detail::get_data(a_ini, projectileRange, section, "Projectile Range Fix", ";Adjusts range of projectile fired while moving for consistent lifetime.");

			a_trampolineSpace += detail::get_data(a_ini, combatDialogue, section, "CombatToNormal Dialogue Fix", ";Fixes bug where NPCs were using LostToNormal dialogue in place of CombatToNormal.");

			a_trampolineSpace += detail::get_data(a_ini, addedSpell, section, "Cast Added Spells on Load", ";Recasts added spell effects on actors.");

			a_trampolineSpace += detail::get_data(a_ini, deathSpell, section, "Cast No-Death-Dispel Spells on Load", ";Recasts no-death-dispel spell effects on dead actors.");

			a_trampolineSpace += detail::get_data(a_ini, furnitureAnimType, section, "IsFurnitureAnimType Fix", ";Patches IsFurnitureAnimType condition/console function to work on furniture references");

			a_trampolineSpace += detail::get_data(a_ini, lightAttachCrash, section, "Light Attach Crash", ";Fixes crash caused by lights attaching on unloaded characters");

			a_trampolineSpace += detail::get_data(a_ini, noConjurationAbsorb, section, "No Conjuration Spell Absorb", ";Adds NoAbsorb flag to all conjuration spells missing this flag");

			a_trampolineSpace += detail::get_data(a_ini, getEquipped, section, "GetEquipped Fix", ";Patches GetEquipped console/condition function to work with left hand equipped items");

			a_trampolineSpace += detail::get_data(a_ini, effectShaderZBuffer, section, "EffectShader Z-Buffer Fix", ";Fixes effect shader z-buffer rendering so particles can show through objects");

			a_trampolineSpace += detail::get_data(a_ini, collisionToggleFix, section, "ToggleCollision Fix", ";Patches ToggleCollision to toggle object collision if selected in console");

			a_trampolineSpace += detail::get_data(a_ini, loadEditorIDs, section, "Load EditorIDs", ";Loads editorIDs for skipped forms at runtime");
#ifdef SKYRIMVR
			a_trampolineSpace += detail::get_data(a_ini, fixVRCrosshairRefEvent, section, "VR CrosshairRefEvent Fix", "; Trigger CrossHairRefEvent with hand selection (normally requires game controller to enable crosshair events)");
#endif
		}

		data<bool> queuedRefCrash{ true };
		data<bool> mapMarker{ true, 1 };
		data<bool> dontTakeBookFlag{ true, 1 };
		data<bool> projectileRange{ true, 1 };
		data<bool> combatDialogue{ true, 1 };
		data<bool> addedSpell{ true, 2 };
		data<bool> deathSpell{ true, 1 };
		data<bool> furnitureAnimType{ true };
		data<bool> lightAttachCrash{ true };
		data<bool> noConjurationAbsorb{ true };
		data<bool> getEquipped{ true };
		data<bool> effectShaderZBuffer{ true };
		data<bool> collisionToggleFix{ true, 1 };
		data<bool> loadEditorIDs{ true };
#ifdef SKYRIMVR
		data<bool> fixVRCrosshairRefEvent{ true };
#endif

	} fixes;

	struct
	{
		void Load(CSimpleIniA& a_ini, size_t& a_trampolineSpace, bool a_clearOld)
		{
			const char* section = a_clearOld ? "Patches" : "Tweaks";

			a_trampolineSpace += detail::get_data(a_ini, factionStealing, section, "Faction Stealing", ";Items will be marked stolen until player is friendly with all present members of faction.");

			a_trampolineSpace += detail::get_data(a_ini, aiFadeOut, section, "Load Door Fade Out", ";Stops NPCs from fading out when using load doors.");

			a_trampolineSpace += detail::get_data(a_ini, voiceModulationValue, section, "Voice Modulation", ";Applies voice distortion effect on NPCs wearing face covering helmets. A value of 1.0 has no effect.\n;Pitch is directly proportional to value. Recommended setting (0.85-0.90).");

			a_trampolineSpace += detail::get_data(a_ini, dopplerShift, section, "Game Time Affects Sounds", ";Scales sound pitch with time speed, eg. Slow Time will massively decrease pitch of all sounds");

			a_trampolineSpace += detail::get_data(a_ini, dynamicSnowMat, section, "Dynamic Snow Material", ";Applies snow collision material to all statics with directional snow");

			a_trampolineSpace += detail::get_data(a_ini, noWaterPhysicsOnHover, section, "Disable Water Ripples On Hover", ";Hovering NPCs will not trigger water ripples");

			a_trampolineSpace += detail::get_data(a_ini, screenshotToConsole, section, "Screenshot Notification To Console", ";Displays screenshot notification as a console message");

			a_trampolineSpace += detail::get_data(a_ini, noCritSneakMsg, section, "No Attack Messages", ";Disables critical and sneak hit messages.\n;0 - off, 1 - only crit, 2 - only sneak, 3 - both");

			a_trampolineSpace += detail::get_data(a_ini, sitToWait.active, section, "Sit To Wait", ";Player can only wait when sitting down");

			a_trampolineSpace += detail::get_data(a_ini, sitToWait.message, section, "Sit To Wait Message", nullptr);

			a_trampolineSpace += detail::get_data(a_ini, noCheatMode, section, "Disable God Mode", ";Disables god/immortal mod.\n;0 - off, 1 - only god mode, 2 - only immortal mode, 3 - both");

			a_trampolineSpace += detail::get_data(a_ini, noHostileAbsorb, section, "No Hostile Spell Absorb", ";Adds NoAbsorb flag to all non-hostile and non-detrimental spells");

			a_trampolineSpace += detail::get_data(a_ini, grabbingIsStealing, section, "Grabbing Is Stealing", ";Grabbing owned items will count as stealing");

			a_trampolineSpace += detail::get_data(a_ini, loadDoorPrompt.type, section, "Load Door Activate Prompt", ";Replaces load door activate prompts with Enter and Exit\n;0 - off, 1 - replaces prompt (Open Skyrim -> Enter Skyrim), 2 - replaces prompt and cell name when moving from interior to exterior (Open Skyrim -> Exit Sleeping Giant Inn)");

			a_trampolineSpace += detail::get_data(a_ini, loadDoorPrompt.enter, section, "Enter Label", nullptr);

			a_trampolineSpace += detail::get_data(a_ini, loadDoorPrompt.exit, section, "Exit Label", nullptr);

			a_trampolineSpace += detail::get_data(a_ini, noPoisonPrompt, section, "No Poison Prompt", ";Disables poison confirmation messages.\n;0 - off, 1 - disable confirmation, 2 - show other messages as notifications (may clip with inventory menu), 3 - both");

#ifdef SKYRIMVR
			a_trampolineSpace += detail::get_data(a_ini, rememberLockPickAngle, section, "Remember Lock Pick Angle", "; Angle is preserved after break");
#endif
			if (a_clearOld) {
				logger::info("Replacing old Patches section with Tweaks");
				a_ini.Delete("Patches", nullptr, true);
			}
		}

		data<bool> factionStealing{ false };
		data<bool> aiFadeOut{ false, 1 };
		data<float> voiceModulationValue{ 1.0f, 1 };
		data<bool> dopplerShift{ false };
		data<bool> dynamicSnowMat{ false };
		data<bool> noWaterPhysicsOnHover{ false };
		data<bool> screenshotToConsole{ false, 1 };
		data<std::uint32_t> noCritSneakMsg{ 0 };
#ifdef SKYRIMVR
		data<bool> rememberLockPickAngle{ false };
#endif
		struct
		{
			data<bool> active{ false, 1 };
			std::string message{ "You cannot wait while standing." };

		} sitToWait;

		data<std::uint32_t> noCheatMode{ 0 };
		data<bool> noHostileAbsorb{ false };
		data<bool> grabbingIsStealing{ false };

		struct
		{
			data<std::uint32_t> type{ 0, 2 };
			std::string enter{ "Enter" };
			std::string exit{ "Exit" };

		} loadDoorPrompt;

		data<std::uint32_t> noPoisonPrompt{ 0, 2 };

	} tweaks;

	struct
	{
		void Load(CSimpleIniA& a_ini, size_t& a_trampolineSpace, bool a_clearOld)
		{
			const char* section = "Experimental";

			//1.1 - remove GetPlayer()
			a_ini.Delete("Experimental", "Fast GetPlayer()", true);
			if (a_clearOld) {
				a_ini.Delete("Experimental", nullptr, true);  //delete and recreate it below tweaks section
			}

			a_trampolineSpace += detail::get_data(a_ini, fastRandomInt, section, "Fast RandomInt()", ";Speeds up Utility.RandomInt calls.");

			a_trampolineSpace += detail::get_data(a_ini, fastRandomFloat, section, "Fast RandomFloat()", ";Speeds up Utility.RandomFloat calls.");

			a_trampolineSpace += detail::get_data(a_ini, orphanedAEFix, section, "Clean Orphaned ActiveEffects", ";Removes active effects from NPCs with missing ability perks.");

			a_trampolineSpace += detail::get_data(a_ini, updateGameTimers, section, "Update GameHour Timers", ";Updates game timers when advancing time using GameHour.SetValue");

			a_trampolineSpace += detail::get_data(a_ini, removeFlushTimeout, section, "Remove Stack Flush Timeout", ";Disables 30 second timeout for suspended stack flush. Warning: This may result in a locked state if Skyrim can't dump stacks.");

		}

		data<bool> fastRandomInt{ false };
		data<bool> fastRandomFloat{ false };
		data<bool> orphanedAEFix{ false };
		data<bool> updateGameTimers{ false };
		data<bool> removeFlushTimeout{ false };


	} experimental;

private:
	struct detail
	{
		static size_t get_data(CSimpleIniA& a_ini, data<std::uint32_t>& a_data, const char* a_section, const char* a_key, const char* a_comment)
		{
			try {
				a_data.value = string::lexical_cast<std::uint32_t>(a_ini.GetValue(a_section, a_key, "0"));
				a_ini.SetValue(a_section, a_key, std::to_string(a_data.value).c_str(), a_comment);

				return a_data.value != 0 ? a_data.space : 0;
			} catch (...) {
				return 0;
			}
		}

		static size_t get_data(CSimpleIniA& a_ini, data<float>& a_data, const char* a_section, const char* a_key, const char* a_comment)
		{
			a_data.value = static_cast<float>(a_ini.GetDoubleValue(a_section, a_key, a_data.value));
			a_ini.SetDoubleValue(a_section, a_key, a_data.value, a_comment);

			return a_data.value != 1.0f ? a_data.space : 0;
		}

		static size_t get_data(CSimpleIniA& a_ini, data<bool>& a_data, const char* a_section, const char* a_key, const char* a_comment)
		{
			a_data.value = a_ini.GetBoolValue(a_section, a_key, a_data.value);
			a_ini.SetBoolValue(a_section, a_key, a_data.value, a_comment);

			return a_data.value ? a_data.space : 0;
		};

		static size_t get_data(CSimpleIniA& a_ini, std::string& a_data, const char* a_section, const char* a_key, const char* a_comment)
		{
			a_data = a_ini.GetValue(a_section, a_key, a_data.c_str());
			a_ini.SetValue(a_section, a_key, a_data.c_str(), a_comment);

			return 0;
		};
	};
};
