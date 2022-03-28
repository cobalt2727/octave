#pragma once

#include "EngineTypes.h"
#include "Log.h"
#include "Engine.h"

#include "Components/PrimitiveComponent.h"

#include "LuaBindings/Component_Lua.h"
#include "LuaBindings/LuaUtils.h"

#if LUA_ENABLED

#define PRIMITIVE_COMPONENT_LUA_NAME "PrimitiveComponent"
#define PRIMITIVE_COMPONENT_LUA_FLAG "cfPrimitiveComponent"
#define CHECK_PRIMITIVE_COMPONENT(L, arg) static_cast<PrimitiveComponent*>(CheckComponentLuaType(L, arg, PRIMITIVE_COMPONENT_LUA_NAME, PRIMITIVE_COMPONENT_LUA_FLAG));

struct PrimitiveComponent_Lua
{
    static int EnablePhysics(lua_State* L);
    static int EnableCollision(lua_State* L);
    static int EnableOverlaps(lua_State* L);
    static int IsPhysicsEnabled(lua_State* L);
    static int IsCollisionEnabled(lua_State* L);
    static int AreOverlapsEnabled(lua_State* L);

    static int GetMass(lua_State* L);
    static int GetLinearDamping(lua_State* L);
    static int GetAngularDamping(lua_State* L);
    static int GetRestitution(lua_State* L);
    static int GetFriction(lua_State* L);
    static int GetRollingFriction(lua_State* L);
    static int GetLinearFactor(lua_State* L);
    static int GetAngularFactor(lua_State* L);
    static int GetCollisionGroup(lua_State* L);
    static int GetCollisionMask(lua_State* L);

    static int SetMass(lua_State* L);
    static int SetLinearDamping(lua_State* L);
    static int SetAngularDamping(lua_State* L);
    static int SetRestitution(lua_State* L);
    static int SetFriction(lua_State* L);
    static int SetRollingFriction(lua_State* L);
    static int SetLinearFactor(lua_State* L);
    static int SetAngularFactor(lua_State* L);
    static int SetCollisionGroup(lua_State* L);
    static int SetCollisionMask(lua_State* L);

    static int GetLinearVelocity(lua_State* L);
    static int GetAngularVelocity(lua_State* L);

    static int AddLinearVelocity(lua_State* L);
    static int AddAngularVelocity(lua_State* L);
    static int SetLinearVelocity(lua_State* L);
    static int SetAngularVelocity(lua_State* L);
    static int AddForce(lua_State* L);
    static int AddImpulse(lua_State* L);
    static int ClearForces(lua_State* L);

    static int EnableCastShadows(lua_State* L);
    static int ShouldCastShadows(lua_State* L);

    static int EnableReceiveShadows(lua_State* L);
    static int ShouldReceiveShadows(lua_State* L);

    static int EnableReceiveSimpleShadows(lua_State* L);
    static int ShouldReceiveSimpleShadows(lua_State* L);

    static int SweepToWorldPosition(lua_State* L);

    static void Bind();
};

#endif
