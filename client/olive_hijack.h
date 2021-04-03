#ifdef OS_WIN
#else
#include <libgen.h>
#include <unistd.h>
#endif

void LaunchOliveCrashHandler(const base::FilePath& report_file)
{
#ifdef OS_WIN
  std::wstring copied_str = report_file.value();
  CreateProcess(L"olive-crashhandler", &copied_str[0], NULL, NULL, TRUE, 0, NULL, NULL, NULL, NULL);
#else
  char path[500];
  if (readlink("/proc/self/exe", path, 500) != -1) {
    char* dir_name = dirname(path);

    char buf[1024];
    sprintf(buf, "%s/olive-crashhandler %s", dir_name, report_file.value().c_str());
    system(buf);
  }
#endif
}
