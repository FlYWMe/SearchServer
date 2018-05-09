#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <cassert>
#include "common/easylog++.h"
#include "common/memusage.h"
#include "libSearch/FaissInterface.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

INITIALIZE_EASYLOGGINGPP

using namespace std;
using namespace dev;

int main()
{
    int d = 512;      // dimension
    int nb = 1000000; // database size
    int nq = 1;       // nb of queries
    string searchMethod("IDMap,Flat");

    vector<float> xb(d * nb, 0);
    vector<float> xq(d * nq, 0);
    vector<long> xid(nb, 0);
    for (int i = 0; i < nb; i++)
    {
        for (int j = 0; j < d; j++)
            xb[d * i + j] = drand48();
        xb[d * i] += i / 1000.;
        xid[i] = i;
    }

    for (int i = 0; i < nq; i++)
    {
        for (int j = 0; j < d; j++)
            xq[d * i + j] = drand48();
        xq[d * i] += i / 1000.;
    }
    shared_ptr<faissSearch> index(new faissSearch(searchMethod, d, true));
    chrono::system_clock::time_point t1 = chrono::system_clock::now();
    index->add_with_ids(nb, xb.data(), xid.data()); // add vectors to the index
    chrono::system_clock::time_point t2 = chrono::system_clock::now();
    cout << "|||Adding time   : " << (chrono::duration_cast<chrono::milliseconds>(t2 - t1)).count() << " ms|||" << endl;
    cout << "n:" << index->ntotal << endl;

    int k = 5;
    { // search xq
        vector<long> I(k * nq, 0);
        vector<float> D(k * nq, 0);
        vector<float> xb1(xb.begin(), xb.begin() + nq * d);
        chrono::system_clock::time_point t4 = chrono::system_clock::now();

        index->search(nq, xb1.data(), k, D.data(), I.data());

        chrono::system_clock::time_point t5 = chrono::system_clock::now();
        cout << "|||Searching time: " << (chrono::duration_cast<chrono::milliseconds>(t5 - t4)).count() << " ms|||" << endl;

        // print results
        cout << "I=" << endl;
        for (int i = 0; i < nq; i++)
        {
            for (int j = 0; j < k; j++)
                cout << setprecision(3) << setw(5) << I[i * k + j] << " ";
            cout << endl;
        }

        cout << "D=" << endl;
        for (int i = 0; i < nq; i++)
        {
            for (int j = 0; j < k; j++)
                cout << setw(5) << D[i * k + j];
            cout << endl;
        }
    }
    const char *dir = "../indexFile";
    if (access(dir, 0) == -1)
    {
        if (mkdir(dir, 0777) == 0)
        {
            cout << "mkdir success." << endl;
        }
    }

    // write to faissIndex file
    time_t nowtime = time(NULL);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y%m%d", localtime(&nowtime));
    string date(tmp);
    const string filePath = "../indexFile/" + date + ".faissIndex";
    chrono::system_clock::time_point t8 = chrono::system_clock::now();
    index->write_index(filePath.c_str());
    chrono::system_clock::time_point t9 = chrono::system_clock::now();
    cout << "|||writing index time: " << (chrono::duration_cast<chrono::milliseconds>(t9 - t8)).count() << " ms|||" << endl;
    
    shared_ptr<faissSearch> new_index(new faissSearch(searchMethod, d, false));
    chrono::system_clock::time_point t10 = chrono::system_clock::now();
    new_index->read_index(filePath.c_str());
    chrono::system_clock::time_point t11 = chrono::system_clock::now();
    cout << "|||reading index time: " << (chrono::duration_cast<chrono::milliseconds>(t11 - t10)).count() << " ms|||" << endl;
    
    {
        vector<long> I(k * nq, 0);
        vector<float> D(k * nq, 0);
        vector<float> xb1(xb.begin(), xb.begin() + nq * d);
        faiss::RangeSearchResult *result = new faiss::RangeSearchResult(nq);
        float radius = 76.5;

        chrono::system_clock::time_point t6 = chrono::system_clock::now();
        new_index->search_range(nq, xb1.data(), radius, result);
        chrono::system_clock::time_point t7 = chrono::system_clock::now();
        cout << "|||SearchRange time: " << (chrono::duration_cast<chrono::milliseconds>(t7 - t6)).count() << " ms|||" << endl;
        cout << "n:" << new_index->ntotal << endl;

        // print results
        for (int i = 0; i < nq; i++)
        {
            cout << endl
                 << "query " << i << " results:" << endl;
            for (size_t j = result->lims[i]; j < result->lims[i + 1]; j++)
            {
                cout << setprecision(3) << setw(5) << result->labels[j] << ": " << result->distances[j] << " ";
            }
        }
    }

    double vm, rss;
    process_mem_usage(&vm, &rss);
    vm /= double(1024 * 1024);
    rss /= double(1024 * 1024);
    printf("done | VM %.1fgb | RSS %.1fgb     \n", vm, rss);
    return 0;
}
