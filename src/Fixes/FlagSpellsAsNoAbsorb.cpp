#include "Fixes.h"
#include "Settings.h"

//flag conjuration spell/hostile spells as no-absorb
//implements tweak as well

namespace Fixes::FlagSpellsAsNoAbsorb
{
	void Install()
	{
		using Archetype = RE::EffectArchetypes::ArchetypeID;
		using SpellFlag = RE::SpellItem::SpellFlag;

		const auto settings = Settings::GetSingleton();

		const auto noConj = settings->GetFixes().noConjurationAbsorb;
		const auto noHostile = settings->GetTweaks().noHostileAbsorb;

		if (!noConj && !noHostile) {
			return;
		}

		if (const auto dataHandler = RE::TESDataHandler::GetSingleton()) {
			constexpr auto is_conjuration = [](const auto& effect) {
				const auto baseEffect = effect ? effect->baseEffect : nullptr;
				return baseEffect && baseEffect->HasArchetype(Archetype::kSummonCreature);
			};

			constexpr auto is_non_hostile = [](const auto& effect) {
				const auto baseEffect = effect ? effect->baseEffect : nullptr;
				return baseEffect && !baseEffect->IsHostile() && !baseEffect->IsDetrimental();
			};

			for (const auto& spell : dataHandler->GetFormArray<RE::SpellItem>()) {
				if (spell && spell->data.flags.none(SpellFlag::kNoAbsorb)) {
					if (noConj && std::ranges::any_of(spell->effects, is_conjuration) || noHostile && std::ranges::all_of(spell->effects, is_non_hostile)) {
						spell->data.flags.set(SpellFlag::kNoAbsorb);
					}
				}
			}
		}
	}
}
