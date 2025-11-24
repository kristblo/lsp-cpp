#include "clientMsgHandlerWrapper.h"

void ClientMsgHandlerWrapper::recurseDocumentLinkResponse(DocumentUri recursionRootUri)
{
    std::string recursionRootUriContents = this->getFileAsString(recursionRootUri);
    this->mClient->DidOpen(recursionRootUri, recursionRootUriContents);
    this->mClient->DocumentLink(recursionRootUri);
    
    printf("got here1: entered recursion\n");
    this->mMsgHandler->bindResponse("textDocument/documentLink", [&, recursionRootUri](value &result)
    {
        printf("got here2: result size is %li\n", result.size());
        for (size_t i = 0; i < result.size(); i++)
        {
            DocumentLink link = result[i].get<DocumentLink>();
            std::string linkUriAsString = link.target.file.c_str();

            //Link can be found in the project dir 
            //AND is NOT already enqueued
            //AND is NOT already registered
            // printf("got here3\n");
            // printf("root: %s\n", this->projectRootDir.c_str());
            if((linkUriAsString.find(this->projectRootDir) != std::string::npos)
                // && !(this->vectorHasElement(
                //         this->enqueuedDocumentLinkRequests, 
                //         (DocumentUri)linkUriAsString))
                && (std::find(this->enqueuedDocumentLinkRequests.begin(), this->enqueuedDocumentLinkRequests.end(), (DocumentUri)linkUriAsString) == this->enqueuedDocumentLinkRequests.end())
                && !(this->documentLinkMap.count(linkUriAsString)))
            {
                this->enqueuedDocumentLinkRequests.emplace_back(linkUriAsString);
                printf("Enqueued: %s\n", linkUriAsString.c_str());
                std::ofstream queueLogFile("logfile.txt", std::ios_base::app);
                queueLogFile << "Enqueued: " << linkUriAsString.c_str() << "\n";
                queueLogFile.close();
            }
            //Register the link regardless of previous global existence
            // printf("got here4\n");
            // printf("recrooturi: %s\n", recursionRootUri.c_str());
            // printf("link: %s\n", link.target.str().c_str());
            this->documentLinkMap[recursionRootUri.c_str()].emplace_back(link);

            if(!this->enqueuedDocumentLinkRequests.empty())
            {
                DocumentUri next = this->enqueuedDocumentLinkRequests.back();
                this->recurseDocumentLinkResponse(next);
                this->enqueuedDocumentLinkRequests.pop_back();
            }
        }
        this->mClient->DidClose(recursionRootUri);
        
    });
}

void ClientMsgHandlerWrapper::mapProjectDocumentLinks(DocumentUri linkMapRoot)
{
    //Filename, link request sent, link result received
    std::map<std::string, std::tuple<bool,bool>> mapQueue;

    //1. Start by checking the root file
    std::string rootFileContents = this->getFileAsString(linkMapRoot);
    this->mClient->DidOpen(linkMapRoot, rootFileContents);
    this->mClient->DocumentLink(linkMapRoot);
    this->mMapQueue[(std::string)linkMapRoot] = {true, false};
    this->inTransit = (std::string)linkMapRoot;
    this->bindResponse = true;

    //4. Keep function alive while waiting for more replies
    bool keepAlive = true;
    while (keepAlive)
    {
        //2. Wait for response
        
        if(this->bindResponse)
        {
            std::ofstream queueLogFile("logfile.txt", std::ios_base::app);
            queueLogFile << "Binding response" << "\n";

            this->mMsgHandler->bindResponse("textDocument/documentLink", [&](value &result)
            {
                //3. Mark for further exploration
                for (size_t i = 0; i < result.size(); i++)
                {
                    DocumentLink currentLink = result[i].get<DocumentLink>();
                    std::string linkUriAsString = currentLink.target.file.c_str();
                    
                    this->documentLinkMap[this->inTransit].emplace_back(currentLink);
                    std::ofstream queueLogFile("logfile.txt", std::ios_base::app);
                    queueLogFile << "Found: " << linkUriAsString.c_str() << "\n";
                    queueLogFile.close();
                    
                    //Is the result a project file? Mark for further exploration if yes
                    if(linkUriAsString.find(this->projectRootDir) != std::string::npos
                        && (this->documentLinkMap.count(linkUriAsString) == 0))
                    {
                        std::ofstream queueLogFile("logfile.txt", std::ios_base::app);
                        queueLogFile << "Enqueued: " << linkUriAsString.c_str();
                        queueLogFile << ", queue size: " << this->mMapQueue.size() << "\n";
                        queueLogFile.close();
                        this->mMapQueue[linkUriAsString] = std::make_tuple(false, false);
                    }
                    
                }
                this->mMapQueue[this->inTransit] = {true, true};
                this->inTransit = "";

                std::ofstream queueLogFile("logfile.txt", std::ios_base::app);
                queueLogFile << "CURRENT QUEUE" << "\n";
                for(auto item = this->mMapQueue.begin(); item != this->mMapQueue.end(); ++item)
                {
                    queueLogFile << item->first.c_str() << " " << std::get<0>(item->second) << std::get<1>(item->second) << "\n";
                }
                
                queueLogFile.close();

                
            });

            this->bindResponse = false;
        }

        //This probably should be protected by semaphore or similar        
        bool kill = true;
        // queueLogFile << "Length of mapQueue:" << this->mMapQueue.size() << "\n";
        // queueLogFile.close();

        for(auto item = this->mMapQueue.begin(); item != this->mMapQueue.end(); ++item)
        {
            // std::ofstream queueLogFile("logfile.txt", std::ios_base::app);
            // queueLogFile << "Investigating " << item->first.c_str() << "\n";
            // queueLogFile.close();

            
            //There are unexplored items in the list
            if(!(std::get<0>(item->second) && std::get<1>(item->second)))
            {
                // std::ofstream queueLogFile("logfile.txt", std::ios_base::app);
                // queueLogFile << "Unexplored: " << item->first.c_str() << "\n";
                // queueLogFile.close();
                
                kill = false;
            }

            //This item is unexplored, and a request has not been sent
            if (!std::get<0>(item->second) && !std::get<1>(item->second))
            {
                std::ofstream queueLogFile("logfile.txt", std::ios_base::app);
                queueLogFile << "New link request: " << item->first.c_str() << "\n";
                queueLogFile.close();

                
                std::string currentFileContents = this->getFileAsString(item->first);
                this->mClient->DidOpen(item->first, currentFileContents);
                this->mClient->DocumentLink(item->first);
                this->mMapQueue[item->first] = {true, false};
                this->inTransit = item->first;
                this->bindResponse = true;
                kill = false;
                break;
            }

            if(kill)
            {
                // std::ofstream queueLogFile("logfile.txt", std::ios_base::app);
                // queueLogFile << "Exiting \n" ;
                // queueLogFile.close();

            }
            
        }
        keepAlive = !kill;
    }

    
}

std::string ClientMsgHandlerWrapper::getFileAsString(DocumentUri fileUri)
{
    printf("getfileasstring: %s\n", fileUri.c_str());
    std::string uriAsString = fileUri.c_str();
    std::string uriToPath = uriAsString.substr(7);
    printf("getfileasstring path: %s\n", uriToPath.c_str());
    std::ifstream fileStream(uriToPath);
    std::stringstream fileBuffer;
    fileBuffer << fileStream.rdbuf();
    std::string contents = fileBuffer.str();

    return contents;
}

template<typename VecParam>
bool ClientMsgHandlerWrapper::vectorHasElement(std::vector<VecParam> vector, VecParam element)
{
    bool hasElement;
    hasElement = (std::find(vector.begin(), vector.end(), element) != vector.end());

    return hasElement;
}

std::map<std::string, std::vector<DocumentLink>> ClientMsgHandlerWrapper::getDocumentLinkMap()
{
    return this->documentLinkMap;
}

ClientMsgHandlerWrapper::ClientMsgHandlerWrapper(
    MapMessageHandler* aMapMsgHandler, 
    ProcessLanguageClient* aClient)
{
    this->mMsgHandler= aMapMsgHandler;
    this->mClient = aClient;
}