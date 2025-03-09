#include "Cache.h"
#include "Fixes.h"
#include "Settings.h"

namespace Fixes::MagicItemFindKeywordFunctorCrash
{
	struct MatchCondition
	{
		static bool thunk(RE::MagicItemFindKeywordFunctor* a_this, RE::Effect* a_effect)
		{
			return a_effect && a_effect->baseEffect && a_effect->baseEffect->HasKeyword(a_this->keyword);
		}
		[[maybe_unused]] static inline REL::Relocation<decltype(thunk)> func;
		static inline constexpr std::size_t                             idx = 0x2;
	};

	void Install()
	{
		stl::write_vfunc<RE::MagicItemFindKeywordFunctor, MatchCondition>();
		logger::info("\t\tInstalled MagicItemFindKeywordFunctor crash fix"sv);
	}
}
