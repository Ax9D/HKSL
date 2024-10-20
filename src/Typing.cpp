#include <Typing.h>
#include <cassert>
#include <string>

namespace HKSL {
uint64_t Float::id() const { return HKSL_FLOAT_TYPE_ID; }
const char *Float::name() const { return "float"; }
TypeKind Float::kind() const { return TypeKind::Float; }
size_t Float::size_of() const { return 4; }

uint64_t Float2::id() const { return HKSL_FLOAT2_TYPE_ID; }
const char *Float2::name() const { return "float2"; }
TypeKind Float2::kind() const { return TypeKind::Float2; }
size_t Float2::size_of() const { return 4 * 2; }

uint64_t Float3::id() const { return HKSL_FLOAT3_TYPE_ID; }
const char *Float3::name() const { return "float3"; }
TypeKind Float3::kind() const { return TypeKind::Float3; }
size_t Float3::size_of() const { return 4 * 3; }

TypeRegistry::TypeRegistry() {
  register_type(std::make_unique<Float>());
  register_type(std::make_unique<Float2>());
  register_type(std::make_unique<Float3>());
}
bool TypeRegistry::is_primitive(const Type &type) {
  return type.kind() != TypeKind::Struct;
}
bool TypeRegistry::register_type(std::unique_ptr<Type> type) {
  // Non primitive types disabled for now
  assert(is_primitive(*type));

  bool exists = !(types.find(type->name()) == types.end());
  types[type->name()] = std::move(type);

  return exists;
}
Type* TypeRegistry::get(const std::string& name) {
    return get(name.c_str());
}
Type* TypeRegistry::get(const char *name) { return types[name].get(); }
Type* TypeRegistry::get_float() { return get("float"); }
Type* TypeRegistry::get_float2() { return get("float2"); }
Type* TypeRegistry::get_float3() { return get("float3"); }
Type* TypeRegistry::get_void() { return get("void"); }

void TypeResolver::register_expr(const Expr *expr, Type *type) {
    type_map[expr] = type;
}

Type* TypeResolver::type_of(const Expr *expr) {
    auto it= type_map.find(expr);

    if(it != type_map.end()) {
        return it->second;
    }

    return nullptr;
}
}