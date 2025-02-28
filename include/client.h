//
// Created by Alex on 2020/1/28.
//

#ifndef LSP_CLIENT_H
#define LSP_CLIENT_H

#define WINDOWS 1
#define LINUX 2
#if defined(_WIN32)
    #define PLATFORM WINDOWS
#elif defined(__linux__)
    #define PLATFORM LINUX
#endif

#include "transport.h"
#include "protocol.h"
#if(PLATFORM == WINDOWS)
#include "windows.h"
#elif(PLATFORM == LINUX)
#include "unistd.h"
#endif

#if(PLATFORM == WINDOWS)
#elif(PLATFORM == LINUX)
#endif


class LanguageClient : public JsonTransport {
public:
    virtual ~LanguageClient() = default;
public:
    RequestID Initialize(option<DocumentUri> rootUri = {}) {
        InitializeParams params;
#if(PLATFORM == WINDOWS)
        params.processId = GetCurrentProcessId();
#elif(PLATFORM == LINUX)        
        params.processId = getpid();
#endif        
        params.rootUri = rootUri;
        return SendRequest("initialize", params);
    }
    RequestID Shutdown() {
        return SendRequest("shutdown");
    }
    RequestID Sync() {
        return SendRequest("sync");
    }
    void Exit() {
        SendNotify("exit");
    }
    void Initialized() {
        SendNotify("initialized");
    }
    RequestID RegisterCapability() {
        return SendRequest("client/registerCapability");
    }
    void DidOpen(DocumentUri uri, string_ref text, string_ref languageId = "cpp") {
        DidOpenTextDocumentParams params;
        params.textDocument.uri = std::move(uri);
        params.textDocument.text = text;
        params.textDocument.languageId = languageId;
        SendNotify("textDocument/didOpen", params);
    }
    void DidClose(DocumentUri uri) {
        DidCloseTextDocumentParams params;
        params.textDocument.uri = std::move(uri);
        SendNotify("textDocument/didClose", params);
    }
    void DidChange(DocumentUri uri, std::vector<TextDocumentContentChangeEvent> &changes,
                   option<bool> wantDiagnostics = {}) {
        DidChangeTextDocumentParams params;
        params.textDocument.uri = std::move(uri);
        params.contentChanges = std::move(changes);
        params.wantDiagnostics = wantDiagnostics;
        SendNotify("textDocument/didChange", params);
    }
    RequestID RangeFomatting(DocumentUri uri, Range range) {
        DocumentRangeFormattingParams params;
        params.textDocument.uri = std::move(uri);
        params.range = range;
        return SendRequest("textDocument/rangeFormatting", params);
    }
    RequestID FoldingRange(DocumentUri uri) {
        FoldingRangeParams params;
        params.textDocument.uri = std::move(uri);
        return SendRequest("textDocument/foldingRange", params);
    }
    RequestID SelectionRange(DocumentUri uri, std::vector<Position> &positions) {
        SelectionRangeParams params;
        params.textDocument.uri = std::move(uri);
        params.positions = std::move(positions);
        return SendRequest("textDocument/selectionRange", params);
    }
    RequestID OnTypeFormatting(DocumentUri uri, Position position, string_ref ch) {
        DocumentOnTypeFormattingParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        params.ch = std::move(ch);
        return SendRequest("textDocument/onTypeFormatting", std::move(params));
    }
    RequestID Formatting(DocumentUri uri) {
        DocumentFormattingParams params;
        params.textDocument.uri = std::move(uri);
        return SendRequest("textDocument/formatting", std::move(params));
    }
    RequestID CodeAction(DocumentUri uri, Range range, CodeActionContext context) {
        CodeActionParams params;
        params.textDocument.uri = std::move(uri);
        params.range = range;
        params.context = std::move(context);
        return SendRequest("textDocument/codeAction", std::move(params));
    }
    RequestID Completion(DocumentUri uri, Position position, option<CompletionContext> context = {}) {
        CompletionParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        params.context = context;
        return SendRequest("textDocument/completion", params);
    }
    RequestID SignatureHelp(DocumentUri uri, Position position) {
        TextDocumentPositionParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        return SendRequest("textDocument/signatureHelp", std::move(params));
    }
    RequestID GoToDefinition(DocumentUri uri, Position position) {
        TextDocumentPositionParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        return SendRequest("textDocument/definition", std::move(params));
    }
    RequestID GoToDeclaration(DocumentUri uri, Position position) {
        TextDocumentPositionParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        return SendRequest("textDocument/declaration", std::move(params));
    }
    RequestID References(DocumentUri uri, Position position) {
        ReferenceParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        return SendRequest("textDocument/references", std::move(params));
    }
    RequestID SwitchSourceHeader(DocumentUri uri) {
        TextDocumentIdentifier params;
        params.uri = std::move(uri);
        return SendRequest("textDocument/references", std::move(params));
    }
    RequestID Rename(DocumentUri uri, Position position, string_ref newName) {
        RenameParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        params.newName = newName;
        return SendRequest("textDocument/rename", std::move(params));
    }
    RequestID Hover(DocumentUri uri, Position position) {
        TextDocumentPositionParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        return SendRequest("textDocument/hover", std::move(params));
    }
    RequestID DocumentSymbol(DocumentUri uri) {
        DocumentSymbolParams params;
        params.textDocument.uri = std::move(uri);
        return SendRequest("textDocument/documentSymbol", std::move(params));
    }
    RequestID DocumentColor(DocumentUri uri) {
        DocumentSymbolParams params;
        params.textDocument.uri = std::move(uri);
        return SendRequest("textDocument/documentColor", std::move(params));
    }
    RequestID DocumentHighlight(DocumentUri uri, Position position) {
        TextDocumentPositionParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        return SendRequest("textDocument/documentHighlight", std::move(params));
    }
    RequestID SymbolInfo(DocumentUri uri, Position position) {
        TextDocumentPositionParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        return SendRequest("textDocument/symbolInfo", std::move(params));
    }
    RequestID TypeHierarchy(DocumentUri uri, Position position, TypeHierarchyDirection direction, int resolve) {
        TypeHierarchyParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        params.direction = direction;
        params.resolve = resolve;
        return SendRequest("textDocument/typeHierarchy", std::move(params));
    }
    RequestID WorkspaceSymbol(string_ref query) {
        WorkspaceSymbolParams params;
        params.query = query;
        return SendRequest("workspace/symbol", std::move(params));
    }
    RequestID ExecuteCommand(string_ref cmd, option<TweakArgs> tweakArgs = {}, option<WorkspaceEdit> workspaceEdit = {}) {
        ExecuteCommandParams params;
        params.tweakArgs = tweakArgs;
        params.workspaceEdit = workspaceEdit;
        params.command = cmd;
        return SendRequest("workspace/executeCommand", std::move(params));
    }
    RequestID DidChangeWatchedFiles(std::vector<FileEvent> &changes) {
        DidChangeWatchedFilesParams params;
        params.changes = std::move(changes);
        return SendRequest("workspace/didChangeWatchedFiles", std::move(params));
    }
    RequestID DidChangeConfiguration(ConfigurationSettings &settings) {
        DidChangeConfigurationParams params;
        params.settings = std::move(settings);
        return SendRequest("workspace/didChangeConfiguration", std::move(params));
    }
public:
    RequestID SendRequest(string_ref method, value params = json()) {
        RequestID id = method.str();
        request(method, params, id);
        return id;
    }
    void SendNotify(string_ref method, value params = json()) {
        notify(method, params);
    }
};
class ProcessLanguageClient : public LanguageClient {
public:
#if(PLATFORM == WINDOWS)
    HANDLE fReadIn = nullptr, fWriteIn = nullptr;
    HANDLE fReadOut = nullptr, fWriteOut = nullptr;
    PROCESS_INFORMATION fProcess = {nullptr};
#elif(PLATFORM == LINUX)
    int pipeParent2Child[2], pipeChild2Parent[2];
    pid_t forkPid; //probably the Server PID
#endif
    
    explicit ProcessLanguageClient(const char *program, const char *arguments = "") {
#if(PLATFORM == WINDOWS)
        SECURITY_ATTRIBUTES sa = {0};
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle = true;
        if (!CreatePipe(&fReadIn, &fWriteIn, &sa, 1024 * 1024)) {
            printf("Create In Pipe error\n");
        }
        if (!CreatePipe(&fReadOut, &fWriteOut, &sa, 1024 * 1024)) {
            printf("Create Out Pipe error\n");
        }
        STARTUPINFO si = {0};
        si.cb = sizeof(si);
        si.hStdInput = fReadIn;
        si.hStdOutput = fWriteOut;
        si.dwFlags = STARTF_USESTDHANDLES;
        if (!CreateProcessA(program, (char *) arguments, 0, 0, TRUE,
        CREATE_NO_WINDOW, 0, 0, (LPSTARTUPINFOA) &si, &fProcess)) {
            printf("Create Process error\n");
        }
#elif(PLATFORM == LINUX)
        if(pipe(pipeParent2Child) != 0)
        {
            printf("Could not open pipe p2c\n");
        }
        if(pipe(pipeChild2Parent) != 0)
        {
            printf("Could not open pipe c2p\n");
        }
        //https://stackoverflow.com/questions/51996946/c-both-input-and-output-pipe-to-the-external-program
        forkPid = fork(); //clone process
        if(forkPid < 0)
        {
            printf("Fork error \n");
        }
        else if(forkPid > 0)//Parent (i.e. client) process
        {
            close(pipeParent2Child[0]);
            close(pipeChild2Parent[1]);
            int status;
            //waitpid
        }
        else //Child (i.e. server) process
        {
            close(pipeParent2Child[1]);
            close(pipeChild2Parent[0]);
            //route stdin and stdout
            dup2(pipeParent2Child[0], STDIN_FILENO);
            dup2(pipeChild2Parent[1], STDOUT_FILENO);
        }
        system(program); //call clangd from child, making this process the server
        

#endif

        //m_exec.start(program, arguments);
        //m_exec.set_wait_timeout(exec_stream_t::s_child, INFINITE);
    }

    ~ProcessLanguageClient() override {
        /*
        DisconnectNamedPipe(fReadIn);
        DisconnectNamedPipe(fWriteIn);
        DisconnectNamedPipe(fReadOut);
        DisconnectNamedPipe(fWriteOut);
        */
#if(PLATFORM == WINDOWS)

        CloseHandle(fReadIn);
        CloseHandle(fWriteIn);
        CloseHandle(fReadOut);
        CloseHandle(fWriteOut);
        if (!TerminateProcess(fProcess.hProcess, 0)) {
            printf("teminate process error!\n");
        }
        if (!TerminateThread(fProcess.hThread, 0)) {
            printf("teminate thread error!\n");
        }
        CloseHandle(fProcess.hThread);
        CloseHandle(fProcess.hProcess);
#elif(PLATFORM == LINUX)
        //TODO: exit elegantly
#endif
    }

    void SkipLine() {
        char readChar;
#if(PLATFORM == WINDOWS)
        DWORD hasRead;
        while (ReadFile(fReadOut, &read, 1, &hasRead, NULL)) {
            if (read == '\n') {
                break;
            }
        }
#elif(PLATFORM == LINUX)
        while(read(STDIN_FILENO, &readChar, 1))
        {
            if(readChar == '\n')
            {
                break;
            }
        }
#endif
    }

    
    int ReadLength() {
        // "Content-Length: "
        char szReadBuffer[255];
        int length = 0;

#if(PLATFORM == WINDOWS)        
        DWORD hasRead;
        while (ReadFile(fReadOut, &szReadBuffer[length], 1, &hasRead, NULL)) {
            if (szReadBuffer[length] == '\n') {
                break;
            }
            length++;
        }
        return atoi(szReadBuffer + 16);
#elif(PLATFORM == LINUX)
        while(read(STDIN_FILENO, &szReadBuffer[length], 1))
        {
            if(szReadBuffer[length] == '\n')
            {
                break;
            }
        }
#endif
    }

    void Read(int length, std::string &out) {
        
        out.resize(length);
        
        #if(PLATFORM == WINDOWS)
        DWORD hasRead;
        int readSize = 0;
        while (ReadFile(fReadOut, &out[readSize], length, &hasRead, NULL)) {
            readSize += hasRead;
            if (readSize >= length) {
                break;
            }
        }
#elif(PLATFORM == LINUX)
        read(STDIN_FILENO, &out[0], length);
#endif
    }

    bool Write(std::string &in) {
#if(PLATFORM == WINDOWS)        
        DWORD hasWritten;
        int writeSize = 0;
        int totalSize = in.length();
        while (WriteFile(fWriteIn, &in[writeSize], totalSize, &hasWritten, 0)) {
            writeSize += hasWritten;
            if (writeSize >= totalSize) {
                break;
            }
        }
        return true;

#elif(PLATFORM == LINUX)
        int writeSize = 0;
        int totalSize = in.length();
        write(STDOUT_FILENO, &in[0], totalSize);
        return true;
#endif
    }
    bool readJson(json &json) override {
        json.clear();
        int length = ReadLength();
        SkipLine();
        std::string read;
        Read(length, read);
        try {
            json = json::parse(read);
        } catch (std::exception &e) {
            //printf("read error -> %s\nread -> %s\n ", e.what(), read.c_str());
        }
        //printf("message %d:\n%s\n", length, read.c_str());
        return true;
    }
    bool writeJson(json &json) override {
        std::string content = json.dump();
        std::string header = "Content-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
        //printf("send:\n%s\n", content.c_str());
        return Write(header);
    }
};

#endif //LSP_CLIENT_H
