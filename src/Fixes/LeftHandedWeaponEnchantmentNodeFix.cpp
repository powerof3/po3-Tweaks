#include "Fixes.h"

//use XMPSE nodes for left handed weapon enchantment
namespace Fixes::LeftHandedWeaponEnchantmentNodeFix
{
	struct detail
	{
		static std::string_view GetWeaponNode(RE::TESObjectWEAP* a_weapon)
		{
			switch (a_weapon->weaponData.animationType.get()) {
			case RE::WEAPON_TYPE::kOneHandSword:
				return "WeaponSwordLeft";
			case RE::WEAPON_TYPE::kOneHandDagger:
				return "WeaponDaggerLeft";
			case RE::WEAPON_TYPE::kOneHandAxe:
				return "WeaponAxeLeft";
			case RE::WEAPON_TYPE::kOneHandMace:
				return "WeaponMaceLeft";
			default:
				return "Shield"sv;
			}
		}
	};

	struct GetTorchNode
	{
		static RE::NiAVObject* func(RE::AIProcess* a_this, const RE::BSTSmartPointer<RE::BipedAnim>& a_biped)
		{
			if (a_this->middleHigh && a_biped) {
				auto weapEntry = a_this->GetCurrentWeapon(true);
				if (auto weapon = weapEntry && weapEntry->object ? weapEntry->object->As<RE::TESObjectWEAP>() : nullptr) {
					return a_biped->root->GetObjectByName(detail::GetWeaponNode(weapon));
				}
				return a_biped->root->GetObjectByName(RE::FixedStrings::GetSingleton()->shield);
			}

			return nullptr;
		}
		static constexpr std::size_t size{ 0x4E };
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> func{ RELOCATION_ID(38799, 39829) };
		stl::asm_replace<GetTorchNode>(func.address());
	}
}
