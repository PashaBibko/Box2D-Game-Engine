#include <engine/entity.h>

#include <engine/base.h>

#include <util/libs.h>
#include <util/vec2.h>

std::vector<std::shared_ptr<Entity>> Entity::instances;

// ----- Entity Member Functions ----- //

void Entity::render() {}
void Entity::update() {}

Vec2 Entity::getSize()
{
	return size;
}

Vec2 Entity::getPosition()
{
	return position;
}

void Entity::setSize(Vec2 size)
{
	size = size;
}

void Entity::setPosition(Vec2 position)
{
	position = position;
}