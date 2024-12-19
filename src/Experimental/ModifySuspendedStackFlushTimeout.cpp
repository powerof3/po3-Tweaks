#include "Experimental.h"
#include "Settings.h"

//modify timeout check for Suspended Stack flushing
namespace Experimental::ModifySuspendedStackFlushTimeout
{
	namespace ModifyLimit
	{
		void Install(double a_milliseconds)
		{
			static REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(53209, 54020), OFFSET(0x3D, 0x10D) };

			struct StackDumpTimeout_Code : Xbyak::CodeGenerator
			{
				StackDumpTimeout_Code(std::uintptr_t a_address, double a_val)
				{
					static auto VAL = static_cast<float>(a_val);

					push(rax);

					mov(rax, stl::unrestricted_cast<std::uintptr_t>(std::addressof(VAL)));
					mulss(xmm0, ptr[rax]);

					pop(rax);

					jmp(ptr[rip]);
					dq(a_address + 0x8);
				}
			};

			StackDumpTimeout_Code code(target.address(), a_milliseconds);
			code.ready();

			auto& trampoline = SKSE::GetTrampoline();
			SKSE::AllocTrampoline(38);

			trampoline.write_branch<6>(
				target.address(),
				trampoline.allocate(code));

			logger::info("set timeout on suspended stack flush to {} seconds"sv, a_milliseconds / 1000.0);
		}
	}

	namespace NoLimit
	{
		void Install()
		{
			static REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(53209, 54020), OFFSET(0x8B, 0x152) };
			REL::safe_write(target.address(), static_cast<std::uint8_t>(0xEB));  // swap jle 0x7e for jmp 0xeb

			logger::info("Removed timeout check on suspended stack flush"sv);
		}
	}

	void Install()
	{
		if (const auto timeoutSeconds = Settings::GetSingleton()->GetExperimental().stackDumpTimeoutModifier; timeoutSeconds != 30.0) {
			if (timeoutSeconds == 0.0) {
				NoLimit::Install();
			} else {
				ModifyLimit::Install(timeoutSeconds * 1000.0);
			}
		}
	}
}
