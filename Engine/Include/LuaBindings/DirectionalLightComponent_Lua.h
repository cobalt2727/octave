#pragma once

#include "EngineTypes.h"
#include "Log.h"
#include "Engine.h"

#include "Components/DirectionalLightComponent.h"

#include "LuaBindings/Component_Lua.h"
#include "LuaBindings/LuaUtils.h"

#if LUA_ENABLED

#define DIRECTIONAL_LIGHT_COMPONENT_LUA_NAME "DirectionalLightComponent"
#define DIRECTIONAL_LIGHT_COMPONENT_LUA_FLAG "cfDirectionalLightComponent"
#define CHECK_DIRECTIONAL_LIGHT_COMPONENT(L, arg) static_cast<DirectionalLightComponent*>(CheckComponentLuaType(L, arg, DIRECTIONAL_LIGHT_COMPONENT_LUA_NAME, DIRECTIONAL_LIGHT_COMPONENT_LUA_FLAG));

struct DirectionalLightComponent_Lua
{
    static int GetDirection(lua_State* L);
    static int SetDirection(lua_State* L);

    static void Bind();
};

#endif
