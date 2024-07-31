#include "Fixes.h"

//fix underwater effects not resetting when above water
namespace Fixes::UnderWaterCamera
{
	struct detail
	{
		static void UpdateUnderWaterVariables(RE::TESWaterSystem* a_manager, bool a_underWater, float a_waterHeight)
		{
			using func_t = decltype(&UpdateUnderWaterVariables);
			static REL::Relocation<func_t> func{ REL_ID(31409, 32216) };
			return func(a_manager, a_underWater, a_waterHeight);
		}
	};

	struct ProcessInWater
	{
		static bool thunk(RE::PlayerCharacter* a_actor, RE::hkpCollidable* a_collidable, float a_waterHeight, float a_deltaTime)
		{
			if (a_collidable->GetCollisionLayer() != RE::COL_LAYER::kCameraSphere) {
				return func(a_actor, a_collidable, a_waterHeight, a_deltaTime);
			}

			if (const auto cameraBody = RE::PlayerCamera::GetSingleton()->rigidBody; cameraBody) {
				float worldWaterHeight{ RE::NI_INFINITY };

				RE::hkVector4 position;
				cameraBody->GetPosition(position);

				const bool underWater = std::roundf(a_waterHeight) > std::roundf(position.quad.m128_f32[2]);  //doesn't really fix, inverts the bug
				if (underWater) {
					worldWaterHeight = a_waterHeight * RE::bhkWorld::GetWorldScaleInverse();
				}

				detail::UpdateUnderWaterVariables(RE::TESWaterSystem::GetSingleton(), underWater, worldWaterHeight);

				return underWater;
			}

			return false;
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static inline constexpr std::size_t idx{ 0x9C };
	};

	void Install()
	{
		stl::write_vfunc<RE::PlayerCharacter, ProcessInWater>();
		logger::info("\t\tInstalled underwater camera fix"sv);
	}
}
