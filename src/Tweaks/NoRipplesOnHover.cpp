#include "Tweaks.h"

//disables ripples for levitating creatures
namespace Tweaks::NoRipplesOnHover
{
	struct detail
	{
		static bool IsLevitatingOnWater(const RE::Character* a_character, const RE::hkpCollidable* a_collidable)
		{
			if (const auto colLayer = static_cast<RE::COL_LAYER>(a_collidable->broadPhaseHandle.collisionFilterInfo & 0x7F); colLayer == RE::COL_LAYER::kCharController) {
				if (bool levitating = false; a_character->GetGraphVariableBool(isLevitating, levitating) && levitating) {
					return true;
				}
			}
			return false;
		}
		static inline constexpr std::string_view isLevitating{ "isLevitating"sv };
	};

	struct ProcessInWater
	{
		struct Player
		{
			static bool thunk(RE::PlayerCharacter* a_actor, RE::hkpCollidable* a_collidable, float a_waterHeight, float a_deltaTime)
			{
				if (detail::IsLevitatingOnWater(a_actor, a_collidable)) {
					return false;
				}
				return func(a_actor, a_collidable, a_waterHeight, a_deltaTime);
			}
			static inline REL::Relocation<decltype(thunk)> func;

			static inline constexpr std::size_t idx{ 0x9C };
		};

		struct NPC
		{
			static bool thunk(RE::Character* a_actor, RE::hkpCollidable* a_collidable, float a_waterHeight, float a_deltaTime)
			{
				if (detail::IsLevitatingOnWater(a_actor, a_collidable)) {
					return false;
				}
				return func(a_actor, a_collidable, a_waterHeight, a_deltaTime);
			}
			static inline REL::Relocation<decltype(thunk)> func;

			static inline constexpr std::size_t idx{ 0x9C };
		};
	};

	void Install()
	{
		stl::write_vfunc<RE::PlayerCharacter, ProcessInWater::Player>();
		stl::write_vfunc<RE::Character, ProcessInWater::NPC>();

		logger::info("Installed no ripples on hover tweak"sv);
	}
}
