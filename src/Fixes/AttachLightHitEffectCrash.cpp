#include "Fixes.h"

//fix BSFadeNode nullptr crash re: AttachLightHitEffectVisitor
namespace Fixes::AttachLightHitEffectCrash
{
	struct AttachLightHitEffectVisitor
	{
		static RE::BSContainer::ForEachResult func(RE::AttachLightHitEffectVisitor* a_this, RE::ReferenceEffect* a_hitEffect)
		{
			if (a_hitEffect->GetAttached()) {
				auto root = a_hitEffect->GetAttachRoot();
				if (const auto attachLightObj = root ?
				                                    root->GetObjectByName(RE::FixedStrings::GetSingleton()->attachLight) :  //crash here because no null check
				                                    nullptr) {
					root = attachLightObj;
				}
				if (root && root != a_this->attachRoot) {
					a_this->attachLightNode = root;
				}
				if (a_this->attachLightNode) {
					return RE::BSContainer::ForEachResult::kStop;
				}
			} else {
				a_this->allAttached = false;
			}
			return RE::BSContainer::ForEachResult::kContinue;
		}

#ifdef SKYRIM_AE
		//FixedStrings::GetSingleton() got inlined
		static inline constexpr std::size_t size{ 0xEC };
#else
		static inline constexpr std::size_t size{ 0x86 };
#endif
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> func{ REL_ID(33610, 34388) };
		stl::asm_replace<AttachLightHitEffectVisitor>(func.address());

		logger::info("Installed light attach crash fix"sv);
	}
}
