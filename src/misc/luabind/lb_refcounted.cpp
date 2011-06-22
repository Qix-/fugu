/*
 * Problem 1:
 * 	Lua keeps a variable referencing an object
 * 	The location (in memory) of the object may change
 * 	Therefore, we need to wrap (a pointer to) the object in a proxy
 * 	And make sure that when the object shifts memory, we update the reference internal to the proxy
 *
 * Problem 2:
 *  We have many of these, so need to maintain a list or something
 *  and make sure we delete any proxies which have been garbage collected on Lua's side
 *
 * Problem 3:
 *  Sometimes objects may be deleted by a competing script.
 *  Need to flag the object as deleted, so other scripts are aware.
 *
 * Problem 4:
 *  We also need container implementations for lua who have a "modified" bits, so
 *  e.g., if I keep an EdgeLoop container and an edge is deleted, I can easily detect
 *  the problem and adjust the container appropriately
 *
 * Q: In VCG, do vertices, edges, and faces all change location in memory??
 *
 * BP22062011
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

/* Some object which may move around in memory. */
class Foo {
public:
	int ox;
	int x;
};

std::ostream& operator<<(std::ostream& o, const Foo& f){
	return o << "Foo (" << f.ox << "," << f.x << ") @ " << &f;
}

/**
 * Set this to true to check if the proxy is valid whenever code
 * calls either pImpl or constImpl. Meant for debugging memory leaks.
 *
 * Default: false
 */
bool PROXY_DEBUG_CHECK_IS_VALID = false;

/**
 * The ostream to push debugging information down.
 *
 * Default: std::cerr;
 */
std::ostream& PROXY_DEBUG_OSTREAM = std::cerr;


/**
 * Proxy provides a simple wrapper around a pointer to an object of type T.
 *
 */
template <class T>
class Proxy {
public:
	Proxy(T* t):mImpl(t){}

	/**
	 * @return Reference to the internal pointer
	 */
	T*& pImpl() {
		if (PROXY_DEBUG_CHECK_IS_VALID){
			if (mImpl==NULL){
				PROXY_DEBUG_OSTREAM << "<Warning> Accessing invalid proxy! (Proxy@" << this << ")";
			}
		}
		return mImpl;
	}

	/**
	 * @return Const reference to the internal object. Assumes it is valid.
	 */
	const T& constImpl() const {
		if (PROXY_DEBUG_CHECK_IS_VALID){
			if (mImpl==NULL){
				PROXY_DEBUG_OSTREAM << "<Warning> Accessing invalid proxy! (Proxy@" << this << ")";
			}
		}
		return *mImpl;
	}

	/**
	 * @return True if the pointer is valid (non-null)
	 */
	bool isValid() const {return mImpl!=NULL;}

	/**
	 * Invalidate the internal pointer. (E.g., if the pointed to object gets deleted)
	 */
	void invalidate(){mImpl = NULL;}
private:
	T* mImpl;
};

// Foo proxy
class FooProxy: public Proxy<Foo> {
public:
	FooProxy(Foo* f):Proxy(f){}
	int getX() const {return constImpl().x;}
	void setX(int x) {pImpl()->x = x;}
	int getOX() const {return constImpl().ox;}
};

std::ostream& operator<<(std::ostream& o, const FooProxy& f){
	return o << "FooProxy (" << f.getOX() << "," << f.getX() << ") @ " << &f << " (with Foo@" << &f.constImpl() << ")";
}

class FooContainer {
public:
	Foo* newFoo(int d){Foo* f = new Foo(); f->ox = d; f->x = d; mFoos.push_back(f); return f;}

	Foo* select(int i){
		return *mFoos.begin();
	}

	FooProxy selectP(int i){
		return FooProxy(select(i));
	}

	boost::shared_ptr<FooProxy> selectP2(int i){
		// maintain a list of all FooProxies...
		FooProxy* fp = new FooProxy(select(i));
		boost::shared_ptr<FooProxy> sp(fp);
		mFooProxies.push_back(sp);
		return sp;
	}

	void shuffle() // move foos around in memory
	{
		BOOST_FOREACH(Foo*& f, mFoos){
			Foo* newFoo = new Foo(*f);
			std::cout << "(" << f->ox << "," << f->x << ") [" << f << "] -> [" << newFoo << "]\n";
			delete f;
			f = newFoo;
		}
	}

	void shuffleAndUpdateProxies() // move foos around in memory and update all proxies
	{
		updateProxyRefs();

		BOOST_FOREACH(Foo*& f, mFoos){
			Foo* oldFoo = f;
			Foo* newFoo = new Foo(*f);
			std::cout << "(" << f->ox << "," << f->x << ") [" << f << "] -> [" << newFoo << "]\n";

			// update all foo proxies..
			BOOST_FOREACH(boost::shared_ptr<FooProxy>& sp, mFooProxies){
				if (sp->pImpl()==oldFoo){
					sp->pImpl() = newFoo;
				}
			}

			delete f;
			f = newFoo;
		}
	}

	// Remove proxies who only have one user (this object)
	void updateProxyRefs(){
		int count = 0;

		// Delete any proxy refs
		std::list<boost::shared_ptr<FooProxy> >::iterator it = mFooProxies.begin();
		while(it!=mFooProxies.end()){
			std::cout << "usecount(" << *it << ") = " << it->use_count() << "\n";
			if (it->use_count()<=1){
				count++;
				it = mFooProxies.erase(it);
			}
			else
				it++;
		}

		if (count > 0)
			std::cout << "FooContainer: " << count << " proxies cleaned up.\n";
	}

	void doSomethingThatDeletesSomeFoos(){
		// Delete a foo from container
		Foo* f = *mFoos.begin();
		mFoos.erase(mFoos.begin());

		// Before we delete the foo, we have to invalidate the proxies
		// (and containers) which reference it
		BOOST_FOREACH(boost::shared_ptr<FooProxy>& sp, mFooProxies){
			if (sp->pImpl()==f) sp->invalidate();
		}

		delete f;
	}

private:
	std::list<Foo*> mFoos;

	std::list<boost::shared_ptr<FooProxy> > mFooProxies;
};

FooContainer* gFooContainer;

void shuffle(){gFooContainer->shuffle();}
void shuffleAndUpdateProxies(){gFooContainer->shuffleAndUpdateProxies();}
void deleteSomeFoos(){gFooContainer->doSomethingThatDeletesSomeFoos();}

void setDebugProxy(bool f){PROXY_DEBUG_CHECK_IS_VALID = f;}

Foo* select1(int i){return gFooContainer->select(i);}
boost::shared_ptr<Foo> select2(int i){return boost::shared_ptr<Foo>(gFooContainer->select(i));}
FooProxy select3(int i){return gFooContainer->selectP(i);}
boost::shared_ptr<FooProxy> selectP2(int i){return gFooContainer->selectP2(i);}

int doBindings(lua_State* L){
	using namespace luabind;
	open(L);

	module(L,"BP")[
	    class_<Foo>("Foo")
	    .def_readwrite("x", &Foo::x)
	    .def(tostring(const_self)),

	    class_<Proxy<Foo> >("ProxyFoo")
	    .property("valid",&Proxy<Foo>::isValid),

	    class_<FooProxy, Proxy<Foo> >("FooProxy")
	    .property("x",&FooProxy::getX,&FooProxy::setX)
	    .def(tostring(const_self)),

	    def("shuffle", &shuffle),
	    def("shuffleAndUpdateProxies", &shuffleAndUpdateProxies),
	    def("deleteSomeFoos", &deleteSomeFoos),
	    def("setDebugProxy", &setDebugProxy),

	    def("select1", &select1),
	    def("select2", &select2),
	    def("select3", &select3),
	    def("selectP2", &selectP2)
	];
	return 0;
}

void doTest(lua_State* L, const char* testName, const char* src);

int main(){
	lua_State* L = lua_open();   /* opens Lua */
	luaL_openlibs(L); /* opens the base libraries */
	doBindings(L);

	gFooContainer = new FooContainer();
	for(int i=0;i<4;i++)
		gFooContainer->newFoo(i);

	// These should cause memory access errors/crashes at x.x = 3;
	// doTest(L, "select1", "local x = BP.select1(0); print(x); BP.shuffle(); x.x = 666; print(x);");
	// doTest(L, "select2", "local x = BP.select2(0); print(x); BP.shuffle(); x.x = 666; print(x);");
	// doTest(L, "select3", "local x = BP.select3(0); print(x); BP.shuffle(); x.x = 666; print(x);");

	// This solution will work :)
	for (int i=0;i<2;i++){
		doTest(L, "selectP2",
				"do local x = BP.selectP2(0);\n"
				"print(x);\n"
				"BP.shuffleAndUpdateProxies();\n"
				"x.x = 666;\n" // x still points to the proper element
				"print(x);\n"
				"BP.deleteSomeFoos();\n" // x may now be deleted
				"if (x.valid) then print(\"x is still valid, x.x=\", x.x);\n"
				"else print(\"x is not valid anymore\");\n end;\n"
				"print(\"Enabling proxy debugger\");\n"
				"BP.setDebugProxy(true);"
				"print(\"Accessing x\");\n"
				"x.x = 777;\n"
				"end;\n");
		// manually call garbage collector, to force deletion of refs
		lua_gc(L,LUA_GCCOLLECT,0);
	}

	//




	lua_close(L);
	return 0;
}

void doTest(lua_State* L, const char* testName, const char* src){
	std::cout << "Test: " << testName << "\n";
	// std::cout << "\"" << src << "\"\n";
	if (luaL_dostring(L, src)){
		std::cerr << "Error: " << lua_tostring(L, -1);
		lua_pop(L,1);
	}
}
