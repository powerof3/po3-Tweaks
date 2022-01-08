#pragma once

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
				REL::Relocation<func_t> func{ REL::ID(23626) };
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
				if (a_player->IsInFaction(faction) || faction->IgnoresStealing() || faction->IgnoresPickpocket()) {
					return true;
				}

				const auto processLists = RE::ProcessLists::GetSingleton();
				if (processLists && processLists->numberHighActors > 0) {
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
		static inline constexpr std::size_t size = 0xAC;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> func{ REL::ID(39584) };
		stl::asm_replace<CanTake>(func.address());

		logger::info("Installed faction stealing tweak"sv);
	}
}

//removes fadeout when going through load doors
namespace AIFadeOut
{
	struct GetFadeState
	{
		static std::uint32_t thunk(RE::AIProcess* a_aiProcess)
		{
			const auto state = func(a_aiProcess);

			const auto middleProcess = a_aiProcess ? a_aiProcess->middleHigh : nullptr;
			const auto torsoNode = middleProcess ? middleProcess->torsoNode : nullptr;
			const auto user = torsoNode ? torsoNode->GetUserData() : nullptr;

			return user && user->IsPlayerRef() ?
                       state :
                       4;
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL::ID(17521), 0x3C5 };
		stl::write_thunk_call<GetFadeState>(target.address());

		logger::info("Installed load door fade out tweak"sv);
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
				a_handle.SetFrequency(Settings::GetSingleton()->tweaks.voiceModulationValue.value);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL::ID(36541),
#ifndef SKYRIMVR
			0x6F3
#else
			0x6e6
#endif
		};
		stl::write_thunk_call<SetObjectToFollow>(target.address());

		logger::info("Installed voice modulation tweak"sv);
	}
}

//shift pitch with time mult
namespace DopplerShift
{
	struct detail
	{
		static bool PlayHandle(RE::BSSoundHandle& a_handle, std::function<void(std::int32_t a_soundID)> func)
		{
			const auto soundID = a_handle.soundID;
			if (soundID == -1) {
				return false;
			}

			if (const auto timeMult = RE::BSTimer::GetCurrentGlobalTimeMult(); timeMult != 1.0f) {
				a_handle.SetFrequency(timeMult);
			}

			a_handle.state = RE::BSSoundHandle::AssumedState::kPlaying;
			func(soundID);

			return true;
		}

		static void Play(RE::BSAudioManager* a_manager, std::int32_t a_soundID)
		{
			using func_t = decltype(&Play);
			REL::Relocation<func_t> func{ REL::ID(66408) };
			return func(a_manager, a_soundID);
		}

		static void PlayAfter(RE::BSAudioManager* a_manager, std::int32_t a_soundID, std::uint32_t a_unk03)
		{
			using func_t = decltype(&PlayAfter);
			REL::Relocation<func_t> func{ REL::ID(66409) };
			return func(a_manager, a_soundID, a_unk03);
		}
	};

	struct DefaultSound
	{
		static void Install()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(66355) };
			stl::asm_replace<DefaultSound>(func.address());  //BSSoundHandle::PlaySound
		}

		static bool func(RE::BSSoundHandle& a_handle)
		{
			return detail::PlayHandle(a_handle, [&](std::int32_t a_soundID) {
				detail::Play(RE::BSAudioManager::GetSingleton(), a_soundID);
			});
		}
		static inline constexpr std::size_t size = 0x33;
	};

	struct Dialogue
	{
		static void Install()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(66356) };
			stl::asm_replace<Dialogue>(func.address());  //BSSoundHandle::PlaySound3D
		}

		static bool func(RE::BSSoundHandle& a_handle, std::uint32_t a_unk02)
		{
			return detail::PlayHandle(a_handle, [&](std::int32_t a_soundID) {
				detail::PlayAfter(RE::BSAudioManager::GetSingleton(), a_soundID, a_unk02);
			});
		}
		static inline constexpr std::size_t size = 0x46;
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
	struct Load3D
	{
		using MAT = RE::MATERIAL_ID;

		static RE::NiAVObject* thunk(RE::TESObjectREFR* a_ref, bool a_backgroundLoading)
		{
			auto node = func(a_ref, a_backgroundLoading);
			if (node) {
				const auto get_directional_mat = [&]() {
					const auto base = a_ref->GetObjectReference();
					const auto stat = base ? base->As<RE::TESObjectSTAT>() : nullptr;
					const auto matObj = stat ? stat->data.materialObj : nullptr;

					auto result = matObj != nullptr && matObj->directionalData.flags.all(RE::BSMaterialObject::DIRECTIONAL_DATA::Flag::kSnow) && stat->data.materialThresholdAngle >= 90.0f;
					if (!result && stat) {
						//find statics with snow txst swap
						if (const auto model = stat->GetAsModelTextureSwap(); model && model->alternateTextures) {
                            const std::span span(model->alternateTextures, model->numAlternateTextures);
							for (const auto& texture : span) {
								if (const auto txst = texture.textureSet; txst && string::icontains(txst->textures[RE::BSTextureSet::Texture::kDiffuse].textureName, "snow"sv)) {
									result = true;
									break;
								}
							}
						}
					}
					return result;
				};

				if (!get_directional_mat()) {
					return node;
				}

				const auto cell = a_ref->GetSaveParentCell();
				if (!cell) {
					return node;
				}

				const auto sky = RE::Sky::GetSingleton();
				std::uint32_t snowState = sky && sky->GetIsSnowing() ? 1 : 0;

				if (snowState == 0) {
					const auto xRegion = cell->extraList.GetByType<RE::ExtraRegionList>();
					const auto regionList = xRegion ? xRegion->list : nullptr;
					if (regionList) {
						static std::vector<RE::FormID> regions;
						if (regions.empty()) {
							const auto dataHandler = RE::TESDataHandler::GetSingleton();
							if (dataHandler) {
								const auto is_snowy_region = [&](const RE::TESRegion* a_region) {
									if (const auto list = a_region->dataList; list) {
										for (const auto& data : list->regionDataList) {
											if (const auto weatherData = data && data->GetType() == RE::TESRegionData::Type::kWeather ?
                                                                             static_cast<RE::TESRegionDataWeather*>(data) :
                                                                             nullptr;
												weatherData) {
												for (const auto& weatherType : weatherData->weatherTypes) {
													if (const auto weather = weatherType->weather; weather && weather->data.flags.any(RE::TESWeather::WeatherDataFlag::kSnow)) {
														return true;
													}
												}
											}
										}
									}
									return false;
								};
								for (const auto& region : dataHandler->GetFormArray<RE::TESRegion>()) {
									if (region && is_snowy_region(region)) {
										regions.push_back(region->GetFormID());
									}
								}
							}
						}

						for (const auto& region : *regionList) {
							if (region && std::ranges::find(regions, region->GetFormID()) != regions.end()) {
								snowState = 2;
								break;
							}
						}
					}
				}

				if (snowState == 0) {
					return node;
				}

				if (const auto world = cell->GetbhkWorld(); world) {
					RE::BSWriteLockGuard locker(world->worldLock);

					RE::BSVisit::TraverseScenegraphCollision(node, [&](const RE::bhkNiCollisionObject* a_col) -> RE::BSVisit::BSVisitControl {
						const auto body = a_col->body.get();
						if (!body) {
							return RE::BSVisit::BSVisitControl::kContinue;
						}

						const auto hkpBody = static_cast<RE::hkpWorldObject*>(body->referencedObject.get());
						const auto hkpShape = hkpBody ? hkpBody->GetShape() : nullptr;

						if (hkpShape && hkpShape->type == RE::hkpShapeType::kMOPP) {
							const auto mopp = static_cast<const RE::hkpMoppBvTreeShape*>(hkpShape);
							const auto childShape = mopp ? mopp->child.childShape : nullptr;
							const auto compressedShape = childShape ? netimmerse_cast<RE::bhkCompressedMeshShape*>(childShape->userData) : nullptr;
							const auto shapeData = compressedShape ? compressedShape->data.get() : nullptr;

							if (shapeData) {
								for (auto& meshMaterial : shapeData->meshMaterials) {
									if (std::ranges::find(blacklistedMat, meshMaterial.materialID) != blacklistedMat.end() || snowState == 2 && meshMaterial.materialID == MAT::kStone) {
										continue;  //some statics have snow directional mats in non-snowy areas
									}
									if (std::ranges::find(stairsMat, meshMaterial.materialID) != stairsMat.end()) {
										meshMaterial.materialID = MAT::kSnowStairs;
									} else {
										meshMaterial.materialID = MAT::kSnow;
									}
								}
							}
						}

						return RE::BSVisit::BSVisitControl::kContinue;
					});
				}
			}
			return node;
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static inline constexpr std::size_t size = 0x6A;

	private:
		static inline constexpr std::array<MAT, 6> blacklistedMat{ MAT::kSnow, MAT::kSnowStairs, MAT::kCloth, MAT::kGlass, MAT::kBone, MAT::kBarrel };
		static inline constexpr std::array<MAT, 7> stairsMat{ MAT::kStoneStairs, MAT::kStoneAsStairs, MAT::kStoneStairsBroken, MAT::kWoodAsStairs, MAT::kWoodStairs };
	};

	inline void Install()
	{
		stl::write_vfunc<RE::TESObjectREFR, Load3D>();
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

			static inline constexpr std::size_t size = 0x9C;
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

			static inline constexpr std::size_t size = 0x9C;
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
		REL::Relocation<std::uintptr_t> target{ REL::ID(35882), 0xA8 };
		stl::write_thunk_call<DebugNotification>(target.address());

		logger::info("Installed screenshot to console tweak"sv);
	}
}

//suppress notifications
namespace NoCritSneakMessage
{
	inline constexpr std::array<std::pair<std::uintptr_t, std::uintptr_t>, 2> ranges{
		std::make_pair(0x20D, 0x220),  //crit
		std::make_pair(0x2D3, 0x2E6),  //sneak
	};

	inline void Install(std::uint32_t a_type)
	{
		REL::Relocation<std::uintptr_t> target{ REL::ID(37633) };

		switch (a_type) {
		case 1:
			for (uintptr_t i = 0x20D; i < 0x220; ++i) {
				REL::safe_write(target.address() + i, REL::NOP);
			}
			break;
		case 2:
			for (uintptr_t i = 0x2D3; i < 0x2E6; ++i) {
				REL::safe_write(target.address() + i, REL::NOP);
			}
			break;
		case 3:
			for (const auto& [start, end] : ranges) {
				for (uintptr_t i = start; i < end; ++i) {
					REL::safe_write(target.address() + i, REL::NOP);
				}
			}
			break;
		default:
			break;
		}

		logger::info("Installed crit/sneak message tweak"sv);
	}
}

//sit 2 wait
namespace SitToWait
{
	struct detail
	{
		static bool ProcessMenu(const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, bool a_unk03)
		{
			using func_t = decltype(&ProcessMenu);
			REL::Relocation<func_t> func{ REL::ID(80077) };
			return func(a_menuName, a_type, a_unk03);
		}

		static bool CanSleepWait(RE::PlayerCharacter* a_player, RE::TESObjectREFR* a_furniture)
		{
			using func_t = decltype(&CanSleepWait);
			REL::Relocation<func_t> func{ REL::ID(39371) };
			return func(a_player, a_furniture);
		}

		static bool CanWait(RE::PlayerCharacter* a_player, RE::TESObjectREFR* a_furniture)
		{
			const auto result = CanSleepWait(a_player, a_furniture);
			if (result && a_player->GetSitSleepState() != RE::SIT_SLEEP_STATE::kIsSitting) {
				const auto tweaks = Settings::GetSingleton()->tweaks;
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
		REL::Relocation<std::uintptr_t> target{ REL::ID(51400),
#ifndef SKYRIMVR
			0x394
#else
			0x681
#endif
		};
		stl::write_thunk_call<HandleWaitRequest>(target.address());

		logger::info("Installed sit to wait tweak"sv);
	}
}

namespace NoCheatMode
{
	struct GodMode
	{
		static void Install()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(22339) };
			stl::asm_replace<GodMode>(func.address());
		}

		static bool func()
		{
			const auto log = RE::ConsoleLog::GetSingleton();
			if (log && log->IsConsoleMode()) {
				log->Print("God Mode disabled");
			}
			return true;
		}
		static inline constexpr std::size_t size = 0x4C;
	};

	struct ImmortalMode
	{
		static void Install()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(22340) };
			stl::asm_replace<ImmortalMode>(func.address());
		}

		static bool func()
		{
			const auto log = RE::ConsoleLog::GetSingleton();
			if (log && log->IsConsoleMode()) {
				log->Print("Immortal Mode disabled");
			}
			return true;
		}
		static inline constexpr std::size_t size = 0x4C;
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

		static std::pair<CELL_TYPE, const char*> GetName(const char* a_cellName)
		{
			const auto crosshairPickData = RE::CrosshairPickData::GetSingleton();
			auto ref = crosshairPickData->target.get();
			if (!ref) {
				ref = crosshairPickData->unk0C.get();
			}
			const auto cell = ref ? ref->GetSaveParentCell() : nullptr;
			if (cell) {
				if (cell->IsInteriorCell()) {
					const auto linkedDoor = ref->extraList.GetTeleportLinkedDoor();
					const auto linkedRef = linkedDoor.get();
					const auto linkedCell = linkedRef ? linkedRef->GetSaveParentCell() : nullptr;
					if (linkedCell && linkedCell->IsExteriorCell()) {
						const auto prompt = Settings::GetSingleton()->tweaks.loadDoorPrompt;
						return { kInterior, prompt.type.value == 2 ?
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
				return type.value == 2 ?
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
		REL::Relocation<std::uintptr_t> target{ REL::ID(17522) };

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
		REL::Relocation<std::uintptr_t> target{ REL::ID(39406) };

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
