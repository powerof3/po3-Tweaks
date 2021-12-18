#include "Experimental.h"

void Experimental::Install()
{
	const auto experimental = Settings::GetSingleton()->experimental;

	logger::info("{:*^30}", "EXPERIMENTAL"sv);

	const auto papyrus = SKSE::GetPapyrusInterface();
	papyrus->Register(::Script::Speedup);

	if (experimental.orphanedAEFix.value) {
		CleanupOrphanedActiveEffects::Install();
	}

	if (experimental.updateGameTimers.value) {
		GameTimers::Install();
	}
	if (experimental.removeFlushTimeout.value) {
		RemoveSuspendedStackFlushTimeout::Install();
	}
}
