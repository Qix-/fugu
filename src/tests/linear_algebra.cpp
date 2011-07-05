/**
 * Tests the linear algebra routines.
 *
 * @author BP
 */

#include <iostream>

#include <boost/test/minimal.hpp>

#include "fg/mat4.h"
#include "fg/functions.h"

bool approx(const fg::Mat4& a, const fg::Mat4& b){
	for(int r=0;r<4;r++){
		for(int c=0;c<4;c++){
			if (!fg::approx(a.get(r,c),b.get(r,c))) return false;
		}
	}
	return true;
}

int test_main(int argc, char* argv[]){
	fg::Mat4 zero = fg::Mat4::Zero();
	for(int r=0;r<4;r++){
		for(int c=0;c<4;c++){
			BOOST_CHECK(fg::approx(zero.get(r,c),0));
		}
	}

	fg::Mat4 id = fg::Mat4::Identity();
	for(int r=0;r<4;r++){
		for(int c=0;c<4;c++){
			if (r==c)
				BOOST_CHECK(fg::approx(id.get(r,c),1));
			else
				BOOST_CHECK(fg::approx(id.get(r,c),0));
		}
	}

	fg::Mat4 a = id + zero;
	BOOST_CHECK(approx(a,id));

	a += id;
	fg::Mat4 twoId = id + id;
	BOOST_CHECK(approx(a,twoId));

	BOOST_CHECK(approx(id*2,twoId));
	BOOST_CHECK(approx(id*id,id));

	fg::Vec3 v(1,2,3);
	BOOST_CHECK(approx(v,v));
	BOOST_CHECK(approx(v,id*v));

	std::cout << "v:" << v << "\n";
	std::cout << "twoId:\n" << twoId << "\n";
	std::cout << "twoId*v:\n" << twoId*v << "\n";
	BOOST_CHECK(!approx(v,twoId*v));

	return 0;
}
