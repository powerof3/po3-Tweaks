#pragma once

#include "Settings.h"

namespace Patches
{
	void Patch();
};

//removes steal tag if all faction members have appropriate relationship rank
class FactionStealing
{
public:
	static void Patch()
	{
		REL::Relocation<std::uintptr_t> func{ REL::ID(39584) };
		stl::asm_replace(func.address(), 0xAC, reinterpret_cast<std::uintptr_t>(fixed_func));
	}

private:
	static bool fixed_func(RE::PlayerCharacter* a_player, RE::TESForm* a_owner, std::int32_t a_cost)
	{
		if (!a_owner) {
			return false;
		}

		const auto player = RE::PlayerCharacter::GetSingleton()->GetActorBase();
		if (a_owner == player) {
			return true;
		}

		auto can_take = [&](RE::TESNPC* a_player, RE::TESNPC* a_npc, std::int32_t a_cost) {
			const auto favorCost = GetFavorCost(a_player, a_npc);
			return favorCost > 1 ? a_cost <= favorCost : false;
		};

		if (const auto npc = a_owner->As<RE::TESNPC>(); npc) {
			return can_take(player, npc, a_cost);
		}

		if (const auto faction = a_owner->As<RE::TESFaction>(); faction) {
			if (a_player->IsInFaction(faction)) {
				return true;
			}

			if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists && processLists->numberHighActors > 0) {
				std::vector<RE::TESNPC*> vec;
				for (auto& handle : processLists->highActorHandles) {
					auto actorPtr = handle.get();
					auto actor = actorPtr.get();
					if (actor && actor->IsInFaction(faction)) {
						if (auto base = actor->GetActorBase(); base) {
							vec.push_back(base);
						}
					}
				}

				return std::ranges::all_of(vec, [&](const auto& npc) {
					return can_take(player, npc, a_cost);
				});
			}
		}

		return false;
	}

	static std::int32_t GetFavorCost(RE::TESNPC* a_player, RE::TESNPC* a_owner)
	{
		using func_t = decltype(&GetFavorCost);
		REL::Relocation<func_t> func{ REL::ID(23626) };
		return func(a_player, a_owner);
	}
};

//removes fadeout when going through load doors
namespace AIFadeOut
{
	inline void Patch()
	{
		REL::Relocation<std::uintptr_t> target{ REL::ID(17521), 0x3CA };

		constexpr std::array<std::uint8_t, 3> bytes{ 0x83, 0xF8, 0x00 };  //cmp eax, 0
		REL::safe_write(target.address(), std::span{ bytes.data(), bytes.size() });
	}
};

//voice distortion while wearing a helmet
namespace VoiceModulation
{
	struct SetObjectToFollow
	{
		static void thunk(RE::BSSoundHandle& a_handle, RE::NiAVObject* a_node)
		{
			func(a_handle, a_node);

			auto user = a_node ? a_node->GetUserData() : nullptr;
			auto biped = user ? user->GetBiped().get() : nullptr;

			if (biped) {
				if (auto helmet = biped->objects[RE::BIPED_OBJECT::kHead].partClone.get()) {
					a_handle.SetFrequency(Settings::GetSingleton()->voiceModulationValue);
				}
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	inline void Patch()
	{
		REL::Relocation<std::uintptr_t> target{ REL::ID(36541), 0x6F3 };
		stl::write_thunk_call<SetObjectToFollow>(target.address());
	}
}

//shift pitch with time mult
class DopplerShift
{
public:
	static void Patch()
	{
		DefaultSound::Patch();
		Dialogue::Patch();
	}

private:
	static bool PlayHandle(RE::BSSoundHandle& a_handle, std::function<void(std::int32_t a_soundID)> func)
	{
		auto soundID = a_handle.soundID;
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

	struct DefaultSound
	{
	public:
		static void Patch()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(66355) };
			stl::asm_replace(func.address(), 0x33, reinterpret_cast<std::uintptr_t>(fixed_func));  //BSSoundHandle::PlaySound
		}

	private:
		static bool fixed_func(RE::BSSoundHandle& a_handle)
		{
			return PlayHandle(a_handle, [&](std::int32_t a_soundID) {
				Play(RE::BSAudioManager::GetSingleton(), a_soundID);
			});
		}

		static DWORD Play(RE::BSAudioManager* a_manager, std::int32_t a_soundID)
		{
			using func_t = decltype(&Play);
			REL::Relocation<func_t> func{ REL::ID(66408) };
			return func(a_manager, a_soundID);
		}
	};

	struct Dialogue
	{
	public:
		static void Patch()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(66356) };
			stl::asm_replace(func.address(), 0x46, reinterpret_cast<std::uintptr_t>(fixed_func));  //BSSoundHandle::PlaySound3D
		}

	private:
		static bool fixed_func(RE::BSSoundHandle& a_handle, std::uint32_t a_unk02)
		{
			return PlayHandle(a_handle, [&](std::int32_t a_soundID) {
				Play(RE::BSAudioManager::GetSingleton(), a_soundID, a_unk02);
			});
		}

		static DWORD Play(RE::BSAudioManager* a_manager, std::int32_t a_soundID, std::uint32_t a_unk03)
		{
			using func_t = decltype(&Play);
			REL::Relocation<func_t> func{ REL::ID(66409) };
			return func(a_manager, a_soundID, a_unk03);
		}
	};
};

//applies snow havok material to objects using snow dir mat
class DynamicSnowMaterial
{
public:
	static void Patch()
	{
		stl::write_vfunc<RE::TESObjectREFR, 0x6A, Load3D>();
	}

private:
	using MAT = RE::MATERIAL_ID;

	struct Load3D
	{
		static RE::NiAVObject* thunk(RE::TESObjectREFR* a_ref, bool a_backgroundLoading)
		{
			auto node = func(a_ref, a_backgroundLoading);
			if (node) {
				auto get_directional_mat = [&](RE::TESObjectREFR* a_ref) {
					const auto base = a_ref->GetBaseObject();
					const auto stat = base ? base->As<RE::TESObjectSTAT>() : nullptr;
					const auto matObj = stat ? stat->data.materialObj : nullptr;

					auto result = matObj != nullptr && matObj->directionalData.flags.all(RE::BSMaterialObject::DIRECTIONAL_DATA::Flag::kSnow) && stat->data.materialThresholdAngle >= 90.0f;
					if (!result && stat) {
						//find statics with snow txst swap
						if (auto model = stat->GetAsModelTextureSwap(); model && model->alternateTextures) {
							std::span<RE::TESModelTextureSwap::AlternateTexture> span(model->alternateTextures, model->numAlternateTextures);
							for (result != true; const auto& texture : span) {
								if (auto txst = texture.textureSet; txst && boost::icontains(txst->textures[RE::BSTextureSet::Texture::kDiffuse].textureName.c_str(), "snow")) {
									result = true;
									break;
								}
							}
						}
					}
					return result;
				};

				if (!get_directional_mat(a_ref)) {
					return node;
				}

				RE::TESObjectCELL* cell = nullptr;
				if (auto TES = RE::TES::GetSingleton(); TES) {
					cell = TES->GetCell(a_ref->GetPosition());
				}
				if (!cell) {
					cell = a_ref->GetParentCell();
				}

				if (!cell) {
					return node;
				}

				auto sky = RE::Sky::GetSingleton();
				std::uint32_t snowState = sky && sky->GetIsSnowing() ? 1 : 0;

				if (snowState == 0) {
					auto xRegion = cell->extraList.GetByType<RE::ExtraRegionList>();
					auto regionList = xRegion ? xRegion->list : nullptr;
					if (regionList) {
						const auto& vec = Settings::GetSingleton()->getRegions();
						for (auto& region : *regionList) {
							if (region && std::ranges::find(vec, region) != vec.end()) {
								snowState == 2;
								break;
							}
						}
					}
				}

				if (snowState == 0) {
					return node;
				}

				if (auto world = cell->GetbhkWorld(); world) {
					RE::BSWriteLockGuard locker(world->worldLock);

					RE::BSVisit::TraverseScenegraphCollision(node, [&](RE::bhkNiCollisionObject* a_col) -> RE::BSVisit::BSVisitControl {
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
	};

	static inline constexpr std::array<MAT, 6> blacklistedMat{ MAT::kSnow, MAT::kSnowStairs, MAT::kCloth, MAT::kGlass, MAT::kBone, MAT::kBarrel };
	static inline constexpr std::array<MAT, 7> stairsMat{ MAT::kStoneStairs, MAT::kStoneAsStairs, MAT::kStoneStairsBroken, MAT::kWoodAsStairs, MAT::kWoodStairs };
};

//disables ripples for levitating creatures
class NoRipplesOnHover
{
public:
	static void Patch()
	{
		stl::write_vfunc<RE::PlayerCharacter, 0x9C, ProcessInWater::Player>();
		stl::write_vfunc<RE::Character, 0x9C, ProcessInWater::NPC>();
	}

private:
	static bool isLevitatingOnWater(RE::Character* a_character, const RE::hkpCollidable* a_collidable)
	{
		if (const std::uint32_t colFilter = a_collidable->broadPhaseHandle.collisionFilterInfo & 127; colFilter == 30) {
			if (bool levitating = false; a_character->GetGraphVariableBool(isLevitating, levitating) && levitating) {
				return true;
			}
		}
		return false;
	}

	struct ProcessInWater
	{
		struct Player
		{
			static bool thunk(RE::PlayerCharacter* a_actor, RE::hkpCollidable* a_collidable, float a_waterHeight, float a_deltaTime)
			{
				if (isLevitatingOnWater(a_actor, a_collidable)) {
					return false;
				}
				return func(a_actor, a_collidable, a_waterHeight, a_deltaTime);
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		struct NPC
		{
			static bool thunk(RE::Character* a_actor, RE::hkpCollidable* a_collidable, float a_waterHeight, float a_deltaTime)
			{
				if (isLevitatingOnWater(a_actor, a_collidable)) {
					return false;
				}
				return func(a_actor, a_collidable, a_waterHeight, a_deltaTime);
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};
	};

	static inline std::string_view isLevitating{ "isLevitating"sv };
};

//print screenshot notification to console
namespace ScreenshotToConsole
{
	struct DebugNotification
	{
		static void thunk(const char* a_notification, const char* a_soundToPlay, bool a_cancelIfAlreadyQueued)
		{
			const auto log = RE::ConsoleLog::GetSingleton();
			if (log) {
				log->Print("%s", a_notification);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	inline void Patch()
	{
		REL::Relocation<std::uintptr_t> target{ REL::ID(35882), 0xA8 };
		stl::write_thunk_call<DebugNotification>(target.address());
	}
}

//suppress notifications
namespace NoCritSneakMessage
{
	inline constexpr std::array ranges{
		std::make_pair(0x20D, 0x220),  //crit
		std::make_pair(0x2D3, 0x2E6),  //sneak
	};

	inline void Patch(std::uint32_t a_type)
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

		static bool can_sleep_wait(RE::PlayerCharacter* a_player, RE::TESObjectREFR* a_furniture)
		{
			using func_t = decltype(&can_sleep_wait);
			REL::Relocation<func_t> func{ REL::ID(39371) };
			return func(a_player, a_furniture);
		}

		static bool CanWait(RE::PlayerCharacter* a_player, RE::TESObjectREFR* a_furniture)
		{
			const auto result = can_sleep_wait(a_player, a_furniture);
			if (result && a_player->GetSitSleepState() != RE::SIT_SLEEP_STATE::kIsSitting) {
				RE::DebugNotification(Settings::GetSingleton()->sitToWaitMessage.c_str(), "UIMenuCancel");
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

	inline void Patch()
	{
		REL::Relocation<std::uintptr_t> target{ REL::ID(51400), 0x394 };
		stl::write_thunk_call<HandleWaitRequest>(target.address());
	}
}

namespace NoCheatMode
{
	class GodMode
	{
	public:
		static void Patch()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(22339) };
			stl::asm_replace(func.address(), 0x4C, fixed_func);
		}

	private:
		static bool fixed_func()
		{
			const auto log = RE::ConsoleLog::GetSingleton();
			if (log && log->IsConsoleMode()) {
				log->Print("God Mode disabled");
			}
			return true;
		}
	};

	class ImmortalMode
	{
	public:
		static void Patch()
		{
			REL::Relocation<std::uintptr_t> func{ REL::ID(22340) };
			stl::asm_replace(func.address(), 0x4C, fixed_func);
		}

	private:
		static bool fixed_func()
		{
			const auto log = RE::ConsoleLog::GetSingleton();
			if (log && log->IsConsoleMode()) {
				log->Print("Immortal Mode disabled");
			}
			return true;
		}
	};

	inline void Patch(std::uint32_t a_type)
	{
		switch (a_type) {
		case 1:
			GodMode::Patch();
			break;
		case 2:
			ImmortalMode::Patch();
			break;
		case 3:
			{
				GodMode::Patch();
				ImmortalMode::Patch();
			}
			break;
		default:
			break;
		}

		GodMode::Patch();
		ImmortalMode::Patch();
	}
}

//experimental
namespace Script
{
	inline bool Speedup(RE::BSScript::Internal::VirtualMachine* a_vm)
	{
		if (!a_vm) {
			return false;
		}

		auto settings = Settings::GetSingleton();

		if (settings->fastRandomInt) {
			a_vm->SetCallableFromTasklets("Utility", "RandomInt", true);
			logger::info("patched Utility.RandomInt"sv);
		}
		if (settings->fastRandomFloat) {
			a_vm->SetCallableFromTasklets("Utility", "RandomFloat", true);
			logger::info("patched Utility.RandomFloat"sv);
		}

		return true;
	}
}
