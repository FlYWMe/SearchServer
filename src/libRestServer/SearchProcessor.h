#ifndef SEARCH_PROCESS_H_
#define SEARCH_PROCESS_H_
#include <vector>
#include <memory>
#include <chrono>
#include <atomic>
#include <ctime>
#include <boost/function.hpp>
#include <thread>
#include <iostream>
#include "common/easylog++.h"
#include "common/configParams.h"

namespace dev
{
class RequestHandler;
}
namespace dev
{

class SearchProcessor
{
public:
  SearchProcessor(dev::ConfigParams *_cp);
  ~SearchProcessor();
  void RegistAPI(RequestHandler *handler_);
  std::string getDate();
  void doWork();

  std::thread threads;
  std::string current;
  std::atomic<bool> exitThread;
  std::string searchMethod;
  bool usegpu;
  int d;
  std::shared_ptr<faissSearch> searchprocessor;
  RequestHandler *handler;
};
}
#endif //SEARCH_PROCESS_H_
