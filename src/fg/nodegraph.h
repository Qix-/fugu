/**
 * \file
 * \author ben
 * 
 * \cond showlicense
 * \verbatim
 * --------------------------------------------------------------
 *    ___     
 *   |  _|___ 
 *   |  _| . | fg: real-time procedural 
 *   |_| |_  | animation and generation 
 *       |___| of 3D forms
 *
 *   Copyright (c) 2011 Centre for Electronic Media Art (CEMA)
 *   Monash University, Australia. All rights reserved.
 *
 *   Use of this software is governed by the terms outlined in 
 *   the LICENSE file.
 * 
 * --------------------------------------------------------------
 * \endverbatim
 * \endcond
 */

#ifndef FG_NODEGRAPH_H
#define FG_NODEGRAPH_H

#include "fg/node.h"

// boost graph stuff
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
// #include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/range/irange.hpp>

namespace fg {
	class NodeGraph;
}

std::ostream& operator<<(std::ostream& o, const fg::NodeGraph& n);

namespace fg {
	/** \brief A directed graph data structure that contains fg::Nodes.
	 *
	 * NodeGraph is used internally within fg::Universe in order to calculate
	 * the correct order in which to update the properties of nodes.
	 */	 
	class NodeGraph {
	public:
		struct node_t {
			typedef boost::vertex_property_tag kind;
		};
		typedef boost::property<node_t, boost::shared_ptr<Node> > NodeProperty;
		typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, NodeProperty> Graph;
		typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;
		typedef boost::property_map<Graph, node_t>::type NodeMap;
		typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
		typedef boost::graph_traits<Graph>::edge_descriptor Edge;

	public:
		NodeGraph();
		boost::shared_ptr<Node> addNode(boost::shared_ptr<Node> n);
		void addEdge(boost::shared_ptr<Node> from, boost::shared_ptr<Node> to);
		void addEdge(int f, int t);

		/// \brief update the node data based on the dependency graph
		void update(); 

		friend std::ostream& (::operator <<)(std::ostream& o, const fg::NodeGraph& n);

	protected:
		IndexMap mIndexMap;
		NodeMap mNodeMap;
		Graph mG;
		boost::shared_ptr<Node> mRootNode;

		/*
		 * Updates the nodes breadth-first
		 */
		class NodeUpdater: public boost::default_bfs_visitor {
		public:
			NodeUpdater(IndexMap im, NodeMap nm);
			void initialize_vertex(Vertex v, Graph g);
			void discover_vertex(Vertex v, Graph g);
			void examine_edge(Edge e, Graph g);
		protected:
			IndexMap index;
			NodeMap nodemap;
		};
	};
}



#endif
