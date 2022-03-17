#pragma once

#include "Cache.h"
#include "Settings.h"

namespace Tweaks
{
	void Install();
}

//removes steal tag if all faction members have appropriate relationship rank
namespace FactionStealing
{
	struct CanTake
	{
		struct detail
		{
			static std::int32_t GetFavorCost(RE::TESNPC* a_player, RE::TESNPC* a_owner)
			{
				using func_t = decltype(&GetFavorCost);
				REL::Relocation<func_t> func{ REL_ID(23626, 24078) };
				return func(a_player, a_owner);
			}

			static bool CanTake(RE::TESNPC* a_playerBase, RE::TESNPC* a_npc, std::int32_t a_cost)
			{
				const auto favorCost = GetFavorCost(a_playerBase, a_npc);
				return favorCost > 1 ?
				           a_cost <= favorCost :
                           false;
			}
		};

		static bool func(const RE::PlayerCharacter* a_player, RE::TESForm* a_owner, std::int32_t a_cost)
		{
			if (!a_owner) {
				return false;
			}

			const auto playerBase = RE::PlayerCharacter::GetSingleton()->GetActorBase();
			if (a_owner == playerBase) {
				return true;
			}

			if (const auto npc = a_owner->As<RE::TESNPC>(); npc) {
				return detail::CanTake(playerBase, npc, a_cost);
			}

			if (const auto faction = a_owner->As<RE::TESFaction>(); faction) {
				if (a_player->IsInFaction(faction)) {
					return true;
				}

				if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists && processLists->numberHighActors > 0) {
					std::vector<RE::TESNPC*> vec;
					for (auto& handle : processLists->highActorHandles) {
						const auto actor = handle.get();
						if (actor && actor->IsInFaction(faction)) {
							if (const auto base = actor->GetActorBase(); base) {
								vec.push_back(base);
							}
						}
					}
					return std::ranges::all_of(vec, [&](const auto& npc) {
						return detail::CanTake(playerBase, npc, a_cost);
					});
				}
			}

			return false;
		}
#ifdef SKYRIM_AE
		//extra inlining
		static inline constexpr std::size_t size{ 0x163 };
#else
		static inline constexpr std::size_t size{ 0xAC };
#endif
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> func{ REL_ID(39584, 40670) };
		stl::asm_replace<CanTake>(func.address());

		logger::info("Installed faction stealing tweak"sv);
	}
}

//voice distortion while wearing a helmet
namespace VoiceModulation
{
	struct SetObjectToFollow
	{
		static void thunk(RE::BSSoundHandle& a_handle, RE::NiAVObject* a_node)
		{
			func(a_handle, a_node);

			const auto user = a_node ? a_node->GetUserData() : nullptr;
			const auto biped = user ? user->GetBiped() : nullptr;

			if (biped && biped->objects[RE::BIPED_OBJECT::kHead].partClone.get()) {
				a_handle.SetFrequency(Settings::GetSingleton()->tweaks.voiceModulationValue);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL_ID(36541, 37542), OFFSET_3(0x6F3, 0x7A2, 0x6E6) };
		stl::write_thunk_call<SetObjectToFollow>(target.address());

		logger::info("Installed voice modulation tweak"sv);
	}
}

//shift pitch with time mult
namespace DopplerShift
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

		static void Play(RE::BSAudioManager* a_manager, std::int32_t a_soundID)
		{
			using func_t = decltype(&Play);
			REL::Relocation<func_t> func{ REL_ID(66408, 67671) };
			return func(a_manager, a_soundID);
		}

		static void PlayAfter(RE::BSAudioManager* a_manager, std::int32_t a_soundID, std::uint32_t a_unk03)
		{
			using func_t = decltype(&PlayAfter);
			REL::Relocation<func_t> func{ REL_ID(66409, 67672) };
			return func(a_manager, a_soundID, a_unk03);
		}
	};

	struct DefaultSound
	{
		static bool func(RE::BSSoundHandle& a_handle)
		{
			return detail::PlaySoundHandle(a_handle, [&](std::int32_t a_soundID) {
				detail::Play(RE::BSAudioManager::GetSingleton(), a_soundID);
			});
		}
		static inline constexpr std::size_t size{ 0x33 };

		static void Install()
		{
			REL::Relocation<std::uintptr_t> func{ REL_ID(66355, 67616) };
			stl::asm_replace<DefaultSound>(func.address());  //BSSoundHandle::PlaySound
		}
	};

	struct Dialogue
	{
		static bool func(RE::BSSoundHandle& a_handle, std::uint32_t a_unk02)
		{
			return detail::PlaySoundHandle(a_handle, [&](std::int32_t a_soundID) {
				detail::PlayAfter(RE::BSAudioManager::GetSingleton(), a_soundID, a_unk02);
			});
		}
		static inline constexpr std::size_t size{ 0x46 };

		static void Install()
		{
			REL::Relocation<std::uintptr_t> func{ REL_ID(66356, 67617) };
			stl::asm_replace<Dialogue>(func.address());  //BSSoundHandle::PlaySound3D
		}
	};

	inline void Install()
	{
		DefaultSound::Install();
		Dialogue::Install();

		logger::info("Installed sound - time sync tweak"sv);
	}
}

//applies snow havok material to objects using snow dir mat
namespace DynamicSnowMaterial
{
	using MAT = RE::MATERIAL_ID;

	struct detail
	{
		static bool must_only_contain_textureset(const RE::TESBoundObject* a_base, const std::pair<std::string_view, std::string_view>& a_modelPath)
		{
			if (const auto model = a_base->As<RE::TESModelTextureSwap>(); model && model->alternateTextures && model->numAlternateTextures > 0) {
				std::span altTextures{ model->alternateTextures, model->numAlternateTextures };
				return std::ranges::all_of(altTextures, [&](const auto& textures) {
					const auto txst = textures.textureSet;
					std::string path = txst ? txst->textures[0].textureName.c_str() : std::string();
					return path.find(a_modelPath.first) != std::string::npos || path.find(a_modelPath.second) != std::string::npos;
				});
			}

			return false;
		}

		static bool is_snow_object(const RE::TESObjectREFR* a_ref)
		{
			auto result = false;

			const auto base = a_ref->GetObjectReference();
			if (!base) {
				return result;
			}

			const auto stat = base->As<RE::TESObjectSTAT>();
			const auto matObject = stat ? stat->data.materialObj : nullptr;

			if (matObject) {  //statics
				const auto editorID = Cache::EditorID::GetSingleton()->GetEditorID(matObject);
				result = string::icontains(editorID, "snow"sv) && stat->data.materialThresholdAngle >= 90.0f;
			}

			if (!result && !matObject) {                                          // snow variants
				result = must_only_contain_textureset(base, { "Snow", "Mask" });  //dirtcliffmask
			}

			if (!result) {  //seasons
				const auto root = a_ref->Get3D();
				result = root && root->HasExtraData("SOS_SNOW_SHADER");
			}

			return result;
		}

		static bool is_stairs(MAT a_matID)
		{
			return std::ranges::find(stairsMat, a_matID) != stairsMat.end();
		}

		static bool is_blacklisted(MAT a_matID)
		{
			return std::ranges::find(blacklistedMat, a_matID) != blacklistedMat.end();
		}

		static inline constexpr std::array blacklistedMat{ MAT::kSnow, MAT::kSnowStairs, MAT::kIce, MAT::kCloth, MAT::kGlass, MAT::kBone, MAT::kBarrel };
		static inline constexpr std::array stairsMat{ MAT::kStoneStairs, MAT::kStoneAsStairs, MAT::kStoneStairsBroken, MAT::kWoodAsStairs, MAT::kWoodStairs };
	};

	struct GetMaterialIDPatch
	{
		static void Install()
		{
			REL::Relocation<std::uintptr_t> target{ REL_ID(35320, 36215), OFFSET(0x600, 0x6A4) };  //BGSImpactManager::PlayImpactEffect

			struct Patch : Xbyak::CodeGenerator
			{
				Patch(std::uintptr_t a_func)
				{
					Xbyak::Label f;
#ifdef SKYRIM_AE
					mov(r8, rdi);
#else
					mov(r8, rbx);
#endif
					jmp(ptr[rip + f]);

					L(f);
					dq(a_func);
				}
			};

			Patch patch{ reinterpret_cast<std::uintptr_t>(GetMaterialID) };
			patch.ready();

			auto& trampoline = SKSE::GetTrampoline();
			SKSE::AllocTrampoline(31);

			_GetMaterialID = trampoline.write_call<5>(target.address(), trampoline.allocate(patch));
		}

	private:
		static MAT GetMaterialID(RE::bhkShape* a_shape, std::uint32_t a_ID, RE::hkpCollidable* a_collidable)
		{
			auto matID = _GetMaterialID(a_shape, a_ID);
			if (a_collidable && !detail::is_blacklisted(matID)) {
				if (const auto ref = RE::TESHavokUtilities::FindCollidableRef(*a_collidable); ref && detail::is_snow_object(ref)) {
					matID = detail::is_stairs(matID) ? MAT::kSnowStairs : MAT::kSnow;
				}
			}
			return matID;
		}
		static inline REL::Relocation<MAT(RE::bhkShape*, std::uint32_t)> _GetMaterialID;
	};

	inline void Install()
	{
		GetMaterialIDPatch::Install();

		logger::info("Installed dynamic snow material tweak"sv);
	}
}

//disables ripples for levitating creatures
namespace NoRipplesOnHover
{
	struct detail
	{
		static bool IsLevitatingOnWater(const RE::Character* a_character, const RE::hkpCollidable* a_collidable)
		{
			if (const std::uint32_t colFilter = a_collidable->broadPhaseHandle.collisionFilterInfo & 127; colFilter == 30) {
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

	inline void Install()
	{
		stl::write_vfunc<RE::PlayerCharacter, ProcessInWater::Player>();
		stl::write_vfunc<RE::Character, ProcessInWater::NPC>();

		logger::info("Installed no ripples on hover tweak"sv);
	}
}

//print screenshot notification to console
namespace ScreenshotToConsole
{
	struct DebugNotification
	{
		static void thunk(const char* a_notification, [[maybe_unused]] const char* a_soundToPlay, [[maybe_unused]] bool a_cancelIfAlreadyQueued)
		{
			const auto log = RE::ConsoleLog::GetSingleton();
			if (log) {
				log->Print("%s", a_notification);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL_ID(35882, 36853), OFFSET(0xA8, 0x9E) };
		stl::write_thunk_call<DebugNotification>(target.address());

		logger::info("Installed screenshot to console tweak"sv);
	}
}

//suppress notifications
namespace NoCritSneakMessage
{
	inline void Install(std::uint32_t a_type)
	{
		REL::Relocation<std::uintptr_t> target{ REL_ID(37633, 38586) };

#ifdef SKYRIM_AE
		std::array id{
			std::make_pair(0x328, 0x33B),  //crit
			std::make_pair(0x3E9, 0x3FC),  //sneak
		};
#else
		std::array id{
			std::make_pair(0x20D, 0x220),  //crit
			std::make_pair(0x2D3, 0x2E6),  //sneak
		};
#endif

		switch (a_type) {
		case 1:
			{
				const auto& [start, end] = id[0];
				for (auto i = start; i < end; ++i) {
					REL::safe_write(target.address() + i, REL::NOP);
				}
			}
			break;
		case 2:
			{
				const auto& [start, end] = id[1];
				for (auto i = start; i < end; ++i) {
					REL::safe_write(target.address() + i, REL::NOP);
				}
			}
			break;
		case 3:
			{
				for (const auto& [start, end] : id) {
					for (auto i = start; i < end; ++i) {
						REL::safe_write(target.address() + i, REL::NOP);
					}
				}
			}
			break;
		default:
			break;
		}

		logger::info("Installed crit/sneak message tweak"sv);
	}
}

//you can only wait while sitting down
namespace SitToWait
{
	struct detail
	{
		static bool ProcessMenu(const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, bool a_unk03)
		{
			using func_t = decltype(&ProcessMenu);
			REL::Relocation<func_t> func{ REL_ID(80077, 82180) };
			return func(a_menuName, a_type, a_unk03);
		}

		static bool CanSleepWait(RE::PlayerCharacter* a_player, RE::TESObjectREFR* a_furniture)
		{
			using func_t = decltype(&CanSleepWait);
			REL::Relocation<func_t> func{ REL_ID(39371, 40443) };
			return func(a_player, a_furniture);
		}

		static bool CanWait(RE::PlayerCharacter* a_player, RE::TESObjectREFR* a_furniture)
		{
			const auto result = CanSleepWait(a_player, a_furniture);
			if (result && a_player->GetSitSleepState() != RE::SIT_SLEEP_STATE::kIsSitting) {
				const auto& tweaks = Settings::GetSingleton()->tweaks;
				RE::DebugNotification(tweaks.sitToWait.message.c_str(), "UIMenuCancel");
				return false;
			}
			return result;
		}
	};

	struct HandleWaitRequest  //no way to determine menu type with just CanWait
	{
		static void thunk(bool a_sleep)
		{
			const auto intfc = RE::InterfaceStrings::GetSingleton();

			if (RE::UI::GetSingleton()->IsMenuOpen(intfc->sleepWaitMenu)) {
				RE::UIMessageQueue::GetSingleton()->AddMessage(intfc->sleepWaitMenu, RE::UI_MESSAGE_TYPE::kHide, nullptr);
			} else if (detail::CanWait(RE::PlayerCharacter::GetSingleton(), nullptr)) {
				detail::ProcessMenu(intfc->sleepWaitMenu, RE::UI_MESSAGE_TYPE::kShow, a_sleep);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL_ID(51400, 52249), OFFSET_3(0x394, 0x379, 0x681) };
		stl::write_thunk_call<HandleWaitRequest>(target.address());

		logger::info("Installed sit to wait tweak"sv);
	}
}

//disables GodMode and ImmortalMode
namespace NoCheatMode
{
	struct GodMode
	{
		static bool func()
		{
			const auto log = RE::ConsoleLog::GetSingleton();
			if (log && log->IsConsoleMode()) {
				log->Print("God Mode disabled");
			}
			return true;
		}
		static inline constexpr std::size_t size{ 0x4C };

		static void Install()
		{
			REL::Relocation<std::uintptr_t> func{ REL_ID(22339, 22814) };
			stl::asm_replace<GodMode>(func.address());
		}
	};

	struct ImmortalMode
	{
		static bool func()
		{
			const auto log = RE::ConsoleLog::GetSingleton();
			if (log && log->IsConsoleMode()) {
				log->Print("Immortal Mode disabled");
			}
			return true;
		}
		static inline constexpr std::size_t size{ 0x4C };

		static void Install()
		{
			REL::Relocation<std::uintptr_t> func{ REL_ID(22340, 22815) };
			stl::asm_replace<ImmortalMode>(func.address());
		}
	};

	inline void Install(std::uint32_t a_type)
	{
		switch (a_type) {
		case 1:
			GodMode::Install();
			break;
		case 2:
			ImmortalMode::Install();
			break;
		case 3:
			{
				GodMode::Install();
				ImmortalMode::Install();
			}
			break;
		default:
			break;
		}

		logger::info("Installed no cheat mode"sv);
	}
}

//send steal alarm when grabbing owned items
namespace GrabbingIsStealing
{
	class GrabReleaseHandler final : public RE::BSTEventSink<RE::TESGrabReleaseEvent>
	{
	public:
		[[nodiscard]] static GrabReleaseHandler* GetSingleton()
		{
			static GrabReleaseHandler singleton;
			return std::addressof(singleton);
		}

	protected:
		using EventResult = RE::BSEventNotifyControl;

		EventResult ProcessEvent(const RE::TESGrabReleaseEvent* a_event, RE::BSTEventSource<RE::TESGrabReleaseEvent>*) override
		{
			const auto ref = a_event && a_event->grabbed ?
			                     a_event->ref :
                                 RE::TESObjectREFRPtr();

			if (ref) {
				const auto player = RE::PlayerCharacter::GetSingleton();
				if (player && !ref->IsAnOwner(player, true, false)) {
					const auto base = ref->GetObjectReference();
					const auto numItems = ref->extraList.GetCount();
					const auto owner = ref->GetOwner();
					player->StealAlarm(ref.get(), base, numItems, 0, owner, true);
				}
			}

			return EventResult::kContinue;
		}

	private:
		GrabReleaseHandler() = default;
		GrabReleaseHandler(const GrabReleaseHandler&) = delete;
		GrabReleaseHandler(GrabReleaseHandler&&) = delete;

		~GrabReleaseHandler() override = default;

		GrabReleaseHandler& operator=(const GrabReleaseHandler&) = delete;
		GrabReleaseHandler& operator=(GrabReleaseHandler&&) = delete;
	};

	inline void Install()
	{
		auto scripts = RE::ScriptEventSourceHolder::GetSingleton();
		if (scripts) {
			scripts->AddEventSink(GrabReleaseHandler::GetSingleton());
			logger::info("Installed Grabbing Is Stealing tweak"sv);
		}
	}
}

//custom load door prompts when exiting or entering
namespace LoadDoorPrompt
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

			auto ref = crosshairPickData->target.get();
			if (!ref) {
				ref = crosshairPickData->unk0C.get();
			}

			if (const auto cell = ref ? ref->GetSaveParentCell() : nullptr) {
				if (cell->IsInteriorCell()) {
					const auto linkedDoor = ref->extraList.GetTeleportLinkedDoor();
					const auto linkedRef = linkedDoor.get();
					const auto linkedCell = linkedRef ? linkedRef->GetSaveParentCell() : nullptr;
					if (linkedCell && linkedCell->IsExteriorCell()) {
						auto& [type, enter, exit] = Settings::GetSingleton()->tweaks.loadDoorPrompt;
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
			auto& [type, enter, exit] = Settings::GetSingleton()->tweaks.loadDoorPrompt;
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

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL_ID(17522, 17923) };

		stl::write_thunk_call<Locked>(target.address() + 0x140);
		stl::write_thunk_call<Normal>(target.address() + 0x168);

		logger::info("Installed load door activate prompt tweak"sv);
	}
}

//disable poison confirmation messagebox
namespace NoPoisonPrompt
{
	struct ShowPoisonConfirmationPrompt
	{
		static void thunk(char*, void (*PoisonWeapon)(std::uint8_t a_result), std::uint8_t a_result, std::uint32_t, std::int32_t, char*, char*)
		{
			PoisonWeapon(a_result);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct ShowPoisonInformationPrompt
	{
		static void thunk(char* a_message, void (*PoisonWeapon)(std::uint8_t a_result), std::uint8_t a_result, std::uint32_t, std::int32_t, char*, char*)
		{
			RE::DebugNotification(a_message, "UIMenuCancel");
			PoisonWeapon(a_result);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	inline void Install(std::uint32_t a_type)
	{
		REL::Relocation<std::uintptr_t> target{ REL_ID(39406, 40481) };

		switch (a_type) {
		case 1:
			stl::write_thunk_call<ShowPoisonConfirmationPrompt>(target.address() + 0x10B);
			break;
		case 2:
			stl::write_thunk_call<ShowPoisonInformationPrompt>(target.address() + 0x143);
			break;
		case 3:
			{
				stl::write_thunk_call<ShowPoisonConfirmationPrompt>(target.address() + 0x10B);
				stl::write_thunk_call<ShowPoisonInformationPrompt>(target.address() + 0x143);
			}
			break;
		default:
			break;
		}

		logger::info("Installed no poison message tweak"sv);
	}
}

//disable player shouting when power attacking in sneak mode
namespace SilentSneakPowerAttack
{
	struct SayCombatDialogue
	{
		static bool thunk(std::uintptr_t a_combatDialogueManager, RE::Actor* a_speaker, RE::Actor* a_target, RE::DIALOGUE_TYPE a_type, RE::DIALOGUE_DATA::Subtype a_subtype, bool a_ignoreSpeakingDone, RE::CombatController* a_combatController)
		{
			if (a_subtype == RE::DIALOGUE_DATA::Subtype::kPowerAttack && a_speaker->IsSneaking()) {
				return false;
			}
			return func(a_combatDialogueManager, a_speaker, a_target, a_type, a_subtype, a_ignoreSpeakingDone, a_combatController);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL_ID(39577, 40663), 0xAA };
		stl::write_thunk_call<SayCombatDialogue>(target.address());

		logger::info("Installed silent sneak power attack tweak"sv);
	}
}

#ifdef SKYRIMVR
//Remember lock pick angle.
//Based on offsets discovered by OnlyIWeDo (https://www.nexusmods.com/skyrimspecialedition/mods/24543) and updated by Umgak (https://www.nexusmods.com/skyrimspecialedition/mods/26838)
namespace RememberLockPickAngle
{
	inline void
		Install()
	{
		constexpr REL::ID LockpickBreakAddr{ 51093 };
		static REL::Relocation<std::uintptr_t> target{ LockpickBreakAddr, 0x242 };
		for (uintptr_t i = 0; i < 7; ++i) {
			REL::safe_write(target.address() + i, REL::NOP);
		}
		logger::info("Installed remember lock pick angle"sv);
	}
}
#endif
