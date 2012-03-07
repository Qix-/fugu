/**
 * \file
 * \brief Exports a fugu frame to an .obj
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
 *   Copyright (c) 2012 Centre for Electronic Media Art (CEMA)
 *   Monash University, Australia. All rights reserved.
 *
 *   Use of this software is governed by the terms outlined in
 *   the LICENSE file.
 *
 * --------------------------------------------------------------
 * \endverbatim
 * \endcond
 */


#include "exporter.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

#include <QObject>

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

bool Exporter::run(int frameNo, int maxFrames){
	assert(mType==OBJ);
	mErrorString = QString("Unknown Error");

	if (mUniverse){
		int nodeCount = 0;

		// make directory if it doesnt exist
		if (not mDirectory.exists()){
			mDirectory.mkpath(".");
		}

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
			oss << "_" << nodeCount;
			int maxFrameDigits = QString::number(maxFrames).length();
			oss << "_" << std::setfill('0') << std::setw(maxFrameDigits) << frameNo << ".obj";

			QString absolutePath = mDirectory.absoluteFilePath(QString::fromStdString(oss.str()));

			// std::cout << "Saving as: \"" << absolutePath.toStdString() << "\"\n";

			int err = vcg::tri::io::ExporterOBJ_Point3d<fg::MeshImpl>::Save(
				*m->mesh()->_impl(),
				absolutePath.toStdString().c_str(),
				vcg::tri::io::Mask::IOM_VERTNORMAL |
				vcg::tri::io::Mask::IOM_VERTTEXCOORD
				/* | vcg::tri::io::Mask::IOM_VERTCOLOR */ // .obj doesnt support vertex colours apparently..
				,
				m->getCompoundTransform()
			);
			if (err>0){
				mErrorString = QString(vcg::tri::io::ExporterOBJ_Point3d<fg::MeshImpl>::ErrorMsg(err));
				return false;
			}


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
