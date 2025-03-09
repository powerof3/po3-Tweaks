#include "Tweaks.h"

//print screenshot notification to console
namespace Tweaks::ScreenshotToConsole
{
	struct DebugNotification
	{
		static void thunk(const char* a_notification, [[maybe_unused]] const char* a_soundToPlay, [[maybe_unused]] bool a_cancelIfAlreadyQueued)
		{
			if (const auto log = RE::ConsoleLog::GetSingleton()) {
				log->Print("%s", a_notification);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(35882, 36853), OFFSET(0xA8, 0x9E) };
		stl::write_thunk_call<DebugNotification>(target.address());

		logger::info("\t\tInstalled screenshot to console tweak"sv);
	}
}
