#include "configParams.h"
#include "json.h"
#include "easylog++.h"
#include "version.h"
#include <exception>
#include <thread>
#include <map>

INITIALIZE_EASYLOGGINGPP

using namespace dev;
using namespace std;
using json = nlohmann::json;

#define CONFIG_WARN "Config file "

// void rolloutHandler(const char *filename, size_t)
// {
//     stringstream ss;
//     static map<string, unsigned int> s_mlogIndex;
//     map<string, unsigned int>::iterator iter = s_mlogIndex.find(filename);
//     if (iter != s_mlogIndex.end())
//     {
//         ss << filename << "." << iter->second++;
//         s_mlogIndex[filename] = iter->second++;
//     }
//     else
//     {
//         ss << filename << "." << 0;
//         s_mlogIndex[filename] = 0;
//     }
//     boost::filesystem::rename(filename, ss.str().c_str());
// }

ConfigParams::ConfigParams(string const &configPath)
{
    cpuCores = thread::hardware_concurrency();
    loadConfig(configPath);
    initEasylogging(logConfigParams);
    versionMajor = VERIFIER_VERSION_MAJOR;
    versionMinor = VERIFIER_VERSION_MINOR;
    versionPatch = VERIFIER_VERSION_PATCH;

#ifdef NDEBUG
    buildType = " Release";
#else
    buildType = "  Debug ";
#endif
}

void ConfigParams::loadConfig(string const &configPath)
{
    ifstream in(configPath);
    json config;
    bool missConfigfile = true;
    if (in.is_open())
    {
        try
        {
            in >> config;
            missConfigfile = false;
        }
        catch (std::exception &e)
        {
            LOG(ERROR) << e.what();
        }
        in.close();
    }

    if (missConfigfile)
    {
        LOG(INFO) << "Use default config.";
        if (!generateDefaultConfig(configPath))
            LOG(ERROR) << "Generate default config fail.";
        return;
    }

    // config http params
    if (config["httpParams"].is_object())
    {
        if (config["httpParams"]["port"].is_number_unsigned())
            port = config["httpParams"]["port"].get<int>();
        else
            LOG(WARNING) << CONFIG_WARN << "Missing port.";

        if (config["httpParams"]["listenIP"].is_string())
            ip = config["httpParams"]["listenIP"].get<string>();
        else
            LOG(WARNING) << CONFIG_WARN << "Missing listenIP.";

        if (config["httpParams"]["httpThreads"].is_number_unsigned())
            threads = config["httpParams"]["httpThreads"].get<int>();
        else
            LOG(WARNING) << CONFIG_WARN << "Missing httpThreads.";
    }
    else
        LOG(WARNING) << CONFIG_WARN << "Missing httpParams.";

    // config search tool
    if (config["searchToolParams"].is_object())
    {
        if (config["searchToolParams"]["searchFactory"].is_string())
            searchFactory = config["searchToolParams"]["searchFactory"].get<string>();
        if (config["searchToolParams"]["dimension"].is_number_unsigned())
            dimension = config["searchToolParams"]["dimension"].get<int>();
        if (config["searchToolParams"]["dataFilePath"].is_string())
            dataFilePath = config["searchToolParams"]["dataFilePath"].get<string>();
        if (config["searchToolParams"]["usegpu"].is_boolean())
            usegpu = config["searchToolParams"]["usegpu"].get<bool>();
    }
    else
        LOG(WARNING) << CONFIG_WARN << "Missing searchToolParams.";

    // config log
    if (config["logConfigParams"].is_object())
    {
        if (config["logConfigParams"]["debugEnabled"].is_boolean())
            logConfigParams.debugEnabled = config["logConfigParams"]["debugEnabled"].get<bool>();
        if (config["logConfigParams"]["warningEnabled"].is_boolean())
            logConfigParams.warningEnabled = config["logConfigParams"]["warningEnabled"].get<bool>();
        if (config["logConfigParams"]["infoEnabled"].is_boolean())
            logConfigParams.infoEnabled = config["logConfigParams"]["infoEnabled"].get<bool>();
    }
    else
        LOG(WARNING) << CONFIG_WARN << "Missing logConfigParams.";
}

bool ConfigParams::generateDefaultConfig(string const &configPath) const
{
    json httpConfig;
    httpConfig["listenIP"] = ip;
    httpConfig["port"] = port;
    httpConfig["httpThreads"] = threads;

    json searchConfig;
    searchConfig["searchFactory"] = searchFactory;
    searchConfig["dimension"] = dimension;
    searchConfig["dataFilePath"] = dataFilePath;
    searchConfig["usegpu"] = usegpu;

    json logParamsConfig;
    logParamsConfig["debugEnabled"] = logConfigParams.debugEnabled;
    logParamsConfig["warningEnabled"] = logConfigParams.warningEnabled;
    logParamsConfig["infoEnabled"] = logConfigParams.infoEnabled;

    json config;
    config["httpParams"] = httpConfig;
    config["searchToolParams"] = searchConfig;
    config["logConfigParams"] = logParamsConfig;

    ofstream os(configPath);
    if (!os.is_open())
        return false;
    os << std::setw(4) << config;
    os.close();
    return true;
}

//日志配置文件放到log目录
void ConfigParams::initEasylogging(const LogConfigParams &logConfig) const
{
    el::Loggers::addFlag(el::LoggingFlag::MultiLoggerSupport); // Enables support for multiple loggers
    el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);

    el::Configurations conf;
    conf.set(el::Level::Global, el::ConfigurationType::Enabled, to_string(true));
    conf.set(el::Level::Trace, el::ConfigurationType::Enabled, to_string(logConfig.traceEnabled));
    conf.set(el::Level::Debug, el::ConfigurationType::Enabled, to_string(logConfig.debugEnabled));
    conf.set(el::Level::Fatal, el::ConfigurationType::Enabled, to_string(logConfig.fatalEnabled));
    conf.set(el::Level::Error, el::ConfigurationType::Enabled, to_string(logConfig.errorEnabled));
    conf.set(el::Level::Warning, el::ConfigurationType::Enabled, to_string(logConfig.warningEnabled));
    conf.set(el::Level::Info, el::ConfigurationType::Enabled, to_string(logConfig.infoEnabled));
    conf.set(el::Level::Verbose, el::ConfigurationType::Enabled, to_string(logConfig.verboseEnabled));

    conf.set(el::Level::Global, el::ConfigurationType::ToFile, to_string(true));
    conf.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, to_string(false));
    conf.set(el::Level::Global, el::ConfigurationType::Format, "%level|%datetime{%Y-%M-%d %H:%m:%s}|%msg");
    conf.set(el::Level::Global, el::ConfigurationType::MillisecondsWidth, to_string(3));
    conf.set(el::Level::Global, el::ConfigurationType::PerformanceTracking, to_string(false));
    conf.set(el::Level::Global, el::ConfigurationType::MaxLogFileSize, to_string(209715200)); // 200MB
    conf.set(el::Level::Global, el::ConfigurationType::LogFlushThreshold, to_string(100));    // flush after every 100 logs

    if (logConfig.debugEnabled)
    {
        conf.set(el::Level::Debug, el::ConfigurationType::Filename, "logs/debug_log_%datetime{%Y%M%d%H}.log");
        conf.set(el::Level::Debug, el::ConfigurationType::Format, "%level|%datetime|%file|%func|%line|%msg");
    }
    if (logConfig.errorEnabled)
    {
        conf.set(el::Level::Error, el::ConfigurationType::Filename, "logs/error_log_%datetime{%Y%M%d%H}.log");
        conf.set(el::Level::Error, el::ConfigurationType::Format, "%level|%datetime|%file|%func|%line|%msg");
    }
    if (logConfig.infoEnabled)
    {
        conf.set(el::Level::Info, el::ConfigurationType::Filename, "logs/info_log_%datetime{%Y%M%d%H}.log");
    }
    el::Loggers::reconfigureAllLoggers(conf);

    // log file rollout
    // el::Helpers::installPreRollOutCallback(rolloutHandler);
}

void ConfigParams::printParams() const
{
    stringstream ss;
    ss << "===================================" << endl;
    ss << ">>>>>> Version "
       << versionMajor << "." << versionMinor << "." << versionPatch
       << buildType << " <<<<<<" << endl;
    ss << "===================================" << endl;
    ss << "=========== Http Config ===========" << endl;
    ss << "CPU    Cores  : " << cpuCores << endl;
    ss << "Listen IP     : " << ip << endl;
    ss << "Listen Port   : " << port << endl;
    ss << "Http   Threads: " << threads << endl;
    ss << "====================================" << endl;
    ss << "======== Search Parameters =========" << endl;
    ss << "searchFactory : " << searchFactory << endl;
    ss << "dimension     : " << dimension << endl;
    ss << "usegpu        : " << usegpu << endl;
    ss << "====================================";
    cout << ss.str() << endl;
    LOG(INFO) << endl
              << ss.str();
}
