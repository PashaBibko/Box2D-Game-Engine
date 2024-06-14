#pragma once

#include <util/libs.h>

// This is where I put all the functions that I cant be bothered to put in a proper file

/*
* @brief Checks if a key exists in a map
* 
* @param map The container to iterate over
* @param key The key to search for
*/
template<typename IDENTIFIER, typename VALUE>
bool inMap(const std::unordered_map<IDENTIFIER, VALUE>& map, const IDENTIFIER& key)
{
	return map.find(key) != map.end();
}

/*
* @brief Calls a function on each item in a map
* 
* @param map The container to iterate over
* @param func The function to call on each item
*/
template<typename MAP, typename FUNCTION>
void callFuncOnMap(MAP& map, const FUNCTION& func)
{
	for (auto& item : map)
	{
		func(item.first, item.second);
	}
}