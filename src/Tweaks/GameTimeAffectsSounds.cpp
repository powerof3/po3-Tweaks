#include "Tweaks.h"

//shift pitch with time mult
namespace Tweaks::GameTimeAffectsSounds
{
	// Default Sound
	struct BSSoundHandle_Play
	{
		static bool func(RE::BSSoundHandle& a_handle)
		{
			const auto soundID = a_handle.soundID;
			if (soundID == -1) {
				return false;
			}

			a_handle.state = RE::BSSoundHandle::AssumedState::kPlaying;

			if (const auto timeMult = RE::BSTimer::QGlobalTimeMultiplier(); timeMult != 1.0f) {
				a_handle.SetFrequency(timeMult);
			}

			Play(RE::BSAudioManager::GetSingleton(), soundID);

			return true;
		}
		static constexpr std::size_t size{ 0x33 };

		static void Install()
		{
			REL::Relocation<std::uintptr_t> func{ RELOCATION_ID(66355, 67616) };
			stl::asm_replace<BSSoundHandle_Play>(func.address());
		}

	private:
		static void Play(RE::BSAudioManager* a_manager, std::uint32_t a_soundID)
		{
			using func_t = decltype(&Play);
			static REL::Relocation<func_t> func{ RELOCATION_ID(66408, 67671) };
			func(a_manager, a_soundID);
		}
	};

	// Dialogue
	struct BSSoundHandle_PlayAfter
	{
		static bool func(RE::BSSoundHandle& a_handle, std::uint32_t a_delay)
		{
			const auto soundID = a_handle.soundID;
			if (soundID == -1) {
				return false;
			}

			a_handle.state = RE::BSSoundHandle::AssumedState::kPlaying;

			if (const auto timeMult = RE::BSTimer::QGlobalTimeMultiplier(); timeMult != 1.0f) {
				a_handle.SetFrequency(timeMult);
			}

			PlayAfter(RE::BSAudioManager::GetSingleton(), soundID, a_delay);

			return true;
		}
		static constexpr std::size_t size{ 0x46 };

		static void Install()
		{
			REL::Relocation<std::uintptr_t> func{ RELOCATION_ID(66356, 67617) };
			stl::asm_replace<BSSoundHandle_PlayAfter>(func.address());
		}

	private:
		static void PlayAfter(RE::BSAudioManager* a_manager, std::int32_t a_soundID, std::uint32_t a_delay)
		{
			using func_t = decltype(&PlayAfter);
			static REL::Relocation<func_t> func{ RELOCATION_ID(66409, 67672) };
			func(a_manager, a_soundID, a_delay);
		}
	};

	void Install()
	{
		BSSoundHandle_Play::Install();
		BSSoundHandle_PlayAfter::Install();

		logger::info("\t\tInstalled sound - time sync tweak"sv);
	}
}
