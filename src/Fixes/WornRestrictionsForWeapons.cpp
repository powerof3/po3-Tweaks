#include "Fixes.h"

//make weapon enchantments support worn restriction list
//adapted from: https://github.com/Exit-9B/AmmoEnchanting/blob/main/src/Ext/EnchantConstructMenu.cpp

namespace Fixes::WornRestrictionsForWeapons
{
	struct detail
	{
		static bool GetCompatibleRestrictions(
			RE::CraftingSubMenus::EnchantConstructMenu::ItemChangeEntry*  a_item,
			RE::CraftingSubMenus::EnchantConstructMenu::EnchantmentEntry* a_effect)
		{
			if (!a_item || !a_item->data || !a_item->data->object || !a_effect || !a_effect->data) {
				return true;
			}

			auto wornRestrictionList = a_effect->data->data.wornRestrictions;
			auto keywordForm = a_item->data->object->As<RE::BGSKeywordForm>();

			if (!wornRestrictionList) {
				return true;
			}

			bool valid = false;

			keywordForm->ForEachKeyword([&](auto* keyword) {
				if (wornRestrictionList->HasForm(keyword)) {
					valid = true;
					return RE::BSContainer::ForEachResult::kStop;
				}
				return RE::BSContainer::ForEachResult::kContinue;
			});

			return valid;
		}
	};

	struct CanSelectEnchantmentEntry
	{
		static bool func(RE::CraftingSubMenus::EnchantConstructMenu* a_this, std::uint32_t a_index, bool a_showNotification)
		{
			if (a_index >= a_this->listEntries.size()) {
				return false;
			}

			using FilterFlag = RE::CraftingSubMenus::EnchantConstructMenu::FilterFlag;

			bool  compatibleRestrictions = true;
			auto& entry = a_this->listEntries[a_index];

			switch (entry->filterFlag.get()) {
			case FilterFlag::EnchantArmor:
			case FilterFlag::EnchantWeapon:
				{
					for (auto& effect : a_this->selected.effects) {
						const auto item = static_cast<RE::CraftingSubMenus::EnchantConstructMenu::ItemChangeEntry*>(entry.get());
						compatibleRestrictions &= detail::GetCompatibleRestrictions(item, effect.get());
					}
				}
				break;
			case FilterFlag::EffectWeapon:
			case FilterFlag::EffectArmor:
				{
					const auto item = a_this->selected.item.get();
					const auto effect = static_cast<RE::CraftingSubMenus::EnchantConstructMenu::EnchantmentEntry*>(entry.get());
					compatibleRestrictions = detail::GetCompatibleRestrictions(item, effect);
				}
				break;
			}

			if (!compatibleRestrictions) {
				if (a_showNotification) {
					static const auto setting = RE::GameSettingCollection::GetSingleton()->GetSetting("sEnchantArmorIncompatible");
					if (setting) {
						RE::DebugNotification(setting->GetString());
					}
				}
				return false;
			}

			switch (entry->filterFlag.get()) {
			case FilterFlag::EnchantWeapon:
				{
					if (!a_this->selected.effects.empty()) {
						return a_this->selected.effects[0]->filterFlag == FilterFlag::EffectWeapon;
					}
				}
				break;
			case FilterFlag::EnchantArmor:
				{
					if (!a_this->selected.effects.empty()) {
						return a_this->selected.effects[0]->filterFlag == FilterFlag::EffectArmor;
					}
				}
				break;
			case FilterFlag::EffectWeapon:
				{
					if (a_this->selected.item) {
						return a_this->selected.item->filterFlag == FilterFlag::EnchantWeapon;
					}
				}
				break;
			case FilterFlag::EffectArmor:
				{
					if (a_this->selected.item) {
						return a_this->selected.item->filterFlag == FilterFlag::EnchantArmor;
					}
				}
				break;
			default:
				break;
			}

			return true;
		}
		static inline constexpr std::size_t size{ OFFSET(0x27D, 0x1E9) };
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> func{ RELOCATION_ID(50569, 51461) };
		stl::asm_replace<CanSelectEnchantmentEntry>(func.address());

		logger::info("\t\tInstalled worn restrictions for weapons patch"sv);
	}
}
