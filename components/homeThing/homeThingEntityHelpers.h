#pragma once

#include <string>
#include "esphome/core/entity_base.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace homething_menu_base {

inline std::string entity_object_id(const EntityBase* entity) {
  char buffer[OBJECT_ID_MAX_LEN];
  size_t length = entity->write_object_id_to(buffer, sizeof(buffer));
  return std::string(buffer, length);
}

inline std::string value_accuracy_string(float value,
                                         int8_t accuracy_decimals) {
  char buffer[VALUE_ACCURACY_MAX_LEN];
  size_t length = value_accuracy_to_buf(buffer, value, accuracy_decimals);
  return std::string(buffer, length);
}

}  // namespace homething_menu_base
}  // namespace esphome
