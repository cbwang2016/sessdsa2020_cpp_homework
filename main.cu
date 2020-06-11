#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <string>
#include <algorithm>
#include "json.hpp"
#include "cuda_bfs.h"

using json = nlohmann::json;
using namespace std;

void split(const string &str, vector<string> &result, const string &sep) {
    result.clear();

    string::size_type i, j, len = str.size(), n = sep.size();

    i = j = 0;

    while (i + n <= len) {
        if (str[i] == sep[0] && str.substr(i, n) == sep) {
            result.push_back(str.substr(j, i - j));
            i = j = i + n;
        } else {
            i++;
        }
    }

    result.push_back(str.substr(j, len - j));
}

pair<vector<vector<int>>, map<string, int>>
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
        split(film["actor"], actors, ",");
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
    return make_pair(adjacency_list, actors_ids);
}

vector<vector<int>> getSubgraphs(const vector<vector<int>> &adjacency_list) {
    vector<vector<int>> subgraphs;
    vector<bool> visited(adjacency_list.size(), false);

    for (auto i = 0; i < adjacency_list.size(); i++) {
        if (visited[i])
            continue;

        stack<int> s;
        vector<int> in_set{i};
        s.push(i);
        visited[i] = true;

        while (!s.empty()) {
            int front = s.top();
            s.pop();
            for (int j : adjacency_list[front]) {
                if (!visited[j]) {
                    visited[j] = true;
                    s.push(j);
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

    return getDiameterGPU(adj);
}

int main() {
    auto tmp = init();
    auto adjacency_list = tmp.first;
    auto actors_ids = tmp.second;

    vector<vector<int>> subgraphs = getSubgraphs(adjacency_list);

    sort(subgraphs.begin(), subgraphs.end(), [](const vector<int> &a, const vector<int> &b) -> bool {
        return a.size() < b.size();
    });

    cout << "subgraphs count: " << subgraphs.size() << endl;

    vector<int> diameters;
    diameters.reserve(subgraphs.size());
    for (const auto &subgraph : subgraphs)
        diameters.push_back(getDiameter(adjacency_list, subgraph));

    for (auto i = subgraphs.end() - 10; i != subgraphs.end(); i++)
        cout << i->size() << " ";
    cout << "\n";
    copy(diameters.end() - 10, diameters.end(), ostream_iterator<int>(cout, " "));
    // print the diameters of the largest 10 subgraphs

    return 0;
}
