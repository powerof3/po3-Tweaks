#include "Cache.h"
#include "Fixes.h"

//Cache skipped formEditorIDs
namespace Fixes::CacheFormEditorIDs
{
	struct detail
	{
		static const char* GetGameVersionImpl()
		{
			using func_t = decltype(&GetGameVersionImpl);
			static REL::Relocation<func_t> func{ RELOCATION_ID(15485, 15650) };
			return func();
		}

		static REL::Version GetGameVersion()
		{
			std::stringstream            ss(GetGameVersionImpl());
			std::string                  token;
			std::array<std::uint16_t, 4> version{};

			for (std::size_t i = 0; i < 4 && std::getline(ss, token, '.'); ++i) {
				version[i] = static_cast<std::uint16_t>(std::stoi(token));
			}

			return REL::Version(version);
		}
	};

	struct SetFormEditorID
	{
		static bool thunk(RE::TESForm* a_this, const char* a_str)
		{
			if (!string::is_empty(a_str) && !a_this->IsDynamicForm()) {
				const auto& [map, lock] = RE::TESForm::GetAllFormsByEditorID();
				const RE::BSWriteLockGuard locker{ lock };
				if (map) {
					map->emplace(a_str, a_this);
				}
				Cache::EditorID::GetSingleton()->CacheEditorID(a_this, a_str);
			}
			return func(a_this, a_str);
		}
		static inline REL::Relocation<decltype(thunk)> func;
		static constexpr size_t                        idx{ 0x33 };
	};

	struct SetFormEditorID_SNDR
	{
		static bool thunk(RE::TESForm* a_this, const char* a_str)
		{
			if (!string::is_empty(a_str) && !a_this->IsDynamicForm()) {
				Cache::EditorID::GetSingleton()->CacheEditorID(a_this, a_str);
			}
			return func(a_this, a_str);
		}
		static inline REL::Relocation<decltype(thunk)> func;
		static constexpr size_t                        idx{ 0x33 };
	};

	struct TESFile_GetChunkData
	{
		static bool thunk(RE::TESFile* a_this, void* ptr, std::uint32_t a_chunkSize)
		{
			auto result = func(a_this, ptr, a_chunkSize);
			if (result) {
				if (auto str = static_cast<const char* const>(ptr); !string::is_empty(str) && a_this->currentform.formID != 0) {
					Cache::EditorID::GetSingleton()->CacheEditorID(a_this->currentform.formID, str);
				}
			}
			return result;
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		//stl::write_vfunc<RE::BGSKeyword, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSLocationRefType, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSAction, SetFormEditorID>();

		stl::write_vfunc<RE::BGSTextureSet, SetFormEditorID>();

		//stl::write_vfunc<RE::BGSMenuIcon, SetFormEditorID>();
		//stl::write_vfunc<RE::TESGlobal, SetFormEditorID>();

		stl::write_vfunc<RE::TESClass, SetFormEditorID>();
		stl::write_vfunc<RE::TESFaction, SetFormEditorID>();

		//stl::write_vfunc<RE::BGSHeadPart, SetFormEditorID>();

		stl::write_vfunc<RE::TESEyes, SetFormEditorID>();

		//stl::write_vfunc<RE::TESRace, SetFormEditorID>();
		//stl::write_vfunc<RE::TESSound, SetFormEditorID>();

		stl::write_vfunc<RE::BGSAcousticSpace, SetFormEditorID>();
		stl::write_vfunc<RE::EffectSetting, SetFormEditorID>();

		//stl::write_vfunc<RE::Script, SetFormEditorID>();

		stl::write_vfunc<RE::TESLandTexture, SetFormEditorID>();
		stl::write_vfunc<RE::EnchantmentItem, SetFormEditorID>();
		stl::write_vfunc<RE::SpellItem, SetFormEditorID>();
		stl::write_vfunc<RE::ScrollItem, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectACTI, SetFormEditorID>();
		stl::write_vfunc<RE::BGSTalkingActivator, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectARMO, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectBOOK, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectCONT, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectDOOR, SetFormEditorID>();
		stl::write_vfunc<RE::IngredientItem, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectLIGH, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectMISC, SetFormEditorID>();
		stl::write_vfunc<RE::BGSApparatus, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectSTAT, SetFormEditorID>();
		stl::write_vfunc<RE::BGSStaticCollection, SetFormEditorID>();

		//does not directly inherit from TESForm for some godforsaken reason
		stl::write_vfunc<RE::BGSMovableStatic, 2, SetFormEditorID>();

		stl::write_vfunc<RE::TESGrass, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectTREE, SetFormEditorID>();
		stl::write_vfunc<RE::TESFlora, SetFormEditorID>();
		stl::write_vfunc<RE::TESFurniture, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectWEAP, SetFormEditorID>();
		stl::write_vfunc<RE::TESAmmo, SetFormEditorID>();
		stl::write_vfunc<RE::TESNPC, SetFormEditorID>();
		stl::write_vfunc<RE::TESLevCharacter, SetFormEditorID>();
		stl::write_vfunc<RE::TESKey, SetFormEditorID>();
		stl::write_vfunc<RE::AlchemyItem, SetFormEditorID>();
		stl::write_vfunc<RE::BGSIdleMarker, SetFormEditorID>();
		stl::write_vfunc<RE::BGSNote, SetFormEditorID>();
		stl::write_vfunc<RE::BGSConstructibleObject, SetFormEditorID>();
		stl::write_vfunc<RE::BGSProjectile, SetFormEditorID>();
		stl::write_vfunc<RE::BGSHazard, SetFormEditorID>();
		stl::write_vfunc<RE::TESSoulGem, SetFormEditorID>();
		stl::write_vfunc<RE::TESLevItem, SetFormEditorID>();

		stl::write_vfunc<RE::TESWeather, SetFormEditorID>();

		stl::write_vfunc<RE::TESClimate, SetFormEditorID>();
		stl::write_vfunc<RE::BGSShaderParticleGeometryData, SetFormEditorID>();
		stl::write_vfunc<RE::BGSReferenceEffect, SetFormEditorID>();
		stl::write_vfunc<RE::TESRegion, SetFormEditorID>();

		//stl::write_vfunc<RE::NavMeshInfoMap, SetFormEditorID>();
		//stl::write_vfunc<RE::TESObjectCELL, SetFormEditorID>();

		stl::write_vfunc<RE::TESObjectREFR, SetFormEditorID>();

		//stl::write_vfunc<RE::Actor, SetFormEditorID>();

		stl::write_vfunc<RE::Character, SetFormEditorID>();
		stl::write_vfunc<RE::PlayerCharacter, SetFormEditorID>();
		stl::write_vfunc<RE::MissileProjectile, SetFormEditorID>();
		stl::write_vfunc<RE::ArrowProjectile, SetFormEditorID>();
		stl::write_vfunc<RE::GrenadeProjectile, SetFormEditorID>();
		stl::write_vfunc<RE::BeamProjectile, SetFormEditorID>();
		stl::write_vfunc<RE::FlameProjectile, SetFormEditorID>();
		stl::write_vfunc<RE::ConeProjectile, SetFormEditorID>();
		stl::write_vfunc<RE::BarrierProjectile, SetFormEditorID>();
		stl::write_vfunc<RE::Hazard, SetFormEditorID>();

		//stl::write_vfunc<RE::TESWorldSpace, SetFormEditorID>();
		//stl::write_vfunc<RE::TESObjectLAND, SetFormEditorID>();
		//stl::write_vfunc<RE::NavMesh, SetFormEditorID>();
		//stl::write_vfunc<RE::TESTopic, SetFormEditorID>();

		stl::write_vfunc<RE::TESTopicInfo, SetFormEditorID>();

		//stl::write_vfunc<RE::TESQuest, SetFormEditorID>();
		//stl::write_vfunc<RE::TESIdleForm, SetFormEditorID>();

		stl::write_vfunc<RE::TESPackage, SetFormEditorID>();
		stl::write_vfunc<RE::DialoguePackage, SetFormEditorID>();
		stl::write_vfunc<RE::TESCombatStyle, SetFormEditorID>();
		stl::write_vfunc<RE::TESLoadScreen, SetFormEditorID>();
		stl::write_vfunc<RE::TESLevSpell, SetFormEditorID>();

		//stl::write_vfunc<RE::TESObjectANIO, SetFormEditorID>();

		stl::write_vfunc<RE::TESWaterForm, SetFormEditorID>();
		stl::write_vfunc<RE::TESEffectShader, SetFormEditorID>();
		stl::write_vfunc<RE::BGSExplosion, SetFormEditorID>();
		stl::write_vfunc<RE::BGSDebris, SetFormEditorID>();
		stl::write_vfunc<RE::TESImageSpace, SetFormEditorID>();

		//stl::write_vfunc<RE::TESImageSpaceModifier, SetFormEditorID>();

		stl::write_vfunc<RE::BGSListForm, SetFormEditorID>();
		stl::write_vfunc<RE::BGSPerk, SetFormEditorID>();
		stl::write_vfunc<RE::BGSBodyPartData, SetFormEditorID>();
		stl::write_vfunc<RE::BGSAddonNode, SetFormEditorID>();
		stl::write_vfunc<RE::ActorValueInfo, SetFormEditorID>();
		stl::write_vfunc<RE::BGSCameraShot, SetFormEditorID>();
		stl::write_vfunc<RE::BGSCameraPath, SetFormEditorID>();

		//stl::write_vfunc<RE::BGSVoiceType, SetFormEditorID>();

		stl::write_vfunc<RE::BGSMaterialType, SetFormEditorID>();
		stl::write_vfunc<RE::BGSImpactData, SetFormEditorID>();
		stl::write_vfunc<RE::BGSImpactDataSet, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectARMA, SetFormEditorID>();
		stl::write_vfunc<RE::BGSEncounterZone, SetFormEditorID>();
		stl::write_vfunc<RE::BGSLocation, SetFormEditorID>();
		stl::write_vfunc<RE::BGSMessage, SetFormEditorID>();

		//stl::write_vfunc<RE::BGSRagdoll, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSDefaultObjectManager, SetFormEditorID>();

		stl::write_vfunc<RE::BGSLightingTemplate, SetFormEditorID>();

		//stl::write_vfunc<RE::BGSMusicType, SetFormEditorID>();

		stl::write_vfunc<RE::BGSFootstep, SetFormEditorID>();
		stl::write_vfunc<RE::BGSFootstepSet, SetFormEditorID>();

		//stl::write_vfunc<RE::BGSStoryManagerBranchNode, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSStoryManagerQuestNode, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSStoryManagerEventNode, SetFormEditorID>();

		stl::write_vfunc<RE::BGSDialogueBranch, SetFormEditorID>();
		stl::write_vfunc<RE::BGSMusicTrackFormWrapper, SetFormEditorID>();
		stl::write_vfunc<RE::TESWordOfPower, SetFormEditorID>();
		stl::write_vfunc<RE::TESShout, SetFormEditorID>();
		stl::write_vfunc<RE::BGSEquipSlot, SetFormEditorID>();
		stl::write_vfunc<RE::BGSRelationship, SetFormEditorID>();
		stl::write_vfunc<RE::BGSScene, SetFormEditorID>();
		stl::write_vfunc<RE::BGSAssociationType, SetFormEditorID>();
		stl::write_vfunc<RE::BGSOutfit, SetFormEditorID>();
		stl::write_vfunc<RE::BGSArtObject, SetFormEditorID>();
		stl::write_vfunc<RE::BGSMaterialObject, SetFormEditorID>();
		stl::write_vfunc<RE::BGSMovementType, SetFormEditorID>();

		stl::write_vfunc<RE::BGSSoundDescriptorForm, SetFormEditorID_SNDR>();

		stl::write_vfunc<RE::BGSDualCastData, SetFormEditorID>();
		stl::write_vfunc<RE::BGSSoundCategory, SetFormEditorID>();
		stl::write_vfunc<RE::BGSSoundOutput, SetFormEditorID>();
		stl::write_vfunc<RE::BGSCollisionLayer, SetFormEditorID>();
		stl::write_vfunc<RE::BGSColorForm, SetFormEditorID>();
		stl::write_vfunc<RE::BGSReverbParameters, SetFormEditorID>();
		stl::write_vfunc<RE::BGSLensFlare, SetFormEditorID>();

#ifdef SKYRIM_AE
		if (detail::GetGameVersion() >= SKSE::RUNTIME_SSE_LATEST) {
			REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(0, 20396), 0x403 };  //TESWeather::Load
			stl::write_thunk_call<TESFile_GetChunkData>(target.address());

			logger::info("\t\tInstalled TESWeather editorID patch"sv);
		}
#endif

		logger::info("\t\tInstalled editorID cache"sv);
	}
}
