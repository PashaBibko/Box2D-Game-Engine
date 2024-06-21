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
struct LevelPtrs
{
	std::vector<GraphicEntity*> graphicEntities;
	std::vector<PhysicalEntity*> physicalEntities;
};


// Future Pasha I am really sorry you are going to have to tidy up this function
// I am really sorry
// I am really sorry
// I am really sorry

LevelDef loadLevelDef(const std::string& levelPath)
{
	std::ifstream file(levelPath);

	if (!file.is_open())
	{
		std::cerr << "Error: Could not open file " << levelPath << std::endl;
		return LevelDef();
	}

	try
	{
		nl::json levelJson;
		file >> levelJson;

		LevelDef def;

		// Parse the graphic entities here

		if (levelJson.contains("graphicEntities"))
		{
			if (!levelJson["graphicEntities"].is_array())
			{
				std::cerr << "Error: graphicEntities is not an array in JSON file " << levelPath << std::endl;
				return LevelDef();
			}

			for (const auto& currentDef : levelJson["graphicEntities"])
			{
				def.graphicEntities.push_back(GraphicDef());

				GraphicDef& defRef = def.graphicEntities.back();

				defRef.size = currentDef["size"].get<Vec2>();
				defRef.position = currentDef["position"].get<Vec2>();
			}
		}

		// Parse the physical entities here

		if (levelJson.contains("physicalEntities"))
		{
			if (!levelJson["physicalEntities"].is_array())
			{
				std::cerr << "Error: physicalEntities is not an array in JSON file " << levelPath << std::endl;
				return LevelDef();
			}

			for (const auto& currentDef : levelJson["physicalEntities"])
			{
				def.physicalEntities.push_back(PhysicalDef());

				PhysicalDef& defRef = def.physicalEntities.back();

				defRef.size = currentDef["size"].get<Vec2>();
				defRef.position = currentDef["position"].get<Vec2>();

				defRef.bodyType = convertFromStr(currentDef["bodyType"].get<std::string>());

				if (currentDef.contains("hitboxes"))
				{
					if (!currentDef["hitboxes"].is_array())
					{
						std::cerr << "Error: hitboxes is not an array in JSON file " << levelPath << std::endl;
						return LevelDef();
					}

					for (const auto& hitbox : currentDef["hitboxes"])
					{
						if (!hitbox.is_array())
						{
							std::cerr << "Error: hitbox is not an array in JSON file " << levelPath << std::endl;
							return LevelDef();
						}

						std::vector<Vec2> vertices;

						for (const auto& vertex : hitbox)
							vertices.push_back(vertex.get<Vec2>());

						defRef.fixtureVertices.push_back(vertices);
					}
				}

				else
				{
					std::cerr << "Error: physicalEntity must contain at least one hitbox in JSON file: " << levelPath << std::endl;
					return LevelDef();
				}
			}
		}

		return def;
	}

	catch (nl::json::parse_error& e)
	{
		std::cerr << "Error: Could not parse JSON file " << levelPath << std::endl;
		std::cerr << e.what() << std::endl;
		return LevelDef();
	}
}

LevelPtrs loadLevel(const LevelDef& levelDef)
{
	LevelPtrs out;

	// Loads the graphic entities
	for (auto& def : levelDef.graphicEntities)
		out.graphicEntities.push_back(GraphicEntity::create(def));

	// Loads the physical entities
	for (auto& def : levelDef.physicalEntities)
		out.physicalEntities.push_back(PhysicalEntity::create(def));

	return out;
}

LevelPtrs loadLevel(const std::string& levelPath)
{
	LevelDef def = loadLevelDef(levelPath);

	return loadLevel(def);
}