#include <iterator>
#include <fstream>

#include <spirv/dxvk_spirv_code_buffer.h>
#include <dxbc_module.h>

#include <shellapi.h>
#include <windows.h>
#include <windowsx.h>

using namespace dxvk;

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow) {
  int     argc = 0;
  LPWSTR* argv = CommandLineToArgvW(
    GetCommandLineW(), &argc);  
  
  if (argc < 3) {
    Logger::err("Usage: dxbc-compiler input.dxbc output.spv");
    return 1;
  }
  
  try {
    std::ifstream ifile(str::fromws(argv[1]), std::ios::binary);
    ifile.ignore(std::numeric_limits<std::streamsize>::max());
    std::streamsize length = ifile.gcount();
    ifile.clear();
    
    ifile.seekg(0, std::ios_base::beg);
    std::vector<char> dxbcCode(length);
    ifile.read(dxbcCode.data(), length);
    
    DxbcReader reader(dxbcCode.data(), dxbcCode.size());
    DxbcModule module(reader);
    
    auto shader = module.compile();
    shader->code(0).store(std::ofstream(
      str::fromws(argv[2]), std::ios::binary));
    return 0;
  } catch (const DxvkError& e) {
    Logger::err(e.message());
    return 1;
  }
}