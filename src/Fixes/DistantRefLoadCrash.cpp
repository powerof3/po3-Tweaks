#include "Fixes.h"

//fix BSFadeNode nullptr crash when loading distant refs
namespace Fixes::DistantRefLoadCrash
{
	struct LoadedVisibleDistantRef
	{
		static void func([[maybe_unused]] RE::TESObjectCELL* a_cell, const RE::TESObjectREFR* a_ref)
		{
			const auto root = a_ref->Get3D();
			const auto fadeNode = root ? root->AsFadeNode() : nullptr;

			if (fadeNode) {
#ifndef SKYRIMVR
				fadeNode->unk144 = 0;
#else
				fadeNode->unk16C = 0;
#endif
			}
		}
		static inline constexpr std::size_t size{ 0x2D };
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> func{ REL_ID(18642, 19116) };
		stl::asm_replace<LoadedVisibleDistantRef>(func.address());

		logger::info("Installed distant ref load crash fix"sv);
	}
}
