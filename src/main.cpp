#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include "client.h"


/**
 * The purpose of this branch is to experiment with and get to know the LSP in order to find out
 * what functionality is needed to use it in the Codeviz project
 * 
 * TODO: Look into Semantic Tokens, currently missing from the client. Could be the solution
 * for getting local variables.
 * 
 * 
 */
int mySymbolTest = 42;

int main() {
    URI uri;
    uri.parse("https://www.baidu.com/test/asdf");
    printf("Host: %s\n", uri.host().c_str());
    printf("Path: %s\n", uri.path().c_str());

    //return 0;

    MapMessageHandler my;
#if(PLATFORM == WINDOWS)
    ProcessLanguageClient client(R"(F:\LLVM\bin\clangd.exe)");
#elif(PLATFORM == LINUX)

    //Use with system()
    //ProcessLanguageClient client("clangd --log=verbose --pretty --all-scopes-completion --background-index > clangd_out.json 2>&1");//
    ProcessLanguageClient client("clangd --pretty --all-scopes-completion --background-index");//  > clangd_out.json 2>&1

    //Use with execlp()
    //ProcessLanguageClient client("clangd", "--log=verbose --pretty --all-scopes-completion --background-index");//  > clangd_out.json 2>&1

#endif
    std::thread thread([&] {
        client.loop(my);
    });

    //string_ref file = "file:///C:/Users/Administrator/Desktop/test.c";
    string_ref file = "file:///home/kristian/lsp-cpp/src/main.cpp";
    //string_ref file = "~/lsp-cpp/";
    string_ref root = "file:///home/kristian/lsp-cpp/";

    
    std::string text;// = "int main() { return 0; }\n";
    std::ifstream t("./src/main.cpp");
    std::stringstream buffer;
    buffer << t.rdbuf();
    text = buffer.str();
    
    //URI library seems partially broken, luckily there's always substring.
    std::string client_file = "file:///home/kristian/lsp-cpp/include/client.h";
    std::string client_text;
    //std::ifstream ct("./include/client.h");
    std::ifstream ct(client_file.substr(7));
    std::stringstream cbuffer;
    cbuffer << ct.rdbuf();
    client_text = cbuffer.str();

    //Erase log file at startup.
    std::ofstream clearFile(LOGFILE, std::ios::trunc);
    clearFile.close();


    int res;
    //printf("DEBUG: entering main program loop\n\r");
    while (scanf("%d", &res)) {
        if (res == 0)
        {
            thread.detach();
            client.~ProcessLanguageClient();
            return 0;
        }
        
        
        if (res == 1) {
            //client.Exit();
            client.Shutdown();
        }
        if (res == 2) {
            client.Initialize(root);
        }
        if (res == 3) {
            client.DidOpen(file, text);
            //client.DidOpen(client_file, client_text);
            client.Sync();
        }
        if (res == 4) {
            client.DidClose(file);
        }
        if (res == 5) {
            client.DocumentSymbol(client_file);
            client.TypeHierarchy(client_file, {260, 20}, TypeHierarchyDirection::Both, 1);
        }
        if (res == 6) {
            client.CallHierarchy(client_file, {43, 14});
            my.bindResponse("textDocument/prepareCallHierarchy", [&client](value &result)
            {
                //printf("%s\n", result.dump(2).c_str());
                client.CallHierarchyIncomingCalls(result);
                //client.CallHierarchyOutgoingCalls(result);
            });
        }
        if (res == 7) {
            client.GoToDeclaration(file, {57,15});
        }
        if (res == 8)
        {
            client.DocumentSymbol(file);
            client.SymbolInfo(file, {16, 7});
            client.WorkspaceSymbol("DidClose");
        }
        if (res == 9)
        {
            client.DocumentLink(file); 
            my.bindResponse("textDocument/documentLink", [&](value &result){
                for (size_t i = 0; i < result.size(); i++)
                {
                    DocumentLink link = result[i].get<DocumentLink>();
                    std::string linkUriAsString = link.target.file.c_str();
                    
                    if (linkUriAsString.find(root) != std::string::npos)
                    {
                        printf("\nlinknum: %li, target: %s\n", i, link.target.file.c_str());
                        std::string targetContents;
                        std::ifstream targetStream(linkUriAsString.substr(7));
                        std::stringstream targetBuffer;
                        targetBuffer << targetStream.rdbuf();
                        targetContents = targetBuffer.str();
                        client.DidOpen(linkUriAsString, targetContents);
                        client.DocumentLink(linkUriAsString);
                    }
                    
                }
                
            });
        }
        
        
    }
    return 0;
}
