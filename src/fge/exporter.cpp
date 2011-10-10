#include "exporter.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

#include "fg/fg.h"
#include "fg/functions.h"
#include "fg/glrenderer.h"
#include "fg/util.h"
#include "fg/mesh.h"
#include "fg/meshimpl.h"
#include "fg/exportmeshnode.h"

Exporter::Exporter(fg::Universe* u):mUniverse(u),mDirectory(){}

Exporter Exporter::ExportFrameToObj(fg::Universe* u){
	Exporter ex(u);
	ex.mType = OBJ;
	return ex;
}

bool Exporter::run(){
	assert(mType==OBJ);

	if (mUniverse){
		int nodeCount = 0;

		// get time
		time_t seconds;
		seconds = time(NULL);
		struct tm* timeinfo;
		timeinfo = localtime(&seconds);

		foreach(boost::shared_ptr<fg::MeshNode> m, mUniverse->meshNodes()){
			// m->mesh()->sync(); // make sure normals are okay
			std::ostringstream oss;
			oss << "obj_";
			oss << std::setw(2) << std::setfill('0')
				<< timeinfo->tm_hour
				<< timeinfo->tm_min
				<< timeinfo->tm_sec;
			oss << "_" << nodeCount << ".obj";

			QString absolutePath = mDirectory.absoluteFilePath(QString::fromStdString(oss.str()));

			std::cout << "Saving as: \"" << absolutePath.toStdString() << "\"\n";
			vcg::tri::io::ExporterOBJ_Point3d<fg::MeshImpl>::Save(
				*m->mesh()->_impl(),
				absolutePath.toStdString().c_str(),
				vcg::tri::io::Mask::IOM_VERTNORMAL |
				vcg::tri::io::Mask::IOM_VERTTEXCOORD
				/* | vcg::tri::io::Mask::IOM_VERTCOLOR */ // .obj doesnt support vertex colours apparently..
				,
				m->getCompoundTransform()
			);
			nodeCount++;
		}

		return true;
	}
	else {
		return false;
	}
}

void outputObjCb(void* clientData){ // output an obj of the current frame..

}
