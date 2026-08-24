#include "Experimental.h"
#include "Settings.h"

void Experimental::Install()
{
	const auto& experimental = Settings::GetSingleton()->GetExperimental();

	REX::INFO("\t[EXPERIMENTAL]");

	ScriptSpeedup::Install();

	if (experimental.orphanedAEFix) {
		CleanupOrphanedActiveEffects::Install();
	}

	if (experimental.updateGameTimers) {
		UpdateGameTimers::Install();
	}

	ModifySuspendedStackFlushTimeout::Install();
}
