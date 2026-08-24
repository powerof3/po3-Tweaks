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

	struct TESFile_GetChunkData
	{
		static bool thunk(RE::TESFile* a_this, void* ptr, std::uint32_t a_chunkSize)
		{
			auto result = func(a_this, ptr, a_chunkSize);
			if (result) {
				if (auto str = static_cast<const char* const>(ptr); !REX::STR::IS_EMPTY(str) && a_this->currentform.formID != 0) {
					Cache::EditorID::GetSingleton()->CacheEditorID(a_this->currentform.formID, str);
				}
			}
			return result;
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		//Cache::write_editorID_vfunc<RE::BGSKeyword>();
		//Cache::write_editorID_vfunc<RE::BGSLocationRefType>();
		//Cache::write_editorID_vfunc<RE::BGSAction>();

		Cache::write_editorID_vfunc<RE::BGSTextureSet>();

		//Cache::write_editorID_vfunc<RE::BGSMenuIcon>();
		//Cache::write_editorID_vfunc<RE::TESGlobal>();

		Cache::write_editorID_vfunc<RE::TESClass>();
		Cache::write_editorID_vfunc<RE::TESFaction>();

		//Cache::write_editorID_vfunc<RE::BGSHeadPart>();

		Cache::write_editorID_vfunc<RE::TESEyes>();

		//Cache::write_editorID_vfunc<RE::TESRace>();
		Cache::write_editorID_vfunc<RE::TESSound>();

		Cache::write_editorID_vfunc<RE::BGSAcousticSpace>();
		Cache::write_editorID_vfunc<RE::EffectSetting>();

		//Cache::write_editorID_vfunc<RE::Script>();

		Cache::write_editorID_vfunc<RE::TESLandTexture>();
		Cache::write_editorID_vfunc<RE::EnchantmentItem>();
		Cache::write_editorID_vfunc<RE::SpellItem>();
		Cache::write_editorID_vfunc<RE::ScrollItem>();
		Cache::write_editorID_vfunc<RE::TESObjectACTI>();
		Cache::write_editorID_vfunc<RE::BGSTalkingActivator>();
		Cache::write_editorID_vfunc<RE::TESObjectARMO>();
		Cache::write_editorID_vfunc<RE::TESObjectBOOK>();
		Cache::write_editorID_vfunc<RE::TESObjectCONT>();
		Cache::write_editorID_vfunc<RE::TESObjectDOOR>();
		Cache::write_editorID_vfunc<RE::IngredientItem>();
		Cache::write_editorID_vfunc<RE::TESObjectLIGH>();
		Cache::write_editorID_vfunc<RE::TESObjectMISC>();
		Cache::write_editorID_vfunc<RE::BGSApparatus>();
		Cache::write_editorID_vfunc<RE::TESObjectSTAT>();
		Cache::write_editorID_vfunc<RE::BGSStaticCollection>();

		//does not directly inherit from TESForm for some godforsaken reason
		stl::write_vfunc<RE::BGSMovableStatic, 2, Cache::SetFormEditorID<RE::TESForm>>();

		Cache::write_editorID_vfunc<RE::TESGrass>();
		Cache::write_editorID_vfunc<RE::TESObjectTREE>();
		Cache::write_editorID_vfunc<RE::TESFlora>();
		Cache::write_editorID_vfunc<RE::TESFurniture>();
		Cache::write_editorID_vfunc<RE::TESObjectWEAP>();
		Cache::write_editorID_vfunc<RE::TESAmmo>();
		Cache::write_editorID_vfunc<RE::TESNPC>();
		Cache::write_editorID_vfunc<RE::TESLevCharacter>();
		Cache::write_editorID_vfunc<RE::TESKey>();
		Cache::write_editorID_vfunc<RE::AlchemyItem>();
		Cache::write_editorID_vfunc<RE::BGSIdleMarker>();
		Cache::write_editorID_vfunc<RE::BGSNote>();
		Cache::write_editorID_vfunc<RE::BGSConstructibleObject>();
		Cache::write_editorID_vfunc<RE::BGSProjectile>();
		Cache::write_editorID_vfunc<RE::BGSHazard>();
		Cache::write_editorID_vfunc<RE::TESSoulGem>();
		Cache::write_editorID_vfunc<RE::TESLevItem>();

		Cache::write_editorID_vfunc<RE::TESWeather>();

		Cache::write_editorID_vfunc<RE::TESClimate>();
		Cache::write_editorID_vfunc<RE::BGSShaderParticleGeometryData>();
		Cache::write_editorID_vfunc<RE::BGSReferenceEffect>();
		Cache::write_editorID_vfunc<RE::TESRegion>();

		//Cache::write_editorID_vfunc<RE::NavMeshInfoMap>();
		//Cache::write_editorID_vfunc<RE::TESObjectCELL>();

		Cache::write_editorID_vfunc<RE::TESObjectREFR>();

		//Cache::write_editorID_vfunc<RE::Actor>();

		Cache::write_editorID_vfunc<RE::Character>();
		Cache::write_editorID_vfunc<RE::PlayerCharacter>();
		Cache::write_editorID_vfunc<RE::MissileProjectile>();
		Cache::write_editorID_vfunc<RE::ArrowProjectile>();
		Cache::write_editorID_vfunc<RE::GrenadeProjectile>();
		Cache::write_editorID_vfunc<RE::BeamProjectile>();
		Cache::write_editorID_vfunc<RE::FlameProjectile>();
		Cache::write_editorID_vfunc<RE::ConeProjectile>();
		Cache::write_editorID_vfunc<RE::BarrierProjectile>();
		Cache::write_editorID_vfunc<RE::Hazard>();

		//Cache::write_editorID_vfunc<RE::TESWorldSpace>();
		//Cache::write_editorID_vfunc<RE::TESObjectLAND>();
		//Cache::write_editorID_vfunc<RE::NavMesh>();
		//Cache::write_editorID_vfunc<RE::TESTopic>();

		Cache::write_editorID_vfunc<RE::TESTopicInfo>();

		//Cache::write_editorID_vfunc<RE::TESQuest>();
		//Cache::write_editorID_vfunc<RE::TESIdleForm>();

		Cache::write_editorID_vfunc<RE::TESPackage>();
		Cache::write_editorID_vfunc<RE::DialoguePackage>();
		Cache::write_editorID_vfunc<RE::TESCombatStyle>();
		Cache::write_editorID_vfunc<RE::TESLoadScreen>();
		Cache::write_editorID_vfunc<RE::TESLevSpell>();

		//Cache::write_editorID_vfunc<RE::TESObjectANIO>();

		Cache::write_editorID_vfunc<RE::TESWaterForm>();
		Cache::write_editorID_vfunc<RE::TESEffectShader>();
		Cache::write_editorID_vfunc<RE::BGSExplosion>();
		Cache::write_editorID_vfunc<RE::BGSDebris>();
		Cache::write_editorID_vfunc<RE::TESImageSpace>();

		//Cache::write_editorID_vfunc<RE::TESImageSpaceModifier>();

		Cache::write_editorID_vfunc<RE::BGSListForm>();
		Cache::write_editorID_vfunc<RE::BGSPerk>();
		Cache::write_editorID_vfunc<RE::BGSBodyPartData>();
		Cache::write_editorID_vfunc<RE::BGSAddonNode>();
		Cache::write_editorID_vfunc<RE::ActorValueInfo>();
		Cache::write_editorID_vfunc<RE::BGSCameraShot>();
		Cache::write_editorID_vfunc<RE::BGSCameraPath>();

		//Cache::write_editorID_vfunc<RE::BGSVoiceType>();

		Cache::write_editorID_vfunc<RE::BGSMaterialType>();
		Cache::write_editorID_vfunc<RE::BGSImpactData>();
		Cache::write_editorID_vfunc<RE::BGSImpactDataSet>();
		Cache::write_editorID_vfunc<RE::TESObjectARMA>();
		Cache::write_editorID_vfunc<RE::BGSEncounterZone>();
		Cache::write_editorID_vfunc<RE::BGSLocation>();
		Cache::write_editorID_vfunc<RE::BGSMessage>();

		//Cache::write_editorID_vfunc<RE::BGSRagdoll>();
		//Cache::write_editorID_vfunc<RE::BGSDefaultObjectManager>();

		Cache::write_editorID_vfunc<RE::BGSLightingTemplate>();

		//Cache::write_editorID_vfunc<RE::BGSMusicType>();

		Cache::write_editorID_vfunc<RE::BGSFootstep>();
		Cache::write_editorID_vfunc<RE::BGSFootstepSet>();

		//Cache::write_editorID_vfunc<RE::BGSStoryManagerBranchNode>();
		//Cache::write_editorID_vfunc<RE::BGSStoryManagerQuestNode>();
		//Cache::write_editorID_vfunc<RE::BGSStoryManagerEventNode>();

		Cache::write_editorID_vfunc<RE::BGSDialogueBranch>();
		Cache::write_editorID_vfunc<RE::BGSMusicTrackFormWrapper>();
		Cache::write_editorID_vfunc<RE::TESWordOfPower>();
		Cache::write_editorID_vfunc<RE::TESShout>();
		Cache::write_editorID_vfunc<RE::BGSEquipSlot>();
		Cache::write_editorID_vfunc<RE::BGSRelationship>();
		Cache::write_editorID_vfunc<RE::BGSScene>();
		Cache::write_editorID_vfunc<RE::BGSAssociationType>();
		Cache::write_editorID_vfunc<RE::BGSOutfit>();
		Cache::write_editorID_vfunc<RE::BGSArtObject>();
		Cache::write_editorID_vfunc<RE::BGSMaterialObject>();
		Cache::write_editorID_vfunc<RE::BGSMovementType>();
		Cache::write_editorID_vfunc<RE::BGSSoundDescriptorForm>();
		Cache::write_editorID_vfunc<RE::BGSDualCastData>();
		Cache::write_editorID_vfunc<RE::BGSSoundCategory>();
		Cache::write_editorID_vfunc<RE::BGSSoundOutput>();
		Cache::write_editorID_vfunc<RE::BGSCollisionLayer>();
		Cache::write_editorID_vfunc<RE::BGSColorForm>();
		Cache::write_editorID_vfunc<RE::BGSReverbParameters>();
		Cache::write_editorID_vfunc<RE::BGSLensFlare>();
		Cache::write_editorID_vfunc<RE::BGSVolumetricLighting>();

#ifdef SKYRIM_AE
		if (detail::GetGameVersion() >= SKSE::RUNTIME_SSE_LATEST) {
			REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(0, 20396), 0x403 };  //TESWeather::Load
			stl::write_thunk_call<TESFile_GetChunkData>(target.address());

			REX::INFO("\t\tInstalled TESWeather editorID patch"sv);
		}
#endif

		REX::INFO("\t\tInstalled editorID cache"sv);
	}
}
