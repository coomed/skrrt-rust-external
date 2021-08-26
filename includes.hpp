#ifndef _INCLUDES_HPP
#define _INCLUDES_HPP

// includes for this header file
#include "utils/globals.hpp"

#define GetConsoleTitleP unsigned long length; std::string result( 1000, '\0' ); length = GetConsoleTitle(const_cast<char *>( result.c_str() ),1000); result.resize(length)
#define NewConsole AllocConsole();SetConsoleTitleW(L"🍆");freopen("CONIN$", "r", stdin);freopen("CONOUT$", "w", stdout);printf("\n")
#define ClearConsole system("cls")
#define HideConsole SleepEx(1500, false); ShowWindow( GetConsoleWindow(), SW_HIDE)
#define ShowConsole ShowWindow( GetConsoleWindow(), SW_SHOW)

#endif // _INCLUDES_HPP