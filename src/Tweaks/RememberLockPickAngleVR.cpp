#include "Tweaks.h"

#ifdef SKYRIMVR
//Remember lock pick angle.
//Based on offsets discovered by OnlyIWeDo (https://www.nexusmods.com/skyrimspecialedition/mods/24543) and updated by Umgak (https://www.nexusmods.com/skyrimspecialedition/mods/26838)
namespace Tweaks::RememberLockPickAngleVR
{
	void Tweaks::RememberLockPickAngleVR::Install()
	{
		constexpr REL::ID LockpickBreakAddr{ 51093 };
		static REL::Relocation<std::uintptr_t> target{ LockpickBreakAddr, 0x242 };
		for (uintptr_t i = 0; i < 7; ++i) {
			REL::safe_write(target.address() + i, REL::NOP);
		}
		logger::info("Installed remember lock pick angle"sv);
	}
}
#endif
