#include "Fixes.h"

//Bandaid fix for SetAlpha not working properly for first person
namespace Fixes::FirstPersonAlpha
{
	struct FirstPersonAlpha
	{
		static void Install()
		{
			REL::Relocation<std::uintptr_t> target{ REL_ID(37777, 38722), 0x55 };

			auto& trampoline = SKSE::GetTrampoline();
			SKSE::AllocTrampoline(14);

			_SetFPAlpha = trampoline.write_call<6>(target.address(), SetFPAlpha);
		}

	private:
		static RE::NiAVObject* SetFPAlpha(RE::PlayerCharacter* player, float alpha_value)
		{
			// fade == false -> alpha_value = 2 to 3
			// fade == true -> alpha_value = 0 to 1
			if (alpha_value >= 2) {
				alpha_value -= 2;
			}
			player->Get3D(true)->UpdateMaterialAlpha(alpha_value, false);
			return nullptr;  // Return null so that the original fade function for 1st person doesn't execute
		}
		static inline REL::Relocation<decltype(SetFPAlpha)> _SetFPAlpha;
	};

	void Install()
	{
		FirstPersonAlpha::Install();
		logger::info("\t\tInstalled first person alpha fix"sv);
	}
}
