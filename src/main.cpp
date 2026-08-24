#include "Cache.h"
#include "Compatibility.h"
#include "Experimental.h"
#include "Fixes.h"
#include "Papyrus.h"
#include "Settings.h"
#include "Tweaks.h"

void MessageHandler(SKSE::MessagingInterface::Message* a_message)
{
	switch (a_message->type) {
	case SKSE::MessagingInterface::kPostLoad:
		{
			REX::INFO("{:*^50}", "POST LOAD PATCH"sv);

#ifndef SKYRIMVR
			Settings::GetSingleton()->Load();
#endif

			Fixes::PostLoad::Install();
			Tweaks::PostLoad::Install();

			Experimental::Install();
		}
		break;
	case SKSE::MessagingInterface::kPostPostLoad:
		{
			REX::INFO("{:*^50}", "PLUGIN COMPATIBILITY CHECK"sv);

			Compatibility::DoCheck();

			REX::INFO("{:*^50}", "POST POST LOAD PATCH"sv);

			Fixes::PostPostLoad::Install();
		}
		break;
	case SKSE::MessagingInterface::kDataLoaded:
		{
			REX::INFO("{:*^50}", "DATA LOADED PATCH"sv);

			Fixes::DataLoaded::Install();
			Tweaks::DataLoaded::Install();
		}
		break;
	default:
		break;
	}
}

constexpr REL::Version MIN_ADDRESS_LIBRARY_V5_RUNTIME{ 1, 7, 99, 0 };

#ifdef SKYRIM_SUPPORT_AE
SKSE_PLUGIN_VERSION = []() {
	SKSE::PluginVersionData v;
	v.PluginVersion(REL::Version{ Version::MAJOR, Version::MINOR, Version::PATCH });
	v.PluginName("powerofthree's Tweaks");
	v.AuthorName("powerofthree");
	v.UsesAddressLibrary();
	v.UsesUpdatedStructs();
	v.CompatibleVersions({ SKSE::RUNTIME_SSE_LATEST });

	if constexpr (SKSE::RUNTIME_SSE_LATEST < MIN_ADDRESS_LIBRARY_V5_RUNTIME) {
		v.MinimumRequiredXSEVersion(REL::Version{ 2, 2, 5 });
	} else {
		v.MinimumRequiredXSEVersion(REL::Version{ 2, 3, 0 });
	}

	return v;
}();
#else
SKSE_PLUGIN_QUERY(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "powerofthree's Tweaks";
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		REX::CRITICAL("Loaded in editor, marking as incompatible");
		return false;
	}

	if (const auto ver = a_skse->RuntimeVersion(); ver < SKSE::RUNTIME_SSE_1_5_39) {
		REX::CRITICAL("Unsupported runtime version {}", ver);
		return false;
	}

	return true;
}
#endif

SKSE_PLUGIN_LOAD(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse, { .log = true,
						   .logName = Version::PROJECT.data(),
						   .trampoline = true,
						   .trampolineSize = 450 });

	const auto runtimeVersion = a_skse->RuntimeVersion();
	
	REX::INFO("Game version : {}", runtimeVersion);

	if constexpr (SKSE::RUNTIME_SSE_LATEST < MIN_ADDRESS_LIBRARY_V5_RUNTIME) {
		if (runtimeVersion >= MIN_ADDRESS_LIBRARY_V5_RUNTIME) {
			REX::FAIL(
				"You are using a newer version of Skyrim than this version of {} supports.\n"
				"Install the correct version of {} for your game version.\n"
				"Runtime: {}\n"
				"Supported: 1.6.1170 (Steam) / 1.6.1179 (GOG)",
				Version::PROJECT, runtimeVersion);
		}
	}

#ifdef SKYRIMVR
	try {
		Settings::GetSingleton()->Load();
	} catch (...) {
		logger::error("Exception caught when loading settings! Default settings will be used");
	}

	REX::INFO("{:*^50}", "PRELOAD PATCH"sv);

	Fixes::PreLoad::Install(a_skse->SKSEVersion());
#endif

	auto papyrus = SKSE::GetPapyrusInterface();
	papyrus->Register(Papyrus::Bind);

	auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener(MessageHandler);

	return true;
}
