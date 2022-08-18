#pragma once

namespace Experimental
{
	void Install();

	namespace CleanupOrphanedActiveEffects
	{
		void Install();
	}

	namespace ModifySuspendedStackFlushTimeout
	{
		void Install();
	}

	namespace ScriptSpeedup
	{
		void Install();
	}

	namespace UpdateGameTimers
	{
		void Install();
	}
}
