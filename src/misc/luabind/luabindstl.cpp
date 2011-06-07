/*
 * Testing the luabind stl stuffs
 */

#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <cstdarg>
#include <vector>
#include <list>

#include <lua.hpp>
#include <lauxlib.h>
#include <lualib.h>

#include <luabind/luabind.hpp>
#include <luabind/class.hpp>
#include <luabind/function.hpp>
#include <luabind/object.hpp>
#include <luabind/operator.hpp>
#include <luabind/iterator_policy.hpp>

#include <boost/foreach.hpp>

class A {
public:
	double x;
};

class B {
public:
	std::vector<A> list;

	// bah this crashes
	std::list<A*> getList(){
		std::list<A*> al;
		BOOST_FOREACH(A& a, list){
			al.push_back(&a);
		}
		return al;
	}

	/* TODO....
	// how about a list wrapper thingo
	class ListWrapper {
	public:
		class Iterator: public std::vector<A>::Iterator {
		public:
			virtual
		}
	};
	*/

};

class C {
public:
	std::vector<A*> list;
};

int doBindings(lua_State* L){
	using namespace luabind;
	open(L);
	module(L)[
	    class_<A>("A").def_readwrite("x", &A::x),

	    class_<B>("B")
	    .def_readwrite("list", &B::list, return_stl_iterator)
	    .def("getList", &B::getList, return_stl_iterator),

	    class_<C>("C").def_readwrite("list", &C::list, return_stl_iterator)
	];
	return 0;
}

void printB(B& b){
	std::cout << "[";
	BOOST_FOREACH(A& a, b.list){
		std::cout << a.x << " ";
	}
	std::cout << "]\n";
}
void printC(C& c){
	std::cout << "[";
	BOOST_FOREACH(A* a, c.list){
		std::cout << a->x << " ";
	}
	std::cout << "]\n";
}

int main(){
	lua_State* L = lua_open();   /* opens Lua */
	luaL_openlibs(L); /* opens the base libraries */
	doBindings(L);

	// Test B
	std::cout << "Test B\n";
	B b;
	for(int i=0;i<10;i++){
		A a;
		a.x = i;
		b.list.push_back(a);
	}
	printB(b);

	BOOST_FOREACH(A& a, b.list){
		a.x += 1;
	}
	printB(b);

	BOOST_FOREACH(A* a, b.getList()){
		a->x += 1;
	}
	printB(b);

	// Add "b" to current lua context
	luabind::globals(L)["b"] = &b;

	//luaL_doString(L, "-- some code here");
	luaL_dostring(L, "for a in b.list do a.x = 6; end");

	printB(b);

	//luaL_doString(L, "-- some code here");
	luaL_dostring(L, "for a in b:getList() do a.x = 6; end");

	printB(b);

	// Test C
	std::cout << "Test C\n";

	C c;
	for(int i=0;i<10;i++){
		A* a = new A();
		a->x = i;
		c.list.push_back(a);
	}
	printC(c);

	BOOST_FOREACH(A* a, c.list){
		a->x += 1;
	}
	printC(c);

	// Add "b" to current lua context
	luabind::globals(L)["c"] = &c;

	//luaL_doString(L, "-- some code here");
	luaL_dostring(L, "for a in c.list do a.x = 6; end");

	printC(c);

	lua_close(L);
	return 0;
}
