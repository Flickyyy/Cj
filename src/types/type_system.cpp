/**
 * @file type_system.cpp
 * @brief Simple static type system implementation for CJ language
 */

#include "cj/types/type_system.h"

namespace cj {

// Initialize static member
UniquePtr<TypeRegistry> TypeRegistry::instance_ = nullptr;

} // namespace cj