#include "Cache.h"
#include "Tweaks.h"

//applies snow havok material to objects using snow dir mat
namespace Tweaks::DynamicSnowMaterial
{
	using MAT = RE::MATERIAL_ID;

	struct detail
	{
		static bool must_only_contain_textureset(const RE::TESBoundObject* a_base, const std::pair<std::string_view, std::string_view>& a_modelPath)
		{
			if (const auto model = a_base->As<RE::TESModelTextureSwap>(); model && model->alternateTextures && model->numAlternateTextures > 0) {
				std::span altTextures{ model->alternateTextures, model->numAlternateTextures };
				return std::ranges::all_of(altTextures, [&](const auto& textures) {
					const auto  txst = textures.textureSet;
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

	void Install()
	{
		GetMaterialIDPatch::Install();

		logger::info("Installed dynamic snow material tweak"sv);
	}
}
