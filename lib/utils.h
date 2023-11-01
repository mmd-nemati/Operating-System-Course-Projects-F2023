#ifndef __UTILS_H__
#define __UTILS_H__

#include <unistd.h>
#include <fcntl.h>

#include "types.h"
#include "colors.h"
#include <string.h>
#include <stdio.h>
#include "../lib/utils.h"

void printSuppliers(SupplierInfo *suppliers, int suppliersCount);
void printOrders(OrderInfo *orders, int ordersCount);
void printIngredients(Ingredient *ingredients, int IngredientsCount);
void printSales(SaleInfo *sales, int salesCount);
void printRecipes(Menu* menu);
void printMenu(Menu* menu);

#endif // __UTILS_H__