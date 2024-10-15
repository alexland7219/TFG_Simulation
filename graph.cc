#include "include/graph.hh"
#include <vector>
#include <cstdlib>
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <climits>
#include <fmt/core.h>
#include <fmt/format.h>
#include <ctime>
#include <string>
#include <utility>
#include <unistd.h>
#include <queue>

Graph::Graph(int vertices){
    n = vertices;
    dir_mx.resize(n, std::vector<int>(n, 0));
    und_mx.resize(n, std::vector<int>(n, 0));
    budget_array.resize(n, 0);
}

Graph::~Graph() {
}

void Graph::init_rand_budgets(int b_min, int b_max){
    std::srand(std::time(0));


    for (int i = 0; i < n; ++i){
        // Random number between b_min and b_max
        budget_array[i] = std::rand() % (b_max + 1 - b_min) + b_min; 
    }
}

void Graph::init_manually_budgets()
{
    for (int i = 0; i < n; ++i)
    {
        fmt::print("\nBUDGET OF {} ? ", i);
        std::cin >> budget_array[i];
    }
}

void Graph::init_rand_budgets_binary(int b_min, int b_max){
    std::srand(std::time(0));

    for (int i = 0; i < n; ++i){
        // Random number between 0 and 1
        if (std::rand() % 2 == 0) budget_array[i] = b_min;
        else                      budget_array[i] = b_max;
    }
}

float Graph::get_opt(){
    return 2*std::reduce(budget_array.begin(), budget_array.end()) / n;
}

void Graph::initialize_randomly(){
    std::srand(std::time(0)); // Seed the random number generator

    for (int i = 0; i < n; ++i) {
        int budget = budget_array[i];

        while (budget > 0) {
            int j = std::rand() % n; // Choose a random node to connect
            if (i == j) continue;

            int spend = std::rand() % (budget + 1); // Random amount to spend on this edge
            dir_mx[i][j] += spend;
            und_mx[i][j] += spend;
            und_mx[j][i] += spend;
            budget -= spend;
        }
    }
}

float Graph::global_avg_flow()
{
    float x = 0;
    for (int i = 0; i < n; ++i)
        x += avg_flow(i);

    return x/n;
}

void Graph::initialize_manually(){
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
    {
        if (j == i) continue;

        fmt::print("\nFROM {} TO {} ? ", i, j);
        std::cin >> dir_mx[i][j];

        und_mx[i][j] += dir_mx[i][j];
        und_mx[j][i] += dir_mx[i][j];

    }
}

void Graph::render_graph(std::string filename, bool is_undir){
    std::ofstream outfile;

    outfile.open(filename + ".dot");

    if (is_undir) outfile << "graph G {" << std::endl;
    else          outfile << "digraph G {" << std::endl;

    for (int i = 0; i < n; ++i)
    for (int j = 0; j < i; ++j)
    {        
        if (is_undir){
            if (und_mx[i][j] == 0) continue;
            outfile << "    " << i << " -- " << j << " [label=" << und_mx[i][j] << "];" << std::endl;
        }
        else {
            if (dir_mx[i][j] != 0)
                outfile << "    " << i << " -> " << j << " [label=" << dir_mx[i][j] << "];" << std::endl;
            if (dir_mx[j][i] != 0)
                outfile << "    " << j << " -> " << i << " [label=" << dir_mx[j][i] << "];" << std::endl;

        }
    }

    outfile << "}" << std::endl;
    outfile.close();

    std::string dotfile = filename + ".dot";
    std::string psfile  = filename + ".ps";

    int pid = fork();

    if (pid == 0)
        execlp("dot", "dot", "-Tps", dotfile.c_str(), "-o", psfile.c_str(), (char *) NULL);

    return;
}

int Graph::min_cut() {

    std::vector<std::vector<int>> mc_matrix(n, std::vector<int>(n, 0));
    
    int current_mc = INT_MAX;
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j)
        {
            // Source is s, sink is j
            mc_matrix[i][j] = edmondsKarp(i, j);
            if (mc_matrix[i][j] < current_mc)
                current_mc = mc_matrix[i][j];
        }

    return current_mc;
}

bool bfs(const std::vector<std::vector<int>>& rGraph, int s, int t, std::vector<int>& parent) {
    int V = rGraph.size();
    std::vector<bool> visited(V, false);
    std::queue<int> q;
    q.push(s);

    visited[s] = true;
    parent[s] = -1;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v = 0; v < V; ++v) {
            if (!visited[v] && rGraph[u][v] > 0) {
                if (v == t) {
                    parent[v] = u;
                    return true;
                }
                q.push(v);
                parent[v] = u;
                visited[v] = true;
            }
        }
    }
    return false;
}

int Graph::edmondsKarp(int s, int t) {
    std::vector<std::vector<int>> rGraph = und_mx;
    std::vector<int> parent(n);

    int max_flow = 0;

    while (bfs(rGraph, s, t, parent)) {
        int path_flow = INT_MAX;

        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            path_flow = std::min(path_flow, rGraph[u][v]);
        }

        for (int v = t; v != s; v = parent[v]) {
            int u = parent[v];
            rGraph[u][v] -= path_flow;
            rGraph[v][u] += path_flow;
        }

        max_flow += path_flow;
    }

    return max_flow;
}


void Graph::verbose()
{
    fmt::print("Description of graph\n");
    fmt::print("-----------------------\n");

    for (int i = 0; i < n; ++i)
    {
        fmt::print("PLAYER {} BUDGET {}\n", i, budget_array[i]);
    }

    fmt::print("\n");
}

int Graph::num_robust_conn(int player, int lower_limit)
{
    int count = 0;

    for (int p = 0; p < n; ++p)
    {
        if (p == player) continue;
        if (edmondsKarp(player, p) > lower_limit) ++count;
    }

    return count;
}

int Graph::best_response_avg(int node)
{
    int orig_budget = budget_array[node];
    std::vector<int> best_allocation = dir_mx[node];

    float max_val = avg_flow(node);
    bool changed = false;

    std::vector<int> allocation(n, 0);
    std::vector<std::vector<int>> all_allocations;
    gen_allocations(orig_budget, 0, allocation, all_allocations);

    for (const auto& alloc : all_allocations)
    {
        for (int i = 0; i < n; ++i){
            und_mx[node][i] -= dir_mx[node][i];
            und_mx[i][node] -= dir_mx[node][i];
        } 

        for (int i = 0; i < n; ++i) {
            dir_mx[node][i] = alloc[i];
            und_mx[node][i] += alloc[i];
            und_mx[i][node] += alloc[i];
        }

        float new_val = avg_flow(node);

        if (new_val > max_val)
        {
            max_val = new_val;
            best_allocation = alloc;
            changed = true;
        }
    }

    // Update the node's allocation to the best found
    for (int i = 0; i < n; ++i) {
        dir_mx[node][i] = best_allocation[i];
        und_mx[node][i] = dir_mx[node][i] + dir_mx[i][node];
        und_mx[i][node] = und_mx[node][i];
    }

    latest_computed_avgflow = max_val;

    return changed;

}

int Graph::best_response(int node)
{
    int orig_budget = budget_array[node];
    std::vector<int> best_allocation = dir_mx[node];

    int old_mc = min_cut();
    std::pair<int, int> min_val(old_mc, num_robust_conn(node, old_mc));

    //fmt::print("{} {}\n", old_mc, min_val.second);
    //for (auto k : best_allocation)
    //    fmt::print("{} ", k);

    bool changed = false;

    // First we find neighbours for the node

    std::vector<int> allocation(n, 0);
    std::vector<std::vector<int>> all_allocations;
    gen_allocations(orig_budget, 0, allocation, all_allocations);

    
    //for (auto& x : all_allocations){
    //    for (auto y  : x) fmt::print("{} ", y);
    //    fmt::print("\n");
    //}

    for (const auto& alloc : all_allocations) {

        for (int i = 0; i < n; ++i){
            und_mx[node][i] -= dir_mx[node][i];
            und_mx[i][node] -= dir_mx[node][i];
        } 

        for (int i = 0; i < n; ++i) {
            dir_mx[node][i] = alloc[i];
            und_mx[node][i] += alloc[i];
            und_mx[i][node] += alloc[i];
        }

        int new_min_cut = min_cut();
        int tiebreaker  = num_robust_conn(node, new_min_cut);

        // Restore the original allocation if the new one is not better
        if (new_min_cut > min_val.first || (new_min_cut == min_val.first && tiebreaker > min_val.second))
        {
            //fmt::print("N {} {} #", new_min_cut, tiebreaker);
            //for (auto k : alloc)
            //    fmt::print("{} ", k);
            //fmt::print("\n");

            min_val.first = new_min_cut;
            min_val.second = tiebreaker;
            best_allocation = alloc;
            changed = true;
        }
    }

    // Update the node's allocation to the best found
    for (int i = 0; i < n; ++i) {
        dir_mx[node][i] = best_allocation[i];
        und_mx[node][i] = dir_mx[node][i] + dir_mx[i][node];
        und_mx[i][node] = und_mx[node][i];
    }

    latest_computed_mincut = min_val.first;
    latest_robustness_chk  = min_val.second;

    return changed;
}

float Graph::avg_flow(int s)
{
    float total = 0;

    for (int i = 0; i < n; ++i)
    {
        if (i == s) continue;

        total += edmondsKarp(s, i);
    }

    return total / (n-1);
}

// Searches all possible combinations
void Graph::gen_allocations(int budget, int idx, std::vector<int>& allocation, std::vector<std::vector<int>>& all_allocations){
    if (idx == allocation.size() - 1) {
        allocation[idx] = budget;
        all_allocations.push_back(allocation);
        return;
    }

    for (int b = 0; b <= budget; ++b)
    {
        allocation[idx] = b;
        gen_allocations(budget - b, idx + 1, allocation, all_allocations);
    }
}

void Graph::swap_edges(int u, int v, int w, int b)
{
    dir_mx[u][v] -= b;
    dir_mx[u][w] += b;
    und_mx[u][v] -= b;
    und_mx[v][u] -= b;
    und_mx[u][w] += b;
    und_mx[w][u] += b;
}