#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <cassert>
#include "common/easylog++.h"
#include "common/memusage.h"
#include "libSearch/FaissInterface.h"

INITIALIZE_EASYLOGGINGPP

using namespace std;
using namespace dev;

int main()
{
    int d = 256;      // dimension
    int nb = 1000; // database size
    int nq = 1;       // nb of queries
    string searchMethod("IDMap,Flat");

    vector<float> xb(d * nb, 0);
    vector<float> xq(d * nq, 0);
    vector<long> xid(nb, 0);
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
    //index->train(nb, xb.data());
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
                cout << setprecision(3) << setw(8) << I[i * k + j] << " ";
            cout << endl;
        }

        cout << "D=" << endl;
        for (int i = 0; i < nq; i++)
        {
            for (int j = 0; j < k; j++)
                cout << setw(8) << D[i * k + j] << " ";
            cout << endl;
        }
    }

    {
        vector<long> I(k * nq, 0);
        vector<float> D(k * nq, 0);
        vector<float> xb1(xb.begin(), xb.begin() + nq * d);
        faiss::IDSelectorRange ids(int(nb / 2), nb);
        vector<long> idlist(100, 0);
        long nremove = 0;
        long loc = 0;

        chrono::system_clock::time_point t6 = chrono::system_clock::now();
        index->remove_ids_range(ids, nremove);

        for (size_t i = 0; i < idlist.size(); i++)
        {
            if (i == idlist.size() - 1)
                loc = 1;
            faiss::IDSelectorRange id(i, i + 1);
            index->remove_ids(id, nremove, loc);
            loc = -1;
        }

        chrono::system_clock::time_point t7 = chrono::system_clock::now();

        cout << "|||Removeing time: " << (chrono::duration_cast<chrono::milliseconds>(t7 - t6)).count() << " ms|||" << endl;
        cout << "n:" << index->ntotal << endl;
        chrono::system_clock::time_point t8 = chrono::system_clock::now();
        index->search(nq, xb1.data(), k, D.data(), I.data());
        chrono::system_clock::time_point t9 = chrono::system_clock::now();
        cout << "|||Searching time: " << (chrono::duration_cast<chrono::milliseconds>(t9 - t8)).count() << " ms|||" << endl;

        // print results
        cout << "I=" << endl;
        for (int i = 0; i < nq; i++)
        {
            for (int j = 0; j < k; j++)
                cout << setprecision(3) << setw(8) << I[i * k + j] << " ";
            cout << endl;
        }

        cout << "D=" << endl;
        for (int i = 0; i < nq; i++)
        {
            for (int j = 0; j < k; j++)
                cout << setw(8) << D[i * k + j] << " ";
            cout << endl;
        }
    }
    double vm, rss;
    process_mem_usage(&vm, &rss);
    vm /= double(1024 * 1024);
    rss /= double(1024 * 1024);
    printf("done | VM %.1fgb | RSS %.1fgb     \n", vm, rss);
    return 0;
}