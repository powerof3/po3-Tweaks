#include "Settings.h"
#include "Tweaks.h"

//custom load door prompts when exiting or entering
namespace Tweaks::LoadDoorPrompt
{
	struct detail
	{
		enum CELL_TYPE : std::uint32_t
		{
			kNone = 0,
			kExterior,
			kInterior
		};

		enum PROMPT_TYPE : std::uint32_t
		{
			kReplacePrompt = 1,
			kReplaceCellAndPrompt
		};

		static std::pair<CELL_TYPE, const char*> GetName(const char* a_cellName)
		{
			const auto crosshairPickData = RE::CrosshairPickData::GetSingleton();
#ifndef SKYRIMVR
			auto ref = crosshairPickData->target.get();
#else
			auto player = RE::PlayerCharacter::GetSingleton();
			auto hand = player->isRightHandMainHand ? RE::VR_DEVICE::kRightController : RE::VR_DEVICE::kLeftController;
			auto ref = crosshairPickData->target[hand].get();
#endif
			if (!ref) {
#ifndef SKYRIMVR
				ref = crosshairPickData->grabPickRef.get();
#else
				ref = crosshairPickData->grabPickRef[hand].get();
#endif
			}

			if (const auto cell = ref ? ref->GetSaveParentCell() : nullptr) {
				if (cell->IsInteriorCell()) {
					const auto linkedDoor = ref->extraList.GetTeleportLinkedDoor();
					const auto linkedRef = linkedDoor.get();
					const auto linkedCell = linkedRef ? linkedRef->GetSaveParentCell() : nullptr;

					if (linkedCell && linkedCell->IsExteriorCell()) {
						auto& [type, enter, exit] = Settings::GetSingleton()->GetTweaks().loadDoorPrompt;
						return { kInterior, type == kReplaceCellAndPrompt ?
												cell->GetName() :
												a_cellName };
					}
				}
				return { kExterior, a_cellName };
			}
			return { kNone, a_cellName };
		}

		static std::string GetDoorLabel(CELL_TYPE a_type, const char* a_default)
		{
			auto& [type, enter, exit] = Settings::GetSingleton()->GetTweaks().loadDoorPrompt;
			if (a_type == kExterior) {
				return enter;
			}
			if (a_type == kInterior) {
				return type == kReplaceCellAndPrompt ?
				           exit :
				           enter;
			}
			return a_default;
		}
	};

	struct Normal
	{
		static int thunk(RE::BSString* a_dest, const char* a_format, const char* a_openLbl, const char* a_cellName)
		{
			auto [result, cellName] = detail::GetName(a_cellName);
			const auto doorLabel = detail::GetDoorLabel(result, a_openLbl);

			return func(a_dest, a_format, doorLabel.c_str(), cellName);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct Locked
	{
		static int thunk(RE::BSString* a_dest, const char* a_format, const char* a_unlockLbl, const char* a_cellName, const char* a_markerLbl, const char* a_lockLevel)
		{
			auto [result, cellName] = detail::GetName(a_cellName);

			return func(a_dest, a_format, a_unlockLbl, cellName, a_markerLbl, a_lockLevel);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL_ID(17522, 17923) };

		stl::write_thunk_call<Locked>(target.address() + 0x140);
		stl::write_thunk_call<Normal>(target.address() + 0x168);

		logger::info("\t\tInstalled load door activate prompt tweak"sv);
	}
}
