#include "graph.h"

class Johnson
{
private:
    vector<vector< vector<int> > >  shortest_paths;
    vector<vector<pair<int, int> > > edges;
    vector<vector<int>> modified_distance_map;
    vector<vector<int>> real_distance_map;
    unsigned int nodes_number = 0;
    unsigned int edges_number = 0;
    int inf = 0;
public:
    Johnson(vector<vector<pair<int, int> > > edges_, unsigned int nodes_number_, unsigned int edges_number_ );
    void do_johnson();
    bool bellman_ford(int ver, vector<int>& dist, const vector<vector<pair<int, int> > >& graph_edges,unsigned int nodes_num);
    void dijkstra(int ver, vector<int>& dist, const vector<vector<pair<int, int> > >& graph_edges, vector< vector<int> >& path);
    vector<vector<int>> get_modified_distance_map() const
    {
        return modified_distance_map;
    }
    vector<vector< vector<int> > > get_shortest_path() const
    {
        return shortest_paths;
    }
    void get_real_distance_map();
    void print_results();
};


Johnson::Johnson(vector<vector<pair<int, int> > > edges_, unsigned int nodes_number_, unsigned int edges_number_)
{
    edges = edges_;
    nodes_number = nodes_number_;
    edges_number = edges_number_;
    shortest_paths.resize(nodes_number);
    inf = std::numeric_limits<int>::max();
}

void Johnson::do_johnson()
{
    vector<vector<pair<int, int> > > new_edges = edges;
    new_edges.resize(nodes_number + 1);
    for(unsigned int i = 0; i < nodes_number; i++)
        new_edges[nodes_number].push_back(make_pair(i, 0));
    vector<int> h;
    if(! bellman_ford(nodes_number, h, new_edges, (nodes_number + 1) ))
    {
        cout << "there is negative cycle" << endl;
        return;
    }
    vector<vector<pair<int, int> > > modified_edges = edges;
    for(unsigned int i = 0; i < modified_edges.size() ; ++i)
        for(unsigned int j = 0; j < modified_edges[i].size(); ++j)
        {
            modified_edges[i][j].second = modified_edges[i][j].second + h[i] - h[modified_edges[i][j].first];
            cout << "new weights " << modified_edges[i][j].second << endl;
        }
    modified_distance_map.resize(nodes_number);
    for(unsigned int i = 0; i < nodes_number; i++)
    {
        dijkstra(i, modified_distance_map[i], modified_edges, shortest_paths[i]);
    }



}

void Johnson::dijkstra(int ver, vector<int>& dist,  const vector<vector<pair<int, int> > >& graph_edges, vector< vector<int> >& path)
{
    dist.resize(nodes_number);
    dist.assign(nodes_number, inf);
    dist[ver] = 0;
    vector<bool> visit (nodes_number, false);
    vector<int> p(nodes_number, -1);
    path.resize(nodes_number);
    for(unsigned int i = 0; i < nodes_number; ++i)
    {
        int ver_for_visit = -1;
        for(unsigned int j = 0; j < nodes_number; ++j)
        {
            if ((!visit[j]) &&
               (ver_for_visit == -1 || dist[j] < dist[ver_for_visit]))

                ver_for_visit = j;
        }
        if ((ver_for_visit != -1 ) && (dist[ver_for_visit] == inf))
            break;
        for(unsigned int j = 0; j < graph_edges[ver_for_visit].size(); ++j)
        {
            int end_ver = graph_edges[ver_for_visit][j].first;
            int weight = graph_edges[ver_for_visit][j].second;
            if ((dist[ver_for_visit] + weight) < dist[end_ver])
            {
                dist[end_ver] = dist[ver_for_visit] + weight;
                p[end_ver] = ver_for_visit;
            }
        }
        visit[ver_for_visit] = true;

    }



    for(int i = 0; i < p.size(); i++)
    {
        int j = i;
        while(1)
        {
            if ((j == ver) || (j == -1))
                break;
            path[i].push_back(p[j]);
            j = p[j];

        }
        reverse(path[i].begin(), path[i].end());
        path[i].push_back(i);
    }

}

bool Johnson::bellman_ford(int ver, vector<int>& dist, const vector<vector<pair<int, int> > >& graph_edges, unsigned int nodes_num)
{

    dist.resize(nodes_num);
    dist.assign(nodes_num, inf);
    dist[ver] = 0;
    int x;

    for(unsigned int k = 0; k < nodes_num; ++k)
    {
        for (unsigned int i = 0; i < graph_edges.size(); ++i)
        {
            x = -1;
            for (unsigned j = 0; j < graph_edges[i].size(); ++j)
            {
                if((dist[graph_edges[i][j].first] < inf) || (i == ver))
                {
                    if (dist[graph_edges[i][j].first] > (dist[i] + graph_edges[i][j].second))
                    {
                        dist[graph_edges[i][j].first] = std::max(-inf, dist[i] + graph_edges[i][j].second);
                        x = graph_edges[i][j].first;
                    }
                }
            }

        }
    }

    if (x == -1)
        return true;
    else
        return false;
}

void Johnson::get_real_distance_map()
{
    real_distance_map.resize(nodes_number);
    for(unsigned int i = 0; i < nodes_number; i++)
    {
        real_distance_map[i].resize(nodes_number);
        real_distance_map[i].assign(nodes_number, 0);
        for(unsigned int j = 0; j < nodes_number; j++)
        {
            for (int k = 0; k < shortest_paths[i][j].size() - 1; k++ )
            {
                if( modified_distance_map[i][j] == inf)
                {
                    real_distance_map[i][j] = inf;
                    break;
                }
                else {
                    for(int m = 0; m < edges[shortest_paths[i][j][k]].size(); m++)
                    {
                        if(edges[shortest_paths[i][j][k]][m].first == k + 1)
                           real_distance_map[i][j] += edges[shortest_paths[i][j][k]][m].second;
                    }
                }
            }
        }

    }
}

void Johnson::print_results()
{
    get_real_distance_map();
    for (unsigned int i = 0 ; i < nodes_number; i ++) {
        for (unsigned int j = 0; j < nodes_number; j++) {
            if( modified_distance_map[i][j] != inf) {
                cout << i << " " << j << " with modified distance = " << modified_distance_map[i][j] << "\t with real distance = " << real_distance_map[i][j] <<  "\t path: ";
                for (auto k:shortest_paths[i][j])
                {
                    cout << k << " ";
                }
                cout << endl;
            }
        }
    }
}