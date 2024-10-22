#pragma once
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <AST.h>

#define HKSL_VOID_TYPE_ID 0
#define HKSL_FLOAT_TYPE_ID 1
#define HKSL_FLOAT2_TYPE_ID 2
#define HKSL_FLOAT3_TYPE_ID 3
#define HKSL_FLOAT4_TYPE_ID 4

namespace HKSL {
enum class TypeKind {
    Float,
    Float2,
    Float3,
    Float4,
    Void,
    Struct
};
class Type {
    public:
        virtual ~Type() = default;
        virtual uint64_t id() const = 0; 
        virtual const char* name() const = 0;
        virtual size_t size_of() const = 0;
        virtual TypeKind kind() const = 0;
};
struct Void: public Type {
    uint64_t id() const override;
    const char* name() const override;
    virtual size_t size_of() const override;
    TypeKind kind() const override;
};

struct Float: public Type {
    uint64_t id() const override;
    const char* name() const override;
    virtual size_t size_of() const override;
    TypeKind kind() const override;
};

struct Float2: public Type {
    uint64_t id() const override;
    const char* name() const override;
    virtual size_t size_of() const override;
    TypeKind kind() const override;
};

struct Float3: public Type {
    uint64_t id() const override;
    const char* name() const override;
    virtual size_t size_of() const override;
    TypeKind kind() const override;
};

struct Float4: public Type {
    uint64_t id() const override;
    const char* name() const override;
    virtual size_t size_of() const override;
    TypeKind kind() const override;
};

class TypeRegistry {
    public:
        TypeRegistry();
        bool register_type(std::unique_ptr<Type> ty);
        Type* get(const char* name);
        Type* get(const std::string& name);
        Type* get_float();
        Type* get_float2();
        Type* get_float3();
        Type* get_void();

    private:
        bool is_primitive(const Type& type);
        std::unordered_map<std::string, std::unique_ptr<Type>> types;
};

class TypeResolver {
    public:
        TypeResolver() = default;
        void register_expr(const Expr* expr, Type* type);
        Type* type_of(const Expr* expr);
    private:
        std::unordered_map<const Expr*, Type*> type_map;
};
}