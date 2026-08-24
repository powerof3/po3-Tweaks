#include "Compatibility.h"

namespace Compatibility
{
	namespace ScrambledBugs
	{
		void DoCheck()
		{
			const REL::Relocation<std::uintptr_t> attachHitEffectArt_ID{ RELOCATION_ID(37804, 38753), 0x6F };

			if (REL::Pattern<"24 F8">().match(attachHitEffectArt_ID.address())) {
				REX::INFO("Scrambled Bugs : AttachHitEffect patch is enabled");
				attachHitEffectArt = true;
			} else {
				REX::INFO("Scrambled Bugs : AttachHitEffect patch not enabled or found");
			}
		}
	}

	void DoCheck()
	{
		ScrambledBugs::DoCheck();
	}
}
