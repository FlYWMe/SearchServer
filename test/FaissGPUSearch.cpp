#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <cassert>
#include "common/easylog++.h"
#include "libSearch/FaissInterface.h"

INITIALIZE_EASYLOGGINGPP

using namespace std;
using namespace dev;

int main()
{
    int d = 256; // dimension
    int nb = 1000000; // database size
    int nq = 5; // nb of queries
    int k = 4;

    // this is typically the fastest one.
    string searchMethod("IDMap,Flat");

    // these ones have better memory usage
    // string searchMethod("IVF64,Flat");
    // string searchMethod("PQ32");
    // string searchMethod("PCA80,Flat");
    // string searchMethod("IVF64,PQ8+16");
    // string searchMethod("IVF64,PQ32");
    // string searchMethod("IMI2x8,PQ32");
    // string searchMethod("OPQ16_64,IMI2x8,PQ8+16");

    vector<float> xb(d * nb, 0);
    vector<long> xid(nb, 0);
    vector<float> xq(d * nq, 0);
    vector<float> sumb(nb, 0);
    vector<float> sumq(nq, 0);
    for (int i = 0; i < nb; i++)
    {
        for (int j = 0; j < d; j++)
        {
            xb[d * i + j] = drand48();
            sumb[i] += xb[d * i + j] * xb[d * i + j];
        }
        sumb[i] = sqrt(sumb[i]);
        xid[i] = i;
    }
    for (int i = 0; i < nb; i++)
    {
        for (int j = 0; j < d; j++)
        {
            xb[d * i + j] /= sumb[i];
        }
    }
    for (int i = 0; i < nq; i++)
    {
        for (int j = 0; j < d; j++)
        {
            xq[d * i + j] = drand48();
            sumq[i] += xq[d * i + j] * xq[d * i + j];
        }
        sumq[i] = sqrt(sumq[i]);        
    }
    for (int i = 0; i < nq; i++)
    {
        for (int j = 0; j < d; j++)
        {
            xq[d * i + j] /= sumq[i];
        }
    }

    shared_ptr<faissSearch> index(new faissSearch(searchMethod, d, true, true));
    cout << "Search " << nq << " from " << nb << " | Use " << searchMethod << endl;

    chrono::system_clock::time_point t1 = chrono::system_clock::now();
    // index->train(nb, xb.data());
    chrono::system_clock::time_point t2 = chrono::system_clock::now();
    cout << "|||Training time : " << (chrono::duration_cast<chrono::milliseconds>(t2 - t1)).count() << " ms|||" << endl;
    cout << "is_trained = " << (index->is_trained ? "true" : "false") << endl;
    // assert(index->is_trained);
    // index->add(nb, xb); // add vectors to the index
    index->add_with_ids(nb, xb.data(), xid.data()); // add vectors to the index
    cout << "ntotal = " << index->ntotal << endl;
    chrono::system_clock::time_point t3 = chrono::system_clock::now();
    cout << "|||Adding time   : " << (chrono::duration_cast<chrono::milliseconds>(t3 - t2)).count() << " ms|||" << endl;

    { // sanity check: search 5 first vectors of xb
        vector<long> I(k * nq, 0);
        vector<float> D(k * nq, 0);
        vector<float> xb5(xb.begin(), xb.begin() + nq * d);
        index->search(nq, xb5.data(), k, D.data(), I.data());

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
                cout << setw(5) << D[i * k + j] << " ";
            cout << endl;
        }
    }

    { // search xq

        vector<long> I(k * nq, 0);
        vector<float> D(k * nq, 0);
        chrono::system_clock::time_point t4 = chrono::system_clock::now();
        index->search(nq, xq.data(), k, D.data(), I.data());
        chrono::system_clock::time_point t5 = chrono::system_clock::now();
        cout << "|||Searching time: " << (chrono::duration_cast<chrono::milliseconds>(t5 - t4)).count() << " ms|||" << endl;

        // print results
        cout << "I (5 first results)=" << endl;
        for (int i = 0; i < nq; i++)
        {
            for (int j = 0; j < k; j++)
                cout << setw(8) << I[i * k + j] << " ";
            cout << endl;
        }

        cout << "D (5 first results)=" << endl;
        for (int i = 0; i < nq; i++)
        {
            for (int j = 0; j < k; j++)
                cout << setw(8) << D[i * k + j] << " ";
            cout << endl;
        }
    }

    return 0;
}