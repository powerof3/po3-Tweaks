#include "Experimental.h"

void Experimental::Install()
{
	const auto experimental = Settings::GetSingleton()->experimental;

	logger::info("{:*^30}", "EXPERIMENTAL"sv);

	const auto papyrus = SKSE::GetPapyrusInterface();
	papyrus->Register(Script::Speedup);

	if (experimental.orphanedAEFix) {
		CleanupOrphanedActiveEffects::Install();
	}

	if (experimental.updateGameTimers) {
		GameTimers::Install();
	}

	ModifySuspendedStackFlushTimeout::Install();
}
