#include "pch.h"
#include "hook.h"

bool Detour32(BYTE* src, BYTE* dst, const uintptr_t len)
{
	if (len < 5) return false;

	DWORD curProction;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProction);

	uintptr_t relativeAddress = dst - src - 5;

	*src = 0xE9;

	*(uintptr_t*)(src + 1) = relativeAddress;

	VirtualProtect(src, len, curProction, &curProction);
	return true;
}

BYTE* TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len)
{
	if (len < 5) return 0;

	BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	memcpy_s(gateway, len, src, len);

	uintptr_t gatewayRelativeAddr = src - gateway - 5;

	*(gateway + len) = 0xE9;

	*(uintptr_t*)((uintptr_t)gateway + len + 1) = gatewayRelativeAddr;

	Detour32(src, dst, len);

	return gateway;
}

Hook::Hook(BYTE* src, BYTE* dst, BYTE* PtrToGatewayPtr, uintptr_t len)
{
	this->src = src;
	this->dst = dst;
	this->len = len;
	this->PtrToGatewayFnsPtr = PtrToGatewayPtr;
}

Hook::Hook(const char* exportName, const char* modName, BYTE* dst, BYTE* PtrToGatewayPtr, uintptr_t len)
{
	HMODULE hMod = GetModuleHandleA(modName);

	this->src = (BYTE*)GetProcAddress(hMod, exportName);
	this->dst = dst;
	this->len = len;
	this->PtrToGatewayFnsPtr = PtrToGatewayPtr;
}

void Hook::Enable()
{
	memcpy(originalBytes, src, len);
	*(uintptr_t*)PtrToGatewayFnsPtr = (uintptr_t)TrampHook32(src, dst, len);
	bStatus = true;
}

void Hook::Disable()
{
	mem::Patch(src, originalBytes, len);
	bStatus = false;
}

void Hook::Toggle()
{
	if (!bStatus) Enable();
	else Disable();
}