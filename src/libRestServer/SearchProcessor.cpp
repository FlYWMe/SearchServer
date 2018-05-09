#include <exception>
#include <ctime>
#include <map>
#include "RequestHandler.h"
#include "common/easylog++.h"
#include "SearchProcessor.h"
#include "common/error.h"

using namespace std;
using namespace dev;
using json = nlohmann::json;

string SearchProcessor::getDate()
{
    time_t nowtime = time(NULL);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y%m%d", localtime(&nowtime));
    string date(tmp);
    return date;
}

void SearchProcessor::doWork()
{
    while (!exitThread)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        string filePath;
        string today = getDate();
        time_t now = time(0);
        tm *ltm = localtime(&now);

        string current = std::to_string(ltm->tm_hour) + ":" + std::to_string(ltm->tm_min) + ":" + std::to_string(ltm->tm_sec);
        RequestHandler::IndexPair idxp;
        idxp.date = "null";
        idxp.index = nullptr;
        handler->recentIndex.push_back(idxp);
        handler->recentIndex.push_back(idxp);
        handler->recentIndex.push_back(idxp);

        if (current == "0:0:0")
        {
            filePath = "../indexFile/" + today + ".faissIndex";
            searchprocessor = handler->search;
            searchprocessor->write_index(filePath.c_str());

            if (handler->recentIndex.size() < 3)
            {
                idxp.date = today;
                idxp.index = searchprocessor;
                handler->recentIndex.push_back(idxp);
            }
            else if (handler->recentIndex.size() == 3)
            {
                handler->recentIndex.erase(handler->recentIndex.begin());
                idxp.date = today;
                idxp.index = searchprocessor;
                handler->recentIndex.push_back(idxp);
            }
            searchprocessor.reset(new faissSearch(searchMethod, d, usegpu));
            handler->search = searchprocessor;
        }
    }
}

SearchProcessor::SearchProcessor(dev::ConfigParams *_cp) : exitThread(false)
{
    searchMethod = _cp->searchFactory;
    usegpu = _cp->usegpu;
    d = _cp->dimension;
    searchprocessor.reset(new faissSearch(searchMethod, d, usegpu));
    try
    {
        threads = std::thread(&SearchProcessor::doWork, this);
    }
    catch (std::exception &e)
    {
        LOG(ERROR) << "FaceData thread init failed!";
        LOG(ERROR) << e.what();
    }
    catch (...)
    {
        LOG(ERROR) << "FaceData thread init failed!";
    }
}

SearchProcessor::~SearchProcessor()
{
    exitThread = true;
    if (threads.joinable())
    {
        threads.join();
    }
}

void SearchProcessor::RegistAPI(RequestHandler *handler_)
{
    handler = handler_;
}