#include <windows.h>
#include <thread>
#include <future>
#include "includes.hpp"
#include "drawing/renderer.hpp"
#include "features/features.hpp"
#include "utils/memory/memory.hpp"
#include "classes/game_object_manager.hpp"
#include "classes/base_networkable.hpp"
#include "features/aimbot.hpp"
#include "classes/ents/entity.hpp"

#pragma comment(lib, "Winmm.lib")

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

int WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd
) {
    // create a new console for debugging
    NewConsole;

    // check if driver is loaded properly
    if (!driver::query_driver()) {
        LOG_R(skCrypt("ERROR: Driver is not loaded! Press enter to exit."));

        std::cin.get();

        return 1;
    }

    timeBeginPeriod(1);

    // wait for rust client
    LOG_Y(skCrypt("Waiting For RustClient.exe....\n"));
    while (!FindWindowA(skCrypt("UnityWndClass"), skCrypt("Rust"))) { SleepEx(5000, false); }

    // init rust memory, etc
    int pid = GetPID(skCrypt("RustClient.exe"));

    // wait for memory to init
    while (pid == 0) { SleepEx(1500, false); }

    // hide console
    LOG_G(skCrypt("Successfully Launched. Press F2 When In Game. Closing this window will terminate the cheat.\n")); //HideConsole;

    drawing->h_Game = FindWindowA(skCrypt("UnityWndClass"), skCrypt("Rust"));

    // start render thread
    std::thread(&Renderer::Initialize, drawing).detach();

    // wait until f2 is pressed
    while (!GetAsyncKeyState(VK_F2)) { SleepEx(500, false); }

    // set has launched to true
    globals->has_launched = true;

    // get rust
    rust = new Rust();

    SleepEx(500, false);

    // get basenetworkable
    BaseNetworkable* bn = new BaseNetworkable(rust->mem->IL2CPPScan(skCrypt("BaseNetworkable")));

    // create GOM 
    gom = new GameObjectManager();

    // set camera 
    globals->camera = gom->GetCamera();

    std::thread(&BaseNetworkable::UpdateThread, bn).detach();
    std::thread(&BaseNetworkable::DiscoveryThread, bn).detach();

    std::thread([]() {
        LOG_G("Threads created\n");

        int i = 200;

        SleepEx(1000, false);
        while (1)
        {
            SleepEx(3, false);

            // ensure local player is valid
            if (!globals->local_player.ent) {
                if (i == 200) {
                    LOG_R(skCrypt("Local Player not found!\n"));
                    i = 0;
                }

                i++;

                continue;
            }
            else {
                if (globals->local_player.Update() != 1) {
                    if (i == 1000) {
                        LOG_R(skCrypt("Local Player not found!\n"));
                        i = 0;
                    }

                    i++;

                    continue;
                }
            }

            HeldItem item = features->weapons[globals->local_player.held_items[globals->local_player.helditem].second];

            // get local player's currently held item ( name, address )
            std::pair<std::string, uint64_t> held_item = globals->local_player.held_items[globals->local_player.helditem];

            // get base_movement
            uint64_t base_movement = rust->mem->Read<uint64_t>(globals->local_player.ent + offsets->playerBaseMovement);

            // update weapon recoil
            features->SetRecoil(held_item);

            // update weapon spread
            features->SetSpread(held_item);

            // update weapon sway
            features->SetSway(held_item);

            // update weapon automatic state
            features->SetAutomatic(held_item);

            // instant bow
            if (settings->features.instant_bow) features->InstantBow(held_item);

            // extended melee
            if (settings->features.extended_melee) features->ExtendedMelee(held_item);

            // block sprint
            //if (settings->features.no_slowdown) features->NoBlockSprint(held_item);

            // can shoot
            if (settings->features.can_jump) features->CanJump(base_movement);

            // instant compound
            if (settings->features.instant_compound) features->InstantCompound(held_item);

            // player fly
            if (settings->features.fly.enabled) features->PlayerFly(base_movement);

            // spiderman
            if (settings->features.spiderman) features->Spiderman(base_movement);

            // unlock_angles
            //if (settings->features.unlock_angles) features->UnlockAngles();

            // instant eoka 
            if (settings->features.instant_eoka) features->InstantEoka(held_item);

            // admin flag
            if (settings->features.admin_flags) features->AdminFlags();

            // always day 
            if (settings->features.set_time) features->SetTime(settings->features.time);
        }
    }).detach();

    /*
    std::thread([]() {
        while (1) {
            HeldItem item = features->weapons[globals->local_player.held_items[globals->local_player.helditem].second];
            std::pair<std::string, uint64_t> held_item = globals->local_player.held_items[globals->local_player.helditem];

            features->NoShotgunSpread(held_item);

            Sleep(1);
        }
    }).detach();
    */

    SetForegroundWindow(drawing->h_Game);
    drawing->HandleWindow();

    LOG_R(skCrypt("Rust has crashed. Sorry for the inconvenience. [closing..]\n"));
}

Rust* rust = 0;
GameObjectManager* gom = 0;