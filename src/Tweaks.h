#pragma once

namespace Tweaks
{
	namespace PostLoad
	{
		void Install();
	}

	namespace DataLoaded
	{
		void Install();
	}
}

namespace Tweaks
{
	namespace DynamicSnowMaterial
	{
		void Install();
	}

	namespace FactionStealing
	{
		void Install();
	}

	namespace GameTimeAffectsSounds
	{
		void Install();
	}

	namespace GrabbingIsStealing
	{
		void Install();
	}

	namespace LoadDoorPrompt
	{
		void Install();
	}

	namespace NoCheatMode
	{
		void Install(std::uint32_t a_type);
	}

	namespace NoCritSneakMessages
	{
		void Install(std::uint32_t a_type);
	}

	namespace NoPoisonPrompt
	{
		void Install(std::uint32_t a_type);
	}

	namespace NoRipplesOnHover
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

#ifdef SKYRIMVR
	namespace RememberLockPickAngleVR
	{
		void Install();
	}
#endif

	namespace ScreenshotToConsole
	{
		void Install();
	}

	namespace SilentSneakPowerAttacks
	{
		void Install();
	}

	namespace SitToWait
	{
		void Install();
	}

	namespace VoiceModulation
	{
		void Install();
	}
}
