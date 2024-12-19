#include "Fixes.h"

//fixes combat dialogue
namespace Fixes::CombatDialogue
{
	struct SayCombatDialogue
	{
		static bool thunk(std::uintptr_t a_combatDialogueManager, RE::Actor* a_speaker, RE::Actor* a_target, RE::DIALOGUE_TYPE a_type, RE::DIALOGUE_DATA::Subtype a_subtype, bool a_ignoreSpeakingDone, RE::CombatController* a_combatController)
		{
			if (a_subtype == RE::DIALOGUE_DATA::Subtype::kLostToNormal && a_target && a_target->IsDead()) {
				const auto combatGroup = a_speaker ? a_speaker->GetCombatGroup() : nullptr;
				if (combatGroup && combatGroup->searchState == 0) {
					a_subtype = RE::DIALOGUE_DATA::Subtype::kCombatToNormal;
				}
			}
			return func(a_combatDialogueManager, a_speaker, a_target, a_type, a_subtype, a_ignoreSpeakingDone, a_combatController);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(43571, 44803), 0x135 };
		stl::write_thunk_call<SayCombatDialogue>(target.address());

		logger::info("\t\tInstalled combat dialogue fix"sv);
	}
}
