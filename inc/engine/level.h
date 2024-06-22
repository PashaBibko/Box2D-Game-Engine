#pragma once

#include <engine/entity.h>

/*
* @brief Defintion class for Level
*/
struct LevelDef
{
	std::vector<GraphicDef> graphicEntities;
	std::vector<PhysicalDef> physicalEntities;
};

/*
* @brief Struct for the pointers of the level entities (used for loading levels)
*/
struct Level
{
	std::vector<GraphicEntity*> graphicEntities;
	std::vector<PhysicalEntity*> physicalEntities;
};

/*
* @brief Loads a LevelDef from a JSON file
* 
* @param levelPath The path to the JSON file
* 
* @return The LevelDef of the file
*/
LevelDef loadLevelDef(const std::string& levelPath);

/*
* @brief Loads a Level from a LevelDef
* 
* @param levelDef The LevelDef to load from
* 
* @return Pointers to all the entities in the level
*/
Level loadLevel(const LevelDef& levelDef);

/*
* @brief Loads a Level from a JSON file
* 
* @param levelPath The path to the JSON file
* 
* @return Pointers to all the entities in the level
*/
Level loadLevel(const std::string& levelPath);

/*
* @brief Saves a LevelDef to a JSON file
* 
* @param levelDef The LevelDef to save
* @param levelPath The path to the JSON file
*/
void saveToJson(Level& levelDef, const std::string& levelPath);