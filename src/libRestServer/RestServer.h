#ifndef REST_SERVER_H_
#define REST_SERVER_H_
#include <string>
#include <algorithm>

#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include "common/configParams.h"
#include "RequestHandler.h"

using namespace Pistache;

/**
 * @brief http server
 * 
 */
class RestServer
{
  public:
    RestServer(dev::ConfigParams *_cp);
    /**
     * @brief init httpEndpoint
     * 
     * @param thr http threads 
     */
    void init(size_t thr = 2);

    void start()
    {
        httpEndpoint->setHandler(router.handler());
        std::cout << "Restserver is started." << std::endl;
        httpEndpoint->serve();
    }

    void shutdown()
    {
        httpEndpoint->shutdown();
    }

  private:
    /// @brief set routes, according request url and routes to decide call witch function
    void setupRoutes();
    void AddData(const Rest::Request &request, Http::ResponseWriter response);
    void SearchData(const Rest::Request &request, Http::ResponseWriter response);
    void SearchRange(const Rest::Request &request, Http::ResponseWriter response);
    void SearchDays(const Rest::Request &request, Http::ResponseWriter response);
    void DeleteData(const Rest::Request &request, Http::ResponseWriter response);
    void DeleteRange(const Rest::Request &request, Http::ResponseWriter response);
    void ReconfigData(const Rest::Request &request, Http::ResponseWriter response);

    typedef std::mutex Lock;
    typedef std::lock_guard<Lock> Guard;
    dev::ConfigParams *cp;
    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;
    std::unique_ptr<dev::RequestHandler> handler;
};

#endif //REST_SERVER_H_