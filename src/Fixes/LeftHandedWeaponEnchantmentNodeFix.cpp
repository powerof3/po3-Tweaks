#include "Fixes.h"

//use correct node for left handed weapon enchantment (XMPSE node in third person/SHIELD in first)
namespace Fixes::LeftHandedWeaponEnchantmentNodeFix
{
	struct GetTorchNode
	{
		static RE::NiAVObject* func(RE::AIProcess* a_this, const RE::BSTSmartPointer<RE::BipedAnim>& a_biped)
		{
			if (a_this->middleHigh && a_biped) {
				auto weapEntry = a_this->GetCurrentWeapon(true);
				if (auto weapon = weapEntry && weapEntry->object ? weapEntry->object->As<RE::TESObjectWEAP>() : nullptr) {
					RE::NiAVObject* node = nullptr;

					char buf[1024];
					weapon->GetNodeName(buf);
					RE::BSFixedString weaponNodeName(buf);

					RE::BSVisit::TraverseScenegraphObjects(a_biped->root, [&](RE::NiAVObject* a_obj) -> RE::BSVisit::BSVisitControl {
						if (a_obj->name == weaponNodeName && a_obj->parent && a_obj->parent->name != RE::FixedStrings::GetSingleton()->weapon) {
							node = a_obj->parent;
							return RE::BSVisit::BSVisitControl::kStop;
						}
						return RE::BSVisit::BSVisitControl::kContinue;
					});

					if (node) {
						return node;
					}
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
