#if BUILDFLAG(IS_WIN)
#include <Windows.h>
#include <shlwapi.h>
#include "util/win/get_function.h"
#else
#include <libgen.h>
#include <unistd.h>
#endif

#ifdef BUILDFLAG(IS_APPLE)
#include <mach-o/dyld.h>
#endif

void LaunchOliveCrashHandler(const base::FilePath& report_file)
{
  static const int path_sz = 2048;

#ifdef BUILDFLAG(IS_WIN)
  // Windows doesn't support exec, so we use exclusive Windows API functions for this.
  // Probably for the best so unicode handling can be consistent.
  wchar_t path[path_sz];
  GetModuleFileName(NULL, path, path_sz);

  PathRemoveFileSpec(path);

  wchar_t formatted_path[path_sz];
  swprintf_s(formatted_path, path_sz, L"%s\\%s %s", path, L"olive-crashhandler.exe", report_file.value().data());

  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  CreateProcess(NULL, formatted_path, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
#else
  // Create path buffer
  char path[path_sz];

#ifdef BUILDFLAG(IS_APPLE)

  // macOS doesn't have "/proc/self/exe", so we use a Cocoa function to retrieve the binary path
  uint32_t _ns_path_sz = path_sz;
  if (_NSGetExecutablePath(path, &_ns_path_sz) == 0) {

#else

  // Use "/proc/self/exe" symlink to get the path of the current directory
  if (readlink("/proc/self/exe", path, path_sz) != -1) {

#endif
    // Strip filename
    char* dir_name = dirname(path);

    // Call "olive-crashhandler" in binary path
    char buf[path_sz];
    sprintf(buf, "%s/olive-crashhandler %s", dir_name, report_file.value().c_str());
    system(buf);
  }
#endif
}
