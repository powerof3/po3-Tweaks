#include "Fixes.h"

//fixes z buffer flag for non-detect life shaders
namespace Fixes::EffectShaderZBuffer
{
	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL_ID(501401, 360087), 0x1C };

		constexpr std::uint8_t zeroes[] = { 0x0, 0x0, 0x0, 0x0 };
		REL::safe_write(target.address(), zeroes, 4);

		logger::info("\t\tInstalled effect shader z buffer fix"sv);
	}
}
