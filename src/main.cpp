#include <iostream>
#include <thread>
#include "client.h"

int main() {
    URI uri;
    uri.parse("https://www.baidu.com/test/asdf");
    printf("Host: %s\n", uri.host());
    printf("Path: %s\n", uri.path());

    //return 0;

#if(PLATFORM == WINDOWS)
    ProcessLanguageClient client(R"(F:\LLVM\bin\clangd.exe)");
#elif(PLATFORM == LINUX)
    ProcessLanguageClient client("clangd --log=verbose");
#endif
    MapMessageHandler my;
    std::thread thread([&] {
        client.loop(my);
    });

    //string_ref file = "file:///C:/Users/Administrator/Desktop/test.c";
    string_ref file = "~/lsp-cpp/test.c";
    //string_ref file = "~/lsp-cpp/";
    string_ref root = "~/lsp-cpp/";

    std::string text = "int main() { return 0; }\n";
    int res;
    while (scanf("%d", &res)) {
        if (res == 1) {
            client.Exit();
            //client.Shutdown();
            thread.detach();
            return 0;
        }
        if (res == 2) {
            client.Initialize();
        }
        if (res == 3) {
            client.DidOpen(file, text);
            client.Sync();
        }
        if (res == 4) {
            client.Formatting(file);
        }
        if (res == 5) {
            client.WorkspaceSymbol("client");
        }
    }
    return 0;
}
