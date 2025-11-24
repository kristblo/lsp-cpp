#ifndef CLMSG_WRAPPER_H
#define CLMSG_WRAPPER_H

#include <iostream>
#include <fstream>
#include <string>
#include <thread>

#include "client.h"
#include "transport.h"

using value = json;
using RequestID = std::string;

class ClientMsgHandlerWrapper {
public:
    ClientMsgHandlerWrapper(MapMessageHandler* aMapMsgHandler, 
                            ProcessLanguageClient* aClient);

    MapMessageHandler* mMsgHandler;
    ProcessLanguageClient* mClient;
    std::string projectRootDir;

    std::string getFileAsString(DocumentUri fileUri);
    void recurseDocumentLinkResponse(DocumentUri recursionRootUri);
    void mapProjectDocumentLinks(DocumentUri linkMapRoot);
    
    template<typename VecParam>
    bool vectorHasElement(std::vector<VecParam> vector, VecParam element);
    std::map<std::string, std::vector<DocumentLink>> getDocumentLinkMap();

private:
    std::map<std::string, std::vector<DocumentLink>> documentLinkMap;
    std::vector<DocumentLink> registeredDocumentLinks;
    std::vector<DocumentUri> enqueuedDocumentLinkRequests;

    std::map<std::string, std::tuple<bool,bool>> mMapQueue;
    std::string inTransit;
    bool bindResponse = 0;


};





#endif //CLMSG_WRAPPER_H