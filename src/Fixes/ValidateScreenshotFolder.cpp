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

	bool is_subpath(const std::filesystem::path& a_path, const std::filesystem::path& a_base)
	{
		const auto mismatch_pair = std::mismatch(a_path.begin(), a_path.end(), a_base.begin(), a_base.end());
		return mismatch_pair.second == a_base.end();
	}

	void Install()
	{
		if (auto setting = RE::GetINISetting("sScreenShotBaseName:Display")) {
			try {
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
					RE::ConsoleLog::GetSingleton()->Print(std::format("[po3 Tweaks] Defaulting to {} folder\n", gameDirectory.string()).c_str());
					if (emptyPath) {
						logger::info("\t\tValidated screenshot location ({})"sv, screenshotFolder.string());
					} else {
						logger::info("\t\tValidated screenshot location ({} -> {})"sv, screenshotFolder.string(), gameDirectory.string());
					}
				} else {
					RE::ConsoleLog::GetSingleton()->Print(std::format("[po3 Tweaks] Screenshot folder ({}) validated successfully\n", screenshotFolder.string()).c_str());
					logger::info("\t\tValidated screenshot location ({})"sv, screenshotFolder.string());
				}
			} catch (std::exception& e) {
				logger::info("\t\tUnable to validate screenshot path (error: {})"sv, e.what());
			} catch (...) {
				logger::info("\t\tUnable to validate screenshot path (unknown error)"sv);
			}
		}
	}
}
