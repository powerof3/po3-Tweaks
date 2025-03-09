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

	namespace PostPostLoad
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

	namespace BreathingSounds
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

#ifdef SKYRIMVR
	namespace CrosshairRefEventVR
	{
		void Install(std::uint32_t a_skse_version);
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

	namespace FirstPersonAlpha
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

	namespace MagicItemFindKeywordFunctorCrash
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

	namespace ReapplyAddedSpells
	{
		void Install();
	}

	namespace ReapplyNoDeathDispelSpells
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

	namespace ValidateScreenshotFolder
	{
		void Install();
	}

	namespace WornRestrictionsForWeapons
	{
		void Install();
	}
}
