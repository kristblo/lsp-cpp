#include <iostream>
#include <thread>
#include <fstream>
#include "client.h"


/**
 * This commit contains a minimum working version of an Ubuntu-compatible version of Alex Tsao's LSP client
 * for Windows. Additionally, it contains some example demonstration fuctionality in the main loop.
 * When compiled and run, the program will call the basic LSP functions on user input.
 * Testing on WSL2 indicates that input occasionally fails. Keep repeating commands
 * 2-7 (by writing <num> into the terminal) until the server responds. Command 1 terminates client and server.
 */


int main() {
    URI uri;
    uri.parse("https://www.baidu.com/test/asdf");
    printf("Host: %s\n", uri.host().c_str());
    printf("Path: %s\n", uri.path().c_str());

    //return 0;

#if(PLATFORM == WINDOWS)
    ProcessLanguageClient client(R"(F:\LLVM\bin\clangd.exe)");
#elif(PLATFORM == LINUX)
    ProcessLanguageClient client("clangd --log=verbose --pretty --all-scopes-completion --background-index");
#endif
    MapMessageHandler my;
    std::thread thread([&] {
        client.loop(my);
    });

    //string_ref file = "file:///C:/Users/Administrator/Desktop/test.c";
    string_ref file = "file:///home/kristblo/lsp-cpp/src/main.cpp";
    //string_ref file = "~/lsp-cpp/";
    string_ref root = "file:///home/kristblo/lsp-cpp/";

    std::string text;// = "int main() { return 0; }\n";
    std::ifstream t("./src/main.cpp");
    std::stringstream buffer;
    buffer << t.rdbuf();
    text = buffer.str();

    int res;
    while (scanf("%d", &res)) {
        if (res == 1) {
            //client.Exit();
            client.Shutdown();
            thread.detach();
            return 0;
        }
        if (res == 2) {
            client.Initialize(root);
        }
        if (res == 3) {
            client.DidOpen(file, text);
            client.Sync();
        }
        if (res == 4) {
            client.Formatting(file);
        }
        if (res == 5) {
            client.DocumentSymbol(file);
        }
        if (res == 6) {
            client.WorkspaceSymbol("include");
        }
        if (res == 7) {
            client.GoToDeclaration(file, {56,16});
        }
    }
    return 0;
}
