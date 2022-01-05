#include <iostream>
#include <thread>
#include "client.h"
// ----------------------------------------------------------------------------
int main() {
    // ----------------------------------------------------------------------------
    URI uri;
    //-uri.parse("https://www.baidu.com/test/asdf");
    uri.parse(R"(file://f:\usr\proj\CCLS\HelloWorld)"); //(ph 2020/08/21)
    printf("%s\n", uri.host().c_str());   //(ph 2020/08/19)
    printf("%s\n", uri.path().c_str());   //(ph 2020/08/19)

    //-return 0;
    //-ProcessLanguageClient client(R"(F:\LLVM\bin\clangd.exe)");
    //-ProcessLanguageClient client(R"(F:\user\Programs\msys64\mingw64\bin\clangd.exe)");
    ProcessLanguageClient client(R"(f:\usr\proj\CCLS\CB_CCLS\bin\Release-4\CB_CCLS-4.exe)"); //(ph 2020/08/21)

    MapMessageHandler my;
    std::thread thread([&] {
       client.loop(my);
    });

    //-string_ref file = "file:///C:/Users/Administrator/Desktop/test.c";
    //string_ref file = "file:///f:/usr/proj/CCLS/alextsao/lsp-cpp-master/lspcpp/main.cpp"; //(ph 2020/08/21)
    string_ref file = "f:\\usr\\proj\\CCLS\\HelloWorld";

    printf("Awaiting request: 1:Exit; 2:Initialize; 3:Sync; 4:Formatting; >");

    std::string text = "int main() { return 0; }\n";
    int res;
    while (true)
    {
        scanf("%d", &res);

        if (res == 1) {
            client.Exit();
            thread.detach();
            return 0;
        }
        if (res == 2) {
            client.Initialize(file);
        }
        if (res == 3) {
            client.DidOpen(file, text);
            client.Sync();
        }
        if (res == 4) {
            client.Formatting(file);
        }
    }
    return 0;
}
