#include "Settings.h"

void Settings::Load()
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

	(void)ini.SaveFile(path);
}

bool Settings::IsTweakInstalled(std::string_view a_tweak)
{
	const auto it = settingsMap.find(stl::as_string(a_tweak));
	return it != settingsMap.end() ? it->second : false;
}

void Settings::Fixes::Load(CSimpleIniA& a_ini)
{
	static const char* section = "Fixes";

	logger::info("Settings - Loading [{}]", section);

	//1.5 - remove GetEquippedFix()
	a_ini.Delete(section, "GetEquipped Fix", true);

	//1.6 - delete QueuedRef Crash
	a_ini.Delete(section, "Queued Ref Crash", true);

	//1.7.6 - delete Flag Stolen Produce
	a_ini.Delete(section, "Flag Stolen Produce", true);

	//1.8.1 - move Offensive Spell AI over to Tweaks
	a_ini.Delete(section, "Offensive Spell AI", true);

	get_value(a_ini, distantRefLoadCrash, section, "Distant Ref Load Crash", ";Fixes loading crash caused by missing 3D on distant references.");
	get_value(a_ini, mapMarker, section, "Map Marker Placement Fix", ";Allows placing map markers near fast travel destinations when fast travel is disabled");
	get_value(a_ini, dontTakeBookFlag, section, "Restore 'Can't Be Taken Book' Flag", ";Enables 'Can't be taken' book flag functionality.");
	get_value(a_ini, projectileRange, section, "Projectile Range Fix", ";Adjusts range of projectile fired while moving for consistent lifetime.");
	get_value(a_ini, combatDialogue, section, "CombatToNormal Dialogue Fix", ";Fixes bug where NPCs were using LostToNormal dialogue in place of CombatToNormal.");
	get_value(a_ini, addedSpell, section, "Cast Added Spells on Load", ";Recasts added spell effects on actors.");
	get_value(a_ini, deathSpell, section, "Cast No-Death-Dispel Spells on Load", ";Recasts no-death-dispel spell effects on dead actors.");
	get_value(a_ini, furnitureAnimType, section, "IsFurnitureAnimType Fix", ";Patches IsFurnitureAnimType condition/console function to work on furniture references");
	get_value(a_ini, lightAttachCrash, section, "Light Attach Crash", ";Fixes crash caused by lights attaching on unloaded characters");
	get_value(a_ini, noConjurationAbsorb, section, "No Conjuration Spell Absorb", ";Adds NoAbsorb flag to all conjuration spells missing this flag");
	get_value(a_ini, effectShaderZBuffer, section, "EffectShader Z-Buffer Fix", ";Fixes effect shader z-buffer rendering so particles can show through objects");
	get_value(a_ini, collisionToggleFix, section, "ToggleCollision Fix", ";Patches ToggleCollision to toggle object collision if selected in console");
	get_value(a_ini, skinnedDecalDelete, section, "Skinned Decal Delete", ";Immediately delete skinned decals when they're marked for removal (ie. removing bloody armor)");
	get_value(a_ini, jumpingBonusFix, section, "Jumping Bonus Fix", ";Jump height is multiplied by 1% per point of JumpingBonus actor value");
	get_value(a_ini, toggleGlobalAIFix, section, "Toggle Global AI Fix", ";TAI console command/Debug.ToggleAI() now toggles all loaded NPC AI");
	get_value(a_ini, useFurnitureInCombat, section, "Use Furniture In Combat", ";Use furniture in combat and prevent getting forced out of furniture when attacked.\n;0 - off, 1 - player only, 2 - player and NPCs");
	get_value(a_ini, breathingSounds, section, "Breathing Sounds", ";Fix creature breathing sounds persisting after cell change");
	get_value(a_ini, loadEditorIDs, section, "Load EditorIDs", ";Loads editorIDs for skipped forms at runtime");
#ifdef SKYRIMVR
	get_value(a_ini, fixVRCrosshairRefEvent, section, "VR CrosshairRefEvent Fix", "; Trigger CrossHairRefEvent with hand selection (normally requires game controller to enable crosshair events)");
#endif
}

void Settings::Tweaks::Load(CSimpleIniA& a_ini, bool a_clearOld)
{
	const char* section = a_clearOld ? "Patches" : "Tweaks";

	logger::info("Settings - Loading [{}]", section);

	//1.5 - delete AIFadeOut()
	a_ini.Delete(section, "Load Door Fade Out", true);

	get_value(a_ini, factionStealing, section, "Faction Stealing", ";Items will be marked stolen until player is friendly with all present members of faction.");
	get_value(a_ini, voiceModulationValue, section, "Voice Modulation", ";Applies voice distortion effect on NPCs wearing face covering helmets. A value of 1.0 has no effect.\n;Pitch is directly proportional to value. Recommended setting (0.85-0.90).");
	get_value(a_ini, gameTimeAffectsSounds, section, "Game Time Affects Sounds", ";Scales sound pitch with time speed, eg. Slow Time will massively decrease pitch of all sounds");
	get_value(a_ini, dynamicSnowMat, section, "Dynamic Snow Material", ";Applies snow collision material to all statics with directional snow");
	get_value(a_ini, noWaterPhysicsOnHover, section, "Disable Water Ripples On Hover", ";Hovering NPCs will not trigger water ripples");
	get_value(a_ini, screenshotToConsole, section, "Screenshot Notification To Console", ";Displays screenshot notification as a console message");
	get_value(a_ini, noCritSneakMsg, section, "No Attack Messages", ";Disables critical and sneak hit messages.\n;0 - off, 1 - only crit, 2 - only sneak, 3 - both");
	get_value(a_ini, sitToWait.active, section, "Sit To Wait", ";Player can only wait when sitting down");
	get_value(a_ini, sitToWait.message, section, "Sit To Wait Message", nullptr);
	get_value(a_ini, noCheatMode, section, "Disable God Mode", ";Disables god/immortal mod.\n;0 - off, 1 - only god mode, 2 - only immortal mode, 3 - both");
	get_value(a_ini, noHostileAbsorb, section, "No Hostile Spell Absorb", ";Adds NoAbsorb flag to all non-hostile and non-detrimental spells");
	get_value(a_ini, grabbingIsStealing, section, "Grabbing Is Stealing", ";Grabbing owned items will count as stealing");
	get_value(a_ini, loadDoorPrompt.type, section, "Load Door Activate Prompt", ";Replaces load door activate prompts with Enter and Exit\n;0 - off, 1 - replaces prompt (Open Skyrim -> Enter Skyrim), 2 - replaces prompt and cell name when moving from interior to exterior (Open Skyrim -> Exit Sleeping Giant Inn)");
	get_value(a_ini, loadDoorPrompt.enter, section, "Enter Label", nullptr);
	get_value(a_ini, loadDoorPrompt.exit, section, "Exit Label", nullptr);
	get_value(a_ini, noPoisonPrompt, section, "No Poison Prompt", ";Disables poison confirmation messages.\n;0 - off, 1 - disable confirmation, 2 - show other messages as notifications (may clip with inventory menu), 3 - both");
	get_value(a_ini, silentSneakPowerAttack, section, "Silent Sneak Power Attacks", ";Prevent player shouting during power attacks if sneaking");
#ifdef SKYRIMVR
	get_value(a_ini, rememberLockPickAngle, section, "Remember Lock Pick Angle", ";Angle is preserved after break");
#endif
	get_value(a_ini, offensiveSpellAI, section, "Offensive Spell AI", ";Check spell condition validity before NPCs equip offensive spells");

	if (a_clearOld) {
		logger::info("Replacing old Patches section with Tweaks");
		a_ini.Delete("Patches", nullptr, true);
	}
}

void Settings::Experimental::Load(CSimpleIniA& a_ini, bool a_clearOld)
{
	const char* section = "Experimental";

	logger::info("Settings - Loading [{}]", section);

	if (a_clearOld) {
		a_ini.Delete("Experimental", nullptr, true);  //delete and recreate it below tweaks section
	}

	//1.1 - remove GetPlayer()
	a_ini.Delete("Experimental", "Fast GetPlayer()", true);

	get_value(a_ini, fastRandomInt, section, "Fast RandomInt()", ";Speeds up Utility.RandomInt calls.");
	get_value(a_ini, fastRandomFloat, section, "Fast RandomFloat()", ";Speeds up Utility.RandomFloat calls.");
	get_value(a_ini, orphanedAEFix, section, "Clean Orphaned ActiveEffects", ";Removes active effects from NPCs with missing ability perks.");
	get_value(a_ini, updateGameTimers, section, "Update GameHour Timers", ";Updates game timers when advancing time using GameHour.SetValue.");
	get_value(a_ini, stackDumpTimeoutModifier, section, "Stack Dump Timeout Modifier", ";How many seconds Papyrus will try to dump script stacks (vanilla : 30 seconds). Setting this to 0 will disable the timeout (warning: this may result in a locked state if Skyrim can't dump stacks).");
}

const Settings::Fixes& Settings::GetFixes() const
{
	return fixes;
}

const Settings::Tweaks& Settings::GetTweaks() const
{
	return tweaks;
}

const Settings::Experimental& Settings::GetExperimental() const
{
	return experimental;
}
