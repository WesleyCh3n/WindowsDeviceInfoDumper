#include <Windows.h>

#include <SetupAPI.h>
#include <devguid.h>
#include <winioctl.h>

// disable codecvt deprecated warning
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
// or
// #pragma warning(disable : 4996)

#include <codecvt>
#include <fstream>
#include <iostream>
#include <locale>

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

  hDevInfo = SetupDiGetClassDevsW(NULL, 0, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES);
  if (hDevInfo == INVALID_HANDLE_VALUE) {
    std::cout << "[Error]: INVALID_HANDLE_VALUE\n";
    return GetLastError();
  }

  std::wofstream fileW(argv[1], std::ios::binary);
  wchar_t devBufW[BUFSIZE] = {0};
  devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
  fileW.imbue(
      std::locale(fileW.getloc(), new std::codecvt_utf8_utf16<wchar_t>));

#define RUN_INFERENCE(P, NAME)                                                 \
  {                                                                            \
    if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &devInfoData, P, nullptr,  \
                                          (PBYTE)devBufW, BUFSIZE, nullptr)) { \
      fileW << "    " << #NAME << ": \"" << devBufW << "\"";                   \
    } else {                                                                   \
      fileW << "    " << #NAME << ": \"[Error: " << GetLastError() << "]\"";   \
    }                                                                          \
  }
  fileW << "[\n";
  for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++) {
    fileW << "  {\n";
    RUN_INFERENCE(SPDRP_CLASS, "class")
    fileW << ",\n";
    RUN_INFERENCE(SPDRP_ENUMERATOR_NAME, "enumerator")
    fileW << ",\n";
    RUN_INFERENCE(SPDRP_DEVICEDESC, "description")
    fileW << ",\n";
    RUN_INFERENCE(SPDRP_MFG, "manufacturer")
    fileW << ",\n";
    RUN_INFERENCE(SPDRP_HARDWAREID, "hardware_id")
    fileW << ",\n";
    RUN_INFERENCE(SPDRP_COMPATIBLEIDS, "compatible_id")
    fileW << ",\n";
    RUN_INFERENCE(SPDRP_CLASSGUID, "class_guid")

    fileW << "\n  }";
    // check if next loop is ending, if not ended add comma
    if (SetupDiEnumDeviceInfo(hDevInfo, i + 1, &devInfoData)) {
      fileW << ",";
    }
    fileW << '\n';
  }
  fileW << "]\n";

  if (GetLastError() != NO_ERROR && GetLastError() != ERROR_NO_MORE_ITEMS) {
    std::cout << "[Error]: " << GetLastError() << '\n';
    exit(EXIT_FAILURE);
  }

  SetupDiDestroyDeviceInfoList(hDevInfo);
  std::cout << "Finished!\n";

  return 0;
}
