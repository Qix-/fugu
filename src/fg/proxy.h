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
	 * Proxy is used to provide safe access to a potentially volatile object across the C++/Lua boundary. See e.g., fg::VertexProxy .
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
}

#endif
