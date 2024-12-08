#ifndef YAML_REFLECTION_H
#define YAML_REFLECTION_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

class ObjectReflection {
  public:
  static void dump_object_properties(const Object* p_object)
  {
    if (!p_object) {
      UtilityFunctions::print("Null object provided");
      return;
    }

    String class_name = p_object->get_class();
    UtilityFunctions::print(vformat("Examining object of class: %s", class_name));

    // Get list of properties
    TypedArray<Dictionary> properties = p_object->get_property_list();

    // Iterate through properties
    for (int i = 0; i < properties.size(); i++) {
      Dictionary prop = properties[i];

      // Skip properties that aren't meant to be visible/edited
      if (!(int(prop["usage"]) & PROPERTY_USAGE_STORAGE)) {
        continue;
      }

      String name = prop["name"];
      Variant::Type type = Variant::Type(int(prop["type"]));
      String type_info;
      Variant value = p_object->get(name);

      // Handle different property types
      switch (type) {
        case Variant::OBJECT: {
          Object* obj = value.operator Object*();
          if (obj) {
            type_info = vformat("Object(%s)", obj->get_class());
            // For resources, we can get even more specific
            Resource* res = Object::cast_to<Resource>(obj);
            if (res) {
              type_info += vformat(" Resource(%s)", res->get_class());
              if (!res->get_path().is_empty()) {
                type_info += vformat(" Path(%s)", res->get_path());
              }
            }
          } else {
            type_info = "Object(null)";
          }
          break;
        }
        default:
          type_info = Variant::get_type_name(type);
          if (prop.has("class_name") && !String(prop["class_name"]).is_empty()) {
            type_info += vformat("(%s)", String(prop["class_name"]));
          }
          if (prop.has("hint_string") && !String(prop["hint_string"]).is_empty()) {
            type_info += vformat(" Hint(%s)", String(prop["hint_string"]));
          }
          break;
      }

      // Print property information
      UtilityFunctions::print(vformat("  Property: %s", name));
      UtilityFunctions::print(vformat("    Type: %s", type_info));
      UtilityFunctions::print(vformat("    Value: %s", value));
      UtilityFunctions::print(vformat("    Usage: %d", int(prop["usage"])));
      UtilityFunctions::print(vformat("    Hint: %d", int(prop["hint"])));
    }
  }

  static Dictionary get_object_properties(const Object* p_object)
  {
    Dictionary result;

    if (!p_object) {
      return result;
    }

    TypedArray<Dictionary> properties = p_object->get_property_list();

    for (int i = 0; i < properties.size(); i++) {
      Dictionary prop = properties[i];

      if (!(int(prop["usage"]) & PROPERTY_USAGE_STORAGE)) {
        continue;
      }

      String name = prop["name"];
      Dictionary prop_info;
      prop_info["type"] = prop["type"];
      prop_info["class_name"] = prop["class_name"];
      prop_info["hint"] = prop["hint"];
      prop_info["hint_string"] = prop["hint_string"];
      prop_info["usage"] = prop["usage"];
      prop_info["value"] = p_object->get(name);

      result[name] = prop_info;
    }

    return result;
  }

  static bool is_property_serializable(const Dictionary& p_property_info)
  {
    // Check if property should be serialized
    if (!(int(p_property_info["usage"]) & PROPERTY_USAGE_STORAGE)) {
      return false;
    }

    // You might want to skip certain types
    Variant::Type type = Variant::Type(int(p_property_info["type"]));
    switch (type) {
      case Variant::CALLABLE:
      case Variant::SIGNAL:
      case Variant::RID:
        return false;
      default:
        return true;
    }
  }

  static String get_resource_type(const Ref<Resource>& p_resource)
  {
    if (p_resource.is_null()) {
      return "";
    }
    return p_resource->get_class();
  }
};

} // namespace godot

#endif // YAML_REFLECTION_H
