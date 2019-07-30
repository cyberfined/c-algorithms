#pragma once

#include <stddef.h>
#include "sort/common.h"

void heapsort(void *base, size_t nmemb, size_t size, comp_func cmp);
