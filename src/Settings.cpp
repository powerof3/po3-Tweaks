#include "Settings.h"

#include "SimpleINI.h"
#undef ERROR

void Settings::Load()
{
	std::error_code ec;
	if (!std::filesystem::exists(path, ec)) {
		CSimpleIniA ini;
		ini.LoadFile(path);
		(void)ini.SaveFile(path);
	} else {
		UpdateINISettings();
	}

	const auto store = REX::FIniSettingStore::GetSingleton();
	store->Init(path, "");

	store->Load();
	store->Save();
}

bool Settings::IsTweakInstalled(std::string_view a_tweak)
{
	return settingsMap.contains(a_tweak);
}

void Settings::UpdateINISettings()
{
	CSimpleIniA ini;
	ini.SetUnicode();

	if (ini.LoadFile(path) < SI_OK) {
		return;
	}

	if (ini.GetValue("Fixes", "bDistantRefLoadCrash")) {
		REX::INFO("No settings to migrate...");
		return;
	}

	for (auto& [section, oldKey, newKey] : GetSettingsToUpdate()) {
		CSimpleIniA::TNamesDepend values;
		if (ini.GetAllValues(section.data(), oldKey.data(), values) && !values.empty()) {
			const auto& entry = values.front();
			ini.SetValue(section.data(), newKey.data(), entry.pItem, entry.pComment);
			ini.Delete(section.data(), oldKey.data(), true);
			REX::INFO("Migrated [{}] {} -> {}", section, oldKey, newKey);
		}
	}

	(void)ini.SaveFile(path);
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
