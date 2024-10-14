#include <fmt/core.h>
#include <fmt/format.h>
#include "include/graph.hh"
#include <cmath>
#include <random>
#include <algorithm>
#include <iostream>
#include <numeric>


Graph br_dynamics_maxflow(const Graph& G)
{
    std::srand(time(0));

    Graph current = G;

    // Simple loop to choose next player
    unsigned int turn = 0;
    std::vector<int> turn_vect;
    std::vector<bool> nay_vect(G.n, false);

    turn_vect.resize(G.n);
    std::iota(turn_vect.begin(), turn_vect.end(), 0);

    std::random_device rd;
    std::mt19937 g(rd());
 
    std::shuffle(turn_vect.begin(), turn_vect.end(), g);

    while (1){

        if (turn % G.n == 0) std::shuffle(turn_vect.begin(), turn_vect.end(), g);
        int player = turn_vect[turn % G.n];

        //if (turn % G.n == 0) fmt::print("------\n");
        //fmt::print("{}\n", player);    


        // Best Response
        bool res = current.best_response(player);

        if (res) {
            fmt::print("PLAYER {} MINCUT {} ROBUST {} [OK] -- ", player, current.latest_computed_mincut, current.latest_robustness_chk);
            std::fill(nay_vect.begin(), nay_vect.end(), false);
        }
        else {
            fmt::print("PLAYER {} MINCUT {} ROBUST {} [KO]\n", player, current.latest_computed_mincut, current.latest_robustness_chk);
            nay_vect[player] = true;

            if (std::all_of(nay_vect.begin(), nay_vect.end(), [](bool value) { return value; })) break;
        }

        if (res){
            for (auto x : current.dir_mx[player])
                fmt::print("{} ",x);
            fmt::print("\n");
        }

        
        ++turn;

    }

    return current;
}

Graph br_dynamics_avgflow(const Graph& G)
{
    std::srand(time(0));

    Graph current = G;

    // Simple loop to choose next player
    unsigned int turn = 0;
    std::vector<int> turn_vect;

    turn_vect.resize(G.n);
    std::iota(turn_vect.begin(), turn_vect.end(), 0);

    std::random_device rd;
    std::mt19937 g(rd());
 
    std::shuffle(turn_vect.begin(), turn_vect.end(), g);

    int consecutive_nay = 0;

    while (1){

        if (turn % G.n == 0) std::shuffle(turn_vect.begin(), turn_vect.end(), g);
        int player = turn_vect[turn % G.n];

        // Best Response
        bool res = current.best_response_avg(player);

        if (res) consecutive_nay = 0;
        else consecutive_nay++;

        std::cout << "PLAYER " << player << " AVGFLOW " << std::to_string(current.latest_computed_avgflow);

        if (res) std::cout << " [OK]" << std::endl;
        else     std::cout << " [KO]" << std::endl;

        if (res){
            for (auto x : current.dir_mx[player])
                fmt::print("{} ",x);
            fmt::print("\n");
        }

        if (consecutive_nay > 2*G.n) break;
        
        ++turn;

    }

    return current;

}

int main()
{
    Graph G(9);

    G.init_rand_budgets_binary(1, 5);
    G.init_manually_budgets();
    G.initialize_randomly();

/*
    G.budget_array = {2, 2, 2, 2, 2, 2};
    
    G.dir_mx = {{0, 2, 0, 0, 0, 0},
                {1, 0, 0, 1, 0, 0},
                {0, 2, 0, 0, 0, 0},
                {0, 0, 0, 0, 2, 0},
                {0, 0, 1, 1, 0, 0},
                {0, 0, 0, 0, 2, 0}};

    for (int i = 0; i < G.n; ++i)
    for (int j = 0; j < i; ++j)
    {
        G.und_mx[i][j] = G.dir_mx[i][j] + G.dir_mx[j][i];
        G.und_mx[j][i] = G.und_mx[i][j];
    }
  */  

    G.verbose();

    G.render_graph("init-dir", false);
    G.render_graph("init", true);

    fmt::print("Initial mincut is {}\n", G.min_cut());
    //fmt::print("Initial Avg Utility is {}\n", G.global_avg_flow());

    fmt::print("OPT equals {}\n\n", G.get_opt());

    Graph H = br_dynamics_maxflow(G); // Change this to use br_dynamics_avgflow

    H.render_graph("final", true);
    H.render_graph("final-dir", false);

    fmt::print("Final mincut is {}\n", H.min_cut());
    //fmt::print("Average Utility is {}\n", H.global_avg_flow());
}
