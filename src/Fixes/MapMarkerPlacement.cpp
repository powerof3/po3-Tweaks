#include "Fixes.h"

//fixes not being able to place markers near POI when fast travel is disabled
namespace Fixes::MapMarkerPlacement
{
	struct IsFastTravelEnabled
	{
		static bool thunk(RE::PlayerCharacter* a_this, bool a_hideNotification)
		{
			const auto enabled = func(a_this, a_hideNotification);
			if (!enabled) {
				if (const auto mapMenu = RE::UI::GetSingleton()->GetMenu<RE::MapMenu>(); mapMenu) {
					mapMenu->PlaceMarker();
				}
			}
			return enabled;
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(52208, 53095), OFFSET_3(0x2C5, 0x328, 0x358) };
		stl::write_thunk_call<IsFastTravelEnabled>(target.address());

		logger::info("\t\tInstalled map marker placement fix"sv);
	}
}
