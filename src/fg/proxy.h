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

#ifndef FG_PROXY_H
#define FG_PROXY_H

#include <ostream>
#include <iostream>
#include <vector>
#include <list>

#include <boost/foreach.hpp>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>

#include "fg/util.h"

namespace fg {

	/** \brief Proxy debugging settings 
	 */
	class ProxySettings {
	public:
		/**
		 * Set this to true to check if the proxy is valid whenever code
		 * calls either pImpl or constImpl. Meant for debugging memory leaks.
		 *
		 * Default: false
		 */
		static bool DEBUG_CHECK_IS_VALID;

		/**
		 * The ostream to push debugging information down.
		 *
		 * Default: std::cerr;
		 */
		static std::ostream& DEBUG_OSTREAM;
	};

	/**
	 * \brief Proxy provides a simple wrapper around a pointer to an object of type T.
	 * 
	 * Proxy is used to provide safe access to a potentially volatile object (esp. across the C++/Lua boundary.) See e.g., fg::VertexProxy .
	 */
	template <class T>
	class Proxy {
	public:
		Proxy(T* t):mImpl(t){}

		/**
		 * @return Reference to the internal pointer
		 */
		T*& pImpl() {
			if (ProxySettings::DEBUG_CHECK_IS_VALID){
				if (mImpl==NULL){
					ProxySettings::DEBUG_OSTREAM << "<Warning> Accessing invalid proxy! (Proxy@" << this << ")";
				}
			}
			return mImpl;
		}

		/**
		 * @return Const reference to the internal object. Assumes it is valid.
		 */
		const T& constImpl() const {
			if (ProxySettings::DEBUG_CHECK_IS_VALID){
				if (mImpl==NULL){
					ProxySettings::DEBUG_OSTREAM << "<Warning> Accessing invalid proxy! (Proxy@" << this << ")";
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

	/**
	 * A ProxyList maintains a list of proxies and is responsible
	 * for updating the internal pointers when the data changes location or
	 * notifying proxies if the data is deleted..
	 */
	template <class T>
	class ProxyList {
	public:
		typedef Proxy<T> TProxy;
		ProxyList();
		~ProxyList();

		/**
		 * Add a new proxy to this list. It will be removed when there are no
		 * more references to the proxy (besides this one).
		 */
		void add(shared_ptr<TProxy>);

		/**
		 * Remove any unreferenced proxies from this list.
		 *
		 * XXX: Using shared_ptr::use_count may be slow according to boost docs...
		 */
		void doGarbageCollection();

		/**
		 * Return a list of pointers to pointers to T,
		 * this will be used in the vcg Allocator to update the
		 * internal refs of Vertex and Face Proxies.
		 */
		std::vector<T**> getUpdateList();

	private:
		std::list<shared_ptr<TProxy> > mProxies;
	};
}

#include "fg/proxy.inl"

#endif
