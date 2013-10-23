#pragma once

#include <map>
#include <cassert>
#include <iostream>

#include <boost/ptr_container/ptr_map.hpp>

namespace CalibFW
{

template<class TKey, class TMapType>
class SafeMap{
public:

	static TMapType const& GetPtrMap(TKey const& k, boost::ptr_map< TKey, TMapType> const& m)
	{
		typename boost::ptr_map<TKey, TMapType>::const_iterator it = m.find(k);
		if (it == m.end())
			CALIB_LOG_FATAL("Item " << k << " can not be found in dictionary.")
		return *it->second;
	}

	static TMapType const& Get ( TKey k, std::map<TKey, TMapType> const& m)
	{
		if (m.find(k) == m.end())
			CALIB_LOG_FATAL("Item " << k << " can not be found in dictionary.")
		return m.at(k);
	}

};

}
