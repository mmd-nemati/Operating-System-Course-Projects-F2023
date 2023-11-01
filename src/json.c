#include "../lib/json.h"
char* readJsonFile(const char* fileName) {
    ssize_t readSize;
    ssize_t totalReadSize = 0;
    ssize_t bufferSize = FILE_MAX_READ;
    char* buffer = (char*)malloc(FILE_MAX_READ);
    assert(buffer != NULL);
    int fd = open(fileName, O_RDONLY);
    if (fd == -1) {
        write(0, "Failed to open the file\n", strlen("Failed to open the file\n"));
        free(buffer);
        return NULL;
    }
    while ((readSize = read(fd, buffer + totalReadSize, FILE_MAX_READ)) > 0) {
        totalReadSize += readSize;
        if (totalReadSize >= bufferSize) {
            bufferSize *= 2;
            buffer = (char*)realloc(buffer, bufferSize);
            assert(buffer != NULL);
        }
    }
    if (readSize == -1) {
        write(0, "Failed to read the file\n", strlen("Failed to read the file\n"));
        free(buffer);
        close(fd);
        return NULL;
    }
    buffer[totalReadSize] = '\0';
    close(fd);
    return buffer;
}

Food* parseFood(cJSON* food_json) {
    Food* food = (Food*)malloc(sizeof(Food));
    assert(food != NULL);
    // food->name = (char*)malloc(sizeof(char) * strlen(food_json->string));
    // assert(food->name != NULL);
    strcpy(food->name, food_json->string);
    food->ingredsNum = cJSON_GetArraySize(food_json);
    // food->ingredients = (Ingredient*)malloc(sizeof(Ingredient) * food->ingredsNum);
    // assert(food->ingredients != NULL);
    cJSON* ingredient;
    int i = 0;
    cJSON_ArrayForEach(ingredient, food_json) {
        // food->ingredients[i].name = (char*)malloc(sizeof(char) * strlen(ingredient->string));
        // assert(food->ingredients[i].name != NULL);
        strcpy(food->ingredients[i].name, ingredient->string);
        food->ingredients[i].amount = ingredient->valueint;
        i++;
    }
    return food;
}

Menu* parseMenu(cJSON* root) {
    Menu* menu = (Menu*)malloc(sizeof(Menu));
    assert(menu != NULL);
    menu->foodsNum = cJSON_GetArraySize(root);
    menu->foods = (Food**)malloc(sizeof(Food*) * menu->foodsNum);
    assert(menu->foods != NULL);
    cJSON* food_json;
    int i = 0;
    cJSON_ArrayForEach(food_json, root){
        menu->foods[i] = parseFood(food_json);
        i++;
    }

    return menu;
}

Menu* readJson(const char* jsonFile) {
    const char* str = readJsonFile(jsonFile);
    cJSON *root = cJSON_Parse(str);
    Menu* menu = parseMenu(root);
    cJSON_Delete(root);
    return menu;
}