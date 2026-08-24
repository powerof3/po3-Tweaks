#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMMNOSOUND
#define NOMINMAX

#include "RE/Skyrim.h"
#include "REX/REX.h"
#include "SKSE/SKSE.h"

#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered/unordered_flat_set.hpp>
#include <spdlog/sinks/basic_file_sink.h>
#include <xbyak/xbyak.h>

using namespace std::literals;

template <class K, class D, class H = boost::hash<K>, class KEqual = std::equal_to<K>>
using FlatMap = boost::unordered_flat_map<K, D, H, KEqual>;

template <class K, class H = boost::hash<K>, class KEqual = std::equal_to<K>>
using FlatSet = boost::unordered_flat_set<K, H, KEqual>;

struct string_hash
{
	using is_transparent = void;

	std::size_t operator()(const char* str) const
	{
		return boost::hash<std::string_view>{}(str);
	}

	std::size_t operator()(std::string_view str) const
	{
		return boost::hash<std::string_view>{}(str);
	}

	std::size_t operator()(const std::string& str) const
	{
		return boost::hash<std::string>{}(str);
	}
};

template <class D>
using StringMap = FlatMap<std::string, D, string_hash, std::equal_to<>>;

namespace stl
{
	void asm_replace(std::uintptr_t a_from, std::size_t a_size, std::uintptr_t a_to);

	template <class T>
	void asm_replace(std::uintptr_t a_from)
	{
		asm_replace(a_from, T::size, reinterpret_cast<std::uintptr_t>(T::func));
	}

	template <class T, std::size_t N = 5>
	void write_thunk_call(std::uintptr_t a_src)
	{
		auto& trampoline = REL::GetTrampoline();
		T::func = trampoline.write_call<N>(a_src, T::thunk);
	}

	template <class F, std::size_t offset, class T>
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

namespace Runtime
{
	inline constexpr REL::Version SSE_1_7_99(1, 7, 99, 0);
	inline constexpr REL::Version MIN_ADDRESS_LIBRARY_V5 = SSE_1_7_99;

	inline REL::Version version{};

	[[nodiscard]] inline bool IsAtLeast1_7_99() noexcept
	{
		return version >= Runtime::SSE_1_7_99;
	}
}

#ifdef SKYRIM_AE
#	define RELOCATION_ID(se, ae) REL::ID(ae)
#	define OFFSET(se, ae) ae
#	define OFFSET_3(se, ae, vr) ae
#elif SKYRIMVR
#	define RELOCATION_ID(se, ae) REL::ID(se)
#	define OFFSET(se, ae) se
#	define OFFSET_3(se, ae, vr) vr
#else
#	define RELOCATION_ID(se, ae) REL::ID(se)
#	define OFFSET(se, ae) se
#	define OFFSET_3(se, ae, vr) se
#endif

#ifdef SKYRIM_AE
#	define OFFSET_VERSIONED(se, ae, ae1799, vr) \
		(Runtime::IsAtLeast1_7_99() ? ae1799 : ae)
#elif SKYRIMVR
#	define OFFSET_VERSIONED(se, ae, ae1799, vr) vr
#else
#	define OFFSET_VERSIONED(se, ae, ae1799, vr) se
#endif

#define DLLEXPORT __declspec(dllexport)

#include "Version.h"
