#pragma once

class Settings : public REX::TSingleton<Settings>
{
private:
	template <class T>
	class Setting : public REX::TIniSetting<T>
	{
	public:
		Setting(std::string_view a_section, std::string_view a_oldKey, std::string_view a_newKey, T a_default) :
			REX::TIniSetting<T>(a_section, a_newKey, a_default),
			oldKey(a_oldKey),
			newKey(a_newKey)
		{
			GetSettingsToUpdate().emplace_back(a_section, a_oldKey, a_newKey);
		}

		virtual void Load(void* a_data, bool a_isBase) override
		{
			REX::TIniSetting<T>::Load(a_data, a_isBase);

			auto& settings = GetSingleton()->settingsMap;

			if constexpr (std::is_same_v<bool, T>) {
				settings.emplace(newKey, this->GetValue());
				settings.emplace(oldKey, this->GetValue());
			} else if constexpr (std::is_floating_point_v<T>) {
				settings.emplace(newKey, this->GetValue() != 1.0f);  //for the one setting that uses a float (Voice Modulation)
				settings.emplace(oldKey, this->GetValue() != 1.0f);
			} else if constexpr (std::is_arithmetic_v<T>) {
				settings.emplace(newKey, this->GetValue() != 0);
				settings.emplace(oldKey, this->GetValue() != 0);
			}
		}

	private:
		std::string_view newKey;
		std::string_view oldKey;
	};

	using Bool = Setting<bool>;
	using F32 = Setting<float>;
	using F64 = Setting<double>;
	using I8 = Setting<std::int8_t>;
	using I16 = Setting<std::int16_t>;
	using I32 = Setting<std::int32_t>;
	using U8 = Setting<std::uint8_t>;
	using U16 = Setting<std::uint16_t>;
	using U32 = Setting<std::uint32_t>;
	using Str = Setting<std::string>;

public:
	struct SettingsToUpdate
	{
		std::string_view section;
		std::string_view oldKey;
		std::string_view newKey;
	};

	class Fixes
	{
	public:
		Bool distantRefLoadCrash{ "Fixes", "Distant Ref Load Crash", "bDistantRefLoadCrash", true };
		Bool mapMarker{ "Fixes", "Map Marker Placement Fix", "bMapMarkerPlacementFix", true };
		Bool dontTakeBookFlag{ "Fixes", "Restore 'Can't Be Taken Book' Flag", "bRestoreCantBeTakenBookFlag", true };
		Bool projectileRange{ "Fixes", "Projectile Range Fix", "bProjectileRangeFix", true };
		Bool combatDialogue{ "Fixes", "CombatToNormal Dialogue Fix", "bCombatToNormalDialogueFix", true };
		Bool addedSpell{ "Fixes", "Cast Added Spells on Load", "bCastAddedSpellsOnLoad", true };
		Bool deathSpell{ "Fixes", "Cast No-Death-Dispel Spells on Load", "bCastNoDeathDispelSpellsOnLoad", true };
		Bool furnitureAnimType{ "Fixes", "IsFurnitureAnimType Fix", "bIsFurnitureAnimTypeFix", true };
		Bool lightAttachCrash{ "Fixes", "Light Attach Crash", "bLightAttachCrash", true };
		Bool noConjurationAbsorb{ "Fixes", "No Conjuration Spell Absorb", "bNoConjurationSpellAbsorb", true };
		Bool effectShaderZBuffer{ "Fixes", "EffectShader Z-Buffer Fix", "bEffectShaderZBufferFix", true };
		Bool collisionToggleFix{ "Fixes", "ToggleCollision Fix", "bToggleCollisionFix", true };
		Bool skinnedDecalDelete{ "Fixes", "Skinned Decal Delete", "bSkinnedDecalDelete", true };
		Bool jumpingBonusFix{ "Fixes", "Jumping Bonus Fix", "bJumpingBonusFix", true };
		Bool toggleGlobalAIFix{ "Fixes", "Toggle Global AI Fix", "bToggleGlobalAIFix", true };
		U32  useFurnitureInCombat{ "Fixes", "Use Furniture In Combat", "iUseFurnitureInCombat", 1 };
		Bool breathingSounds{ "Fixes", "Breathing Sounds", "bBreathingSounds", true };
		Bool validateScreenshotFolder{ "Fixes", "Validate Screenshot Location", "bValidateScreenshotLocation", true };
		Bool loadEditorIDs{ "Fixes", "Load EditorIDs", "bLoadEditorIDs", true };
		Bool firstPersonAlpha{ "Fixes", "First Person SetAlpha Fix", "bFirstPersonSetAlphaFix", true };
		Bool wornRestrictionsForWeapons{ "Fixes", "Worn Restrictions For Weapons", "bWornRestrictionsForWeapons", true };
		Bool magicItemFindKeywordFunctorCrash{ "Fixes", "MagicItemFindKeywordFunctor Crash", "bMagicItemFindKeywordFunctorCrash", true };
		Bool leftHandedWeaponEnchantmentNodeFix{ "Fixes", "Left Handed Weapon Enchantment Node Fix", "bLeftHandedWeaponEnchantmentNodeFix", true };
#ifdef SKYRIMVR
		Bool fixVRCrosshairRefEvent{ "Fixes", "VR CrosshairRefEvent Fix", "bVRCrosshairRefEventFix", true };
#endif
	};

	class Tweaks
	{
	public:
		struct SitToWait
		{
			Bool active{ "Tweaks", "Sit To Wait", "bSitToWait", false };
			Str  message{ "Tweaks", "Sit To Wait Message", "sSitToWaitMessage", "You cannot wait while standing." };
		};

		struct LoadDoorPrompt
		{
			U32 type{ "Tweaks", "Load Door Activate Prompt", "iLoadDoorActivatePrompt", 0 };
			Str enter{ "Tweaks", "Enter Label", "sEnterLabel", "Enter" };
			Str exit{ "Tweaks", "Exit Label", "sExitLabel", "Exit" };
		};

		Bool factionStealing{ "Tweaks", "Faction Stealing", "bFactionStealing", false };
		F32  voiceModulationValue{ "Tweaks", "Voice Modulation", "fVoiceModulation", 1.0f };
		Bool gameTimeAffectsSounds{ "Tweaks", "Game Time Affects Sounds", "bGameTimeAffectsSounds", false };
		Bool dynamicSnowMat{ "Tweaks", "Dynamic Snow Material", "bDynamicSnowMaterial", false };
		Bool noWaterPhysicsOnHover{ "Tweaks", "Disable Water Ripples On Hover", "bDisableWaterRipplesOnHover", false };
		Bool screenshotToConsole{ "Tweaks", "Screenshot Notification To Console", "bScreenshotNotificationToConsole", false };
		U32  noCritSneakMsg{ "Tweaks", "No Attack Messages", "iNoAttackMessages", 0 };
#ifdef SKYRIMVR
		Bool rememberLockPickAngle{ "Tweaks", "Remember Lock Pick Angle", "bRememberLockPickAngle", false };
#endif
		SitToWait      sitToWait{};
		U32            noCheatMode{ "Tweaks", "Disable God Mode", "iDisableGodMode", 0 };
		Bool           noHostileAbsorb{ "Tweaks", "No Hostile Spell Absorb", "bNoHostileSpellAbsorb", false };
		Bool           grabbingIsStealing{ "Tweaks", "Grabbing Is Stealing", "bGrabbingIsStealing", false };
		LoadDoorPrompt loadDoorPrompt{};
		U32            noPoisonPrompt{ "Tweaks", "No Poison Prompt", "iNoPoisonPrompt", 0 };
		Bool           silentSneakPowerAttack{ "Tweaks", "Silent Sneak Power Attacks", "bSilentSneakPowerAttacks", false };
		Bool           offensiveSpellAI{ "Tweaks", "Offensive Spell AI", "bOffensiveSpellAI", false };
	};

	class Experimental
	{
	public:
		Bool fastRandomInt{ "Experimental", "Fast RandomInt()", "bFastRandomInt", false };
		Bool fastRandomFloat{ "Experimental", "Fast RandomFloat()", "bFastRandomFloat", false };
		Bool orphanedAEFix{ "Experimental", "Clean Orphaned ActiveEffects", "bCleanOrphanedActiveEffects", false };
		Bool updateGameTimers{ "Experimental", "Update GameHour Timers", "bUpdateGameHourTimers", false };
		F64  stackDumpTimeoutModifier{ "Experimental", "Stack Dump Timeout Modifier", "fStackDumpTimeoutModifier", 30.0 };
	};

	void Load();

	static std::vector<SettingsToUpdate>& GetSettingsToUpdate()
	{
		static std::vector<SettingsToUpdate> settingsToUpdate;
		return settingsToUpdate;
	};

	[[nodiscard]] const Fixes&        GetFixes() const;
	[[nodiscard]] const Tweaks&       GetTweaks() const;
	[[nodiscard]] const Experimental& GetExperimental() const;

	bool IsTweakInstalled(std::string_view a_tweak);

private:
	static constexpr auto path = "Data/SKSE/Plugins/po3_Tweaks.ini";

	void UpdateINISettings();

	// members
	Fixes           fixes{};
	Tweaks          tweaks{};
	Experimental    experimental{};
	StringMap<bool> settingsMap{};
};
