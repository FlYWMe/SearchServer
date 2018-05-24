#include "common/easylog++.h"
#include "FaissInterface.h"

#ifdef CUDA_VERSION
#include "faiss/gpu/GpuAutoTune.h"
#include "faiss/gpu/StandardGpuResources.h"
#include "faiss/gpu/utils/DeviceUtils.h"
#endif

using namespace std;
using namespace dev;

faissSearch::faissSearch(const string &indexKey, const int dimension, bool useGPU, bool initGpuResources, faiss::MetricType metric)
{
    faissIndex.reset(faiss::index_factory(dimension, indexKey.c_str(), metric));
    is_trained = faissIndex->is_trained;
    usegpu = useGPU;
    ntotal = faissIndex->ntotal;
    dim = dimension;
    if (useGPU && initGpuResources)
    {
#ifdef CUDA_VERSION
        ngpus = faiss::gpu::getNumDevices();
        for (int i = 0; i < ngpus; i++)
        {
            res.push_back(new faiss::gpu::StandardGpuResources);
            devs.push_back(i);
        }
        options->indicesOptions = faiss::gpu::INDICES_64_BIT;
        options->useFloat16CoarseQuantizer = false;
        options->useFloat16 = false;
        options->usePrecomputed = false;
        options->reserveVecs = 0;
        options->storeTransposed = false;
        options->verbose = true;
        initGpuResources = true;
#else
        LOG(WARNING) << "This release doesn't support GPU search";
#endif
    }
    if (usegpu)
    {
        faissIndex.reset(faiss::gpu::index_cpu_to_gpu_multiple(res, devs, faissIndex.get()));
    }
}

bool faissSearch::reset()
{
    try
    {
        faissIndex->reset();
        ntotal = faissIndex->ntotal;
    }
    catch (std::exception &e)
    {
        LOG(ERROR) << e.what();
        return false;
    }
    return true;
}

bool faissSearch::load(const string &filePath, unordered_map<idx_t, vector<float>> &data)
{
    ifstream in(filePath, ifstream::binary | ifstream::in);
    if (!in.is_open())
    {
        return false;
    }
    int header[5] = {0};
    in.read((char *)header, sizeof(header));
    int dimension = header[0];
    unsigned int count = header[1];
    int fileFormatVersion = header[2];
    if (fileFormatVersion != 1 || dim != dimension)
    {
        in.close();
        return false;
    }
    unsigned int id = 0;
    for (size_t i = 0; i < count; i++)
    {
        vector<float> feature(dim, 0);
        in.read((char *)&id, sizeof(int));
        in.read((char *)feature.data(), dim * sizeof(float));
        data[static_cast<idx_t>(id)] = move(feature);
    }
    in.close();
    return true;
}

bool faissSearch::load(const string &filePath, vector<idx_t> &ids, vector<float> &data)
{
    ifstream in(filePath, ifstream::binary | ifstream::in);
    if (!in.is_open())
    {
        return false;
    }
    int header[5] = {0};
    in.read((char *)header, sizeof(header));
    int dimension = header[0];
    unsigned int count = header[1];
    int fileFormatVersion = header[2];
    if (fileFormatVersion != 1 || dim != dimension)
    {
        in.close();
        return false;
    }
    data.clear();
    data.resize(count * dim);
    ids.clear();
    ids.resize(count);
    unsigned int id = 0;
    vector<float> feature(dim);
    for (size_t i = 0; i < count; i++)
    {

        in.read((char *)&id, sizeof(int));
        ids[i] = static_cast<idx_t>(id);
        in.read((char *)&data[i * dim], dim * sizeof(float));
    }
    in.close();
    return true;
}

bool faissSearch::write_index(const char *filePath)
{
    try
    {
        if (usegpu)
        {
            faissIndex.reset(faiss::gpu::index_gpu_to_cpu(faissIndex.get()));
        }
        faiss::write_index(faissIndex.get(), filePath);
        if (usegpu)
        {
            faissIndex.reset(faiss::gpu::index_cpu_to_gpu_multiple(res, devs, faissIndex.get(), options));
        }
    }
    catch (std::exception &e)
    {
        LOG(ERROR) << e.what();
        if (usegpu)
        {
            faissIndex.reset(faiss::gpu::index_cpu_to_gpu_multiple(res, devs, faissIndex.get(), options));
        }
        return false;
    }
    return true;
}

bool faissSearch::read_index(const char *filePath)
{
    try
    {
        faissIndex.reset(faiss::read_index(filePath));
        if (usegpu)
        {
            faissIndex.reset(faiss::gpu::index_cpu_to_gpu_multiple(res, devs, faissIndex.get(), options));
        }
        ntotal = faissIndex->ntotal;
    }
    catch (std::exception &e)
    {
        LOG(ERROR) << e.what();
        return false;
    }
    return true;
}

void faissSearch::train(idx_t n, const float *data)
{
    if (!is_trained)
        faissIndex->train(n, data);
    is_trained = faissIndex->is_trained;
}

void faissSearch::add(const vector<vector<float>> &data)
{
    for (auto item : data)
        faissIndex->add(1, item.data());
    ntotal += data.size();
}

void faissSearch::add(idx_t n, const vector<float> &data)
{

    faissIndex->add(n, data.data());
    ntotal += n;
}

bool faissSearch::add_with_ids(idx_t n, const float *xdata, const long *xids)
{
    try
    {
        faissIndex->add_with_ids(n, xdata, xids);
        ntotal += n;
    }
    catch (std::exception &e)
    {
        LOG(ERROR) << e.what();
        return false;
    }
    return true;
}

bool faissSearch::search(idx_t n, const float *data, idx_t k, float *distances, long *labels) const
{
    try
    {
        faissIndex->search(n, data, k, distances, labels);
    }
    catch (std::exception &e)
    {
        LOG(ERROR) << e.what();
        return false;
    }
    return true;
}

bool faissSearch::search_range(idx_t n, const float *data, float radius, faiss::RangeSearchResult *result)
{
    try
    {
        if (usegpu)
        {
            faissIndex.reset(faiss::gpu::index_gpu_to_cpu(faissIndex.get()));
        }
        faissIndex->range_search(n, data, radius, result);
        if (usegpu)
        {
            faissIndex.reset(faiss::gpu::index_cpu_to_gpu_multiple(res, devs, faissIndex.get(), options));
        }
    }
    catch (std::exception &e)
    {
        LOG(ERROR) << e.what();
        if (usegpu)
        {
            faissIndex.reset(faiss::gpu::index_cpu_to_gpu_multiple(res, devs, faissIndex.get(), options));
        }
        return false;
    }
    return true;
}

bool faissSearch::remove_ids(const faiss::IDSelector &sel, long &nremove, long &location)
{
    if (usegpu && (location == 0 || location == 2))
    {
        faissIndex.reset(faiss::gpu::index_gpu_to_cpu(faissIndex.get()));
    }
    try
    {
        nremove = faissIndex->remove_ids(sel);
        ntotal -= nremove;
        if ((location == 1 || location == 2) && usegpu)
        {
            faissIndex.reset(faiss::gpu::index_cpu_to_gpu_multiple(res, devs, faissIndex.get(), options));
        }
    }
    catch (std::exception &e)
    {
        LOG(ERROR) << e.what();
        if (usegpu)
        {
            faissIndex.reset(faiss::gpu::index_cpu_to_gpu_multiple(res, devs, faissIndex.get(), options));
        }
        return false;
    }
    return true;
}

bool faissSearch::remove_ids_range(const faiss::IDSelector &sel, long &nremove)
{
    if (usegpu)
    {
        faissIndex.reset(faiss::gpu::index_gpu_to_cpu(faissIndex.get()));
    }
    try
    {
        nremove = faissIndex->remove_ids(sel);
        ntotal -= nremove;
        if (usegpu)
        {
            faissIndex.reset(faiss::gpu::index_cpu_to_gpu_multiple(res, devs, faissIndex.get(), options));
        }
    }
    catch (std::exception &e)
    {
        LOG(ERROR) << e.what();
        if (usegpu)
        {
            faissIndex.reset(faiss::gpu::index_cpu_to_gpu_multiple(res, devs, faissIndex.get(), options));
        }
        return false;
    }
    return true;
}

bool faissSearch::index_display()
{
    try
    {
        faissIndex->display();
        return true;
    }
    catch (std::exception &e)
    {
        LOG(ERROR) << e.what();
        return false;
    }
}
