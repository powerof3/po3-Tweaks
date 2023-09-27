#pragma once

class Settings : public ISingleton<Settings>
{
public:
	class Fixes
	{
	public:
		void Load(CSimpleIniA& a_ini);

		bool          distantRefLoadCrash{ true };
		bool          mapMarker{ true };
		bool          dontTakeBookFlag{ true };
		bool          projectileRange{ true };
		bool          combatDialogue{ true };
		bool          addedSpell{ true };
		bool          deathSpell{ true };
		bool          furnitureAnimType{ true };
		bool          lightAttachCrash{ true };
		bool          noConjurationAbsorb{ true };
		bool          effectShaderZBuffer{ true };
		bool          collisionToggleFix{ true };
		bool          skinnedDecalDelete{ true };
		bool          jumpingBonusFix{ true };
		bool          toggleGlobalAIFix{ true };
		bool          breathingSounds{ true };
		std::uint32_t useFurnitureInCombat{ 1 };
		bool          loadEditorIDs{ true };
#ifdef SKYRIMVR
		bool fixVRCrosshairRefEvent{ true };
#endif
	};

	class Tweaks
	{
	public:
		void Load(CSimpleIniA& a_ini, bool a_clearOld);

		struct SitToWait
		{
			bool        active{ false };
			std::string message{ "You cannot wait while standing." };
		};

		struct LoadDoorPrompt
		{
			std::uint32_t type{ 0 };
			std::string   enter{ "Enter" };
			std::string   exit{ "Exit" };
		};

		bool          factionStealing{ false };
		float         voiceModulationValue{ 1.0f };
		bool          gameTimeAffectsSounds{ false };
		bool          dynamicSnowMat{ false };
		bool          noWaterPhysicsOnHover{ false };
		bool          screenshotToConsole{ false };
		std::uint32_t noCritSneakMsg{ 0 };
#ifdef SKYRIMVR
		bool rememberLockPickAngle{ false };
#endif
		SitToWait      sitToWait{};
		std::uint32_t  noCheatMode{ 0 };
		bool           noHostileAbsorb{ false };
		bool           grabbingIsStealing{ false };
		LoadDoorPrompt loadDoorPrompt{};
		std::uint32_t  noPoisonPrompt{ 0 };
		bool           silentSneakPowerAttack{ false };
		bool           offensiveSpellAI{ false };
	};

	class Experimental
	{
	public:
		void Load(CSimpleIniA& a_ini, bool a_clearOld);

		bool   fastRandomInt{ false };
		bool   fastRandomFloat{ false };
		bool   orphanedAEFix{ false };
		bool   updateGameTimers{ false };
		double stackDumpTimeoutModifier{ 30.0 };
	};

	void Load();

	[[nodiscard]] const Fixes&        GetFixes() const;
	[[nodiscard]] const Tweaks&       GetTweaks() const;
	[[nodiscard]] const Experimental& GetExperimental() const;

	bool IsTweakInstalled(std::string_view a_tweak);

private:
	template <class T>
	static void get_value(CSimpleIniA& a_ini, T& a_value, const char* a_section, const char* a_key, const char* a_comment)
	{
		ini::get_value(a_ini, a_value, a_section, a_key, a_comment);

		if constexpr (std::is_same_v<bool, T>) {
			GetSingleton()->settingsMap.emplace(a_key, a_value);
		} else if constexpr (std::is_floating_point_v<T>) {
			GetSingleton()->settingsMap.emplace(a_key, a_value != 1.0);  //for the one setting that uses a float (Voice Modulation)
		} else if constexpr (std::is_arithmetic_v<T>) {
			GetSingleton()->settingsMap.emplace(a_key, a_value != 0);
		}
	}

	Fixes        fixes{};
	Tweaks       tweaks{};
	Experimental experimental{};

	ankerl::unordered_dense::segmented_map<std::string, bool> settingsMap{};
};
