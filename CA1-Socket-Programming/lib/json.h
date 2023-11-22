#ifndef __JSON_H__
#define __JSON_H__

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include "types.h"
#include "cJSON.h"

#include "../lib/json.h"

Food* parseFood(cJSON* food_json);
Menu* parseMenu(cJSON* root);
Menu* readJson(const char* jsonFile);

#endif // __JSON_H__