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
