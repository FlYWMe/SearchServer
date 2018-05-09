#ifndef FAISS_SEARCH_INTERFACE_H_
#define FAISS_SEARCH_INTERFACE_H_

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include "faiss/AutoTune.h"
#include "faiss/AuxIndexStructures.h"
#include "faiss/gpu/GpuAutoTune.h"
#include "faiss/index_io.h"

namespace dev
{
using std::string;
using std::unordered_map;
using std::vector;
typedef long idx_t;

class faissSearch
{
public:
  int dim;         ///< vector dimension
  idx_t ntotal;    ///< total nb of indexed vectors
  bool is_trained; ///< set if the Index does not require training, or if training is done already

  faissSearch(const string &indexKey, const int d, bool useGPU = true, faiss::MetricType metric = faiss::METRIC_L2);
  virtual void reset();
  /**
     * @brief Load dataset from disk
     * 
     * @param filePath dataset file path
     * @param data input dataset 
     */
  virtual bool load(const string &filePath, unordered_map<idx_t, vector<float>> &data);

  virtual bool load(const string &filePath, vector<idx_t> &ids, vector<float> &data);
  /**
   * @brief write index to the file
   * 
   * @param filePath the path of the index file
   */
  virtual bool write_index(const char *filePath);
    /**
   * @brief read index from the file
   * 
   * @param filePath the path of the index file
   */
  virtual bool read_index( const char *filePath);
  /** 
     * @brief Perform training on a representative set of vectors
     * 
     * @param data training vectors, size n *d
     */
  virtual void train(idx_t n, const float *data);
  /**
     * @brief Add n vectors of dimension d to the index.
     * 
     * @param data input matrix, size n * d
     */
  virtual void add(const vector<vector<float>> &data);
  virtual void add(idx_t n, const vector<float> &data);
  /**
     * @brief Same as add, but stores xids instead of sequential ids.
     * 
     * @param data input matrix, size n * d
     * @param if ids is not empty ids for the vectors
     */
  virtual bool add_with_ids(idx_t n, const float *xdata, const long *xids);
  /**
  * @brief for each query vector, find its k nearest neighbors in the database
  * 
  * @param n queries size
  * @param data query vectors
  * @param k top k nearest neighbors
  * @param distances top k nearest distances
  * @param labels neighbors of the queries
  */
  virtual bool search(idx_t n, const float *data, idx_t k, float *distances, long *labels) const;
  virtual bool search_range(idx_t n, const float *x, float radius, faiss::RangeSearchResult *result);
  virtual bool remove_ids(const faiss::IDSelector &sel, long &nremove, long &location);
  virtual bool remove_ids_range(const faiss::IDSelector &sel, long &nremove);
  virtual bool index_display();

private:
  std::shared_ptr<faiss::Index> faissIndex = nullptr;
  int ngpus = 0;
  bool usegpu = true;
  vector<faiss::gpu::GpuResources *> res;
  vector<int> devs;
  faiss::gpu::GpuMultipleClonerOptions *options = new faiss::gpu::GpuMultipleClonerOptions();
  bool initGpuResources = false;
};
}

#endif //FAISS_SEARCH_INTERFACE_H_