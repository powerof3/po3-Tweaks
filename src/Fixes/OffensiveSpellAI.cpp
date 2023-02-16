#include "Fixes.h"

//check spell condition validity before equipping offensive magic
namespace Fixes::OffensiveSpellAI
{
	namespace CombatMagicCaster
	{
		struct CheckStartCast
		{
			static bool thunk(RE::CombatMagicCasterOffensive* a_this, RE::CombatController* a_controller)
			{
				auto result = func(a_this, a_controller);
				if (result) {
					result = a_this->CheckTargetValid(a_controller);
				}
				return result;
			}
			static inline REL::Relocation<decltype(thunk)> func;

			static inline std::size_t idx{ 0x6 };
		};

		void Install()
		{
			stl::write_vfunc<RE::CombatMagicCasterOffensive, CheckStartCast>();
		}
	}

	namespace CombatInventoryMagic
	{
		void Install()
		{
			CheckShouldEquip<RE::CombatInventoryItemMagic>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemMagic_CombatMagicCasterOffensive_[0]);
			CheckShouldEquip<RE::CombatInventoryItemShout>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemShout_CombatMagicCasterOffensive_[0]);
			CheckShouldEquip<RE::CombatInventoryItemStaff>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemStaff_CombatMagicCasterOffensive_[0]);
			CheckShouldEquip<RE::CombatInventoryItemPotion>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemPotion_CombatMagicCasterOffensive_[0]);
			CheckShouldEquip<RE::CombatInventoryItemScroll>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemScroll_CombatMagicCasterOffensive_[0]);
		}
	}

	void Install()
	{
		CombatInventoryMagic::Install();
		CombatMagicCaster::Install();

		logger::info("\t\tInstalled offensive spell AI fix"sv);
	}
}
