#pragma once

#include "../processor/asm_uitility.h"
#include "../Stack/utility/log.h"
#include "arguments.h"


/**
 * @brief Create new label element in labels variable
 *
 * @return Exit status
 */
int
creat_lab ( char* code, labels* lab, size_t PC, size_t offset );

/**
 * @brief Adds tag into labels
 */
StkError
add_tag ( char* byte_code, char* tag, size_t PC, labels* lab, size_t line );

/**
 * @brief Saves changes in labels variable
 *
 * @return Exit status
 */
StkError
save_change( labels* lab, char* code, int count, size_t PC, bool* was_init );
