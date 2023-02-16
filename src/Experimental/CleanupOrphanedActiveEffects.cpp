#include "Experimental.h"

//cleanup orphan active effects when character has missing ability perks that are referenced in the save
namespace Experimental::CleanupOrphanedActiveEffects
{
	struct detail
	{
		static void init_ability_perk_map(std::map<RE::SpellItem*, std::set<RE::BGSPerk*>>& a_map)
		{
			if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
				for (const auto& perk : dataHandler->GetFormArray<RE::BGSPerk>()) {
					for (const auto& entry : perk->perkEntries) {
						if (entry && entry->GetType() == RE::PERK_ENTRY_TYPE::kAbility) {
							if (const auto abilityEntry = static_cast<RE::BGSAbilityPerkEntry*>(entry); abilityEntry && abilityEntry->ability) {
								a_map[abilityEntry->ability].insert(perk);
							}
						}
					}
				}
			}
		}
	};

	struct LoadGame
	{
		static void thunk(RE::Character* a_this, std::uintptr_t a_buf)
		{
			func(a_this, a_buf);

			if (a_this && !a_this->IsPlayerRef()) {
				static std::map<RE::SpellItem*, std::set<RE::BGSPerk*>> abilityPerkMap;
				if (abilityPerkMap.empty()) {
					detail::init_ability_perk_map(abilityPerkMap);
				}

				auto& addedSpells = a_this->addedSpells;
				for (auto it = addedSpells.begin(); it != addedSpells.end();) {
					bool result = false;
					if (auto spell = *it; spell && spell->GetSpellType() == RE::MagicSystem::SpellType::kAbility) {
						if (abilityPerkMap.contains(spell)) {
							const auto base = a_this->GetActorBase();
							if (base && !std::ranges::any_of(abilityPerkMap[spell], [&](const auto& perk) {
									return base->GetPerkIndex(perk).has_value();
								})) {
								result = true;
							}
						}
					}
					if (result) {
						it = addedSpells.erase(it);
					} else {
						++it;
					}
				}
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static inline constexpr std::size_t idx{ 0x0F };
	};

	void Install()
	{
		stl::write_vfunc<RE::Character, LoadGame>();

		logger::info("\t\tInstalled orphan AE cleanup fix"sv);
	}
}
