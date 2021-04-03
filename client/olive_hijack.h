#ifdef OS_WIN
#else
#include <libgen.h>
#include <unistd.h>
#endif

#ifdef OS_APPLE
#include <mach-o/dyld.h>
#endif

void LaunchOliveCrashHandler(const base::FilePath& report_file)
{
#ifdef OS_WIN
  // Windows doesn't support exec, so we use exclusive Windows API functions for this.
  // Probably for the best so unicode handling can be consistent.
  std::wstring copied_str = report_file.value();
  CreateProcess(L"olive-crashhandler", &copied_str[0], NULL, NULL, TRUE, 0, NULL, NULL, NULL, NULL);
#else
  // Create path buffer
  static const int path_sz = 2048;
  char path[path_sz];

#ifdef OS_APPLE

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
