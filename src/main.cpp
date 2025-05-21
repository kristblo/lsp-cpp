#include <iostream>
#include <thread>
#include <fstream>
#include "client.h"


/**
 * The purpose of this branch is to experiment with and get to know the LSP in order to find out
 * what functionality is needed to use it in the Codeviz project
 */
int mySymbolTest = 42;

int main() {
    URI uri;
    uri.parse("https://www.baidu.com/test/asdf");
    printf("Host: %s\n", uri.host().c_str());
    printf("Path: %s\n", uri.path().c_str());

    //return 0;

#if(PLATFORM == WINDOWS)
    ProcessLanguageClient client(R"(F:\LLVM\bin\clangd.exe)");
#elif(PLATFORM == LINUX)
    ProcessLanguageClient client("clangd --log=verbose --pretty --all-scopes-completion --background-index > clangd_out.json 2>&1");
#endif
    MapMessageHandler my;
    std::thread thread([&] {
        client.loop(my);
    });

    //string_ref file = "file:///C:/Users/Administrator/Desktop/test.c";
    string_ref file = "file:///home/kristblo/lsp-cpp/src/main.cpp";
    //string_ref file = "~/lsp-cpp/";
    string_ref root = "file:///home/kristblo/lsp-cpp/";

    string_ref client_file = "file:///home/kristblo/lsp-cpp/include/client.h";

    std::string text;// = "int main() { return 0; }\n";
    std::ifstream t("./src/main.cpp");
    std::stringstream buffer;
    buffer << t.rdbuf();
    text = buffer.str();

    std::string client_text;
    std::ifstream ct("./include/client.h");
    std::stringstream cbuffer;
    cbuffer << ct.rdbuf();
    client_text = cbuffer.str();

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
            client.DidOpen(client_file, client_text);
            client.Sync();
        }
        if (res == 4) {
            client.DidClose(file);
        }
        if (res == 5) {
            client.DocumentSymbol(client_file);
        }
        if (res == 6) {
            client.WorkspaceSymbol("");
        }
        if (res == 7) {
            client.GoToDeclaration(file, {57,15});
        }
        if (res == 8)
        {
            client.SymbolInfo(file, {12, 5});
        }
        if (res == 9)
        {
            client.DocumentLink(client_file);
        }
        
        
    }
    return 0;
}
