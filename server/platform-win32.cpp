#include "platform.h"
#include "zf_log.h"
#include <WinSock2.h>
#include <setupapi.h>
#include <iostream>


std::string serial_to_address(uint32_t serial)
{
    //static const std::string addr_prefix = "com:\\\\.\\COM";
    static const std::string addr_prefix = "\\.\\COM";
    char devstr[4]; // large enough to hold a 999
    sprintf_s(devstr, "%i", serial);

    const std::string addr = addr_prefix + devstr;
    return addr;
}

int initialization()
{
    WSADATA wsaData;

    int iResult;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        ZF_LOGE("WSAStartup failed: %d\n", iResult);
        return 1;
    }
    return 0;
}

uint32_t get_id_from_usb_location(const char *sp_port_name, bool& ok)
{
    char description[128];
    SP_DEVINFO_DATA device_info_data;
    device_info_data.cbSize = sizeof(device_info_data);
    HDEVINFO device_info;
    int i;
    uint32_t id = 0;
    ok = false;
    char location[256];
    
    device_info = SetupDiGetClassDevs(NULL, 0, 0,
        DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (device_info != INVALID_HANDLE_VALUE)
    {
        for (i = 0; SetupDiEnumDeviceInfo(device_info, i, &device_info_data); i++)
        {
            HKEY device_key;
            char value[8];
            DWORD size, type;

            /* Check if this is the device we are looking for. */
            device_key = SetupDiOpenDevRegKey(device_info, &device_info_data,
                DICS_FLAG_GLOBAL, 0,
                DIREG_DEV, KEY_QUERY_VALUE);
            if (device_key == INVALID_HANDLE_VALUE)
                continue;
            size = sizeof(value);
            if (RegQueryValueExA(device_key, "PortName", NULL, &type, (LPBYTE)value,
                &size) != ERROR_SUCCESS || type != REG_SZ) {
                RegCloseKey(device_key);
                continue;
            }
            RegCloseKey(device_key);
            value[sizeof(value)-1] = 0;
            if (strcmp(value, sp_port_name))
                continue;

            if (SetupDiGetDeviceRegistryProperty(
                device_info,
                &device_info_data,
                SPDRP_LOCATION_PATHS,
                nullptr,
                (PBYTE)location,
                255,
                nullptr))
            {
                const char *phub, *pnusb;
                int hub, nusb;
                if ((phub = strstr(location, "#USBROOT(")) != nullptr && (pnusb = strstr(location, "#USB(")) != nullptr)
                {
                    sscanf(phub, "#USBROOT(%d)", &hub);
                    sscanf(pnusb, "#USB(%d)", &nusb);
                    // the same logicis on Linux
                    // hub is already decremented 
                    id = (uint32_t)((hub + 1 - 1) * 4 + nusb);
                    if (id > 9) id += 6; // bvvu strange stuff
                    ok = true;
                }
                
            }

        }
    }
    return id;
}

static HANDLE _h_already_started;

bool is_already_started()
{
    const char szUniqueNamedMutex[] = "xinet_server_m";
    _h_already_started = CreateMutex(NULL, TRUE, szUniqueNamedMutex);
    if (ERROR_ALREADY_EXISTS == GetLastError())
    {
        return true;
    }
    return false;
}

void release_already_started_mutex()
{
    ReleaseMutex(_h_already_started); // Explicitly release mutex
    CloseHandle(_h_already_started); // close handle before terminating
}