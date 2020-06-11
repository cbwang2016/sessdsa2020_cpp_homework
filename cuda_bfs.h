#ifndef CUDA_TEST_CUDA_BFS_H
#define CUDA_TEST_CUDA_BFS_H

#include <cuda.h>
#include <string>
#include <vector>

using namespace std;

extern "C" {

__global__
void simpleBfs(const int N, const int loop_offset, const int *k_adjacencyList, const int *k_edgesOffset,
               const int *k_edgesSize, bool *visited_mempool, int *q_mempool, int *k_result) {
    const int thid = loop_offset + blockIdx.x * blockDim.x + threadIdx.x;
    if (thid >= N)
        return;
    if (threadIdx.x == 0 && blockIdx.x == 0)
        printf("%d / %d\n", thid, N);

    bool *visited = &visited_mempool[(blockIdx.x * blockDim.x + threadIdx.x) * N];
    int *q = &q_mempool[(blockIdx.x * blockDim.x + threadIdx.x) * N];
    memset(visited, false, sizeof(bool) * N);
    int queue_start = 0, queue_end = 1;
    q[0] = thid;
    visited[thid] = true;

    int count = -1, marker, u;
    while (queue_start != queue_end) {
        marker = q[queue_end - 1];
        do {
            u = q[queue_start++];
            for (int i = k_edgesOffset[u]; i < k_edgesOffset[u] + k_edgesSize[u]; i++) {
                int v = k_adjacencyList[i];
                if (!visited[v]) {
                    visited[v] = true;
                    q[queue_end++] = v;
                }
            }
        } while (marker != u);
        count++;
    }

    k_result[thid] = count;
}

}

int getDiameterGPU(const vector<vector<int>> &adj) {
    int *v_adj_size = new int[adj.size()];
    int *v_adj_offset = new int[adj.size()];
    int edge_count = 0;
    for (int i = 0; i < adj.size(); i++) {
        v_adj_size[i] = adj[i].size();
        v_adj_offset[i] = edge_count;
        edge_count += adj[i].size();
    }

    int *v_adj = new int[edge_count];
    edge_count = 0;
    for (const auto &item : adj)
        for (int i : item)
            v_adj[edge_count++] = i;

    int *results = new int[adj.size()];
    int *k_adjacencyList;
    int *k_edgesOffset;
    int *k_edgesSize;
    int *k_result;
    int *q_mempool;
    bool *visited_mempool;

    const int block_size = 16;
    const int threads_per_block = 128;
    const int total_threads = block_size * threads_per_block;

    cudaMalloc(&k_adjacencyList, sizeof(int) * edge_count);
    cudaMalloc(&k_edgesOffset, sizeof(int) * adj.size());
    cudaMalloc(&k_edgesSize, sizeof(int) * adj.size());
    cudaMalloc(&k_result, sizeof(int) * adj.size());
    cudaMalloc(&q_mempool, sizeof(int) * adj.size() * total_threads);
    cudaMalloc(&visited_mempool, sizeof(bool) * adj.size() * total_threads);

    cudaMemcpy(k_adjacencyList, v_adj, sizeof(int) * edge_count, cudaMemcpyHostToDevice);
    cudaMemcpy(k_edgesOffset, v_adj_offset, sizeof(int) * adj.size(), cudaMemcpyHostToDevice);
    cudaMemcpy(k_edgesSize, v_adj_size, sizeof(int) * adj.size(), cudaMemcpyHostToDevice);


    for (unsigned int i = 0; i < adj.size(); i += total_threads) {
        simpleBfs <<<block_size, threads_per_block>>>(
                adj.size(),
                i,
                k_adjacencyList,
                k_edgesOffset,
                k_edgesSize,
                visited_mempool,
                q_mempool,
                k_result);
    }
    cudaDeviceSynchronize();

    cudaMemcpy(results, k_result, sizeof(int) * adj.size(), cudaMemcpyDeviceToHost);
    cudaFree(k_adjacencyList);
    cudaFree(k_edgesOffset);
    cudaFree(k_edgesSize);
    cudaFree(k_result);
    cudaFree(q_mempool);
    cudaFree(visited_mempool);

    int rtn = *std::max_element(results, results + adj.size());

    delete[] v_adj;
    delete[] v_adj_offset;
    delete[] v_adj_size;
    delete[] results;

    return rtn;
}


#endif //CUDA_TEST_CUDA_BFS_H
