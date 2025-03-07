#pragma once

#include "Stream.h"
#include "EngineTypes.h"

#include <string>
#include <glm/glm.hpp>

typedef bool(*DatumChangeHandlerFP)(class Datum* prop, const void* newValue);

class Asset;
class AssetRef;
class TableDatum;
class RTTI;

enum class DatumType : uint8_t
{
    Integer,
    Float,
    Bool,
    String,
    Vector2D,
    Vector,
    Color,
    Asset,
    Enum,
    Byte,
    Table,
    Pointer,

    Count
};

union DatumData
{
    int32_t* i;
    float* f;
    bool* b;
    std::string* s;
    glm::vec2* v2;
    glm::vec3* v3;
    glm::vec4* v4;
    AssetRef* as;
    //ActorRef* ac;
    uint32_t* e;
    uint8_t* by;
    TableDatum* t;
    RTTI** p;
    void* vp;
};

class Datum
{
public:

    Datum();
    Datum(
        DatumType type,
        void* owner,
        void* data,
        uint32_t count = 1u,
        DatumChangeHandlerFP changeHandler = nullptr);
    Datum(const Datum& src);
    virtual ~Datum();

    // Conversion constructors
    Datum(int32_t value);
    Datum(float value);
    Datum(bool value);
    Datum(const char* value);
    Datum(const std::string& value);
    Datum(glm::vec2 value);
    Datum(glm::vec3 value);
    Datum(glm::vec4 value);
    Datum(Asset* value);
    Datum(const AssetRef& value);
    Datum(uint8_t value);
    Datum(RTTI* value);

    // Conversion operators
    operator int32_t() const { return GetInteger(); }
    operator uint32_t() const { return (mType == DatumType::Integer) ? uint32_t(GetInteger()) : GetEnum(); }
    operator float() const { return (mType == DatumType::Integer) ? float(GetInteger()) : GetFloat(); }
    operator bool() const { return GetBool(); }
    operator const char*() const { return GetString().c_str(); }
    operator std::string() const { return GetString(); }
    operator glm::vec2() const { return (mType == DatumType::Color) ? glm::vec2(GetColor()) : GetVector2D(); }
    operator glm::vec3() const { return (mType == DatumType::Color) ? glm::vec3(GetColor()) : GetVector(); }
    operator glm::vec4() const { return GetColor(); }
    operator Asset*() const { return GetAsset(); }
    //operator AssetRef() const; Not sure if needed??
    operator uint8_t() const { return (mType == DatumType::Integer) ? uint8_t(GetInteger()) : GetByte(); }
    operator RTTI*() const { return (mType == DatumType::Asset) ? ((RTTI*)GetAsset()) : GetPointer(); }

    DatumType GetType() const;
    void SetType(DatumType type);

    uint32_t GetCount() const;
    void SetCount(uint32_t count);
    
    uint32_t GetDataTypeSize() const;
    uint32_t GetDataTypeSerializationSize() const;

    bool IsExternal() const;
    bool IsValid() const;

    virtual void ReadStream(Stream& stream, bool external);
    virtual void WriteStream(Stream& stream) const;
    virtual uint32_t GetSerializationSize() const;

    void SetInteger(int32_t value, uint32_t index = 0);
    void SetFloat(float value, uint32_t index = 0);
    void SetBool(bool value, uint32_t index = 0);
    void SetString(const std::string& value, uint32_t index = 0);
    void SetVector2D(const glm::vec2& value, uint32_t index = 0);
    void SetVector(const glm::vec3& value, uint32_t index = 0);
    void SetColor(const glm::vec4& value, uint32_t index = 0);
    void SetAsset(const Asset* value, uint32_t index = 0);
    void SetEnum(uint32_t value, uint32_t index = 0);
    void SetByte(uint8_t value, uint32_t index = 0);
    void SetTableDatum(const TableDatum& value, uint32_t index = 0);
    void SetPointer(RTTI* value, uint32_t index = 0);

    void SetValue(const void* value, uint32_t index = 0, uint32_t count = 1);
    void SetValueRaw(const void* value, uint32_t index = 0);

    void SetExternal(int32_t* data,  uint32_t count = 1);
    void SetExternal(float* data,  uint32_t count = 1);
    void SetExternal(bool* data,  uint32_t count = 1);
    void SetExternal(std::string* data,  uint32_t count = 1);
    void SetExternal(glm::vec2* data,  uint32_t count = 1);
    void SetExternal(glm::vec3* data,  uint32_t count = 1);
    void SetExternal(glm::vec4* data,  uint32_t count = 1);
    void SetExternal(AssetRef* data,  uint32_t count = 1);
    void SetExternal(uint32_t* data,  uint32_t count = 1);
    void SetExternal(uint8_t* data,  uint32_t count = 1);
    void SetExternal(TableDatum* data, uint32_t count = 1);
    void SetExternal(RTTI** data, uint32_t count = 1);

    int32_t GetInteger(uint32_t index = 0) const;
    float GetFloat(uint32_t index = 0) const;
    bool GetBool(uint32_t index = 0) const;
    const std::string& GetString(uint32_t index = 0) const;
    const glm::vec2& GetVector2D(uint32_t index = 0) const;
    const glm::vec3& GetVector(uint32_t index = 0) const;
    const glm::vec4& GetColor(uint32_t index = 0) const;
    Asset* GetAsset(uint32_t index = 0) const;
    uint32_t GetEnum(uint32_t index = 0) const;
    uint8_t GetByte(uint32_t index = 0) const;
    TableDatum& GetTableDatum(uint32_t index = 0);
    const TableDatum& GetTableDatum(uint32_t index = 0) const;
    RTTI* GetPointer(uint32_t index = 0) const;

    void PushBack(int32_t value);
    void PushBack(float value);
    void PushBack(bool value);
    void PushBack(const std::string& value);
    void PushBack(const char* value);
    void PushBack(const glm::vec2& value);
    void PushBack(const glm::vec3& value);
    void PushBack(const glm::vec4& value);
    void PushBack(Asset* value);
    void PushBack(uint32_t value);
    void PushBack(uint8_t value);
    TableDatum* PushBackTableDatum(const TableDatum& value);
    void PushBack(RTTI* value);

    TableDatum* FindTableDatum(const char* key);
    TableDatum* FindTableDatum(int32_t key);

    // Table get/set convenience functions
    int32_t GetIntegerField(const char* key);
    float GetFloatField(const char* key);
    bool GetBoolField(const char* key);
    std::string GetStringField(const char* key);
    glm::vec2 GetVector2DField(const char* key);
    glm::vec3 GetVectorField(const char* key);
    glm::vec4 GetColorField(const char* key);
    Asset* GetAssetField(const char* key);
    RTTI* GetPointerField(const char* key);
    TableDatum& GetTableField(const char* key);

    int32_t GetIntegerField(int32_t key);
    float GetFloatField(int32_t key);
    bool GetBoolField(int32_t key);
    std::string GetStringField(int32_t key);
    glm::vec2 GetVector2DField(int32_t key);
    glm::vec3 GetVectorField(int32_t key);
    glm::vec4 GetColorField(int32_t key);
    Asset* GetAssetField(int32_t key);
    RTTI* GetPointerField(int32_t key);
    TableDatum& GetTableField(int32_t key);

    void SetIntegerField(const char* key, int32_t value);
    void SetFloatField(const char* key, float value);
    void SetBoolField(const char* key, bool value);
    void SetStringField(const char* key, const std::string& value);
    void SetVector2DField(const char* key, glm::vec2 value);
    void SetVectorField(const char* key, glm::vec3 value);
    void SetColorField(const char* key, glm::vec4 value);
    void SetAssetField(const char* key, Asset* value);
    void SetPointerField(const char* key, RTTI* value);
    void SetTableField(const char* key, const TableDatum& value);

    void SetIntegerField(int32_t key, int32_t value);
    void SetFloatField(int32_t key, float value);
    void SetBoolField(int32_t key, bool value);
    void SetStringField(int32_t key, const std::string& value);
    void SetVector2DField(int32_t key, glm::vec2 value);
    void SetVectorField(int32_t key, glm::vec3 value);
    void SetColorField(int32_t key, glm::vec4 value);
    void SetAssetField(int32_t key, Asset* value);
    void SetPointerField(int32_t key, RTTI* value);
    void SetTableField(int32_t key, const TableDatum& value);

    bool HasField(const char* key);
    bool HasField(int32_t key);

    // Assignment
    Datum& operator=(const Datum& src);

    Datum& operator=(int32_t src);
    Datum& operator=(float src);
    Datum& operator=(bool src);
    Datum& operator=(const std::string& src);
    Datum& operator=(const char* src);
    Datum& operator=(const glm::vec2 src);
    Datum& operator=(const glm::vec3& src);
    Datum& operator=(const glm::vec4& src);
    Datum& operator=(Asset* src);
    // No operator= for enum value since it conflicts with int32_t
    Datum& operator=(uint8_t src);
    Datum& operator=(RTTI* srC);

    // Equivalence
    bool operator==(const Datum& other) const;
    
    bool operator==(const int32_t& other) const;
    bool operator==(const float& other) const;
    bool operator==(const bool& other) const;
    bool operator==(const std::string& other) const;
    bool operator==(const char* other) const;
    bool operator==(const glm::vec2& other) const;
    bool operator==(const glm::vec3& other) const;
    bool operator==(const glm::vec4& other) const;
    bool operator==(const Asset*& other) const;
    bool operator==(const uint32_t& other) const;
    bool operator==(const uint8_t& other) const;
    bool operator==(const RTTI*& other) const;

    bool operator!=(const Datum& other) const;

    bool operator!=(const int32_t& other) const;
    bool operator!=(const float& other) const;
    bool operator!=(const bool& other) const;
    bool operator!=(const std::string& other) const;
    bool operator!=(const char* other) const;
    bool operator!=(const glm::vec2& other) const;
    bool operator!=(const glm::vec3& other) const;
    bool operator!=(const glm::vec4& other) const;
    bool operator!=(const Asset*& other) const;
    bool operator!=(const uint32_t& other) const;
    bool operator!=(const uint8_t& other) const;
    bool operator!=(const RTTI*& other) const;

    virtual bool IsProperty() const;
    virtual bool IsTableDatum() const;
    virtual void DeepCopy(const Datum& src, bool forceInternalStorage);

protected:

    void Reserve(uint32_t capacity);
    virtual void Destroy();

    void PreSet(uint32_t index, DatumType type);
    void PreSetExternal(DatumType type);
    void PostSetExternal(DatumType type, uint32_t count);
    void PrePushBack(DatumType type);
    void PreGet(uint32_t index, DatumType type) const;
    void PreAssign(DatumType type);

    void ConstructData(DatumData& dataUnion, uint32_t index);
    void DestructData(DatumData& dataUnion, uint32_t index);
    void CopyData(DatumData& dst, uint32_t dstIndex, DatumData& src, uint32_t srcIndex);

    virtual void Reset();

public:

    DatumType mType = DatumType::Count;
    bool mExternal = false;
    void* mOwner = nullptr;
    DatumData mData = {};
    DatumChangeHandlerFP mChangeHandler = nullptr;
    uint8_t mCount = 0;
    uint8_t mCapacity = 0;
};
