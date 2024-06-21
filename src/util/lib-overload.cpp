#include <util/lib-overload.h>

#include <util/libs.h>
#include <util/vec2.h>

void to_json(nl::json& j, const Vec2& v)
{
	j = nl::json{ { "x", v.x }, { "y", v.y } };
}

void from_json(const nl::json& j, Vec2& v)
{
	j.at("x").get_to(v.x);
	j.at("y").get_to(v.y);
}