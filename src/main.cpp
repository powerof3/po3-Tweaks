#include "Fixes.h"
#include "Patches.h"
#include "Settings.h"

void MessageHandler(SKSE::MessagingInterface::Message* a_message)
{
	switch (a_message->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		{
			const auto settings = Settings::GetSingleton();
			if (settings->dynamicSnowMat) {
				settings->LoadSnowyRegions();
			}
			if (settings->noConjurationAbsorb) {
				logger::info("Installed no conjuration spell absorb fix");
				SpellNoAbsorb::Fix();
			}
		}
		break;
	default:
		break;
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= "po3_Tweaks.log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S] [%l] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	logger::info("loaded");

	SKSE::Init(a_skse);

	Settings::GetSingleton()->Load();

	SKSE::AllocTrampoline(140);

	Fixes::Fix();
	Patches::Patch();

	auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener(MessageHandler);

	return true;
}
