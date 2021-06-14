#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#include <SimpleIni.h>
#include <frozen/map.h>
#include <xbyak/xbyak.h>

#pragma warning(disable: 4100)

#pragma warning(push)
#ifdef NDEBUG
#	include <spdlog/sinks/basic_file_sink.h>
#else
#	include <spdlog/sinks/msvc_sink.h>
#endif
#pragma warning(pop)

namespace logger = SKSE::log;
namespace string = SKSE::stl::string;

using namespace std::literals;

namespace stl
{
	using SKSE::stl::adjust_pointer;
	using SKSE::stl::is;
	using SKSE::stl::to_underlying;

	void asm_replace(std::uintptr_t a_from, std::size_t a_size, std::uintptr_t a_to);

	template <class T>
	void write_thunk_call(std::uintptr_t a_src)
	{
		auto& trampoline = SKSE::GetTrampoline();
		T::func = trampoline.write_call<5>(a_src, T::thunk);
	}
}

#define DLLEXPORT __declspec(dllexport)

#include "Version.h"
