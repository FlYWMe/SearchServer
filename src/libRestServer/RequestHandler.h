#ifndef REQUEST_HANDLER_H_
#define REQUEST_HANDLER_H_
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <chrono>
#include <ctime>
#include <algorithm>
#include "common/json.h"
#include "common/configParams.h"
#include "libSearch/FaissInterface.h"
#include "SearchProcessor.h"
#define CURRENT_SYS_TIMEPOINT std::chrono::system_clock::now()
#define CURRENT_STEADY_TIMEPOINT std::chrono::steady_clock::now()
typedef std::vector<std::chrono::system_clock::time_point> TIMEPOINTS;

namespace dev
{
/**
 * @brief handle request
 * 
 */
class RequestHandler
{

public:
  struct IndexPair
  {
    std::string date;
    std::shared_ptr<faissSearch> index;
  };
  std::list<IndexPair> recentIndex;

  struct Ids_lab_pair
  {
    float dis;
    long label;
  };
  std::vector<Ids_lab_pair> ids_lab;

  RequestHandler(dev::ConfigParams *_cp);
  ~RequestHandler();
  /// @brief Add vectors to the index
  void Add(std::string const &request, std::string &response, string &httpCode);
  /// @brief for each query vector, find its k nearest neighbors in the database
  void Query(std::string const &request, std::string &response, string &httpCode);
  /// @brief for each query vector, find all vectors with distance < radius
  void QueryRange(std::string const &request, std::string &response, string &httpCode);
  /// @brief remove id from index
  void Remove(std::string const &request, std::string &response, string &httpCode);
  /// @brief remove ids
  void RemoveRange(std::string const &request, std::string &response, string &httpCode);
  /// @brief reload data
  void Reconfig(std::string const &request, std::string &response, string &httpCode);
  void QueryDays(std::string const &request, std::string &response, string &httpCode);
  static bool disSort(Ids_lab_pair &a, Ids_lab_pair &b) { return (a.dis < b.dis); }

  std::shared_ptr<faissSearch> search = nullptr;
  std::shared_ptr<faissSearch> searchdays = nullptr;
  /**
   * @brief generate response string 
   * 
   * @param interface name
   * @param response json 
   * @param errorMsg 
   * @param timePoints 
   * @return std::string 
   */
  std::string FillResponse(const std::string &interface, nlohmann::json &response, const char *errorMsg = NULL, const TIMEPOINTS &timePoints = TIMEPOINTS(1, CURRENT_SYS_TIMEPOINT));

private:
  void WriteLog(const std::string &interface, const nlohmann::json &resp, const TIMEPOINTS &timePoints);
  const dev::ConfigParams *cp = nullptr;
  std::shared_ptr<SearchProcessor> search_processor = nullptr;

}; // class RequestHandler
} // namespace dev

#endif //REQUEST_HANDLER_H_