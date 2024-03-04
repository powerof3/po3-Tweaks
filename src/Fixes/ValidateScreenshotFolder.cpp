#include "Fixes.h"
#include "Settings.h"

//validates default screenshot path
//defaults to game dir/Screenshots if path doesn't exist

namespace Fixes::ValidateScreenshotFolder
{
	bool has_root_directory(const std::filesystem::path& a_path)
	{
		auto path = a_path.string();
		return path.contains(":\\") || path.contains(":/");
	}

	void Install()
	{
		if (auto setting = RE::GetINISetting("sScreenShotBaseName:Display")) {
			std::filesystem::path screenshotFolder;
			std::string           newBaseName;

			auto folder = setting->GetString();
			bool emptyPath = string::is_empty(folder);

			if (emptyPath) {
				screenshotFolder = std::filesystem::current_path();
				newBaseName = "Screenshot";
			} else {
				screenshotFolder = folder;
				screenshotFolder.make_preferred();

				if (has_root_directory(screenshotFolder)) {
					if (!std::filesystem::exists(screenshotFolder)) {
						newBaseName = "Screenshot";
					}
				} else {
					// folder will be generated in skyrim root					
					screenshotFolder = std::filesystem::current_path() /= screenshotFolder;
					screenshotFolder.remove_filename();
				}
			}

			if (!newBaseName.empty()) {
				//to-do: port this to clib
				if (setting->data.s) {
					RE::free(setting->data.s);
				}

				std::size_t strLen = newBaseName.length() + 1;
				setting->data.s = RE::malloc<char>(strLen);
				std::memcpy(setting->data.s, newBaseName.c_str(), sizeof(char) * strLen);

				if (emptyPath) {
					RE::ConsoleLog::GetSingleton()->Print("[po3 Tweaks] sScreenShotBaseName:Display ini setting is empty");
				} else {
					RE::ConsoleLog::GetSingleton()->Print(std::format("[po3 Tweaks] Screenshot folder ({}) does not exist", screenshotFolder.string()).c_str());
				}
				RE::ConsoleLog::GetSingleton()->Print(std::format("[po3 Tweaks] Defaulting to {} folder\n", std::filesystem::current_path().string()).c_str());
				if (emptyPath) {
					logger::info("\t\tValidated screenshot location ({})"sv, screenshotFolder.string());
				} else {
					logger::info("\t\tValidated screenshot location ({} -> {})"sv, screenshotFolder.string(), std::filesystem::current_path().string());
				}
			} else {
				RE::ConsoleLog::GetSingleton()->Print(std::format("[po3 Tweaks] Screenshot folder ({}) validated successfully\n", screenshotFolder.string()).c_str());
				logger::info("\t\tValidated screenshot location ({})"sv, screenshotFolder.string());
			}
		}
	}
}
