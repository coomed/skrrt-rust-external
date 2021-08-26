#ifndef _MEMORY_HPP
#define _MEMORY_HPP

// includes for this header
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <string>
#include <TlHelp32.h>
#include <map>
#include <mutex>
#include "offsets.hpp"
#include "driver.hpp"
#define LOG_Y(f_, ...) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); printf("    [SKRRT] "); SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); printf((f_), ##__VA_ARGS__)
#define LOG_G(f_, ...) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2); printf("    [SKRRT] "); SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); printf((f_), ##__VA_ARGS__)
#define LOG_R(f_, ...) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4); printf("    [SKRRT] "); SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); printf((f_), ##__VA_ARGS__)
#define LOG_B(f_, ...) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1); printf("    [SKRRT] "); SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); printf((f_), ##__VA_ARGS__)

#define _USERMODE 0

// thread safe std::map
template <typename TKey, typename TVal>
struct ThreadMap
{
    std::map<TKey, TVal> map;
    std::mutex mutex;
};

// thread safe std::vector
template <typename Type>
struct ThreadVec
{
    std::vector<Type> vec;
    std::mutex mutex;
};

// a class for interacting with memory
class Memory
{
public: // member vars
    HANDLE handle = 0;  // holds the handle to rust
    int process_id = 0; // holds rust's process id
    uint64_t up_addy = 0;   // holds the unityplayer.dll address 
    uint64_t ga_addy = 0;   // holds the gameassembly.dll address 
public:
    // constructor for memory class, requires a process id
    Memory(int _process_id)
    {
        // stores the process id
        this->process_id = _process_id;

#if _USERMODE == 1

        // Open a handle to the target process with PROCESS_ALL_ACCESS
        HANDLE _handle = OpenProcess(PROCESS_ALL_ACCESS, false, _process_id);

        // Check if the handle is valid
        if (_handle == INVALID_HANDLE_VALUE)
            return;

        // Store the handle value.
        this->handle = _handle;
#else
        driver::get_process_id("RustClient.exe");
#endif
        this->up_addy = GetModuleBase("UnityPlayer.dll");
        LOG_G("UnityPlayer.dll Module Base: 0x%llx\n", this->up_addy);

        this->ga_addy = GetModuleBase("GameAssembly.dll");
        LOG_G("GameAssembly.dll Module Base: 0x%llx\n", this->ga_addy);
    }

    // close a handle to the target and remove it
    bool Detach()
    {
#if _USERMODE == 1

        CloseHandle(this->handle);
        this->handle = INVALID_HANDLE_VALUE;
        return true;

#endif
    }

    // read value from memory address by type, specifying a pointer chain to follow
    template<typename Type>
    Type ReadChain(uint64_t address, std::vector<uint64_t> chain)
    {
        uint64_t current = address;
        for (int i = 0; i < chain.size() - 1; i++)
        {
            current = this->Read<uint64_t>(current + chain[i]);
        }
        return this->Read<Type>(current + chain[chain.size() - 1]);
    }

    // write buffer to address 
    bool Write(uint64_t address, LPVOID buffer, SIZE_T size)
    {
#if _USERMODE == 1

        return WriteProcessMemory(this->handle, (LPVOID)address, buffer, size, 0);

#endif

        return driver::WriteVirtualMemoryRaw((UINT_PTR)address, (UINT_PTR)buffer, size);
    }

    // write value to memory address by type
    template<typename type>
    bool Write(uint64_t address, type to_write)
    {
        type buffer = to_write;
        return this->Write(address, &buffer, sizeof(buffer));
    }

    // read memory overload for ReadProcessMemory style calls
    bool Read(uint64_t address, LPVOID buffer, SIZE_T size)
    {
        if (address <= 0xFFFFFFFF) return 0;

#if _USERMODE == 1

        // will store the number of bytes read from the target address
        SIZE_T read;

        // perform the read
        if (ReadProcessMemory(this->handle, reinterpret_cast<void*>(address), buffer, size, &read) == 0)
        {
            // read failed, return false
            return false;
        }

        // check read size matches intended read size
        if (read < size) return false;

        // read succeeded, all bytes read.
        return true;

#endif

        driver::read((UINT_PTR)address, buffer, size);

        // read succeeded, all bytes read.
        return true;
    }

    // read value from memory address by type
    template<typename type>
    type Read(uint64_t address)
    {
        type buffer;
        this->Read(address, &buffer, sizeof(buffer));
        return buffer;
    }

    // read an ASCII string to an std::string
    std::string ReadASCII(uint64_t address)
    {
        // allocate our buffer with string size.
        char buffer[1024];

        // read the string at address into the buffer
        this->Read(address, &buffer, 1024);

        // return buffer string
        return std::string(buffer);
    }

    // Read an ASCII string to an std::string
    std::string ReadMonoString(uint64_t address)
    {
        /* Allocate the buffer for the string */
        char buffer[512];

        /* Read into the ptr */
        address = this->Read<uint64_t>(address);

        /* Read the string into the buffer */
        this->Read(address, buffer, 512);

        /* Return the read string */
        return buffer;
    }

    // read a unicode string to an std::wstring
    std::wstring ReadUnicode(uint64_t address)
    {
        // allocate our buffer with string size
        wchar_t buffer[1024 * sizeof(wchar_t)];

        // read the string at address into the buffer
        this->Read(address, &buffer, 1024 * sizeof(wchar_t));

        // convert the buffer to a std::wstring and return
        return std::wstring(buffer);
    }

    // read a native string
    std::string ReadNative(uint64_t address)
    {
        // allocate our buffer with string size.
        char buffer[1024];

        // read the string at address into the buffer
        this->Read(this->Read<uint64_t>(address), &buffer, 1024);

        // return buffer string
        return std::string(buffer);
    }

    // get the base address of a module loaded into target proc
    uint64_t GetModuleBase(const char* module_name)
    {
#if _USERMODE == 1

        // Get a snapshot of the target process.
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, (DWORD)this->process_id);

        // Check if our snapshot is valid.
        if (snapshot == INVALID_HANDLE_VALUE)
        {
            return 0;
        }

        // Create our empty moduleentry buffer.
        MODULEENTRY32 mod_entry;
        mod_entry.dwSize = sizeof(MODULEENTRY32);

        // Check the first item in our list is valid
        if (Module32First(snapshot, &mod_entry) == FALSE)
        {
            return 0;
        }

        // Iterate through the list until we find our module.
        while (Module32Next(snapshot, &mod_entry) == TRUE)
        {
            if (strcmp(mod_entry.szModule, module_name) == 0)
            {
                return (uint64_t)mod_entry.modBaseAddr;
            }
        }

        // Return 0 if we reach here.
        return 0;

#endif


        return driver::get_module_base_address(module_name);
    }

    // Implementation for this->scan_for_class
    uint64_t IL2CPPScan(const char* class_name)
    {
#if _USERMODE == 1

        uint64_t base = this->GetModuleBase("GameAssembly.dll");
        IMAGE_DOS_HEADER dos_header = this->Read<IMAGE_DOS_HEADER>(base);
        IMAGE_SECTION_HEADER data_header = this->Read<IMAGE_SECTION_HEADER>(base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (3 * 40));
        IMAGE_SECTION_HEADER next_section = this->Read<IMAGE_SECTION_HEADER>(base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (4 * 40));
        DWORD data_size = next_section.VirtualAddress - data_header.VirtualAddress;

        // Check if this section is .data, if not, return 0
        if (strcmp((char*)data_header.Name, ".data"))
        {
            return 0;
        }

        // Scan for the requested class
        for (uint64_t offset = data_size; offset > 0; offset -= 8)
        {
            char klass_name[0x100] = { 0 };
            auto klass = this->Read<uint64_t>(base + data_header.VirtualAddress + offset);
            if (klass == 0) { continue; }
            auto name_pointer = this->Read<uint64_t>(klass + 0x10);
            if (name_pointer == 0) { continue; }
            this->Read(name_pointer, klass_name, sizeof(klass_name));
            if (!strcmp(klass_name, class_name))
            {
                return klass;
            }
        }

        return 0;

#endif

        return driver::scan_for_klass(class_name);
    }
};

// a class for handling the rust process
class Rust
{
public: // member vars
    int process_id = 0; // holds the process id
    Memory* mem = 0; // memory class for this process
public:
    // constructor for process class, requries a process name
    Rust()
    {
        // get the process id of target process
        this->process_id = Rust::GetPID("RustClient.exe");

        // Check if process ID is valid
        if (this->process_id == 0) {
            LOG_R("Had issue grabbing RustClient.exe ProcessID.\n");
            return;
        }

        LOG_G("Grabbed ProcessID from RustClient.exe. %d\n", this->process_id);

        // create memory class
        this->mem = new Memory(this->process_id);
    }

    int GetPID(const char* process_name)
    {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

        // Check if first process is valid.
        if (!Process32First(snapshot, &entry) == TRUE)
        {
            return 0;
        }

        // Iterate until we either find our process or reach the end of the list.
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (strcmp(entry.szExeFile, process_name) == 0)
            {
                return (int)entry.th32ProcessID;
            }
        }

        // Return 0, if we reach this point the process was not found.
        return 0;
    }
};
extern Rust* rust;

#endif // _MEMORY_HPP