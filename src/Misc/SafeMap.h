#pragma once

#include <map>
#include <cassert>
#include <iostream>

namespace CalibFW
{

template<  class TKey, class TMapType >
class SafeMap{
public:

	static TMapType const& Get ( TKey k, std::map< TKey, TMapType> const& m )
	{
		if ( m.find( k ) == m.end())
		{
			std::cout << "Item " << k << " can not be found in dictionary " << std::endl;
			assert(false);
		}
		return m.at(k);
	}
};

}
