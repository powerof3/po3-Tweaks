#include "Experimental.h"
#include "Settings.h"

void Experimental::Install()
{
	const auto& experimental = Settings::GetSingleton()->GetExperimental();

	logger::info("{:*^30}", "EXPERIMENTAL"sv);

	ScriptSpeedup::Install();

	if (experimental.orphanedAEFix) {
		CleanupOrphanedActiveEffects::Install();
	}

	if (experimental.updateGameTimers) {
		UpdateGameTimers::Install();
	}

	ModifySuspendedStackFlushTimeout::Install();
}
