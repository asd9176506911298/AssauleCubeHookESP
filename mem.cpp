#include "pch.h"
#include "mem.h"

void mem::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{
	DWORD oldproction;
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldproction);
	WriteProcessMemory(hProcess, dst, src, size, nullptr);
	VirtualProtectEx(hProcess, dst, size, oldproction, &oldproction);
}

void mem::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess)
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);

	PatchEx(dst, nopArray, size, hProcess);
	delete[] nopArray;
}

uintptr_t mem::FindDMAAdy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int>offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); i++)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), nullptr);
		addr += offsets[i];
	}
	return addr;
}

void mem::Patch(BYTE* dst, BYTE* src, unsigned int size)
{
	DWORD oldproction;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldproction);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldproction, &oldproction);
}

void mem::Nop(BYTE* dst, unsigned int size)
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);
	Patch(dst, nopArray, size);
	delete[] nopArray;
}

uintptr_t mem::FindDMAAdy(uintptr_t ptr, std::vector<unsigned int>offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); i++)
	{
		addr = *(uintptr_t*)addr;
		addr += offsets[i];
	}
	return addr;
}

