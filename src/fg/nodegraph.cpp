#include "fg/nodegraph.h"

using namespace boost;

namespace fg {
	NodeGraph::NodeGraph():mG(){
		// get the property map for vertex indices
		mIndexMap = get(vertex_index, mG);
		mNodeMap = get(node_t(), mG);

		mRootNode = boost::shared_ptr<Node>(new Node());
		addNode(mRootNode);
	}

	boost::shared_ptr<Node> NodeGraph::addNode(boost::shared_ptr<Node> n){
		add_vertex(mG);
		n->_setGraphIndex(num_vertices(mG)-1);
		boost::put(mNodeMap, n->_getGraphIndex(), n);

		addEdge(mRootNode,n);
		return n;
	}

	void NodeGraph::addEdge(boost::shared_ptr<Node> from, boost::shared_ptr<Node> to){
		add_edge(from->_getGraphIndex(),to->_getGraphIndex(),mG);
	}

	void NodeGraph::addEdge(int f, int t){
		add_edge(f,t,mG);
	}

	///< update the node data based on the dependency graph
	void NodeGraph::update()
	{
		NodeUpdater vis(mIndexMap,mNodeMap);
		breadth_first_search(mG, vertex(0, mG), visitor(vis));
	}

	NodeGraph::NodeUpdater::NodeUpdater(IndexMap im, NodeMap nm):index(im),nodemap(nm){}
	void NodeGraph::NodeUpdater::initialize_vertex(Vertex v, Graph g){
		// std::cout << "init: " << v << "\n";
		// std::cout << (index)(v) << ":" << *(nodemap)(v) << "\n";
		(nodemap)(v)->resetCompoundTransform();
	}

	void NodeGraph::NodeUpdater::discover_vertex(Vertex v, Graph g){
		boost::shared_ptr<Node> n = nodemap(v);
		// If the compound transform hasn't been applied,
		// then we must be at the root
		if (!n->hasCompoundTransformBeenApplied()){
			n->applyCompoundTransform();
		}
	}

	void NodeGraph::NodeUpdater::examine_edge(Edge e, Graph g){
		// propagate combines to all children
		nodemap(target(e,g))->applyCompoundTransform(nodemap(source(e,g))->getCompoundTransform());
	}
}

std::ostream& operator<<(std::ostream& o, const fg::NodeGraph& n){
	typedef graph_traits<fg::NodeGraph::Graph>::vertex_iterator vertex_iter;
	std::pair<vertex_iter, vertex_iter> vp;
	for (vp = vertices(n.mG); vp.first != vp.second; ++vp.first)
	  std::cout << n.mIndexMap[*vp.first] <<  "( " << *n.mNodeMap[*vp.first] << ")\n";
	std::cout << std::endl;

	typedef graph_traits<fg::NodeGraph::Graph>::edge_iterator edge_iter;
	std::pair<edge_iter, edge_iter> ep;
	for (ep = edges(n.mG); ep.first != ep.second; ++ep.first)
		std::cout << n.mIndexMap(source(*ep.first,n.mG)) << "->"
		<< n.mIndexMap(target(*ep.first,n.mG)) << "\n";
	std::cout << std::endl;
}
