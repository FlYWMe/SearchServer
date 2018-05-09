#include "RestServer.h"

using namespace std;
using namespace Pistache;

namespace dev
{

void handleReady(const Rest::Request &, Http::ResponseWriter response)
{
    response.send(Http::Code::Ok, "ready");
}
}

RestServer::RestServer(dev::ConfigParams *_cp)
    : cp(_cp), httpEndpoint(new Http::Endpoint(Address(cp->ip, Port(cp->port)))),
      handler(new dev::RequestHandler(_cp))
{
    init(cp->threads);
}

void RestServer::init(size_t thr)
{
    auto opts = Http::Endpoint::options()
                    .threads(thr)
                    .flags(Tcp::Options::InstallSignalHandler | Tcp::Options::ReuseAddr);
    httpEndpoint->init(opts);
    setupRoutes();
}

void RestServer::setupRoutes()
{
    using namespace Rest;
    Routes::Post(router, "/add", Routes::bind(&RestServer::AddData, this));
    Routes::Post(router, "/search", Routes::bind(&RestServer::SearchData, this));
    Routes::Post(router, "/searchRange", Routes::bind(&RestServer::SearchRange, this));
    Routes::Post(router, "/searchDays", Routes::bind(&RestServer::SearchDays, this));
    Routes::Post(router, "/delete", Routes::bind(&RestServer::DeleteData, this));
    Routes::Post(router, "/deleteRange", Routes::bind(&RestServer::DeleteRange, this));
    Routes::Post(router, "/reconfig", Routes::bind(&RestServer::ReconfigData, this));
    Routes::Get(router, "/ready", Routes::bind(&dev::handleReady));
}

void RestServer::AddData(const Rest::Request &request, Http::ResponseWriter response)
{
    string responseBody;
    string httpCode;
    handler->Add(request.body(), responseBody, httpCode);
    if (httpCode == "200")
        response.send(Http::Code::Ok, responseBody);
    else
        response.send(Http::Code::Bad_Request, responseBody);
}

void RestServer::SearchData(const Rest::Request &request, Http::ResponseWriter response)
{
    string responseBody;
    string httpCode;
    handler->Query(request.body(), responseBody, httpCode);
    if (httpCode == "200")
        response.send(Http::Code::Ok, responseBody);
    else
        response.send(Http::Code::Bad_Request, responseBody);
}

void RestServer::SearchRange(const Rest::Request &request, Http::ResponseWriter response)
{
    string responseBody;
    string httpCode;
    handler->QueryRange(request.body(), responseBody, httpCode);
    if (httpCode == "200")
        response.send(Http::Code::Ok, responseBody);
    else
        response.send(Http::Code::Bad_Request, responseBody);
}

void RestServer::SearchDays(const Rest::Request &request, Http::ResponseWriter response)
{
    string responseBody;
    string httpCode;
    handler->QueryDays(request.body(), responseBody, httpCode);
    if (httpCode == "200")
        response.send(Http::Code::Ok, responseBody);
    else
        response.send(Http::Code::Bad_Request, responseBody);
}

void RestServer::DeleteData(const Rest::Request &request, Http::ResponseWriter response)
{
    string responseBody;
    string httpCode;
    handler->Remove(request.body(), responseBody, httpCode);
    if (httpCode == "200")
        response.send(Http::Code::Ok, responseBody);
    else
        response.send(Http::Code::Bad_Request, responseBody);
}

void RestServer::DeleteRange(const Rest::Request &request, Http::ResponseWriter response)
{
    string responseBody;
    string httpCode;
    handler->RemoveRange(request.body(), responseBody, httpCode);
    if (httpCode == "200")
        response.send(Http::Code::Ok, responseBody);
    else
        response.send(Http::Code::Bad_Request, responseBody);
}

void RestServer::ReconfigData(const Rest::Request &request, Http::ResponseWriter response)
{
    string responseBody;
    string httpCode;
    handler->Reconfig(request.body(), responseBody, httpCode);
    if (httpCode == "200")
        response.send(Http::Code::Ok, responseBody);
    else
        response.send(Http::Code::Bad_Request, responseBody);
}