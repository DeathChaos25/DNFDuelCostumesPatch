// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <stdio.h>
#include "config.h"

bool DEBUG = 0;

// v1.0.0 = 0x1422ef2e0
void* FindFileInPakFiles = sigScan(
    "\x48\x89\x5C\x24\x00\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\x6C\x24\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x45\x17\x4C\x8B\xE2\x49\x8B\xD8\x48\x8D\x91\x00\x00\x00\x00\x4C\x8B\xF1\x49\x8B\xCC\x41\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x84\xC0\x0F\x84\x00\x00\x00\x00\x49\x8B\xD4\x48\x8D\x4D\xAF\xE8\x00\x00\x00\x00\x41\x80\xBE\x00\x00\x00\x00\x00\x0F\x84\x00\x00\x00\x00\x8B\x75\xB7\x33\xFF\x44\x8D\x46\xFF\x85\xF6\x75\x03\x44\x8B\xC7",
    "xxxx?xxxxxxxxxxxxxxx?xxx????xxx????xxxxxxxxxxxxxxxx????xxxxxxxx????x????xxxx????xxxxxxxx????xxx?????xx????xxxxxxxxxxxxxxxx");

// v1.0.0 = 0x1422f3e30
void* IsNonPakFilenameAllowed = sigScan(
    "\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x56\x57\x41\x56\x48\x83\xEC\x30\x48\x8B\x00\x45\x33\xC0\x48\x8D\x4C\x24\x00\x00\x8B\xF2\xE8\x00\x00\x00\x00\x48\x8D\x2D\x00\x00\x00\x00\x83\x78\x08\x00\x74\x05\x48\x8B\x10\xEB\x03",
    "xxxx?xxxx?xxxxxxxxxx?xxxxxxx??xxx????xxx????xxxxxxxxxxx");

// v1.11 = 0x140b200cb
void* GetPlayerInputStage = sigScan(
    "\x48\x8D\x05\x2A\x2A\x2A\x2A\x4C\x03\xC0\x33\xC0",
    "xxx????xxxxx");

// v1.11 = 0x140D05F90
void* CharSelectConfirmMaybe = sigScan(
    "\x89\x54\x24\x2A\x55\x56\x57\x41\x54\x41\x55\x48\x8D\x6C\x24\x2A",
    "xxx?xxxxxxxxxxx?");

// v1.11 = 0x140D499B0
void* PostCSSFunction = sigScan(
    "\x48\x8B\xC4\x55\x48\x8D\x68\x2A\x48\x81\xEC\xB0\x00\x00\x00\x48\x89\x58\x2A\x48\x89\x70\x2A\x48\x89\x78\x2A\x4C\x89\x60\x2A",
    "xxxxxxx?xxxxxxxxxx?xxx?xxx?xxx?");

// v1.11 = 0x140e46de0
void* SpawnPlayerBattle_Sig = sigScan(
    "\x48\x89\x5C\x24\x2A\x48\x89\x74\x24\x2A\x57\x48\x83\xEC\x60\x48\x8B\x01\x48\x8B\xFA\x48\x8B\xF1",
    "xxxx?xxxx?xxxxxxxxxxxxxx");

// v1.11 = 0x140df04a0
void* PreventColorDupes_Sig = sigScan(
    "\x48\x89\x5C\x24\x2A\x4C\x8B\x15\x2A\x2A\x2A\x2A",
    "xxxx?xxx????");

uint64_t CurrentBaseAddress = GetCurrentBaseAddress();
uint64_t baseAddressGhidra = 0x140000000;

UE4PlayerInput* PlayerInput1;
UE4PlayerInput* PlayerInput2;

int Player1ColorID = 0;
int Player2ColorID = 0;
int Player1CostumeID = 1;
int Player2CostumeID = 1;
int Player1CharID = -1;
int Player2CharID = -1;
bool HasPlayer1Costume = false;
bool HasPlayer2Costume = false;
bool OnlineP1Fix = false;

bool FileExists(LPCWSTR path)
{
    DWORD dwAttrib = GetFileAttributesW(path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool isCharStringMatchCharID(wchar_t* Name, int PlayerSide) // online fix, lol
{
    int charaID = -1;
    bool result = false;

    if (PlayerSide == 1) charaID = Player1CharID;
    else charaID = Player2CharID;

    // if (!Name) return false;

    wchar_t TargetCharString[10];

    swprintf(TargetCharString, 10, L"%02dA", charaID + 1);

    if (wcsstr(Name, TargetCharString))
    {
        result = true;
    }
    else result = false;

    if (result)
    {
        if (DEBUG) printf("isCharStringMatchCharID success! %S matched %S\n", Name, TargetCharString);
    }
    else
    {
        if (DEBUG) printf("isCharStringMatchCharID failure,  %S did not match %S\n", Name, TargetCharString);
    }

    return result;
}

const wchar_t* gameDataStart = L"../../../";

HOOK(void*, __fastcall, hook_FindFileInPakFiles, FindFileInPakFiles,
    void* PakFile, 
    wchar_t* Filename,
    void** FileEntry
)
{
    const TCHAR* TargetFile = *(TCHAR**)Filename;
    
    if ( wcsstr(TargetFile, L"../../../RED") != NULL && FileExists(TargetFile) && config::enableLooseFileLoad )
    {
        printf("Redirected File: %S\r\n", TargetFile);
        return 0; // file exists loosely, return false so the game thinks that it doesn't exist in the .pak
    }

    if (wcsstr(TargetFile, L"../../../RED/Content/Chara/") && config::enableConsole && config::enableFileAccessLog) //only log files accessed in pak
    {
        printf("FindFileInPakFiles: %S\r\n", TargetFile);
    }

    return orig_hook_FindFileInPakFiles(PakFile, Filename, FileEntry);
}

HOOK(uint64_t, __fastcall, hook_IsNonPakFilenameAllowed, IsNonPakFilenameAllowed,
    void* thisptr,
    void* Filename
)
{
    return 1;
}

HOOK(void, __fastcall, hook_CharSelectConfirmMaybe, CharSelectConfirmMaybe,
    CharSelectStruct* a1,
    int a2, // character ID
    int a3, // character color
    int a4, // costume id
    char a5
)
{
    if (a1->PlayerID == 1) // check player 1
    {
        Player1CharID = a2;

        if (a1->CanPlayerSelect == 0) // Game reselects characters before match start
        {
            a4 = Player1CostumeID - 1;
            Player1ColorID = a3;
        }
        else if (PlayerInput1->L1 == 1)
        {
            Player1CostumeID = 2;
        }
        else if (PlayerInput1->L2 == 1)
        {
            Player1CostumeID = 3;
        }
        else if (PlayerInput1->R1 == 1)
        {
            Player1CostumeID = 4;
        }
        else if (PlayerInput1->R2 == 1)
        {
            Player1CostumeID = 5;
        }
        else Player1CostumeID = 1;

        a4 = Player1CostumeID - 1;

        if (DEBUG) printf("P1 picked Costume %d; char id %d color %d\n", Player1CostumeID, a2, a3);

        if (Player1CostumeID > 1)
        {
            HasPlayer1Costume = true;
        }
    }
    else if (a1->PlayerID == 2) // check player 2
    {
        Player2CharID = a2;
        if (a1->CanPlayerSelect == 0) // Game reselects characters before match start
        {
            a4 = Player2CostumeID - 1;
            Player2ColorID = a3;
        }
        else if (PlayerInput2->L1 == 1 || (PlayerInput1->L1 == 1 && PlayerInput1->Triangle))
        {
            Player2CostumeID = 2;
        }
        else if (PlayerInput2->L2 == 1 || (PlayerInput1->L2 == 1 && PlayerInput1->Triangle))
        {
            Player2CostumeID = 3;
        }
        else if (PlayerInput2->R1 == 1 || (PlayerInput1->R1 == 1 && PlayerInput1->Triangle))
        {
            Player2CostumeID = 4;
        }
        else if (PlayerInput2->R2 == 1 || (PlayerInput1->R2 == 1 && PlayerInput1->Triangle))
        {
            Player2CostumeID = 5;
        }
        else Player2CostumeID = 1;

        a4 = Player2CostumeID - 1;

        if (DEBUG) printf("P2 picked Costume %d; char id %d color %d\n", Player2CostumeID, a2, a3);

        if (Player2CostumeID > 1)
        {
            HasPlayer2Costume = true;
        }
    }
    orig_hook_CharSelectConfirmMaybe(a1, a2, a3, a4, a5);
}

HOOK(void, __fastcall, hook_AssetPreloadFunctionBattle, PostCSSFunction,
    FSpawnPlayerInfo* a1, u64 a2)
{
    if (a1->SideID == SIDE_1P && a1->MemberID == 0)
    {
        if (DEBUG) printf("AssetPreloadFunctionBattle: P1 Chara %S with color %d\n", a1->CharaID.Name, a1->ColorID);
        if (DEBUG) isCharStringMatchCharID(a1->CharaID.Name, 1);

        if (HasPlayer1Costume)
        {
            if (isCharStringMatchCharID(a1->CharaID.Name, 1) && a1->ColorID == Player1ColorID)
            {
                a1->CostumeID = Player1CostumeID - 1;
                if (DEBUG) printf("AssetPreloadFunctionBattle: Applied Costume %d to P1\n", a1->CostumeID + 1);
            }
            else
            {
                OnlineP1Fix = true;
                if (DEBUG) printf("AssetPreloadFunctionBattle: P1 character mismatch, expected char id %d color %d, received %S and %d instead\n", Player1CharID, Player1ColorID, a1->CharaID.Name, a1->ColorID);
            }
        }
        else
        {
            a1->CostumeID = 0;
        }
        
        // if (!OnlineP1Fix) a1->ColorID = Player1ColorID;
    }
    else if (a1->SideID == SIDE_2P && a1->MemberID == 0)
    {
        if (DEBUG) printf("AssetPreloadFunctionBattle: P2 Chara %S with color %d\n", a1->CharaID.Name, a1->ColorID);

        if (HasPlayer2Costume)
        {
            a1->CostumeID = Player2CostumeID - 1;
            if (DEBUG) printf("AssetPreloadFunctionBattle: Applied Costume %d to P2\n", a1->CostumeID + 1);
        }
        else if (OnlineP1Fix && isCharStringMatchCharID(a1->CharaID.Name, 1) && a1->ColorID == Player1ColorID) // online fix
        {
            a1->CostumeID = Player1CostumeID - 1;
            if (DEBUG) printf("AssetPreloadFunctionBattle: Applied Online P2 Costume Fix\n");
        }
        else
        {
            a1->CostumeID = 0;
        }
        
        // if (!OnlineP1Fix) a1->ColorID = Player2ColorID;
    }

    return orig_hook_AssetPreloadFunctionBattle(a1, a2);
}

HOOK(void, __fastcall, hook_SpawnPlayerBattle, SpawnPlayerBattle_Sig,
    u64 a1,
    FSpawnPlayerInfo* a3
)
{
    if (a3->SideID == SIDE_1P && a3->MemberID == 0)
    {
        if (HasPlayer1Costume && !OnlineP1Fix)
        {
            a3->CostumeID = Player1CostumeID - 1;
            //HasPlayer1Costume = false;
            if (DEBUG) printf("AssetLoadFunctionBattle: Applied Costume %d to P1\n", a3->CostumeID + 1);
        }
        else
        {
            a3->CostumeID = 0;
        }
        // a3->ColorID = Player1ColorID;
    }
    else if (a3->SideID == SIDE_2P && a3->MemberID == 0)
    {
        if (OnlineP1Fix) // online fix
        {
            a3->CostumeID = Player1CostumeID - 1;
            OnlineP1Fix = false;
            if (DEBUG) printf("AssetLoadFunctionBattle: Applied Costume %d to P2\n", a3->CostumeID + 1);
            if (DEBUG) printf("Applied Online P2 Costume Fix\n");
        }
        else if (HasPlayer2Costume)
        {
            a3->CostumeID = Player2CostumeID - 1;
            if (DEBUG) printf("AssetLoadFunctionBattle: Applied Costume %d to P2\n", a3->CostumeID + 1);
            //HasPlayer2Costume = false;
        }
        else
        {
            a3->CostumeID = 0;
        }
        // a3->ColorID = Player2ColorID;
    }

    return orig_hook_SpawnPlayerBattle(a1, a3);
}

HOOK(u32, __fastcall, hook_PreventColorDupes, PreventColorDupes_Sig,
    u64 a1, u64 a2, u64 a3
)
{
    u32 result = orig_hook_PreventColorDupes(a1, a2, a3);

    if (DEBUG) printf("hook_PreventColorDupes called, a1 -> %d, a2 -> %d, a3 -> %d, result -> %d\n", a1, a2, a3, result);

    if (Player1CostumeID == Player2CostumeID) return result;
    else return (u32)a2;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        config::init();

        if ( config::enableConsole ) // open console output
        {
            AllocConsole();
            AttachConsole(GetCurrentProcessId());
            freopen("CON", "w", stdout);

            if (DEBUG) printf("Current base addres is 0x%p\n", CurrentBaseAddress);
        }

        if ( GetPlayerInputStage )
        {
            if (DEBUG) printf("Signature for GetPlayerInputStage fount at 0x%p\n", ((u64)GetPlayerInputStage - CurrentBaseAddress) + baseAddressGhidra);
            if (DEBUG) printf("Player Input Address at 0x%p\n", GetAddressFromGlobalRef((u64)GetPlayerInputStage));
            PlayerInput1 = (UE4PlayerInput*)GetAddressFromGlobalRef((u64)GetPlayerInputStage);
            PlayerInput2 = (UE4PlayerInput*)(GetAddressFromGlobalRef((u64)GetPlayerInputStage) + 0x2C);
        }
        if ( FindFileInPakFiles )
        {
            if (DEBUG) printf("Signature for FindFileInPakFiles fount at 0x%p\n", ((u64)FindFileInPakFiles - CurrentBaseAddress) + baseAddressGhidra);
            INSTALL_HOOK(hook_FindFileInPakFiles);
        }
        if ( IsNonPakFilenameAllowed && config::enableLooseFileLoad )
        {
            if (DEBUG) printf("Signature for IsNonPakFilenameAllowed fount at 0x%p\n", ((u64)IsNonPakFilenameAllowed - CurrentBaseAddress) + baseAddressGhidra);
            INSTALL_HOOK(hook_IsNonPakFilenameAllowed);
        }
        if ( config::enableCostumes && CharSelectConfirmMaybe )
        {
            if (DEBUG) printf("Signature for CharSelectConfirmMaybe fount at 0x%p\n", ((u64)CharSelectConfirmMaybe - CurrentBaseAddress) + baseAddressGhidra);
            INSTALL_HOOK(hook_CharSelectConfirmMaybe);
        }
        if ( config::enableCostumes && PostCSSFunction )
        {
            if (DEBUG) printf("Signature for AssetPreloadFunctionBattle fount at 0x%p\n", ((u64)PostCSSFunction - CurrentBaseAddress) + baseAddressGhidra);
            INSTALL_HOOK(hook_AssetPreloadFunctionBattle);
        }
        if ( config::enableCostumes && SpawnPlayerBattle_Sig )
        {
            if (DEBUG) printf("Signature for SpawnPlayerBattle fount at 0x%p\n", ((u64)SpawnPlayerBattle_Sig - CurrentBaseAddress) + baseAddressGhidra);
            INSTALL_HOOK(hook_SpawnPlayerBattle);
        }
        if ( config::enableCostumes && PreventColorDupes_Sig )
        {
            if (DEBUG) printf("Signature for PreventColorDupes fount at 0x%p\n", ((u64)PreventColorDupes_Sig - CurrentBaseAddress) + baseAddressGhidra);
            // INSTALL_HOOK(hook_PreventColorDupes);
        }

        return TRUE;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}