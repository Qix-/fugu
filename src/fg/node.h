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

#ifndef FG_NODE_H
#define FG_NODE_H

#include <ostream>

#include "fg/mat4.h"

// need to forward decl operator<<
namespace fg {class Node;}
std::ostream& operator<<(std::ostream& o, const fg::Node& n);

namespace fg {
	/**
	 * \brief A Node is a primitive (primarily spatial) entity in fg.
	 *
	 * Nodes can be linked together in hierarchical and other (spatial) relationships.
	 */
	class Node {
	public:
		Node();
		virtual ~Node();

		Mat4& getRelativeTransform();
		Mat4& getCompoundTransform();

		void resetCompoundTransform();
		void applyCompoundTransform();
		void applyCompoundTransform(const Mat4& mParentTransform);
		bool hasCompoundTransformBeenApplied() const;

		void _setGraphIndex(int gi); ///< used internally by the dependency graph in fg universe
		int _getGraphIndex() const; ///< used internally by the dependency graph in fg universe

		friend std::ostream& (::operator <<)(std::ostream& o, const fg::Node& n);
	protected:
		Mat4 mRelativeTransform;
		Mat4 mCompoundTransform; // Computed by running through the dependency graph and premultiplying for all childof relationships

		bool mHasCompoundTransformBeenApplied;
		int mGraphIndex; // index into the dependency graph
	};
}

#endif
