/**
 * Playing around with boost graph
 * BP
 */

#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <vector>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
// #include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/range/irange.hpp>

using namespace boost;

class Node {
public:
	Node(int i):data(i),combined(0),mHasBeenCombined(false){}
	void reset(){combined = 0; mHasBeenCombined = false;}
	void combine(int combinedParents=0){
		combined = combinedParents + data;
		mHasBeenCombined = true;
	}
	int combinedValue(){return combined;}
	bool hasBeenCombined(){return mHasBeenCombined;}
public:
	int data, combined;
	bool mHasBeenCombined;
	// int mGraphIndex; // need one index per graph?
};

std::ostream& operator<<(std::ostream& o, const Node& n){
	return o << "Node@" << &n << "[" << n.data << "," << n.combined << "]";
}

class NodeGraph {
public:
	typedef property<node_t, boost::shared_ptr<Node> > NodeProperty;
	typedef adjacency_list<vecS, vecS, directedS, NodeProperty> Graph;
	typedef property_map<Graph, vertex_index_t>::type IndexMap;
	typedef property_map<Graph, node_t>::type NodeMap;
	typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
	typedef boost::graph_traits<Graph>::edge_descriptor Edge;

public:
	NodeGraph(){}

	void addNode(boost::shared_ptr<Node> n);
	void addEdge(boost::shared_ptr<Node> from, boost::shared_ptr<Node> to);

	void update(); // update the node data based on the dependency graph

protected:

};




struct node_t {
	typedef vertex_property_tag kind;
};

typedef property<node_t, boost::shared_ptr<Node> > NodeProperty;
typedef adjacency_list<vecS, vecS, directedS, NodeProperty> Graph;
typedef property_map<Graph, vertex_index_t>::type IndexMap;
typedef property_map<Graph, node_t>::type NodeMap;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;

/**
 * Combines the nodes values based on the graph
 * is a boost graph visitor
 */
class NodeCombiner: public default_bfs_visitor {
public:
	NodeCombiner(IndexMap im, NodeMap nm):index(im),nodemap(nm){}
	void initialize_vertex(Vertex v, Graph g){
		// std::cout << "init: " << v << "\n";
		// std::cout << (index)(v) << ":" << *(nodemap)(v) << "\n";
		(nodemap)(v)->reset();
	}

	void discover_vertex(Vertex v, Graph g){
		boost::shared_ptr<Node> n = nodemap(v);
		if (!n->hasBeenCombined()){
			n->combine();
		}
	}

	void examine_edge(Edge e, Graph g){
		// propagate combines to all children
		nodemap(target(e,g))->combine(nodemap(source(e,g))->combinedValue());
	}

protected:
	IndexMap index;
	NodeMap nodemap;
};

int main(){
	Graph g;

	// get the property map for vertex indices
	IndexMap index = get(vertex_index, g);
	NodeMap nodemap = get(node_t(), g);

	std::cout << "Building graph...\n";
	int numNodes = 5;
	for(int i=0;i<numNodes;i++){
		add_vertex(g);
		boost::put(nodemap, i, boost::shared_ptr<Node>(new Node(i)));
		if (i > 1 && i%2==0){
			add_edge(i,i-1,g);
			std::cout << "(" << i << "->" << i-1 << ") ";
		}
		else if (i>1){
			add_edge(i-1,i,g);
			std::cout << "(" << i-1 << "->" << i << ") ";
		}

		// node 0 is the root node, to which all others are connected
		if (i!=0){
			add_edge(0,i,g);
			std::cout << "(" << 0 << "->" << i << ") ";
		}
	}

	// What happens if we put a cycle in?
	add_edge(numNodes-1,0,g);
	std::cout << "\nCausing cycle(" << numNodes-1 << "->" << 0 << ")\n";

	std::cout << "** nodes before update **\n";
	typedef graph_traits<Graph>::vertex_iterator vertex_iter;
	std::pair<vertex_iter, vertex_iter> vp;
	for (vp = vertices(g); vp.first != vp.second; ++vp.first)
	  std::cout << index[*vp.first] <<  "( " << *nodemap[*vp.first] << ")\n";
	std::cout << std::endl;

	// to propagate updates ..
	// 1. reset all node combined info
	/* NB: This is done in the init phase of the breadth first traversal
	for(int i=0;i<numNodes;i++){
		nodemap[i]->reset();
	}
	*/

	// 2. do a breadth first traversal on nodes n,
	// if n hasn't been combined then combine() it
	// for c in n.children combine(n.combinedValue)

	NodeCombiner vis(index,nodemap);
	breadth_first_search(g, vertex(0, g), visitor(vis));

	// WHA?
	// breadth_first_search(g, )

	/*
	typedef std::list< Vertex > container;
	container c;
  	topological_sort(g, std::front_inserter(c));
  	BOOST_FOREACH(Vertex& v, c){
  		std::cout << v << "\n";
  	}*/

	/*
	// add the edges to the graph object
	for (int i = 0; i < num_edges; ++i)
	  add_edge(edge_array[i].first, edge_array[i].second, g);
	  */

	std::cout << "** nodes after update **\n";
	for (vp = vertices(g); vp.first != vp.second; ++vp.first)
	  std::cout << index[*vp.first] <<  "( " << *nodemap[*vp.first] << ")\n";
	std::cout << std::endl;

	return 0;
}
