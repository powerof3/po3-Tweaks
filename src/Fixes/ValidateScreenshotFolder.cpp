#include "Fixes.h"
#include "Settings.h"

//validates default screenshot path
//defaults to game dir/Screenshots if path doesn't exist

namespace Fixes::ValidateScreenshotFolder
{
	RE::Setting* set_ini_string(RE::Setting* a_setting, const char* a_str)
	{
		using func_t = decltype(&set_ini_string);
		REL::Relocation<func_t> func{ RELOCATION_ID(73882, 75619) };
		return func(a_setting, a_str);
	}

	bool has_root_directory(const std::filesystem::path& a_path)
	{
		auto path = a_path.string();
		return path.contains(":\\") || path.contains(":/");
	}

	bool is_subpath(const std::filesystem::path& a_path, const std::filesystem::path& a_base)
	{
		const auto mismatch_pair = std::mismatch(a_path.begin(), a_path.end(), a_base.begin(), a_base.end());
		return mismatch_pair.second == a_base.end();
	}

	void Install()
	{
		if (auto setting = RE::GetINISetting("sScreenShotBaseName:Display")) {
			std::filesystem::path gameDirectory = std::filesystem::current_path();
			gameDirectory.make_preferred();

			std::filesystem::path screenshotFolder{};
			std::string           newBaseName{};

			auto folder = setting->GetString();
			bool emptyPath = string::is_empty(folder);

			if (emptyPath) {
				screenshotFolder = gameDirectory;
				newBaseName = "Screenshot";
			} else {
				screenshotFolder = folder;
				screenshotFolder.make_preferred();

				if (has_root_directory(screenshotFolder)) {
					if (!is_subpath(screenshotFolder, gameDirectory) && !std::filesystem::exists(screenshotFolder)) {
						newBaseName = "Screenshot";
					}
				} else {
					// folder will be generated in skyrim root
					screenshotFolder = gameDirectory /= screenshotFolder;
					screenshotFolder.remove_filename();
				}
			}

			if (!newBaseName.empty()) {
				set_ini_string(setting, newBaseName.c_str());

				if (emptyPath) {
					RE::ConsoleLog::GetSingleton()->Print("[po3 Tweaks] sScreenShotBaseName:Display ini setting is empty");
				} else {
					RE::ConsoleLog::GetSingleton()->Print(std::format("[po3 Tweaks] Screenshot folder ({}) does not exist", screenshotFolder.string()).c_str());
				}
				RE::ConsoleLog::GetSingleton()->Print(std::format("[po3 Tweaks] Defaulting to {} folder\n", gameDirectory.string()).c_str());
				if (emptyPath) {
					logger::info("\t\tValidated screenshot location ({})"sv, screenshotFolder.string());
				} else {
					logger::info("\t\tValidated screenshot location ({} -> {})"sv, screenshotFolder.string(), gameDirectory.string());
				}
			} else {
				logger::info("\t\tValidated screenshot location ({})"sv, screenshotFolder.string());
			}
		}
	}
}
