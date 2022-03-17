#pragma once

namespace Papyrus
{
	using VM = RE::BSScript::Internal::VirtualMachine;
	using StackID = RE::VMStackID;

	bool IsTweakInstalled(VM*, StackID, RE::StaticFunctionTag*, RE::BSFixedString a_tweak);

	bool Bind(VM* a_vm);
}
