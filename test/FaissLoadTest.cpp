#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <cassert>
#include <unordered_map>
#include <fstream>
#include "common/easylog++.h"
#include "libSearch/FaissInterface.h"

INITIALIZE_EASYLOGGINGPP

using namespace std;
using namespace dev;

bool writeToFile(vector<float> &data, string &filePath, idx_t d, idx_t n)
{
    idx_t header[5] = {0};
    ofstream out(filePath, ofstream::out | ofstream::binary);
    if (!out.is_open())
    {
        return false;
    }
    header[0] = d;
    header[1] = n;
    header[2] = 1;
    out.write((char *)header, sizeof(header));
    for (idx_t i = 0; i < header[1]; i++)
    {
        out.write((char *)&i, sizeof(idx_t));
        out.write((char *)&data[i * header[0]], header[0] * sizeof(float));
    }
    out.close();
    return true;
}

int main()
{
    int d = 512;     // dimension
    int nb = 200000; // database size
    int nq = 10000;  // nb of queries

    string searchMethod("Flat");

    vector<float> xb(d * nb, 0);
    vector<float> xq(d * nq, 0);
    for (int i = 0; i < nb; i++)
    {
        for (int j = 0; j < d; j++)
            xb[d * i + j] = drand48();
        xb[d * i] += i / 1000.;
    }

    for (int i = 0; i < nq; i++)
    {
        for (int j = 0; j < d; j++)
            xq[d * i + j] = drand48();
        xq[d * i] += i / 1000.;
    }
    shared_ptr<faissSearch> index(new faissSearch(searchMethod, d));

    string fileName = "data.bin";
    if (!writeToFile(xb, fileName, d, nb))
    {
        cout << "data write to file failed." << endl;
        return 0;
    }
    cout << "data write to file done." << endl;
    unordered_map<idx_t, vector<float>> umap;
    if (!index->load(fileName, umap))
    {
        cout << "load data with umap is failed." << endl;
        return 0;
    }
    for (unsigned int i = 0; i < umap.size(); i++)
    {
        for (int j = 0; j < d; j++)
        {
            if (xb[i * d + j] != umap[i][j])
            {
                cout << "number " << i << "data is wrong!" << endl;
                return -1;
            }
        }
    }
    cout << "load data with umap done." << endl;

    vector<idx_t> ids;
    vector<float> features;
    if (!index->load(fileName, ids, features))
    {
        cout << "load data without umap failed." << endl;
        return 0;
    }
    for (size_t i = 0; i < features.size(); i++)
    {
        if (xb[i] != features[i])
        {
            cout << "number " << i << "data is wrong!" << endl;
            return -1;
        }
    }
    cout << "load data without umap done." << endl;

    return 0;
}
