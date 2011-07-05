#ifndef FG_NODE_H
#define FG_NODE_H

#include "fg/mat4.h"

namespace fg {
	/**
	 * A Node is a primitive (primarily spatial) entity in fg.
	 *
	 * Nodes can be linked together in hierarchical and other (spatial) relationships.
	 */
	class Node {
	public:
		Node();
		virtual ~Node();

		Mat4& getTransform();
	private:
		Mat4 mTransform;
	};
}

#endif
