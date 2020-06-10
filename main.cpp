#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include "json.hpp"
#include "pystring/pystring.h"

using json = nlohmann::json;
using namespace std;

tuple<vector<vector<int>>, map<string, int>>

init() {
    ifstream f("Film.json");
    json data;
    f >> data;
    cout << "films count: " << data.size() << endl;

    vector<vector<int>> adjacency_list;
    map<string, int> actors_ids;

    int count = 0;
    for (auto film : data) {
        vector<string> actors;
        pystring::split(film["actor"], actors, ",");
        for (const string &actor_name : actors) {
            if (!actors_ids.count(actor_name)) {
                actors_ids[actor_name] = count;
                count += 1;
                adjacency_list.emplace_back();
            }
        }

        for (const string &actor_name : actors)
            for (const string &actor2_name : actors)
                if (actor_name != actor2_name)
                    adjacency_list[actors_ids[actor_name]].push_back(actors_ids[actor2_name]);
    }
    cout << "actors count: " << actors_ids.size() << endl;
    return {adjacency_list, actors_ids};
}

vector<vector<int>> getSubgraphs(const vector<vector<int>> &adjacency_list) {
    vector<vector<int>> subgraphs;
    vector<bool> visited(adjacency_list.size(), false);

    for (auto i = 0; i < adjacency_list.size(); i++) {
        if (visited[i])
            continue;

        queue<int> q;
        vector<int> in_set{i};
        q.push(i);
        visited[i] = true;

        while (!q.empty()) {
            int front = q.front();
            q.pop();
            for (int j : adjacency_list[front]) {
                if (!visited[j]) {
                    visited[j] = true;
                    q.push(j);
                    in_set.push_back(j);
                }
            }
        }
        subgraphs.push_back(in_set);
    }

    return subgraphs;
}

int getMaxDistance(const vector<vector<int>> &adj, int index0, vector<bool> &visited, vector<int> &q) {
    fill(visited.begin(), visited.end(), false);
    int queue_start = 0, queue_end = 1;
    q[0] = (index0);
    visited[index0] = true;

    int count = -1, marker, front;
    while (queue_start != queue_end) {
        marker = q[queue_end - 1];
        do {
            front = q[queue_start++];
            for (int i : adj[front]) {
                if (!visited[i]) {
                    visited[i] = true;
                    q[queue_end++] = i;
                }
            }
        } while (marker != front);

        count++;
    }
    return count;
}

int getDiameter(const vector<vector<int>> &adjacency_list, const vector<int> &actors_set) {
    int count = 0;
    map<int, int> id_map;
    for (auto old_id : actors_set) {
        id_map[old_id] = count;
        count += 1;
    }

    vector<vector<int>> adj;
    adj.reserve(actors_set.size());
    for (auto old_id : actors_set) {
        vector<int> l;
        l.reserve(adjacency_list[old_id].size());
        for (int i : adjacency_list[old_id])
            l.push_back(id_map[i]);
        adj.push_back(l);
    }

    int diameter = -1;
    vector<bool> visited(adj.size()); //preallocate "visited" and "q" for better performance
    vector<int> q(adj.size()); // use vector<int> as a queue for faster performance
#pragma omp parallel for default(none) shared(cout, actors_set, adj) firstprivate(visited, q) reduction(max:diameter)
    for (auto i = 0; i < actors_set.size(); i++) {
        diameter = max(diameter, getMaxDistance(adj, i, visited, q));
        if (i % 500 == 0 and actors_set.size() > 1000)
            cout << i << endl;
    }
    return diameter;
}

int main() {
    auto[adjacency_list, actors_ids] = init();

    vector<vector<int>> subgraphs = getSubgraphs(adjacency_list);

    sort(subgraphs.begin(), subgraphs.end(), [](const vector<int> &a, const vector<int> &b) -> bool {
        return a.size() < b.size();
    });

    cout << "subgraphs count: " << subgraphs.size() << endl;

    vector<int> diameters;
    diameters.reserve(subgraphs.size());
    for (const auto &subgraph : subgraphs)
        diameters.push_back(getDiameter(adjacency_list, subgraph));

    copy(diameters.end() - 10, diameters.end(), ostream_iterator<int>(cout, " "));
    // print the diameters of the largest 10 subgraphs

    return 0;
}
