#include "Cache.h"
#include "Tweaks.h"

//applies snow havok material to objects using snow dir mat
namespace Tweaks::DynamicSnowMaterial
{
	struct detail
	{
		static bool must_only_contain_textureset(const RE::TESBoundObject* a_base, const std::pair<std::string_view, std::string_view>& a_modelPath)
		{
			if (const auto model = a_base->As<RE::TESModelTextureSwap>(); model && model->alternateTextures && model->numAlternateTextures > 0) {
				std::span altTextures{ model->alternateTextures, model->numAlternateTextures };
				return std::ranges::all_of(altTextures, [&](const auto& textures) {
					if (const auto txst = textures.textureSet) {
						const std::string_view path{ txst->textures[0].textureName.c_str() };
						return path.contains(a_modelPath.first) || path.contains(a_modelPath.second);
					}
					return false;
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
				result = must_only_contain_textureset(base, { "Snow", "Mask" });  // dirtcliffmask
			}

			if (!result) {  //seasons
				const auto root = a_ref->Get3D();
				result = root && root->HasExtraData("SOS_SNOW_SHADER");
			}

			return result;
		}

		static bool is_stairs(RE::MATERIAL_ID a_matID)
		{
			return std::ranges::find(stairsMat, a_matID) != stairsMat.end();
		}

		static bool is_blacklisted(RE::MATERIAL_ID a_matID)
		{
			return std::ranges::find(blacklistedMat, a_matID) != blacklistedMat.end();
		}

		static constexpr std::array blacklistedMat{ RE::MATERIAL_ID::kSnow, RE::MATERIAL_ID::kSnowStairs, RE::MATERIAL_ID::kIce, RE::MATERIAL_ID::kCloth, RE::MATERIAL_ID::kGlass, RE::MATERIAL_ID::kBone, RE::MATERIAL_ID::kBarrel };
		static constexpr std::array stairsMat{ RE::MATERIAL_ID::kStoneStairs, RE::MATERIAL_ID::kStoneAsStairs, RE::MATERIAL_ID::kStoneStairsBroken, RE::MATERIAL_ID::kWoodAsStairs, RE::MATERIAL_ID::kWoodStairs };
	};

	struct GetMaterialIDPatch
	{
		static void Install()
		{
			REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(35320, 36215), OFFSET(0x600, 0x6A4) };  //BGSImpactManager::PlayImpactEffect

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
			_GetMaterialID = trampoline.write_call<5>(target.address(), trampoline.allocate(patch));
		}

	private:
		static RE::MATERIAL_ID GetMaterialID(RE::bhkShape* a_shape, std::uint32_t a_ID, RE::hkpCollidable* a_collidable)
		{
			auto matID = _GetMaterialID(a_shape, a_ID);
			if (a_collidable && !detail::is_blacklisted(matID)) {
				if (const auto ref = RE::TESHavokUtilities::FindCollidableRef(*a_collidable); ref && detail::is_snow_object(ref)) {
					matID = detail::is_stairs(matID) ? RE::MATERIAL_ID::kSnowStairs : RE::MATERIAL_ID::kSnow;
				}
			}
			return matID;
		}
		static inline REL::Relocation<RE::MATERIAL_ID(RE::bhkShape*, std::uint32_t)> _GetMaterialID;
	};

	void Install()
	{
		GetMaterialIDPatch::Install();

		logger::info("\t\tInstalled dynamic snow material tweak"sv);
	}
}
