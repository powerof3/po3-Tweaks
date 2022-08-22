#include "Fixes.h"

//mark produce as stolen if pulled from leveled lists
namespace Fixes::FlagStolenProduce
{
	static REL::Relocation<std::uintptr_t> target{ REL_ID(14692, 14864) };  //TESProduceForm::HandleActivate

	namespace AddStolenTag
	{
		struct CALCED_OBJECT
		{
			RE::TESForm* object;                   // 00
			std::uint32_t count;                   // 08
			std::uint32_t pad0C;                   // 0C
			RE::ContainerItemExtra containerItem;  // 10
		};
		static_assert(sizeof(CALCED_OBJECT) == 0x28);

		struct AddCalcedObjectsToInventoryPatch
		{
			static void Install()
			{
				struct Patch : Xbyak::CodeGenerator
				{
					Patch(std::uintptr_t a_func)
					{
						Xbyak::Label f;
#ifdef SKYRIM_AE
						mov(r9, r14);
#else
						mov(r9, r12);
#endif
						jmp(ptr[rip + f]);

						L(f);
						dq(a_func);
					}
				};

				Patch patch{ reinterpret_cast<std::uintptr_t>(AddCalcedObjectsToInventory) };
				patch.ready();

				auto& trampoline = SKSE::GetTrampoline();
				SKSE::AllocTrampoline(31);

				_AddCalcedObjectsToInventory = trampoline.write_call<5>(target.address() + OFFSET(0xA27, 0x21C), trampoline.allocate(patch));
			}

		private:
			static void AddCalcedObjectsToInventory(RE::BSScrapArray<CALCED_OBJECT>& a_calcedObjects, RE::TESObjectREFR* a_targetRef, bool a_unk03, RE::TESObjectREFR* a_sourceRef)
			{
				const auto owner = a_sourceRef->GetOwner();
				for (auto& calcedObject : a_calcedObjects) {
					calcedObject.containerItem.owner = owner;
				}
				return _AddCalcedObjectsToInventory(a_calcedObjects, a_targetRef, a_unk03);
			}
			static inline REL::Relocation<void(const RE::BSScrapArray<CALCED_OBJECT>&, RE::TESObjectREFR*, bool)> _AddCalcedObjectsToInventory;
		};

		void Install()
		{
			AddCalcedObjectsToInventoryPatch::Install();
		}
	}

	//consistent with other SendStealAlarm's
	namespace AddWarning
	{
		void Install()
		{
			struct Patch : Xbyak::CodeGenerator
			{
				Patch()
				{
					mov(byte[rsp + 0x30], 1);  //a_allowWarning = true
				}
			};

			Patch patch;
			patch.ready();

			REL::safe_write(target.address() + OFFSET(0x289, 0x299), std::span{ patch.getCode(), patch.getSize() });
		}
	}

	void Install()
	{
		AddStolenTag::Install();
		AddWarning::Install();

		logger::info("Installed flag stolen produce fix"sv);
	}
}
