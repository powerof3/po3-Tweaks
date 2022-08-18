#include "Fixes.h"

//remove decals and decrement decal count immediately when queued for deletion
namespace Fixes::SkinnedDecalDelete
{
	struct RemoveItem
	{
		static RE::NiPointer<RE::BSTempEffect>*& thunk(
			RE::BSTArray<RE::NiPointer<RE::BSTempEffect>>& a_this, 
			RE::NiPointer<RE::BSTempEffect>*& a_return, 
			RE::NiPointer<RE::BSTempEffect>*& a_item)
		{
			auto& result = func(a_this, a_return, a_item);

			if (a_item && (*a_item)->initialized) {
				const auto decal = (*a_item)->As<RE::BSTempEffectGeometryDecal>();
				const auto decalNode = decal ? decal->decalNode : nullptr;
				if (decalNode && decalNode->parent) {
					decalNode->parent->DetachChild(decalNode.get());
					auto& count = RE::BGSDecalManager::GetSingleton()->skinDecalCount;
					if (count > 0) {
						--count;
					}
				}
			}

			return result;
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL_ID(15118, 15295), 0x12B };
		stl::write_thunk_call<RemoveItem>(target.address());

		logger::info("Installed skinned decal delete fix"sv);
	}
}
