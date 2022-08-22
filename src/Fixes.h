#pragma once

namespace Fixes
{
	namespace PreLoad
	{
		void Install(std::uint32_t a_skse_version);
	}

	namespace PostLoad
	{
		void Install();
	}

	namespace DataLoaded
	{
		void Install();
	}
}

namespace Fixes
{
	namespace AttachLightHitEffectCrash
	{
		void Install();
	}

	namespace CacheFormEditorIDs
	{
		void Install();
	}

	namespace CombatDialogue
	{
		void Install();
	}

#ifdef SKYRIM_VR
	namespace CrosshairRefEventVR
	{
		void Install();
	}
#endif

	namespace DistantRefLoadCrash
	{
		void Install();
	}

	namespace EffectShaderZBuffer
	{
		void Install();
	}

	namespace FlagSpellsAsNoAbsorb
	{
		void Install();
	}

	namespace FlagStolenProduce
	{
		void Install();
	}

	namespace IsFurnitureAnimTypeForFurniture
	{
		void Install();
	}

	namespace MapMarkerPlacement
	{
		void Install();
	}

	namespace OffensiveSpellAI
	{
		template <class T>
		class CheckShouldEquip
		{
		public:
			static void Install(REL::ID a_vtable_id)
			{
				REL::Relocation<std::uintptr_t> vtbl{ a_vtable_id };
				func = vtbl.write_vfunc(0xF, thunk);
			}

		private:
			static bool thunk(RE::CombatInventoryItemMagicT<T, RE::CombatMagicCasterOffensive>* a_this, RE::CombatController* a_controller)
			{
				auto result = func(a_this, a_controller);
				if (result) {
					const auto target = a_controller->handleCount ?
					                        a_controller->cachedTarget :
                                            a_controller->targetHandle.get();

					return target && RE::CombatMagicCaster::CheckTargetValid(a_controller, target.get(), a_this);
				}
				return result;
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		void Install();
	}

	namespace ProjectileRange
	{
		void Install();
	}

	namespace ReapplySpellsOnLoad::Added
	{
		void Install();
	}

	namespace ReapplySpellsOnLoad::OnDeath
	{
		void Install();
	}

	namespace RestoreCantTakeBook
	{
		void Install();
	}

	namespace RestoreJumpingBonus
	{
		void Install();
	}

	namespace SkinnedDecalDelete
	{
		void Install();
	}

	namespace ToggleCollision
	{
		void Install();
	}

	namespace ToggleGlobalAI
	{
		void Install();
	}

	namespace UnderWaterCamera
	{
		void Install();
	}

	namespace UseFurnitureInCombat
	{
		void Install();
	}
}
