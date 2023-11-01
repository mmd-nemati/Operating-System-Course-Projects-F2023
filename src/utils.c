#include "../lib/utils.h"

void printSuppliers(SupplierInfo *suppliers, int suppliersCount) {
    char buffer[BUFFER_SIZE];
    if (suppliersCount == 0) {
        sprintf(buffer, "%sNo available supplier%s\n", ANSI_RED, ANSI_RST);
        write(STDOUT_FILENO, buffer, strlen(buffer));
        return;
    }
    write(STDOUT_FILENO, "--------------------\n", 21);
    write(STDOUT_FILENO, "<username>::<port>\n", 19);
    for (int i = 0; i < suppliersCount; i++) {
        sprintf(buffer, "%s%s%s::%s%hu%s\n", ANSI_YEL, suppliers[i].username, ANSI_RST, 
                    ANSI_GRN, suppliers[i].port, ANSI_RST);
        write(STDOUT_FILENO, buffer, strlen(buffer));
    }
    write(STDOUT_FILENO, "--------------------\n", 21);
}

void printOrders(OrderInfo *orders, int ordersCount) {
    char buffer[BUFFER_SIZE];
    if (ordersCount == 0) {
        sprintf(buffer, "%sNo available order%s\n", ANSI_YEL, ANSI_RST);
        write(STDOUT_FILENO, buffer, strlen(buffer));
        return;
    }
    write(STDOUT_FILENO, "--------------------\n", 21);
    write(STDOUT_FILENO, "<username>::<port> --> <food>\n", 30);
    for (int i = 0; i < ordersCount; i++) {
        sprintf(buffer, "%s%s%s::%s%hu%s --> %s%s%s\n", ANSI_YEL, orders[i].username, ANSI_RST, ANSI_GRN,
                orders[i].port, ANSI_RST, ANSI_PUR, orders[i].food, ANSI_RST);
        write(STDOUT_FILENO, buffer, strlen(buffer));
    }
    write(STDOUT_FILENO, "--------------------\n", 21);
}

void printIngredients(Ingredient *ingredients, int IngredientsCount) {
    char buffer[BUFFER_SIZE];
    if (IngredientsCount == 0) {
        sprintf(buffer, "%sNo available ingredient%s\n", ANSI_YEL, ANSI_RST);
        write(STDOUT_FILENO, buffer, strlen(buffer));
        return;
    }
    write(STDOUT_FILENO, "--------------------\n", strlen("--------------------\n"));
    write(STDOUT_FILENO, "<name> --> <amount>\n", 20);
    for (int i = 0; i < IngredientsCount; i++) {
        sprintf(buffer, "%s%s%s --> %s%hu%s\n", ANSI_YEL, ingredients[i].name, ANSI_RST, ANSI_GRN,
                ingredients[i].amount, ANSI_RST);
        write(STDOUT_FILENO, buffer, strlen(buffer));
    }
    write(STDOUT_FILENO, "--------------------\n", strlen("--------------------\n"));
}

void printSales(SaleInfo *sales, int salesCount) {
    char buffer[BUFFER_SIZE];
    if (salesCount == 0) {
        sprintf(buffer,"%sNo available sale%s\n", ANSI_YEL, ANSI_RST);
        write(STDOUT_FILENO, buffer,strlen(buffer));
        return;
    }
    char *accStr = "accepted";
    char *denStr = "rejected";
    write(STDOUT_FILENO,"--------------------\n", strlen("--------------------\n"));
    write(STDOUT_FILENO,"<username>--<food> --> <result>\n",32);
    for (int i = 0; i < salesCount; i++) {
        sprintf(buffer,"%s%s%s--%s%s%s --> \n",ANSI_YEL,sales[i].username,
                ANSI_RST,ANSI_GRN,sales[i].food,
                ANSI_RST);
        write(STDOUT_FILENO,buffer,strlen(buffer));
        if(sales[i].result == ACCEPTED)
            sprintf(buffer,"%s%s%s\n",ANSI_GRN,"accepted",ANSI_RST);
        else
            sprintf(buffer,"%s%s%s\n",ANSI_RED,"rejected",ANSI_RST);
        write(STDOUT_FILENO,buffer,strlen(buffer));
    }
    write(STDOUT_FILENO,"--------------------\n", strlen("--------------------\n"));
}

void printRecipes(Menu* menu) {
    char buffer[BUFFER_SIZE];
    write(STDOUT_FILENO, "--------------------\n", strlen("--------------------\n"));
    for (int i = 0; i < menu->foodsNum; i++) {
        sprintf(buffer, "\n%d- %s%s%s:\n", i + 1, ANSI_PUR, menu->foods[i]->name, ANSI_RST);
        write(STDOUT_FILENO, buffer, strlen(buffer));
        for (int j = 0; j < menu->foods[i]->ingredsNum; j++) {
            sprintf(buffer, "\t\t%s%s%s : %s%d%s\n", ANSI_YEL, menu->foods[i]->ingredients[j].name, ANSI_RST,
                    ANSI_GRN, menu->foods[i]->ingredients[j].amount, ANSI_RST);
            write(STDOUT_FILENO, buffer, strlen(buffer));
        }
    }
    write(STDOUT_FILENO, "--------------------\n", strlen("--------------------\n"));
}

void printMenu(Menu* menu) {
    char buffer[BUFFER_SIZE];
    write(STDOUT_FILENO, "--------------------\n", strlen("--------------------\n"));
    for (int i = 0; i < menu->foodsNum; i++) {
        sprintf(buffer, "\n%d- %s%s%s\n", i + 1, ANSI_PUR, menu->foods[i]->name, ANSI_RST);
        write(STDOUT_FILENO, buffer, strlen(buffer));
    }
    write(STDOUT_FILENO, "--------------------\n", strlen("--------------------\n"));
}