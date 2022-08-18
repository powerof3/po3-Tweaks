#include "Tweaks.h"

//disable player shouting when power attacking in sneak mode
namespace Tweaks::SilentSneakPowerAttacks
{
	struct SayCombatDialogue
	{
		static bool thunk(std::uintptr_t a_combatDialogueManager, RE::Actor* a_speaker, RE::Actor* a_target, RE::DIALOGUE_TYPE a_type, RE::DIALOGUE_DATA::Subtype a_subtype, bool a_ignoreSpeakingDone, RE::CombatController* a_combatController)
		{
			if (a_subtype == RE::DIALOGUE_DATA::Subtype::kPowerAttack && a_speaker->IsSneaking()) {
				return false;
			}
			return func(a_combatDialogueManager, a_speaker, a_target, a_type, a_subtype, a_ignoreSpeakingDone, a_combatController);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL_ID(39577, 40663), 0xAA };
		stl::write_thunk_call<SayCombatDialogue>(target.address());

		logger::info("Installed silent sneak power attack tweak"sv);
	}
}
