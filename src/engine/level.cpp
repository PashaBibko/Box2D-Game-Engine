#include <engine/level.h>

#include <engine/entity.h>

static void parseGraphicEntitiesJSON(const nl::json& levelJson, LevelDef& def)
{
	// Checks that graphicEntities is an array
	if (!levelJson["graphicEntities"].is_array())
		throw std::runtime_error("Error: graphicEntities is not an array in JSON file");

	// Loops through each graphic entity
	for (const auto& currentDef : levelJson["graphicEntities"])
	{
		// Adds a new GraphicDef to the vector
		def.graphicEntities.push_back(GraphicDef());

		// Gets a reference to the last element in the vector
		GraphicDef& defRef = def.graphicEntities.back();

		// Sets the size and position of the GraphicDef
		defRef.size = currentDef["size"].get<Vec2>();
		defRef.position = currentDef["position"].get<Vec2>();
	}
}

static void parsePhysicalEntitiesJSON(const nl::json& levelJson, LevelDef& def)
{
	// Checks that physicalEntities is an array
	if (!levelJson["physicalEntities"].is_array())
		throw std::runtime_error("Error: physicalEntities is not an array in JSON file");

	// Loops through each physical entity
	for (const auto& currentDef : levelJson["physicalEntities"])
	{
		// Adds a new PhysicalDef to the vector
		def.physicalEntities.push_back(PhysicalDef());

		// Gets a reference to the last element in the vector
		PhysicalDef& defRef = def.physicalEntities.back();

		// Sets the size and position of the PhysicalDef
		defRef.size = currentDef["size"].get<Vec2>();
		defRef.position = currentDef["position"].get<Vec2>();

		// Converts the bodyType from a string to a b2BodyType
		defRef.bodyType = convertFromStr(currentDef["bodyType"].get<std::string>());

		// Checks if the hitboxes array exists
		if (currentDef.contains("hitboxes"))
		{
			// Checks that hitboxes is an array
			if (!currentDef["hitboxes"].is_array())
				throw std::runtime_error("Error: hitboxes is not an array in JSON file");

			// Loops through each hitbox
			for (const auto& hitbox : currentDef["hitboxes"])
			{
				// Checks that the hitbox is an array
				if (!hitbox.is_array())
					throw std::runtime_error("Error: hitbox is not an array in JSON file");

				// Creates a vector of Vec2s
				std::vector<Vec2> vertices;

				// Loops through each vertex in the hitbox
				for (const auto& vertex : hitbox)
					vertices.push_back(vertex.get<Vec2>());

				// Adds the vertices to the PhysicalDef
				defRef.fixtureVertices.push_back(vertices);
			}
		}
	}
}

LevelDef loadLevelDef(const std::string& levelPath)
{
	// Opens the file
	std::ifstream file(levelPath);

	// Checks if the file is open
	if (!file.is_open())
		throw std::runtime_error("Error: could not open file " + levelPath);

	// Loads file into a JSON object
	nl::json levelJson;
	file >> levelJson;

	// Closes the file
	file.close();

	// Creates a new LevelDef
	LevelDef def;

	// Parses the graphic entities if they exist
	if (levelJson.contains("graphicEntities"))
		parseGraphicEntitiesJSON(levelJson, def);

	// Parses the physical entities if they exist
	if (levelJson.contains("physicalEntities"))
		parsePhysicalEntitiesJSON(levelJson, def);

	// Returns the LevelDef
	return def;
}

Level loadLevel(const LevelDef& levelDef)
{
	// Creates a new LevelPtrs
	Level level;

	// Loads the graphic entities
	for (auto& def : levelDef.graphicEntities)
		level.graphicEntities.push_back(GraphicEntity::create(def));

	// Loads the physical entities
	for (auto& def : levelDef.physicalEntities)
		level.physicalEntities.push_back(PhysicalEntity::create(def));

	// Returns the LevelPtrs
	return level;
}

Level loadLevel(const std::string& levelPath)
{
	// Loads the LevelDef from the JSON file
	LevelDef def = loadLevelDef(levelPath);

	// Returns the LevelPtrs
	return loadLevel(def);
}
