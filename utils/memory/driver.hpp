#ifndef _DRIVER_H
#define _DRIVER_H

#pragma once
#include <msxml.h>    
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <vector>
#include <random>
#include <memoryapi.h>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <string_view>

typedef struct _QUERY_DRIVER_STATUS {
	BOOL received;
} QUERY_DRIVER_STATUS;

typedef struct _READ_KERNEL
{
	UINT_PTR address;
	ULONGLONG size;
	void* output;
}READ_KERNEL;

typedef struct _READ_STRING_KERNEL
{
	UINT_PTR address;
	ULONGLONG size;
	void* buffer_address;
}READ_STRING_KERNEL;

typedef struct _WRITE_KERNEL
{
	UINT_PTR address;
	ULONGLONG size;
	void* buffer_address;
}WRITE_KERNEL;

typedef struct _WRITE_STRING_KERNEL
{
	UINT_PTR address;
	ULONGLONG size;
	void* buffer_address;
}WRITE_STRING_KERNEL;

typedef struct _GET_BASE_KERNEL
{
	UINT_PTR address;
	ULONG64 base_address;
	const char* module_name;
}GET_BASE_KERNEL;

typedef struct _SCAN_CLASS_KERNEL
{
	UINT_PTR address;
	const char* class_name;
	ULONG64 klass;
}SCAN_CLASS_KERNEL;

typedef struct _GET_PID_KERNEL
{
	UINT_PTR address;
	const char* process_name;
	ULONG pid_of_source;
}GET_PID_KERNEL;

typedef struct _ALLOC_MEM_KERNEL
{
	UINT_PTR address;
	ULONG	alloc_type;
	ULONGLONG size;
	void* output;
}ALLOC_MEM_KERNEL;

typedef struct _CHANGE_PROTECTION_KERNEL
{
	ULONG		protection;
	ULONG		protection_old;
}CHANGE_PROTECTION_KERNEL;

typedef struct _memory_struct
{
	BOOLEAN query;
	QUERY_DRIVER_STATUS query_driver;

	BOOLEAN read;
	READ_KERNEL read_kernel;

	BOOLEAN read_string;
	READ_STRING_KERNEL read_string_kernel;

	BOOLEAN scan_for_klass;
	SCAN_CLASS_KERNEL scan_class_kernel;

	BOOLEAN write;
	WRITE_KERNEL write_kernel;

	BOOLEAN write_string;
	WRITE_STRING_KERNEL write_string_kernel;

	BOOLEAN get_base;
	GET_BASE_KERNEL get_base_kernel;

	BOOLEAN get_pid;
	GET_PID_KERNEL get_pid_kernel;

	BOOLEAN alloc_memory;
	ALLOC_MEM_KERNEL alloc_memory_kernel;

	BOOLEAN	change_protection;
	CHANGE_PROTECTION_KERNEL change_protection_kernel;

	HWND window_handle;
	UINT_PTR thread_context;
}memory_struct;

template<typename ... A>
uint64_t call_hook(bool secondary, const A ... arguments)
{
	void* control_function = GetProcAddress(LoadLibrary("win32u.dll"), "NtDxgkGetTrackedWorkloadStatistics");
	const auto control = static_cast<uint64_t(__stdcall*)(A...)>(control_function);

	return control(arguments ...);
}

namespace driver
{
	static bool query_driver(bool secondary = false) {
		memory_struct m = { 0 };
		m.query = TRUE;

		call_hook(secondary, &m);

		return m.query_driver.received == 1;
	}

	static void get_process_id(const char* process_name, bool secondary = false)
	{
		memory_struct m = { 0 };
		m.get_pid = TRUE;
		m.get_pid_kernel.process_name = process_name;
		m.get_base = FALSE;
		m.read = FALSE;
		m.read_string = FALSE;
		m.write = FALSE;
		m.write_string = FALSE;
		m.change_protection = FALSE;
		m.alloc_memory = FALSE;
		call_hook(secondary, &m);
	}

	static ULONG64 scan_for_klass(const char* class_name, bool secondary = false)
	{
		memory_struct m = { 0 };
		m.query = FALSE;
		m.get_base = FALSE;
		m.read = FALSE;
		m.scan_for_klass = TRUE;
		m.scan_class_kernel.class_name = class_name;
		m.get_pid = FALSE;
		m.read_string = FALSE;
		m.write = FALSE;
		m.write_string = FALSE;
		m.change_protection = FALSE;
		m.alloc_memory = FALSE;
		call_hook(secondary, &m);

		ULONG64 klass = NULL;
		klass = m.scan_class_kernel.klass;
		return klass;
	}

	static ULONG64 get_module_base_address(const char* module_name, bool secondary = false)
	{

		memory_struct m = { 0 };
		m.get_base = TRUE;
		m.query = FALSE;
		m.read = FALSE;
		m.get_pid = FALSE;
		m.read_string = FALSE;
		m.write = FALSE;
		m.write_string = FALSE;
		m.get_base_kernel.module_name = module_name;
		m.change_protection = FALSE;
		m.alloc_memory = FALSE;
		call_hook(secondary, &m);

		ULONG64 base = NULL;
		base = m.get_base_kernel.base_address;
		return base;
	}

	static void read(UINT_PTR ReadAddress, void* buffer, uintptr_t size, bool secondary = false)
	{
		memory_struct m;
		m.read = TRUE;
		m.query = FALSE;
		m.read_kernel.output = buffer;
		m.read_kernel.size = size;
		m.read_kernel.address = ReadAddress;
		m.get_pid = FALSE;
		m.read_string = FALSE;
		m.write_string = FALSE;
		m.write = FALSE;
		m.get_base = FALSE;
		m.change_protection = FALSE;
		m.alloc_memory = FALSE;
		call_hook(secondary, &m);
	}

	template <class T>
	T read(UINT_PTR ReadAddress, bool secondary = false)
	{

		T response{};

		memory_struct m;
		m.read = TRUE;
		m.query = FALSE;
		m.read_kernel.size = sizeof(T);
		m.read_kernel.address = ReadAddress;
		m.read_kernel.output = &response;
		m.get_pid = FALSE;
		m.read_string = FALSE;
		m.write_string = FALSE;
		m.write = FALSE;
		m.get_base = FALSE;
		m.change_protection = FALSE;
		m.alloc_memory = FALSE;
		call_hook(secondary, &m);

		return response;
	}

	static bool WriteVirtualMemoryRaw(UINT_PTR WriteAddress, UINT_PTR SourceAddress, SIZE_T WriteSize, bool secondary = false);

	template<typename S>
	bool write(UINT_PTR WriteAddress, const S& value, bool secondary = false)
	{
		return WriteVirtualMemoryRaw(WriteAddress, (UINT_PTR)&value, sizeof(S), secondary);
	}
	bool WriteVirtualMemoryRaw(UINT_PTR WriteAddress, UINT_PTR SourceAddress, SIZE_T WriteSize, bool secondary)
	{
		memory_struct m;
		m.write = TRUE;
		m.query = FALSE;
		m.write_kernel.address = WriteAddress;
		m.write_kernel.buffer_address = (void*)SourceAddress;
		m.write_kernel.size = WriteSize;
		m.get_pid = FALSE;
		m.read = FALSE;
		m.read_string = FALSE;
		m.get_base = FALSE;
		m.write_string = FALSE;
		m.change_protection = FALSE;
		m.alloc_memory = FALSE;
		call_hook(secondary, &m);

		return true;
	}
}

#endif //_DRIVER_H