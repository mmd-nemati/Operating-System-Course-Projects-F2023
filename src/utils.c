#include "../lib/utils.h"

char* strip(char *str) {
    for (int i = 0; i < strlen(str); i++) 
        if (str[i] == '\n') {
            str[i] = '\0';
            break;
        }
    return str;
}
struct termios lockTerminal() {
    struct termios originalTermios;
    struct termios modifiedTermios;
    if (tcgetattr(STDIN_FILENO, &originalTermios) == -1) {
        logTerminalError("tcgetattr");
        exit(EXIT_FAILURE);
    }
    modifiedTermios = originalTermios;
    modifiedTermios.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &modifiedTermios) == -1) {
        logTerminalError("tcsetattr");
        exit(EXIT_FAILURE);
    }
    return originalTermios;
}


ReqIngredData* getReqIngredData() {
    ReqIngredData *request = (ReqIngredData*)malloc(sizeof(ReqIngredData));
    char buffer[BUFFER_SIZE];
    char inp[BUFFER_SIZE];

    memset(buffer, '\0', BUFFER_SIZE);
    memset(inp, '\0', BUFFER_SIZE);
    sprintf(inp, "--> %sport%s of supplier: ", ANSI_GRN, ANSI_RST);
    write(STDOUT_FILENO, inp, BUFFER_SIZE);
    read(0, buffer, BUFFER_SIZE);
    request->port = (unsigned short)atoi(strtok(buffer, "\n"));

    memset(buffer, '\0', BUFFER_SIZE);
    memset(inp, '\0', BUFFER_SIZE);
    sprintf(inp, "--> name of %singredient%s: ", ANSI_YEL, ANSI_RST);
    write(STDOUT_FILENO, inp, BUFFER_SIZE);
    read(0, buffer, BUFFER_SIZE);
    strcpy(request->name, strtok(buffer, "\n"));
    
    memset(buffer, '\0', BUFFER_SIZE);
    memset(inp, '\0', BUFFER_SIZE);
    sprintf(inp, "--> number of %singredient%s: ", ANSI_YEL, ANSI_RST);
    write(STDOUT_FILENO, inp, BUFFER_SIZE);
    read(0, buffer, BUFFER_SIZE);
    request->amount = atoi(strtok(buffer, "\n"));

    return request;
}    

OrderFoodData* getReqFoodData() {
    OrderFoodData *request = (OrderFoodData*)malloc(sizeof(OrderFoodData));
    char buffer[BUFFER_SIZE];
    char inp[BUFFER_SIZE];
    memset(inp, '\0', BUFFER_SIZE);

    memset(buffer, '\0', BUFFER_SIZE);
    memset(inp, '\0', BUFFER_SIZE);
    sprintf(inp, "--> %sport%s of restaurant: ", ANSI_GRN, ANSI_RST);
    write(STDOUT_FILENO, inp, BUFFER_SIZE);
    read(0, buffer, BUFFER_SIZE);
    request->port = (unsigned short)atoi(strtok(buffer, "\n"));

    memset(buffer, '\0', BUFFER_SIZE);
    memset(inp, '\0', BUFFER_SIZE);
    sprintf(inp, "--> name of %sfood%s: ", ANSI_PUR, ANSI_RST);
    write(STDOUT_FILENO, inp, BUFFER_SIZE);
    read(0, buffer, BUFFER_SIZE);
    strcpy(request->food, strtok(buffer, "\n"));

    return request;
}  

unsigned short extractPort(char *buffer, int tokNum) {
    char token[100];
    strtok(&buffer[ID_SIZE], "-");
    for (int i = 0; i < tokNum - 2; i++) 
        strtok(NULL, "-");
    
    return (unsigned short)atoi(strtok(NULL, "-"));
}

int getNumOfOrdersResult(OrderInfo *orders, int ordersCount, OrderResult result) {
    int num = 0;
    for (int i = 0; i < ordersCount; i++)
        if (orders[i].result == result)
            num++;

    return num;
}

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
    if (getNumOfOrdersResult(orders, ordersCount, PENDING) == 0) {
        sprintf(buffer,"%sNo available order%s\n", ANSI_YEL, ANSI_RST);
        write(STDOUT_FILENO, buffer,strlen(buffer));
        return;
    }
    write(STDOUT_FILENO, "--------------------\n", 21);
    write(STDOUT_FILENO, "<username>::<port> --> <food>\n", 30);
    printf("orders: %d\n", ordersCount);
    for (int i = 0; i < ordersCount; i++) {
        if (orders[i].result == PENDING) {
            sprintf(buffer, "%s%s%s::%s%hu%s --> %s%s%s\n", ANSI_YEL, orders[i].username, ANSI_RST, ANSI_GRN,
                    orders[i].port, ANSI_RST, ANSI_PUR, orders[i].food, ANSI_RST);
            write(STDOUT_FILENO, buffer, strlen(buffer));
        }
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

void printSales(OrderInfo *orders, int ordersCount) {
    char buffer[BUFFER_SIZE];
    if ((getNumOfOrdersResult(orders, ordersCount, ACCEPTED) + 
            getNumOfOrdersResult(orders, ordersCount, DENIED)) == 0) {
        sprintf(buffer,"%sNo available sale%s\n", ANSI_YEL, ANSI_RST);
        write(STDOUT_FILENO, buffer,strlen(buffer));
        return;
    }
    char *accStr = "accepted";
    char *denStr = "rejected";
    write(STDOUT_FILENO,"--------------------\n", strlen("--------------------\n"));
    write(STDOUT_FILENO,"<username>--<food> --> <result>\n",32);
    for (int i = 0; i < ordersCount; i++) {
        if (orders[i].result != PENDING) {
            sprintf(buffer,"%s%s%s--%s%s%s --> ",ANSI_YEL,orders[i].username,
                    ANSI_RST, ANSI_PUR, orders[i].food,
                    ANSI_RST);
            write(STDOUT_FILENO,buffer,strlen(buffer));
            if(orders[i].result == ACCEPTED)
                sprintf(buffer,"%s%s%s\n",ANSI_GRN,"accepted",ANSI_RST);
            else
                sprintf(buffer,"%s%s%s\n",ANSI_RED,"rejected",ANSI_RST);
            write(STDOUT_FILENO,buffer,strlen(buffer));
        }
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