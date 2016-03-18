#include <algorithm>
#include <vector>
#include <queue>
#include <stack>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;
enum NODE_TYPE { SOURCE, DESTINATION, MIDDLE };
enum NODE_STATUS { EXPLORED, FRONTIER, NEW };

class pipe {

public:
	string dest;
	int pipe_length, no_offtime;
	vector<pair<int, int> > off_times;
	pipe() {}
	pipe(string d, int p_l, int no_off, vector<pair<int, int> > o_t)
	{
		dest = d;
		pipe_length = p_l;
		no_offtime = no_off;
		off_times = o_t;
	}
	bool if_not_offtime(int cur_tim)
	{
		for (vector<pair<int, int> >::iterator p = off_times.begin(); p < off_times.end(); p++)
		{
			if (cur_tim >= p->first && cur_tim <= p->second)	return false;
		}
		return true;
	}
};

class node {

public:
	string name;
	NODE_TYPE n_type;
	NODE_STATUS n_status;
	int path_value;
	vector<pipe> n_pipes;
	node() {}
	node(const string t_name, NODE_TYPE t_type)
	{
		name = t_name;
		n_type = t_type;
		path_value = 0;
		n_status = NEW;
	}
	struct child_node
	{
		child_node(const pipe& t_p) :p(t_p) {}
		pipe p;
		bool operator()(const node& n)
		{
			if (p.dest == n.name) return true;
			else return false;
		}
	};
	vector<node> find_children_ucs(vector<node> node_list)
	{
		vector<node> r_nodes;
		for (vector<pipe>::iterator p = n_pipes.begin(); p < n_pipes.end(); p++)
		{
			if (p->if_not_offtime(path_value % 24))
			{
				vector<node>::iterator it = find_if(node_list.begin(), node_list.end(), child_node(*p));
				it->path_value = (path_value + p->pipe_length);
				r_nodes.push_back(*it);
			}

		}
		return r_nodes;
	}
	vector<node> find_children_bfs_dfs(vector<node> node_list)
	{
		vector<node> r_nodes;
		for (vector<pipe>::iterator p = n_pipes.begin(); p < n_pipes.end(); p++)
		{
			vector<node>::iterator it = find_if(node_list.begin(), node_list.end(), child_node(*p));
			it->path_value = (path_value + 1);
			r_nodes.push_back(*it);
		}
		return r_nodes;
	}
};
vector<node> add_node(string inp, vector<node> nodes_t, NODE_TYPE n)
{
	istringstream s(inp);
	while (!s.eof())
	{
		string x;
		getline(s, x, ' ');
		nodes_t.push_back(node(x, n));
	}
	return nodes_t;
}
struct find_node
{
	find_node(const string& t_p) :node1(t_p) {}
	string node1;
	bool operator()(const node& n)
	{
		if (n.name == node1) return true;
		else return false;
	}
};

vector<node> add_pipe(string inp, vector<node> p_nodes)
{
	istringstream s(inp);
	while (!s.eof())
	{
		string x, source, destination;
		int pipe_len, no_otime, x_s, x_d;
		vector<node>::iterator itr_s, itr_d;
		vector < pair<int, int> > off_times;
		getline(s, source, ' ');
		getline(s, destination, ' ');
		getline(s, x, ' ');
		pipe_len = atoi(x.c_str());
		getline(s, x, ' ');
		no_otime = atoi(x.c_str());
		for (int i = 0; i < no_otime; ++i)
		{
			getline(s, x, '-');
			x_s = atoi(x.c_str());
			getline(s, x, ' ');
			x_d = atoi(x.c_str());
			off_times.push_back(make_pair(x_s, x_d));
		}
		itr_s = find_if(p_nodes.begin(), p_nodes.end(), find_node(source));
		pipe p(destination, pipe_len, no_otime, off_times);
		itr_s->n_pipes.push_back(p);
		return p_nodes;
	}
}

bool sort_frontier(const node& A, const node& B)
{
	return (A.path_value > B.path_value) || ((A.path_value == B.path_value) && (A.name < B.name));
}
bool sort_frontier_dfs(const node& A, const node& B)
{
	return (A.name > B.name);
}

struct frontier_replace
{
	frontier_replace(const node t_p) :n(t_p) {}
	node n;
	bool operator()(node n_t)
	{
		if ((n_t.name == n.name) && (n.path_value < n_t.path_value)) return true;
		else return false;
	}
};
vector<node> set_nodestate(vector<node> nodes_ucs, string node_name, NODE_STATUS status)
{
	vector<node>::iterator itr_s = find_if(nodes_ucs.begin(), nodes_ucs.end(), find_node(node_name));
	itr_s->n_status = status;
	return nodes_ucs;
}
node ucs(vector<node> nodes_ucs, node source)
{
	vector<node> children;
	vector<node> frontier;
	vector<node> explored;
	vector<node>::iterator itr;
	node cur;
	frontier.push_back(source);
	while (5)
	{
		if (frontier.empty())
		{
			cur.path_value = -1;
			return cur;
		}
		cur = frontier.back();
		cout << "first pop\t" << cur.name << endl;
		frontier.pop_back();
		if (cur.n_type == DESTINATION) return cur;
		nodes_ucs = set_nodestate(nodes_ucs, cur.name, EXPLORED);
		explored.push_back(cur);
		children = cur.find_children_ucs(nodes_ucs);
		for (vector<node>::iterator n = children.begin(); n < children.end(); n++)
		{
			if ((n->n_status != FRONTIER) && (n->n_status != EXPLORED))
			{
				nodes_ucs = set_nodestate(nodes_ucs, n->name, FRONTIER);
				frontier.push_back(*n);
			}
			else if (n->n_status == FRONTIER)
			{
				replace_if(frontier.begin(), frontier.end(), frontier_replace(*n), *n);
			}
		}
		sort(frontier.begin(), frontier.end(), sort_frontier);
	}
}
node bfs(vector<node> nodes_bfs, node source)
{
	vector<node> children;
	queue<node> frontier;
	vector<node> explored;
	vector<node>::iterator itr;
	node cur;
	frontier.push(source);
	while (5)
	{
		if (frontier.empty())
		{
			cur.path_value = -1;
			return cur;
		}
		cur = frontier.front();
		cout << "first pop\t" << cur.name << endl;
		frontier.pop();
		if (cur.n_type == DESTINATION) return cur;
		nodes_bfs = set_nodestate(nodes_bfs, cur.name, EXPLORED);
		explored.push_back(cur);
		children = cur.find_children_bfs_dfs(nodes_bfs);
		sort(children.begin(), children.end(), sort_frontier);
		for (vector<node>::iterator n = children.begin(); n < children.end(); n++)
		{
			if ((n->n_status != FRONTIER) && (n->n_status != EXPLORED))
			{
				if (n->n_type == DESTINATION) return *n;
				nodes_bfs = set_nodestate(nodes_bfs, n->name, FRONTIER);
				frontier.push(*n);
			}
		}
	}
}
node dfs(vector<node> nodes_dfs, node source)
{
	vector<node> children;
	stack<node> frontier;
	vector<node> explored;
	vector<node>::iterator itr;
	node cur;
	frontier.push(source);
	while (5)
	{
		if (frontier.empty())
		{
			cur.path_value = -1;
			return cur;
		}
		cur = frontier.top();
		cout << "first pop\t" << cur.name << endl;
		frontier.pop();
		if (cur.n_type == DESTINATION) return cur;
		if (cur.n_status != EXPLORED)
		{
			nodes_dfs = set_nodestate(nodes_dfs, cur.name, EXPLORED);
			explored.push_back(cur);
			children = cur.find_children_bfs_dfs(nodes_dfs);
			sort(children.begin(), children.end(), sort_frontier_dfs);
			for (vector<node>::iterator n = children.begin(); n < children.end(); n++)
			{
				if ((n->n_status != EXPLORED))
				{
					if (n->n_type == DESTINATION) return *n;
					nodes_dfs = set_nodestate(nodes_dfs, n->name, FRONTIER);
					frontier.push(*n);
				}
			}
		}
	}
}
bool source_node(const node& n)
{
	if (n.n_type == SOURCE) return true;
	else return false;
}

int main(void)
{
	int no_it;
	string cur_line;
	ifstream i_file;
	ofstream o_file;
	i_file.open("sample.txt");
	o_file.open("output.txt");
	if (i_file.is_open())
	{
		getline(i_file, cur_line);
		no_it = atoi(cur_line.c_str());
		string algo;
		for (int i = 0; i < no_it; i++)
		{
			int no_pipe;
			vector<node> nodes;
			getline(i_file, algo);
			cout << algo << endl;
			getline(i_file, cur_line);
			nodes = add_node(cur_line, nodes, SOURCE);
			getline(i_file, cur_line);
			nodes = add_node(cur_line, nodes, DESTINATION);
			getline(i_file, cur_line);
			nodes = add_node(cur_line, nodes, MIDDLE);
			getline(i_file, cur_line);
			no_pipe = atoi(cur_line.c_str());
			for (int j = 0; j < no_pipe; j++)
			{
				getline(i_file, cur_line);
				nodes = add_pipe(cur_line, nodes);
			}
			string offset_time;
			getline(i_file, offset_time);
			getline(i_file, cur_line);
			vector<node>::iterator it = find_if(nodes.begin(), nodes.end(), source_node);
			it->path_value = atoi(offset_time.c_str());
			node output;
			if (algo == "UCS")	output = ucs(nodes, *it);
			else if (algo == "DFS")	output = dfs(nodes, *it);
			else if (algo == "BFS")	output = bfs(nodes, *it);
			else cout << "wrong input\n";
			if (output.path_value == -1) o_file << "None\n";
			else	o_file << output.name << " " << output.path_value % 24 << endl;
			nodes.clear();
		}
	}
}

