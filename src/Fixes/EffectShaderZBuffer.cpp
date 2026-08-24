#include "Fixes.h"

//fixes z buffer flag for non-detect life shaders
namespace Fixes::EffectShaderZBuffer
{
	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(501401, 360087), 0x1C };

		constexpr std::uint8_t zeroes[] = { 0x0, 0x0, 0x0, 0x0 };
		REL::WriteSafe(target.address(), zeroes, 4);

		REX::INFO("\t\tInstalled effect shader z buffer fix"sv);
	}
}
