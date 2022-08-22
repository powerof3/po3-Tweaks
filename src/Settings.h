#pragma once

class Settings
{
public:
	[[nodiscard]] static Settings* GetSingleton();

	void Load();

	bool IsTweakInstalled(std::string_view a_tweak);

	struct Fixes
	{
		void Load(CSimpleIniA& a_ini);

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
		bool toggleGlobalAIFix{ true };
		bool offensiveSpellAI{ true };
		bool flagStolenProduce{ true };

		std::uint32_t useFurnitureInCombat{ 1 };

		bool loadEditorIDs{ true };
#ifdef SKYRIMVR
		bool fixVRCrosshairRefEvent{ true };
#endif

	} fixes;

	struct Tweaks
	{
		void Load(CSimpleIniA& a_ini, bool a_clearOld);

		bool factionStealing{ false };
		float voiceModulationValue{ 1.0f };
		bool gameTimeAffectsSounds{ false };
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

	struct Experimental
	{
		void Load(CSimpleIniA& a_ini, bool a_clearOld);

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
			} else if constexpr (std::is_arithmetic_v<T> || std::is_enum_v<T>) {
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
