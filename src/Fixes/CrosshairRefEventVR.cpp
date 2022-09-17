#include "Fixes.h"

#ifdef SKYRIMVR
//fixes VR CrosshairRefEvent and GetCurrentCrosshairRef to also take the hand selection
//thanks to @adamhynek for help with offsets and fixing stupid bugs
namespace Fixes::CrosshairRefEventVR
{
	struct LookupByHandle
	{
		static bool thunk(RE::RefHandle& a_refHandle, RE::NiPointer<RE::TESObjectREFR>& a_refrOut)
		{
			bool result = func(a_refHandle, a_refrOut);
			if (patchSKSE)
				// set skse g_curCrosshairRef
				REL::safe_write<std::uintptr_t>((std::uintptr_t)(sksevr_base + 0x15D9F0), (std::uint64_t)a_refrOut.get());
			const SKSE::CrosshairRefEvent event{ a_refrOut };
			RE::BSTEventSource<SKSE::CrosshairRefEvent>* source = SKSE::GetCrosshairRefEventSource();
			if (source) {
				//fire SKSECrosshairRefEvent
				source->SendEvent(std::addressof(event));
			}
			return result;
		}
		static inline REL::Relocation<decltype(thunk)> func;
		static inline std::uintptr_t sksevr_base;
		static inline bool patchSKSE = false;
	};

	void Install(std::uint32_t a_skse_version)
	{
		LookupByHandle::sksevr_base = reinterpret_cast<uintptr_t>(GetModuleHandleA("sksevr_1_4_15"));
		bool code_match = true;
		const uint8_t* read_addr = (uint8_t*)(uintptr_t)(LookupByHandle::sksevr_base + 0x52ef0);
		static const uint8_t read_expected[] = { 0x48, 0x8b, 0x05, 0xf9, 0xaa, 0x10, 0x00 };
		const uint8_t* write_addr = (uint8_t*)(uintptr_t)(LookupByHandle::sksevr_base + 0xdd15);
		static const uint8_t write_expected[] = { 0x48, 0x89, 0x0d, 0xd4, 0xfc, 0x14, 0x00 };
		if (std::memcmp((const void*)read_addr, read_expected, sizeof(read_expected)) && std::memcmp((const void*)write_addr, write_expected, sizeof(write_expected))) {
			logger::info("VR CrosshairRefEvent: Read and write crosshair code is not expected"sv);
			code_match = false;
		}
		if (a_skse_version == 33554624 && code_match) {  //2.0.12
			LookupByHandle::patchSKSE = true;
			logger::info("VR CrosshairRefEvent: Found patchable sksevr_1_4_15.dll version {} with base {:x}", a_skse_version, LookupByHandle::sksevr_base);
		} else
			logger::info("VR CrosshairRefEvent: Found unknown sksevr_1_4_15.dll version {} with base {:x}; not patching", a_skse_version, LookupByHandle::sksevr_base);
		REL::Relocation<std::uintptr_t> target{ REL::Offset(0x6D2F82) };
		stl::write_thunk_call<LookupByHandle>(target.address());

		logger::info("Installed VR CrosshairRefEvent fix"sv);
	}
}
#endif
