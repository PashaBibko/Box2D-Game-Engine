#pragma once

#include <util/libs.h>
#include <util/vec2.h>

void to_json(nl::json& j, const Vec2& v);

void from_json(const nl::json& j, Vec2& v);