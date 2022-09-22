#include <Windows.h>

#include <SetupAPI.h>
#include <devguid.h>
#include <winioctl.h>

#include <iostream>

#pragma comment(lib, "Setupapi.lib")

int getDeviceInfo(HDEVINFO devInfoSet, PSP_DEVINFO_DATA devInfoData,
                  DWORD property, char *&buf) {
  // property:
  // https://learn.microsoft.com/zh-tw/windows-hardware/drivers/install/accessing-device-instance-spdrp-xxx-properties
  DWORD DataT;
  DWORD bufsize = 0;
  int res = 0;
  while (!SetupDiGetDeviceRegistryPropertyA(devInfoSet, devInfoData, property,
                                            &DataT, (PBYTE)buf, bufsize,
                                            &bufsize)) {
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
      if (buf)
        LocalFree(buf);
      buf = (char *)LocalAlloc(LPTR, bufsize * 2);
    } else {
      res = GetLastError();
      std::cout << "[Error class]: " << res << '\n';
      return res;
    }
  }
  return res;
}

int main(int argc, char *argv[]) {
  int res = 0;
  HDEVINFO hDevInfo;
  SP_DEVINFO_DATA DeviceInfoData;
  DWORD i;

  hDevInfo = SetupDiGetClassDevsA(NULL, 0, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);

  if (hDevInfo == INVALID_HANDLE_VALUE) {
    res = GetLastError();
    std::cout << "[Error]: INVALID_HANDLE_VALUE\n";
    return res;
  }

  // Enumerate through all devices in Set.
  DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
  for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++) {
    char *clsBuf = NULL;
    if (getDeviceInfo(hDevInfo, &DeviceInfoData, SPDRP_CLASS, clsBuf) == 0) {
      std::cout << "[Class]: " << clsBuf << '\n';
      LocalFree(clsBuf);
    }
    char *descBuf = NULL;
    if (getDeviceInfo(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, descBuf) ==
        0) {
      std::cout << "[Description]: " << descBuf << '\n';
      LocalFree(descBuf);
    }
    char *hwIDBuf = NULL;
    if (getDeviceInfo(hDevInfo, &DeviceInfoData, SPDRP_HARDWAREID, hwIDBuf) ==
        0) {
      std::cout << "[Hardware ID]: " << hwIDBuf << '\n';
      LocalFree(hwIDBuf);
    }
    std::cout << '\n';
  }

  if (GetLastError() != NO_ERROR && GetLastError() != ERROR_NO_MORE_ITEMS) {
    std::cout << "[Error]: " << GetLastError() << '\n';
    exit(EXIT_FAILURE);
  }

  SetupDiDestroyDeviceInfoList(hDevInfo);

  return 0;
}
