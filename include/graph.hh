#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <cstdlib>


class Graph {
public:
    // Constructor and Destructor
    Graph(int vertices);
    ~Graph();

    Graph& operator=(const Graph& other) {
        if (this != &other) { // Self-assignment check
            n = other.n;

            // Deep copy of dir_mx
            dir_mx.resize(other.dir_mx.size());
            for (int i = 0; i < n; ++i) {
                dir_mx[i].resize(other.dir_mx[i].size());
                std::copy(other.dir_mx[i].begin(), other.dir_mx[i].end(), dir_mx[i].begin());
            }

            // Deep copy of und_mx
            und_mx.resize(other.und_mx.size());
            for (int i = 0; i < n; ++i) {
                und_mx[i].resize(other.und_mx[i].size());
                std::copy(other.und_mx[i].begin(), other.und_mx[i].end(), und_mx[i].begin());
            }

            // Deep copy of budget_array
            budget_array.resize(other.budget_array.size());
            std::copy(other.budget_array.begin(), other.budget_array.end(), budget_array.begin());

            // Perform deep copy of other members if needed

        }
        return *this;
    }

    // Properties of Graph
    int n;

    std::vector<std::vector<int>> dir_mx;
    std::vector<std::vector<int>> und_mx;

    std::vector<int> budget_array;

    int latest_computed_mincut;
    float latest_computed_avgflow;
    int latest_robustness_chk;

    // Initializer Functions for budget array
    void init_rand_budgets(int b_min, int b_max);
    void init_rand_budgets_binary(int b_min, int b_max);
    void init_manually_budgets();

    // Initializer Functions for 
    void initialize_randomly();
    void initialize_manually();

    // Rendering the graph
    void render_graph(std::string filename, bool is_undir);

    // Computing Local Edge Connectivity between s and t
    int edmondsKarp(int s, int t);

    // Returns the minimum cut / maximum flow value of the graph
    int min_cut();

    // Returns the average flow utility of s
    float avg_flow(int s);

    // Returns the social utility for average flow
    float global_avg_flow();

    // Explains the graph, output to stdout
    void verbose();

    // Get the OPT value
    float get_opt();

    // Returns the number of robust connections of a player given the critical limit
    int num_robust_conn(int player, int lower_limit);

    int best_response(int player);
    int best_response_avg(int player);

    // Transfers from (u, v) to (v, w) a total of b units of budget
    void swap_edges(int u, int v, int w, int b);

    private:

    void gen_allocations(int budget, int idx, std::vector<int>& allocation, std::vector<std::vector<int>>& all_allocations);

};

#endif