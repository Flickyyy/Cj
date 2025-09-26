/**
 * @file cj_stdlib.h
 * @brief CJ Language Standard Library
 * @author CJ Language Team
 * @version 0.1.0
 */

#ifndef CJ_STDLIB_H
#define CJ_STDLIB_H

#include "../common.h"
#include "../types/type_system.h"
#include "containers.h"
#include "math.h"
#include "string_utils.h"
#include "type_conversion.h"
#include "iterators.h"

namespace cj {
namespace stdlib {

/**
 * @brief Initialize the CJ standard library
 */
void Initialize();

/**
 * @brief Cleanup the CJ standard library
 */
void Cleanup();

/**
 * @brief Get standard library version
 */
const char* GetVersion();

} // namespace stdlib
} // namespace cj

#endif // CJ_STDLIB_H