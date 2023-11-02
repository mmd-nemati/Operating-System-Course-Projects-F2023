#ifndef __UTILS_H__
#define __UTILS_H__

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

#include "types.h"
#include "colors.h"
#include "logger.h"

// #include "../lib/utils.h"
char* strip(char *str);
ReqIngredData* getReqIngredData();
OrderFoodData* getReqFoodData();
unsigned short extractPort(char *buffer, int tokNum);
void printSuppliers(SupplierInfo *suppliers, int suppliersCount);
void printOrders(OrderInfo *orders, int ordersCount);
void printIngredients(Ingredient *ingredients, int IngredientsCount);
void printSales(OrderInfo *orders, int ordersCount);
void printRecipes(Menu* menu);
void printMenu(Menu* menu);
struct termios lockTerminal();
#endif // __UTILS_H__