--[[
	Extrusions.
	Demonstrates the use of a self updating vertex proxy (fg::VertexProxy)
--]]

module(...,package.seeall)

require "table"

local m = nil -- the mesh
local time = 0
local tentacle = nil
local sphereVerts = nil -- all verts on the initial sphere

function randomElement(t)
	return t[math.floor(fg.random(1,#t+1))]	
end

function convertToTable(vtxSet)
	local t = {}
	for v in vtxSet.all do
		table.insert(t,v)
	end
	return t
end

function setup()
	m = fg.mesh.primitives.sphere()	
	fgu:addMesh(m)
	
	sphereVerts = convertToTable(m:selectAllVertices())	
	tentacle = new_tentacle(m,randomElement(sphereVerts)) -- m:selectRandomVertex()	 
end

function update(dt)
	time = time + dt
	if (time > 1) then
		if (tentacle~=nil) then
			if (not tentacle:update(dt)) then
				tentacle = nil
				tentacle = new_tentacle(m,randomElement(sphereVerts)) -- m:selectRandomVertex())
			end
		end
		time = 0
	end
	
	--[[	
	if (time > .1) then
		-- extrude the same vertex over and over again		
		fg.extrude(m,v,1)
		time = 0
		ex = ex + 1
		
		if (ex > maxEx) then
			verts = convertToTable(fg.getVerticesAtDistance(m,v,2))
			v = randomElement(verts)
			ex = 0
		end
	end
	--]]
end

--[[
		vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalizedPerFace(*m);

		// after every 3 extrusions, grow a sucker on a random place at a
		// distance of 3 edges from the center

		suckerCounter--;
		if (suckerCounter<=0){
			suckerCounter = 3;

			vcg::tri::Nring<MyMesh>::clearFlags(m); // Probably necessary..
			vcg::tri::Nring<MyMesh> ring(v,m);
			ring.expand(4); // NB: expand(2 or more) will not give the verts in proper sequence, have to fix this...
			MyMesh::VertexType* sv = ring.lastV[(int)_random(0,ring.lastV.size()-1)];
			ring.clear();

			// to figure out the size, just compute the distance to a neighbouring vertex
			double sz = 1;
			for(int i=0;i<3;i++){
				if (sv->VFp()->V(i)!=sv){
					sz = vcg::Norm(sv->VFp()->V(i)->P() - sv->P());
					break;
				}
			}
			growSucker(m,sv,vpul,sz);
		}
	}
--]]

function new_tentacle(m,v)
	-- create initial params
	local obj = {
		mesh = m,
		vertex = v,
		attractor=.4,		
		numSegments= 7,
		time=0,
		startSegLength = .3,		
		numSubSegments = 3,
		segWidth = -0.05,		
	}
	
	-- derived params
	obj.segLengthDiff = -(obj.startSegLength-0.05) * 1./obj.numSegments
	obj.segLength = obj.startSegLength
	
	-- update function
	obj.update = function(self,dt)
		-- ATM just build the whole thing in one step
		-- self.time = self.time + dt
		
		if (not self.vertex.valid) then 
			print("a tentacle's leading vertex has become invalid!")
			return false
		end
		
		-- else, grow a thing
		local n = self.vertex:getN()
		n:normalise()
		
		for k=1,self.numSegments do
			local r = fg.vec3(fg.random(-.3,.3),1,fg.random(-.3,.3))
			r:normalise()					
			
			n = n*(1-self.attractor) + r*(self.attractor)
			n:normalise()
						
			for i=1,self.numSubSegments do
				fg.extrude(self.mesh,self.vertex,2,n,self.segLength/self.numSubSegments,self.segWidth)

			end
			self.segLength = self.segLength + self.segLengthDiff
			
			-- recompute the normals if needed 
			-- m:sync()										
		end		
		return false			
	end	
	
	-- finally return the object
	return obj
end

