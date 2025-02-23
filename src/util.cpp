#include "pch.h"
#include "util.h"

namespace util
{
    VOID LoadConfig()
    {
        ini.SetUnicode();
        ini.LoadFile("mhypbase.ini");
        if (ini.GetBoolValue("Basic", "EnableConsole", false)) {
            InitConsole();
        }
        ClientVersion = ini.GetValue("Basic", "ClientVersion", nullptr);
        if (ClientVersion == nullptr) {
            char pathOut[MAX_PATH] = {};
            GetModuleFileName(NULL, pathOut, MAX_PATH);
            auto path = std::filesystem::path(pathOut).parent_path() / "pkg_version";
            std::ifstream infile(path);
            std::string line;
            std::regex str_expr = std::regex("UserAssembly.dll.*\"([0-9a-f]{32})\"");
            auto match = std::smatch();
            while (std::getline(infile, line)) {
                std::regex_search(line, match, str_expr);
                if (match.size() == 2) {
                    auto str_match = match[1].str();
                    if (str_match == "a1db328df8d382d2c5cdb350947f53e1") {
                        ClientVersion = "OSRELWin3.2.0";
                    }
                    else if (str_match == "7640df29c451b40215a1fc50993d87aa") {
                        ClientVersion = "OSCBWin3.2.50";
                    }
                    else if (str_match == "3c2d5b43477a8e59d97eb39a73826a20") {
                        ClientVersion = "OSCBWin3.2.51";
                    }
                    else if (str_match == "ec5b1355a9232d6de15655ea02626414") {
                        ClientVersion = "OSCBWin3.2.52";
                    }
                    else if (str_match == "88995f3ae431a88dc7d0176addeba816") {
                        ClientVersion = "OSCBWin3.2.53";
                    }
                    else {
                        ClientVersion = "Offset";
                    }
                    std::cout << "[init] Version detected " << ClientVersion << std::endl;
                    break;
                }
            }
        }
        RSAPublicKey = ini.GetValue("Value", "RSAPublicKey", nullptr);
        RSAPrivateKey = ini.GetValue("Value", "RSAPrivateKey", nullptr);
    }

    const char* GetRSAPublicKey() 
    {
        return RSAPublicKey;
    }

    const char* GetRSAPrivateKey()
    {
        return RSAPrivateKey;
    }

    bool GetBoolValue(const char* a_pSection, const char* a_pKey, bool a_nDefault)
    {
        return ini.GetBoolValue(a_pSection, a_pKey, a_nDefault);
    }

    long GetOffsetValue(const char* a_pKey, long a_nDefault)
    {
        return ini.GetLongValue(ClientVersion, a_pKey, a_nDefault);
    }

    VOID SaveConfig()
    {
        ini.SaveFile("mhypbase.ini");
    }

    VOID InitConsole()
    {
        AllocConsole();
        freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE **)stderr, "CONOUT$", "w", stderr);
        auto consoleWindow = GetConsoleWindow();
        SetForegroundWindow(consoleWindow);
        ShowWindow(consoleWindow, SW_RESTORE);
        ShowWindow(consoleWindow, SW_SHOW);
    }

    VOID DisableLogReport()
    {
        char pathOut[MAX_PATH] = {};
        GetModuleFileName(NULL, pathOut, MAX_PATH);
        auto path = std::filesystem::path(pathOut).parent_path() / "GenshinImpact_Data\\Plugins";
        std::error_code ec;
        std::filesystem::rename(path / "Astrolabe.dll", path / "Astrolabe.dll.bak", ec);
        std::filesystem::rename(path / "MiHoYoMTRSDK.dll", path / "MiHoYoMTRSDK.dll.bak", ec);
    }

    // https://github.com/yubie-re/vmp-virtualprotect-bypass/blob/main/src/vp-patch.hpp
    VOID DisableVMProtect()
    {
        DWORD oldProtect = 0;
        auto ntdll = GetModuleHandleA("ntdll.dll");
        BYTE callcode = ((BYTE *)GetProcAddress(ntdll, "NtQuerySection"))[4] - 1; // Since the syscall code is partially corrupted, we have to figure out what it is (always 1 less than NtQuerySection) since it changes based on windows version.
        BYTE restore[] = {0x4C, 0x8B, 0xD1, 0xB8, callcode};                      // x64 ntdll
        auto nt_vp = (BYTE *)GetProcAddress(ntdll, "NtProtectVirtualMemory");
        VirtualProtect(nt_vp, sizeof(restore), PAGE_EXECUTE_READWRITE, &oldProtect); // They don't even check if we are vping vp 👎😹👎
        memcpy(nt_vp, restore, sizeof(restore));
        VirtualProtect(nt_vp, sizeof(restore), oldProtect, &oldProtect);
    }

    VOID Dump(VOID *ptr, int buflen)
    {
        unsigned char *buf = (unsigned char *)ptr;
        int i;
        for (i = 0; i < buflen; i++)
        {
            printf("%02x ", buf[i]);
        }
        printf("\n");
    }

    VOID HexDump(VOID *ptr, int buflen)
    {
        unsigned char *buf = (unsigned char *)ptr;
        int i, j;
        for (i = 0; i < buflen; i += 16)
        {
            printf("%06x: ", i);
            for (j = 0; j < 16; j++)
                if (i + j < buflen)
                    printf("%02x ", buf[i + j]);
                else
                    printf("   ");
            printf(" ");
            for (j = 0; j < 16; j++)
                if (i + j < buflen)
                    printf("%c", isprint(buf[i + j]) ? buf[i + j] : '.');
            printf("\n");
        }
    }
}
