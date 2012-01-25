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
	Node(int i):data(i),combined(0),mHasBeenCombined(false),mGraphIndex(-1){}
	void reset(){combined = 0; mHasBeenCombined = false;}
	void combine(int combinedParents=0){
		combined = combinedParents + data;
		mHasBeenCombined = true;
	}
	int combinedValue(){return combined;}
	bool hasBeenCombined(){return mHasBeenCombined;}

	void _setGraphIndex(int gi){mGraphIndex=gi;}
	int _getGraphIndex(){return mGraphIndex;}
public:
	int data, combined;
	bool mHasBeenCombined;

	int mGraphIndex; // need one index per graph?
};

std::ostream& operator<<(std::ostream& o, const Node& n){
	return o << "Node@" << &n << "[" << n.data << "," << n.combined << "]";
}

class NodeGraph {
public:
	struct node_t {
		typedef vertex_property_tag kind;
	};
	typedef property<node_t, boost::shared_ptr<Node> > NodeProperty;
	typedef adjacency_list<vecS, vecS, directedS, NodeProperty> Graph;
	typedef property_map<Graph, vertex_index_t>::type IndexMap;
	typedef property_map<Graph, node_t>::type NodeMap;
	typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
	typedef boost::graph_traits<Graph>::edge_descriptor Edge;

public:
	NodeGraph():mG(){
		// get the property map for vertex indices
		mIndexMap = get(vertex_index, mG);
		mNodeMap = get(node_t(), mG);

		mRootNode = boost::shared_ptr<Node>(new Node(0));
		addNode(mRootNode);
	}

	boost::shared_ptr<Node> addNode(boost::shared_ptr<Node> n){
		add_vertex(mG);
		n->_setGraphIndex(num_vertices(mG)-1);
		boost::put(mNodeMap, n->_getGraphIndex(), n);

		addEdge(mRootNode,n);
		return n;
	}

	void addEdge(boost::shared_ptr<Node> from, boost::shared_ptr<Node> to){
		add_edge(from->_getGraphIndex(),to->_getGraphIndex(),mG);
	}

	void addEdge(int f, int t){
		add_edge(f,t,mG);
	}

	///< update the node data based on the dependency graph
	void update()
	{
		NodeCombiner vis(mIndexMap,mNodeMap);
		breadth_first_search(mG, vertex(0, mG), visitor(vis));
	}

	friend std::ostream& operator<<(std::ostream& o, const NodeGraph& n);

protected:
	IndexMap mIndexMap;
	NodeMap mNodeMap;
	Graph mG;
	boost::shared_ptr<Node> mRootNode;

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
};

std::ostream& operator<<(std::ostream& o, const NodeGraph& n){
	typedef graph_traits<NodeGraph::Graph>::vertex_iterator vertex_iter;
	std::pair<vertex_iter, vertex_iter> vp;
	for (vp = vertices(n.mG); vp.first != vp.second; ++vp.first)
	  std::cout << n.mIndexMap[*vp.first] <<  "( " << *n.mNodeMap[*vp.first] << ")\n";
	std::cout << std::endl;

	typedef graph_traits<NodeGraph::Graph>::edge_iterator edge_iter;
	std::pair<edge_iter, edge_iter> ep;
	for (ep = edges(n.mG); ep.first != ep.second; ++ep.first)
		std::cout << n.mIndexMap(source(*ep.first,n.mG)) << "->"
		<< n.mIndexMap(target(*ep.first,n.mG)) << "\n";
	std::cout << std::endl;
}

int main(){
	NodeGraph ng;
	std::cout << "Building graph...\n";
	int numNodes = 5;

	boost::shared_ptr<Node> lastNode = boost::shared_ptr<Node>(new Node(1));
	ng.addNode(lastNode);

	for(int i=2;i<=numNodes;i++){
		boost::shared_ptr<Node> newNode = ng.addNode(boost::shared_ptr<Node>(new Node(i)));
		ng.addEdge(lastNode,newNode);
		lastNode = newNode;
	}

	std::cout << "** graph before update **\n";
	std::cout << ng;

	ng.update();

	std::cout << "** graph after update **\n";
	std::cout << ng;

	return 0;
}
