#include "Tweaks.h"

//suppress notifications
namespace Tweaks::NoCritSneakMessages
{
	void Install(std::uint32_t a_type)
	{
		REL::Relocation<std::uintptr_t> target{ REL_ID(37633, 38586) };

#ifdef SKYRIM_AE
		std::array id{
			std::make_pair(0x328, 0x33B),  //crit
			std::make_pair(0x3E9, 0x3FC),  //sneak
		};
#else
		std::array id{
			std::make_pair(0x20D, 0x220),  //crit
			std::make_pair(0x2D3, 0x2E6),  //sneak
		};
#endif

		if (a_type == 3) {
			for (const auto& [start, end] : id) {
				for (auto i = start; i < end; ++i) {
					REL::safe_write(target.address() + i, REL::NOP);
				}
			}
		} else {
			auto idx = a_type - 1;

		    const auto& [start, end] = id[idx];
			for (auto i = start; i < end; ++i) {
				REL::safe_write(target.address() + i, REL::NOP);
			}
		}

		logger::info("Installed crit/sneak message tweak"sv);
	}
}
