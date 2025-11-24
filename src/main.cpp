#include <iostream>
#include <thread>
#include <fstream>
#include <string>

#include "client.h"
#include "clientMsgHandlerWrapper.h"


/**
 * The purpose of this branch is to experiment with the message handler and get a better
 * understanding of how to use the callbacks
 * 
 * TODO: Look into Semantic Tokens, currently missing from the client. Could be the solution
 * for getting local variables.
 * 
 * 
 */
int mySymbolTest = 42;

int main(int argc, char* argv[]) {
    // URI uri;
    // uri.parse("https://www.baidu.com/test/asdf");
    // printf("Host: %s\n", uri.host().c_str());
    // printf("Path: %s\n", uri.path().c_str());

    if (argc < 3)
    {
        std::cerr << "Not enough arguments. Do ./<program> <rootDir> <src/main.cpp> \
            <compiler> [compile args]\n";
        return 1;
    }
    

    //Tested with the following input arguments:
    //./LspClientTest $PWD src/main.cpp g++ -I$PWD/include

    //Generate compile commands for clangd based on input arguments.
    std::vector<std::string> arguments;
    for (size_t i = 0; i < argc; i++)
    {
        arguments.push_back(argv[i]);
    }
    
    using json = nlohmann::json;        
    json compileCommands;

    std::string rootDir = arguments[1];
    compileCommands["directory"] = rootDir;
    std::string fileToCompile = rootDir + "/" + arguments[2];
    compileCommands["file"] = fileToCompile;

    std::string commands;
    for (size_t i = 3; i < argc; i++)
    {
        commands += arguments[i] + " ";
    }
    commands += "-c " + fileToCompile;
    compileCommands["command"] = commands;

    json compileCommandsArray = json::array();
    compileCommandsArray.push_back(compileCommands);

    std::ofstream compileCommandsOutputFile("compile_commands.json");
    compileCommandsOutputFile << compileCommandsArray.dump(2);
    compileCommandsOutputFile.close();

    //End of compile commands generation
    



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
    string_ref file = "file:///home/kristblo/lsp-cpp/src/main.cpp";
    //string_ref file = "~/lsp-cpp/";
    //string_ref root = "file:///home/kristblo/lsp-cpp/";

    std::string rootUriAsInputString = "file://" + rootDir + "/";
    string_ref rootUriAsStringRef = rootUriAsInputString;
    printf("Root URI: %s", rootUriAsStringRef.c_str());

    
    std::string text;// = "int main() { return 0; }\n";
    std::ifstream t("./src/main.cpp");
    std::stringstream buffer;
    buffer << t.rdbuf();
    text = buffer.str();
    
    //URI library seems partially broken, luckily there's always substring.
    std::string client_file = "file:///home/kristblo/lsp-cpp/include/client.h";
    std::string client_text;
    //std::ifstream ct("./include/client.h");
    std::ifstream ct(client_file.substr(7));
    std::stringstream cbuffer;
    cbuffer << ct.rdbuf();
    client_text = cbuffer.str();

    //Erase log file at startup.
    std::ofstream clearFile(LOGFILE, std::ios::trunc);
    clearFile.close();

    ClientMsgHandlerWrapper wrapperTest(&my, &client);
    wrapperTest.projectRootDir = rootDir;


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
            client.Initialize(rootUriAsStringRef);
        }
        if (res == 3) {
            //client.DidOpen(file, text);
            //client.DidOpen(client_file, client_text);
            //client.Sync();
            std::ofstream queueLogFile("logfile.txt", std::ios_base::app);
            queueLogFile << "done recourse\n";
            queueLogFile.close();
            for(auto it = wrapperTest.getDocumentLinkMap().cbegin(); it != wrapperTest.getDocumentLinkMap().cend(); it++)
            {
                printf("File: %s\n", it->first.c_str());
                
                std::ofstream queueLogFile("logfile.txt", std::ios_base::app);
                queueLogFile << "File: " << it->first.c_str() << "\n";
                queueLogFile.close();

                for(DocumentLink reference: it->second)
                {
                    printf("Reference: %s\n", reference.target.str().c_str());
                    
                    std::ofstream queueLogFile("logfile.txt", std::ios_base::app);
                    queueLogFile << "Reference" << reference.target.str().c_str() << "\n";
                    queueLogFile.close();                    
                }
            }            
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
            // std::string fileContents = wrapperTest.getFileAsString(file);
            // client.DidOpen(file, fileContents);
            // client.DocumentLink(file); 
            
            // my.bindResponse("textDocument/documentLink", [&](value &result)
            // {
            //     for (size_t i = 0; i < result.size(); i++)
            //     {
            //         DocumentLink link = result[i].get<DocumentLink>();
            //         std::string linkUriAsString = link.target.file.c_str();
                    
            //         if (linkUriAsString.find(rootDir) != std::string::npos)
            //         {
            //             printf("\nlinknum: %li, target: %s\n", i, link.target.file.c_str());
            //             std::string targetContents;
            //             std::ifstream targetStream(linkUriAsString.substr(7));
            //             std::stringstream targetBuffer;
            //             targetBuffer << targetStream.rdbuf();
            //             targetContents = targetBuffer.str();
            //             client.DidOpen(linkUriAsString, targetContents);
            //             client.DocumentLink(linkUriAsString);
            //         }
                    
            //     }
                
            // });
            

            //wrapperTest.recurseDocumentLinkResponse(file);
            wrapperTest.mapProjectDocumentLinks(file);
            printf("done recurse\n");
        }
        
        
    }
    return 0;
}
