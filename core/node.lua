
node = fg.node
meshnode = fg.meshnode

document[[A node is a positionable and orientable object. 
Every object in a universe is a node.]](node)
document[[A meshnode is a node that references a mesh. 
When placed in a universe it transforms that mesh. 
Multiple meshnodes can reference the same mesh.]](meshnode)

foreach({node,meshnode}, function(_,f) categorise(f,"universe") end)

