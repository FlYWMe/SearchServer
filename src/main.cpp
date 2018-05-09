#include "common/easylog++.h"
#include "libRestServer/RestServer.h"

using namespace std;
using namespace dev;

void help()
{
    cout << "Usage verifier [config.json]" << endl;
}

int main(int argc, char *argv[])
{
    string configPath("config.json");
    if (argc == 2)
    {
        configPath = argv[1];
        cout << "Config file   : " << configPath << endl;
    }
    else if (argc == 1)
    {
        cout << "Config file   : config.json" << endl;
    }
    else
    {
        help();
        return -1;
    }

    std::shared_ptr<dev::ConfigParams> cp = nullptr;
    try
    {
        cp.reset(new ConfigParams(configPath));
    }
    catch (std::exception &e)
    {
        cerr << e.what() << endl;
        return -1;
    }

    cp->printParams();

    try
    {
        // start rest-http-server
        RestServer restServer(cp.get());
        restServer.start();
        restServer.shutdown();
    }
    catch (std::exception &e)
    {
        LOG(ERROR) << e.what();
        cerr << e.what();
        return -1;
    }
    return 0;
}
