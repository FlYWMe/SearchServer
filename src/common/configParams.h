#ifndef CONFIG_PARAMS_H_
#define CONFIG_PARAMS_H_
#include <string>

namespace dev
{

/**
 * @brief prase config parameters from json config file
 * 
 */
struct ConfigParams
{
    /// @brief constructor
    explicit ConfigParams(std::string const &configPath);
    /// @brief print configure parameters
    void printParams() const;

    int port = 2333;
    int threads = 2;
    int cpuCores = 0;
    std::string ip = "0.0.0.0";
    std::string searchFactory = "IDMap,Flat";
    unsigned int dimension = 256;
    std::string dataFilePath = "data.bin";
    bool usegpu = true;

    int versionMajor;
    int versionMinor;
    int versionPatch;
    std::string buildType;
    struct LogConfigParams
    {
        bool traceEnabled = false;
        bool debugEnabled = false;
        bool fatalEnabled = false;
        bool errorEnabled = true;
        bool warningEnabled = false;
        bool infoEnabled = true;
        bool verboseEnabled = false;
    } logConfigParams;

  private:
    /**
     * @brief load config
     * 
     * @param configPath path of configure file
     */
    void loadConfig(std::string const &configPath);

    /**
     * @brief generate default configure file when configure file don't exist
     * 
     * @param configPath path of default configure file
     * @return true success
     * @return false fail
     */
    bool generateDefaultConfig(std::string const &configPath) const;

    /**
     * @brief initialize log configure
     * 
     * @param logConfig log configure parameter
     */
    void initEasylogging(const LogConfigParams &logConfig) const;
}; // struct ConfigParams
} // namespace dev

#endif //CONFIG_PARAMS_H_
