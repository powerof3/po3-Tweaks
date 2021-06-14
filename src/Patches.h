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
		REL::Relocation<std::uintptr_t> vtbl{ RE::TESObjectREFR::VTABLE[0] };  //TESObjectREFR vtbl
		_Load3D = vtbl.write_vfunc(0x6A, Load3D);
	}

private:
	using MAT = RE::MATERIAL_ID;

	static RE::NiAVObject* Load3D(RE::TESObjectREFR* a_ref, bool a_backgroundLoading)
	{
		auto node = _Load3D(a_ref, a_backgroundLoading);
		if (node) {
			auto get_directional_mat = [&](RE::TESObjectREFR* a_ref) {
				const auto base = a_ref->GetBaseObject();
				const auto stat = base ? base->As<RE::TESObjectSTAT>() : nullptr;
				const auto matObj = stat ? stat->data.materialObj : nullptr;

				return matObj != nullptr && matObj->directionalData.flags.all(RE::BSMaterialObject::DIRECTIONAL_DATA::Flag::kSnow) && stat->data.materialThresholdAngle >= 90.0f;
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
									continue;
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
	static inline REL::Relocation<decltype(Load3D)> _Load3D;

	static inline constexpr std::array<MAT, 7> blacklistedMat{ MAT::kSnow, MAT::kSnowStairs, MAT::kIce, MAT::kCloth, MAT::kGlass, MAT::kBone, MAT::kBarrel };
	static inline constexpr std::array<MAT, 7> stairsMat{ MAT::kStoneStairs, MAT::kStoneAsStairs, MAT::kStoneStairsBroken, MAT::kWoodAsStairs, MAT::kWoodStairs };
};

//disables ripples for levitating creatures
class NoRipplesOnHover
{
public:
	static void Patch()
	{
		Player::Patch();
		NPC::Patch();
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

	struct Player
	{
		static void Patch()
		{
			REL::Relocation<std::uintptr_t> vtbl{ RE::PlayerCharacter::VTABLE[0] };  //Player vtbl
			_ProcessInWater = vtbl.write_vfunc(0x9C, ProcessInWater);
		}

		static bool ProcessInWater(RE::PlayerCharacter* a_actor, RE::hkpCollidable* a_collidable, float a_waterHeight, float a_deltaTime)
		{
			//30 - water surface
			//36 - underwater

			if (isLevitatingOnWater(a_actor, a_collidable)) {
				return false;
			}
			return _ProcessInWater(a_actor, a_collidable, a_waterHeight, a_deltaTime);
		}
		static inline REL::Relocation<decltype(ProcessInWater)> _ProcessInWater;
	};

	struct NPC
	{
		static void Patch()
		{
			REL::Relocation<std::uintptr_t> vtbl{ RE::Character::VTABLE[0] };  //Character vtbl
			_ProcessInWater = vtbl.write_vfunc(0x9C, ProcessInWater);
		}

		static bool ProcessInWater(RE::Character* a_actor, RE::hkpCollidable* a_collidable, float a_waterHeight, float a_deltaTime)
		{
			if (isLevitatingOnWater(a_actor, a_collidable)) {
				return false;
			}
			return _ProcessInWater(a_actor, a_collidable, a_waterHeight, a_deltaTime);
		}
		static inline REL::Relocation<decltype(ProcessInWater)> _ProcessInWater;
	};

	static inline std::string_view isLevitating{ "isLevitating"sv };
};

namespace Script
{
	inline bool Speedup(RE::BSScript::Internal::VirtualMachine* a_vm)
	{
		if (!a_vm) {
			return false;
		}

		auto settings = Settings::GetSingleton();

		if (settings->fastGetPlayer) {
			a_vm->SetCallableFromTasklets("Game", "GetPlayer", true);
			logger::info("patched Game.GetPlayer"sv);
		}
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
