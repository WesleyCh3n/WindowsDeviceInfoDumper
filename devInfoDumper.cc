#include <Windows.h>

#include <SetupAPI.h>
#include <devguid.h>
#include <winioctl.h>

#include <fstream>
#include <iostream>

#pragma comment(lib, "Setupapi.lib")

static const DWORD BUFSIZE = 1024;

//
// https://www.usbzh.com/article/detail-676.html
// property:
// https://learn.microsoft.com/zh-tw/windows-hardware/drivers/install/accessing-device-instance-spdrp-xxx-properties
//
int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage\n\t./devInfoDumper <filename>.json\n";
    exit(EXIT_FAILURE);
  }
  HDEVINFO hDevInfo;
  SP_DEVINFO_DATA devInfoData;

  hDevInfo = SetupDiGetClassDevsA(NULL, 0, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);
  if (hDevInfo == INVALID_HANDLE_VALUE) {
    std::cout << "[Error]: INVALID_HANDLE_VALUE\n";
    return GetLastError();
  }

  std::ofstream file(argv[1]);
  char devBuf[BUFSIZE] = {0};
  devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

#define RUN_INFERENCE(P, NAME)                                                 \
  {                                                                            \
    if (SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData, P, nullptr,  \
                                          (PBYTE)devBuf, BUFSIZE, nullptr)) {  \
      file << "    " << #NAME << ": \"" << devBuf << "\"";                     \
      std::cout << #NAME << ": " << devBuf << '\n';                            \
    } else {                                                                   \
      file << "    " << #NAME << ": \"[Error: " << GetLastError() << "]\"";    \
      std::cout << #NAME << ": [Error:" << GetLastError() << "]\n";            \
    }                                                                          \
  }

  file << "[\n";
  for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++) {
    file << "  {\n";

    RUN_INFERENCE(SPDRP_CLASS, "class")
    file << ",\n";
    RUN_INFERENCE(SPDRP_ENUMERATOR_NAME, "enumerator")
    file << ",\n";
    RUN_INFERENCE(SPDRP_DEVICEDESC, "description")
    file << ",\n";
    RUN_INFERENCE(SPDRP_MFG, "manufacturer")
    file << ",\n";
    RUN_INFERENCE(SPDRP_HARDWAREID, "hardware_id")
    file << ",\n";
    RUN_INFERENCE(SPDRP_COMPATIBLEIDS, "compatible_id")
    file << ",\n";
    RUN_INFERENCE(SPDRP_CLASSGUID, "class_guid")

    file << "\n  }";
    // check if next loop is ending, if not ended add comma
    if (SetupDiEnumDeviceInfo(hDevInfo, i + 1, &devInfoData)) {
      file << ",";
      std::cout << '\n';
    }
    file << '\n';
  }
  file << "]\n";

  if (GetLastError() != NO_ERROR && GetLastError() != ERROR_NO_MORE_ITEMS) {
    std::cout << "[Error]: " << GetLastError() << '\n';
    exit(EXIT_FAILURE);
  }

  SetupDiDestroyDeviceInfoList(hDevInfo);

  return 0;
}
