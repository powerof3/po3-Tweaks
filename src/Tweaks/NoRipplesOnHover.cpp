#include "Tweaks.h"

//disables ripples for levitating creatures
namespace Tweaks::NoRipplesOnHover
{
	static inline constexpr std::string_view isLevitating{ "isLevitating"sv };

    struct GetCharController
	{
		static RE::bhkCharacterController* thunk(RE::AIProcess* a_currentProcess)
		{
			if (const auto user = a_currentProcess ? a_currentProcess->GetUserData() : nullptr; user) {
				bool levitating = false;
				if (user->GetGraphVariableBool(isLevitating, levitating) && levitating) {
					return nullptr;
				}
			}

			return func(a_currentProcess);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL_ID(36621, 37629), 0x35 };
		stl::write_thunk_call<GetCharController>(target.address());

		logger::info("Installed no ripples on hover tweak"sv);
	}
}
