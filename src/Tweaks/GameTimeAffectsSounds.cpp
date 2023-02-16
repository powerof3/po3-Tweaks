#include "Tweaks.h"

//shift pitch with time mult
namespace Tweaks::GameTimeAffectsSounds
{
	struct detail
	{
		static bool PlaySoundHandle(RE::BSSoundHandle& a_soundHandle, std::function<void(std::int32_t a_soundID)> func)
		{
			const auto soundID = a_soundHandle.soundID;
			if (soundID == -1) {
				return false;
			}

			a_soundHandle.state = RE::BSSoundHandle::AssumedState::kPlaying;

			if (const auto timeMult = RE::BSTimer::GetCurrentGlobalTimeMult(); timeMult != 1.0f) {
				a_soundHandle.SetFrequency(timeMult);
			}

			func(soundID);

			return true;
		}
	};

	struct DefaultSound
	{
		static bool func(RE::BSSoundHandle& a_handle)
		{
			return detail::PlaySoundHandle(a_handle, [&](std::int32_t a_soundID) {
				Play(RE::BSAudioManager::GetSingleton(), a_soundID);
			});
		}
		static inline constexpr std::size_t size{ 0x33 };

		static void Install()
		{
			REL::Relocation<std::uintptr_t> func{ REL_ID(66355, 67616) };
			stl::asm_replace<DefaultSound>(func.address());  //BSSoundHandle::PlaySound
		}

	private:
		static void Play(RE::BSAudioManager* a_manager, std::int32_t a_soundID)
		{
			using func_t = decltype(&Play);
			REL::Relocation<func_t> func{ REL_ID(66408, 67671) };
			return func(a_manager, a_soundID);
		}
	};

	struct Dialogue
	{
		static bool func(RE::BSSoundHandle& a_handle, std::uint32_t a_unk02)
		{
			return detail::PlaySoundHandle(a_handle, [&](std::int32_t a_soundID) {
				PlayAfter(RE::BSAudioManager::GetSingleton(), a_soundID, a_unk02);
			});
		}
		static inline constexpr std::size_t size{ 0x46 };

		static void Install()
		{
			REL::Relocation<std::uintptr_t> func{ REL_ID(66356, 67617) };
			stl::asm_replace<Dialogue>(func.address());  //BSSoundHandle::PlaySound3D
		}

	private:
		static void PlayAfter(RE::BSAudioManager* a_manager, std::int32_t a_soundID, std::uint32_t a_unk03)
		{
			using func_t = decltype(&PlayAfter);
			REL::Relocation<func_t> func{ REL_ID(66409, 67672) };
			return func(a_manager, a_soundID, a_unk03);
		}
	};

	void Install()
	{
		DefaultSound::Install();
		Dialogue::Install();

		logger::info("\t\tInstalled sound - time sync tweak"sv);
	}
}
