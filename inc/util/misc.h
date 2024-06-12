#pragma once

#include <util/libs.h>

template<typename IDENTIFIER, typename VALUE>
bool inMap(const std::unordered_map<IDENTIFIER, VALUE>& map, const IDENTIFIER& key)
{
	return map.find(key) != map.end();
}

template<typename MAP, typename FUNCTION>
void callFuncOnMap(MAP& map, const FUNCTION& func)
{
	for (auto& item : map)
	{
		func(item.first, item.second);
	}
}