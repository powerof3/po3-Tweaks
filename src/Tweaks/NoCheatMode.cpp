#include "Tweaks.h"

//disables GodMode or ImmortalMode
namespace Tweaks::NoCheatMode
{
	struct GodMode
	{
		static bool func()
		{
			const auto log = RE::ConsoleLog::GetSingleton();
			if (log && RE::ConsoleLog::IsConsoleMode()) {
				log->Print("God Mode disabled");
			}
			return true;
		}
		static inline constexpr std::size_t size{ 0x4C };

		static void Install()
		{
			REL::Relocation<std::uintptr_t> func{ REL_ID(22339, 22814) };
			stl::asm_replace<GodMode>(func.address());
		}
	};

	struct ImmortalMode
	{
		static bool func()
		{
			const auto log = RE::ConsoleLog::GetSingleton();
			if (log && RE::ConsoleLog::IsConsoleMode()) {
				log->Print("Immortal Mode disabled");
			}
			return true;
		}
		static inline constexpr std::size_t size{ 0x4C };

		static void Install()
		{
			REL::Relocation<std::uintptr_t> func{ REL_ID(22340, 22815) };
			stl::asm_replace<ImmortalMode>(func.address());
		}
	};

	void Install(std::uint32_t a_type)
	{
		switch (a_type) {
		case 1:
			GodMode::Install();
			break;
		case 2:
			ImmortalMode::Install();
			break;
		case 3:
			{
				GodMode::Install();
				ImmortalMode::Install();
			}
			break;
		default:
			break;
		}

		logger::info("\t\tInstalled no cheat mode"sv);
	}
}
