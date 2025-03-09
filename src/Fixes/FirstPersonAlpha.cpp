#include "Fixes.h"

//Bandaid fix for SetAlpha not working properly for first person
namespace Fixes::FirstPersonAlpha
{
	struct FirstPersonAlpha
	{
		static RE::NiAVObject* thunk(RE::PlayerCharacter* a_player, float a_alpha)
		{
			// fade == false -> alpha_value = 2 to 3
			// fade == true -> alpha_value = 0 to 1
			if (a_alpha >= 2) {
				a_alpha -= 2;
			}
			a_player->Get3D(true)->UpdateMaterialAlpha(a_alpha, false);
			return nullptr;  // Return null so that the original fade function for 1st person doesn't execute
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(37777, 38722), 0x55 };
		stl::write_thunk_call<FirstPersonAlpha, 6>(target.address());

		logger::info("\t\tInstalled first person alpha fix"sv);
	}
}
