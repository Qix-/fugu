
namespace fg {
	template <class T> ProxyList<T>::ProxyList():mProxies(){}
	template <class T> ProxyList<T>::~ProxyList(){}

	template <class T>
	void ProxyList<T>::add(shared_ptr<TProxy> v){
		mProxies.push_back(v);
	}

	template <class T>
	void ProxyList<T>::doGarbageCollection(){
		int count = 0;

		// Delete any proxy refs
		typename std::list<shared_ptr<TProxy> >::iterator it = mProxies.begin();
		while(it!=mProxies.end()){
			std::cout << "usecount(" << *it << ") = " << it->use_count() << "\n";
			if (it->use_count()<=1){
				count++;
				it = mProxies.erase(it);
			}
			else
				it++;
		}

		if (count > 0)
			std::cout << "ProxyList: " << count << " proxies cleaned up\n";
	}

	template <class T>
	std::vector<T**> ProxyList<T>::getUpdateList()
	{
		std::vector<T**> vpl;
		BOOST_FOREACH(shared_ptr<TProxy>& pvp, mProxies){
			vpl.push_back(&pvp->pImpl());
		}
		return vpl;
	}
}
