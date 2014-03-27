/*
 * misc.h
 *
 *  Created on: 11.02.2014
 */

#ifndef TESTS_MISC_H_
#define TESTS_MISC_H_

#include "main/mtypes.h"

void test_initialize_context(struct gl_context *ctx, gl_api api);
char* test_load_text_file(void *ctx, const char *file_name);


#endif /* TESTS_MISC_H_ */
