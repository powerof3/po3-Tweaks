#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMMNOSOUND

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#include <ankerl/unordered_dense.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <xbyak/xbyak.h>

#include <ClibUtil/simpleINI.hpp>
#include <ClibUtil/singleton.hpp>
#include <ClibUtil/string.hpp>

namespace logger = SKSE::log;
namespace string = clib_util::string;
namespace ini = clib_util::ini;

using namespace std::literals;
using namespace clib_util::singleton;

template <class K, class D>
using Map = ankerl::unordered_dense::map<K, D>;

struct string_hash
{
	using is_transparent = void;  // enable heterogeneous overloads
	using is_avalanching = void;  // mark class as high quality avalanching hash

	[[nodiscard]] std::uint64_t operator()(std::string_view str) const noexcept
	{
		return ankerl::unordered_dense::hash<std::string_view>{}(str);
	}
};

template <class D>
using StringMap = ankerl::unordered_dense::map<std::string, D, string_hash, std::equal_to<>>;

namespace stl
{
	using namespace SKSE::stl;

	void asm_replace(std::uintptr_t a_from, std::size_t a_size, std::uintptr_t a_to);

	template <class T>
	void asm_replace(std::uintptr_t a_from)
	{
		asm_replace(a_from, T::size, reinterpret_cast<std::uintptr_t>(T::func));
	}

	template <class T, size_t N = 5>
	void write_thunk_call(std::uintptr_t a_src)
	{
		auto& trampoline = SKSE::GetTrampoline();
		SKSE::AllocTrampoline(14);

		T::func = trampoline.write_call<N>(a_src, T::thunk);
	}

	template <class F, size_t offset, class T>
	void write_vfunc()
	{
		REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[offset] };
		T::func = vtbl.write_vfunc(T::idx, T::thunk);
	}

	template <class F, class T>
	void write_vfunc()
	{
		write_vfunc<F, 0, T>();
	}
}

#ifdef SKYRIM_AE
#	define REL_ID(se, ae) REL::ID(ae)
#	define OFFSET(se, ae) ae
#	define OFFSET_3(se, ae, vr) ae
#elif SKYRIMVR
#	define REL_ID(se, ae) REL::ID(se)
#	define OFFSET(se, ae) se
#	define OFFSET_3(se, ae, vr) vr
#else
#	define REL_ID(se, ae) REL::ID(se)
#	define OFFSET(se, ae) se
#	define OFFSET_3(se, ae, vr) se
#endif

#define DLLEXPORT __declspec(dllexport)

#include "Version.h"
