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
#include <luabind/adopt_policy.hpp>

#include <boost/foreach.hpp>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lambda/lambda.hpp>

class A {
public:
	double x;
};

// listOfA helpers
typedef std::list<A*> ListOfA;
ListOfA& ListOfA_all(ListOfA* la){
	return *la;
}

class B {
public:
	std::vector<A> list;

	// bah this doesn't work in lua, but works in c++, why?
	std::list<A*> getList(){
		std::list<A*> al;
		BOOST_FOREACH(A& a, list){
			al.push_back(&a);
		}
		return al;
	}

	// this works..
	std::list<A*>& getListR(){
		static std::list<A*> l;
		l.clear();
		BOOST_FOREACH(A& a, list){
			l.push_back(&a);
		}
		return l;
	}

	// This works finally!
	ListOfA* getNewList(){
		ListOfA* al = new ListOfA();
		BOOST_FOREACH(A& a, list){
			al->push_back(&a);
		}
		return al;
	}

	// A cleaner way?
	boost::shared_ptr<ListOfA> getNewSPList(){
		boost::shared_ptr<ListOfA> al(new ListOfA());
		BOOST_FOREACH(A& a, list){
			al->push_back(&a);
		}
		return al;
	}

	// how about this then... NO
	std::list<boost::reference_wrapper<A> > getListB(){
		std::list<boost::reference_wrapper<A> > l;
		BOOST_FOREACH(A& a, list){
			l.push_back(boost::ref(a));
		}
		return l;
	}

	// non iterator access? YES
	int size(){return list.size();}
	A& get(int i){
		return list[i];
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
	    class_<ListOfA>("ListOfA")
	    .property("all", &ListOfA_all, return_stl_iterator),
	    // Can probably do it inline using boost lambda, but I can't figure out how...
	    // .property("all", boost::lambda::ret<ListOfA&>(*boost::lambda::_1), return_stl_iterator), // return self

	    class_<A>("A").def_readwrite("x", &A::x),

	    class_<B>("B")
	    .def_readwrite("list", &B::list, return_stl_iterator)
	    .def("getList", &B::getList, return_stl_iterator)
	    .def("getListR", &B::getListR, return_stl_iterator)
	    .def("getNewList", &B::getNewList, adopt(result)) // NB: Adopt because its a new list
	    .def("getNewSPList", &B::getNewSPList)
	    .def("getListB", &B::getListB, return_stl_iterator)
	    .def("get", &B::get)
	    .def("size", &B::size),

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

void doTest(lua_State* L, const char* testName, const char* src);

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

	std::cout << "BOOST_FOREACH(A& a, b.list)\n";
	BOOST_FOREACH(A& a, b.list){
		a.x += 1;
	}
	printB(b);

	std::cout << "BOOST_FOREACH(A& a, b.getListB)\n";
	BOOST_FOREACH(A& a, b.getListB()){
		a.x += 1;
	}
	printB(b);

	std::cout << "BOOST_FOREACH(A* a, b.getList)\n";
	BOOST_FOREACH(A* a, b.getList()){
		a->x += 1;
	}
	printB(b);

	std::cout << "for(int i=0;i<b.list.size();i++)\n";
	for(int i=0;i<b.list.size();i++){
		A& a = b.get(i);
		a.x += 1;
	}
	printB(b);

	std::cout << "Lua stuffs\n";

	// Add "b" to current lua context
	luabind::globals(L)["b"] = &b;

	//luaL_doString(L, "-- some code here");
	doTest(L, "iterator access", "for a in b.list do a.x = 6; end");
	printB(b);

	doTest(L, "index based", "for i=0,(b:size()-1) do b:get(i).x=10-i; end");
	printB(b);

	doTest(L, "static list", "for a in b:getListR() do a.x = 6; end");
	printB(b);

	doTest(L, "list copy", "for a in b:getList() do print(a.x); a.x = 7; end");
	printB(b);

	doTest(L, "new list", "v = b:getNewList(); for a in v.all do a.x = 7.5; end");
	printB(b);

	doTest(L, "new sp list", "v = b:getNewSPList(); for a in v.all do a.x = 7.6; end");
	printB(b);

	doTest(L, "boost::ref", "for a in b:getListB() do a.x = 8; end");
	printB(b);

	/*
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

	std::cout << "Lua stuffs\n";

	// Add "b" to current lua context
	luabind::globals(L)["c"] = &c;

	//luaL_doString(L, "-- some code here");
	luaL_dostring(L, "for a in c.list do a.x = 6; end");

	printC(c);
	 */

	lua_close(L);
	return 0;
}

void doTest(lua_State* L, const char* testName, const char* src){
	std::cout << "Test: " << testName << "\n";
	std::cout << "\"" << src << "\"\n";
	if (luaL_dostring(L, src)){
		std::cerr << "Error: " << lua_tostring(L, -1);
		lua_pop(L,1);
		// exit(1);
	}
}
