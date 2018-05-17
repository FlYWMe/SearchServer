#include <exception>
#include <ctime>
#include <thread>
#include "RequestHandler.h"
#include "common/easylog++.h"
#include "libSearch/FaissInterface.h"
#include "common/error.h"

using namespace std;
using namespace dev;
using json = nlohmann::json;

RequestHandler::RequestHandler(ConfigParams *_cp) : cp(_cp)
{
    search.reset(new faissSearch(_cp->searchFactory, _cp->dimension, _cp->usegpu, true));
    searchdays.reset(new faissSearch(cp->searchFactory, cp->dimension, cp->usegpu));
    search_processor.reset(new SearchProcessor(_cp));
    search_processor->RegistAPI(this);
    vector<long> ids;
    vector<float> features;
    if (!search->load(_cp->dataFilePath, ids, features))
    {
        LOG(ERROR) << "load data failed." << endl;
        return;
    }
    LOG(INFO) << "load data success." << endl;
    bool res = search->add_with_ids((idx_t)ids.size(), features.data(), ids.data());
    if (!res)
    {
        LOG(ERROR) << "add loaded data failed." << endl;
        return;
    }
    LOG(INFO) << "add loaded data success." << endl;
}

RequestHandler::~RequestHandler() {}

string RequestHandler::FillResponse(const string &interface, json &response, const char *errorMsg, const TIMEPOINTS &timePoints)
{
    response["time_used"] = (chrono::duration_cast<chrono::milliseconds>(CURRENT_SYS_TIMEPOINT - timePoints.front())).count();
    if (errorMsg)
        response["error_message"] = errorMsg;
    WriteLog(interface, response, timePoints);
    return response.dump();
}

void RequestHandler::WriteLog(const string &interface, const json &resp, const TIMEPOINTS &timePoints)
{
    char timeStr[10];
    auto timeLong = chrono::duration_cast<chrono::milliseconds>(CURRENT_SYS_TIMEPOINT - timePoints.front());
    auto tp = chrono::system_clock::to_time_t(timePoints.front());
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", std::localtime(&tp));
    LOG(INFO) << "|Req Time: " << timeStr << left << "|Interface: " << interface << "|Time Used: " << timeLong.count() << " ms"
              << "|Result: " << resp << "|indexNum: " << search->ntotal << endl;
}

void RequestHandler::Reconfig(string const &request, string &responseBody, string &httpCode)
{
    TIMEPOINTS timepoints(1, CURRENT_SYS_TIMEPOINT);
    string interfaceName("/reconfig");
    json value = json::parse(request);
    json resp;
    if (value["reconfigFilePath"].is_null())
    {
        responseBody = FillResponse(interfaceName, resp, MISSING_ARGUMENTS);
        httpCode = BAD_REQUEST;
        return;
    }
    if (!value["reconfigFilePath"].is_string())
    {
        responseBody = FillResponse(interfaceName, resp, INVALID_RECONFIG_FILEPATH);
        httpCode = BAD_REQUEST;
        return;
    }
    string reconfigPath = value["reconfigFilePath"];
    cp = new ConfigParams(reconfigPath);
    search.reset(new faissSearch(cp->searchFactory, cp->dimension, cp->usegpu));
    vector<long> ids;
    vector<float> features;
    if (!search->load(cp->dataFilePath, ids, features))
    {
        responseBody = FillResponse(interfaceName, resp, RELOAD_FAIL);
        httpCode = BAD_REQUEST;
        return;
    }
    bool res = search->add_with_ids((idx_t)ids.size(), features.data(), ids.data());
    if (!res)
    {
        responseBody = FillResponse(interfaceName, resp, READD_FAIL);
        httpCode = BAD_REQUEST;
        return;
    }
    responseBody = FillResponse(interfaceName, resp, RECONFIG_SUCCESS, timepoints);
    httpCode = OK;
}

void RequestHandler::Add(string const &request, string &responseBody, string &httpCode)
{
    TIMEPOINTS timepoints(1, CURRENT_SYS_TIMEPOINT);
    string interfaceName("/add");
    json value = json::parse(request);
    json resp;
    if (value["ntotal"].is_null() || value["data"].is_null())
    {
        responseBody = FillResponse(interfaceName, resp, MISSING_ARGUMENTS);
        httpCode = BAD_REQUEST;
        return;
    }
    if (!value["ntotal"].is_number())
    {
        responseBody = FillResponse(interfaceName, resp, INVALID_NTOTAL);
        httpCode = BAD_REQUEST;
        return;
    }
    if (!value["data"].is_object())
    {
        responseBody = FillResponse(interfaceName, resp, INVALID_DATA);
        httpCode = BAD_REQUEST;
        return;
    }
    int n = value["ntotal"];
    vector<long> ids(n, 0);
    unsigned int dim = cp->dimension;
    vector<float> features;
    features.reserve(n * dim);
    json object = value["data"];
    if (object.size() == 0)
    {
        responseBody = FillResponse(interfaceName, resp, EMPTY_DATA);
        httpCode = BAD_REQUEST;
        return;
    }
    unordered_map<string, json> queries = object;
    int count = 0;
    for (auto &query : queries)
    {
        ids[count] = stol(query.first);
        if (query.second.size() != dim)
        {
            responseBody = FillResponse(interfaceName, resp, INVALID_DIMENSION);
            httpCode = BAD_REQUEST;
            return;
        }
        features.insert(features.end(), query.second.begin(), query.second.end());
        count++;
    }

    bool res = search->add_with_ids(n, features.data(), ids.data());
    responseBody = FillResponse(interfaceName, resp, res ? ADD_SUCCESS : ADD_FAIL, timepoints);
    httpCode = OK;
}

void RequestHandler::Query(string const &request, string &responseBody, string &httpCode)
{
    TIMEPOINTS timepoints(1, CURRENT_SYS_TIMEPOINT);
    string interfaceName("/search");
    json value = json::parse(request);
    json resp = json::object();
    if (value["topk"].is_null() || value["qtotal"].is_null() || value["queries"].is_null())
    {
        responseBody = FillResponse(interfaceName, resp, MISSING_ARGUMENTS);
        httpCode = BAD_REQUEST;
        return;
    }
    if (!value["topk"].is_number())
    {
        responseBody = FillResponse(interfaceName, resp, INVALID_TOPK);
        httpCode = BAD_REQUEST;
        return;
    }
    if (!value["qtotal"].is_number())
    {
        responseBody = FillResponse(interfaceName, resp, INVALID_QTOTAL);
        httpCode = BAD_REQUEST;
        return;
    }
    if (!value["queries"].is_object())
    {
        responseBody = FillResponse(interfaceName, resp, INVALID_QUERIES);
        httpCode = BAD_REQUEST;
        return;
    }
    unsigned int dim = cp->dimension;
    unsigned int n = value["qtotal"];
    idx_t k = value["topk"];
    vector<float> features;
    features.reserve(n * dim);
    vector<string> ids(n);
    json object = value["queries"];
    if (object.size() == 0 || n == 0)
    {
        responseBody = FillResponse(interfaceName, resp, EMPTY_QUERIES);
        httpCode = BAD_REQUEST;
        return;
    }
    unordered_map<string, vector<float>> queries = object;
    int count = 0;
    for (auto &query : queries)
    {
        ids[count] = query.first;
        if (query.second.size() != dim)
        {
            responseBody = FillResponse(interfaceName, resp, INVALID_DIMENSION);
            httpCode = BAD_REQUEST;
            return;
        }
        features.insert(features.end(), query.second.begin(), query.second.end());
        count++;
    }

    vector<float> resDistance(n * k, 0);
    vector<long> resLabels(n * k, 0);
    bool res = search->search(n, features.data(), k, resDistance.data(), resLabels.data());
    json queryMap;
    json resultMap;
    vector<float> dis(k, 0);
    vector<long> label(k, 0);
    for (size_t i = 0; i < n; i++)
    {
        label.assign(resLabels.begin() + i * k, resLabels.begin() + i * k + k);
        queryMap["labels"] = label;
        dis.assign(resDistance.begin() + i * k, resDistance.begin() + i * k + k);
        queryMap["distance"] = dis;
        resultMap[ids[i]] = queryMap;
    }
    resp["result"] = resultMap;
    responseBody = FillResponse(interfaceName, resp, res ? SEARCH_SUCCESS : SEARCH_FAIL, timepoints);
    httpCode = OK;
}

void RequestHandler::QueryRange(string const &request, string &responseBody, string &httpCode)
{
    TIMEPOINTS timepoints(1, CURRENT_SYS_TIMEPOINT);
    string interfaceName("/searchRange");
    json value = json::parse(request);
    json resp = json::object();
    if (value["nq"].is_null() || value["radius"].is_null() || value["queries"].is_null())
    {
        responseBody = FillResponse(interfaceName, resp, MISSING_ARGUMENTS);
        httpCode = BAD_REQUEST;
        return;
    }
    if (!value["nq"].is_number())
    {
        responseBody = FillResponse(interfaceName, resp, INVALID_NQ);
        httpCode = BAD_REQUEST;
        return;
    }

    if (!value["radius"].is_number())
    {
        responseBody = FillResponse(interfaceName, resp, INVALID_RADIUS);
        httpCode = BAD_REQUEST;
        return;
    }
    if (!value["queries"].is_object())
    {
        responseBody = FillResponse(interfaceName, resp, INVALID_QUERIES);
        httpCode = BAD_REQUEST;
        return;
    }
    float radius = value["radius"];
    idx_t nq = value["nq"];
    unsigned int dim = cp->dimension;
    vector<float> features;
    features.reserve(nq * dim);
    vector<string> ids(nq);
    json object = value["queries"];
    if (object.size() == 0 || nq == 0)
    {
        responseBody = FillResponse(interfaceName, resp, EMPTY_QUERIES);
        httpCode = BAD_REQUEST;
        return;
    }
    unordered_map<string, vector<float>> queries = object;
    int count = 0;
    for (auto &query : queries)
    {
        ids[count] = query.first;
        if (query.second.size() != dim)
        {
            responseBody = FillResponse(interfaceName, resp, INVALID_DIMENSION);
            httpCode = BAD_REQUEST;
            return;
        }
        features.insert(features.end(), query.second.begin(), query.second.end());
        count++;
    }

    faiss::RangeSearchResult *result = new faiss::RangeSearchResult(nq);
    bool res = search->search_range(nq, features.data(), radius, result);
    json queryMap;
    json resultMap;
    size_t num = 0;
    for (idx_t i = 0; i < nq; i++)
    {
        num = result->lims[i + 1] - result->lims[i];
        vector<float> dis(num, 0);
        vector<long> label(num, 0);
        label.assign(result->labels + result->lims[i], result->labels + result->lims[i + 1]);
        dis.assign(result->distances + result->lims[i], result->distances + result->lims[i + 1]);
        queryMap["labels"] = label;
        queryMap["distance"] = dis;
        resultMap[ids[i]] = queryMap;
    }
    resp["result"] = resultMap;

    responseBody = FillResponse(interfaceName, resp, res ? SEARCH_RANGE_SUCCESS : SEARCH_RANGE_FAIL, timepoints);
    httpCode = OK;
}

void RequestHandler::Remove(string const &request, string &responseBody, string &httpCode)
{
    TIMEPOINTS timepoints(1, CURRENT_SYS_TIMEPOINT);
    string interfaceName("/delete");
    json value = json::parse(request);
    json resp;
    if (value["ids"].is_null())
    {
        responseBody = FillResponse(interfaceName, resp, MISSING_ARGUMENTS);
        httpCode = BAD_REQUEST;
        return;
    }
    if (!value["ids"].is_array())
    {
        responseBody = FillResponse(interfaceName, resp, INVALID_IDS);
        httpCode = BAD_REQUEST;
        return;
    }
    json list = value["ids"];
    if (list.size() == 0)
    {
        responseBody = FillResponse(interfaceName, resp, EMPTY_DATA);
        httpCode = BAD_REQUEST;
        return;
    }
    vector<long> ids = list;
    bool res = true;
    long nremove = 0;
    long location = 0;
    long sum = 0;
    for (size_t i = 0; i < ids.size(); i++)
    {
        if (i == ids.size() - 1)
        {
            location = 1;
            if (i == 0)
                location = 2;
        }
        if (!search->remove_ids(faiss::IDSelectorRange(ids[i], ids[i] + 1), nremove, location))
        {
            res = false;
            break;
        }
        location = -1;
        sum += nremove;
    }
    resp["ndelete"] = sum;
    responseBody = FillResponse(interfaceName, resp, res ? DELETE_SUCCESS : DELETE_FAIL, timepoints);
    httpCode = OK;
}

void RequestHandler::RemoveRange(string const &request, string &responseBody, string &httpCode)
{
    TIMEPOINTS timepoints(1, CURRENT_SYS_TIMEPOINT);
    string interfaceName("/deleteRange");
    json value = json::parse(request);
    json resp;
    if (value["start"].is_null() || value["end"].is_null())
    {
        responseBody = FillResponse(interfaceName, resp, MISSING_ARGUMENTS);
        httpCode = BAD_REQUEST;
        return;
    }
    if (!value["start"].is_number() || !value["end"].is_number())
    {
        responseBody = FillResponse(interfaceName, resp, INVALID_RANGE);
        httpCode = BAD_REQUEST;
        return;
    }
    long st = value["start"];
    long ed = value["end"];
    bool res = true;
    long nremove = 0;
    res = search->remove_ids_range(faiss::IDSelectorRange(st, ed + 1), nremove);

    resp["ndelete"] = nremove;
    responseBody = FillResponse(interfaceName, resp, res ? DELETERANGE_SUCCESS : DELETERANGE_FAIL, timepoints);
    httpCode = OK;
}
void RequestHandler::QueryDays(string const &request, string &responseBody, string &httpCode)
{
    TIMEPOINTS timepoints(1, CURRENT_SYS_TIMEPOINT);
    string interfaceName("/searchDays");
    json value = json::parse(request);
    json resp = json::object();
    if (value["topk"].is_null() || value["qtotal"].is_null() || value["queries"].is_null() || value["days"].is_null())
    {
        responseBody = FillResponse(interfaceName, resp, MISSING_ARGUMENTS);
        httpCode = BAD_REQUEST;
        return;
    }
    if (!value["topk"].is_number())
    {
        responseBody = FillResponse(interfaceName, resp, INVALID_TOPK);
        httpCode = BAD_REQUEST;
        return;
    }
    if (!value["qtotal"].is_number())
    {
        responseBody = FillResponse(interfaceName, resp, INVALID_QTOTAL);
        httpCode = BAD_REQUEST;
        return;
    }
    if (!value["queries"].is_object())
    {
        responseBody = FillResponse(interfaceName, resp, INVALID_QUERIES);
        httpCode = BAD_REQUEST;
        return;
    }
    if (!value["days"].is_array())
    {
        responseBody = FillResponse(interfaceName, resp, INVALID_DAYS);
        httpCode = BAD_REQUEST;
        return;
    }
    unsigned int dim = cp->dimension;
    unsigned int n = value["qtotal"];
    idx_t k = value["topk"];
    vector<float> features;
    features.reserve(n * dim);
    vector<string> ids(n);
    vector<string> days = value["days"];
    unsigned int daysNum = days.size();

    json object = value["queries"];
    if (object.size() == 0 || n == 0)
    {
        responseBody = FillResponse(interfaceName, resp, EMPTY_QUERIES);
        httpCode = BAD_REQUEST;
        return;
    }
    unordered_map<string, vector<float>> queries = object;
    int count = 0;
    for (auto &query : queries)
    {
        ids[count] = query.first;
        if (query.second.size() != dim)
        {
            responseBody = FillResponse(interfaceName, resp, INVALID_DIMENSION);
            httpCode = BAD_REQUEST;
            return;
        }
        features.insert(features.end(), query.second.begin(), query.second.end());
        count++;
    }

    vector<float> resDistance(n * k, 0);
    vector<long> resLabels(n * k, 0);

    list<IndexPair>::iterator it = recentIndex.begin();
    IndexPair st = *it;
    it++;
    IndexPair md = *it;
    it++;
    IndexPair ed = *it;
    string filePath;
    bool res;
    vector<vector<float>> ndis(n);
    vector<vector<long>> nlab(n);
    for (size_t i = 0; i < n; i++)
    {
        ndis[i].reserve(k * daysNum);
        nlab[i].reserve(k * daysNum);
    }
    for (size_t i = 0; i < daysNum; i++)
    {
        if (days[i] == st.date)
            searchdays = st.index;
        else if (days[i] == md.date)
            searchdays = md.index;
        else if (days[i] == ed.date)
            searchdays = ed.index;
        else
        {
            filePath = "../indexFile/" + days[i] + ".faissIndex";
            if ((access(filePath.c_str(), 0)) == -1)
            {
                responseBody = FillResponse(interfaceName, resp, NOTEXIST_INDEX);
                httpCode = BAD_REQUEST;
                return;
            }
            searchdays->read_index(filePath.c_str());
        }
        res = searchdays->search(n, features.data(), k, resDistance.data(), resLabels.data());
        if (!res)
        {
            responseBody = FillResponse(interfaceName, resp, SEARCHDAYS_FAIL);
            httpCode = OK;
            return;
        }
        for (size_t i = 0; i < n; i++)
        {
            nlab[i].insert(nlab[i].end(), resLabels.begin() + i * k, resLabels.begin() + i * k + k);
            ndis[i].insert(ndis[i].end(), resDistance.begin() + i * k, resDistance.begin() + i * k + k);
        }
    }

    json queryMap;
    json resultMap;
    vector<float> kdis(k, 0);
    vector<long> klabel(k, 0);
    Ids_lab_pair ipair;
    ids_lab.resize(k * daysNum);
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < ndis[i].size(); j++)
        {
            ipair.dis = ndis[i][j];
            ipair.label = nlab[i][j];
            ids_lab[j] = (ipair);
        }
        sort(ids_lab.begin(), ids_lab.end(), disSort);
        for (idx_t i = 0; i < k; i++)
        {
            klabel[i] = ids_lab[i].label;
            kdis[i] = ids_lab[i].dis;
        }

        queryMap["labels"] = klabel;
        queryMap["distance"] = kdis;
        resultMap[ids[i]] = queryMap;
    }

    resp["result"] = resultMap;
    responseBody = FillResponse(interfaceName, resp, SEARCHDAYS_SUCCESS, timepoints);
    httpCode = OK;
}
