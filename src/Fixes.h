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

	namespace IsFurnitureAnimTypeForFurniture
	{
		void Install();
	}

	namespace MapMarkerPlacement
	{
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

	namespace UseFurnitureInCombat
	{
		void Install();
	}
}
