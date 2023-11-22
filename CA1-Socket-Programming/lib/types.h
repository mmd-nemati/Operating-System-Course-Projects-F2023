#ifndef __TYPES_H__
#define __TYPES_H__

#include <netinet/in.h>
#include <sys/select.h>

#define BUF_NAME  32
#define BUF_PNAME 64
#define ID_SIZE   100
#define MAX_FOOD_NAME   100
#define BUF_CLI   128
#define BUFFER_SIZE   512
#define MESSAGE_SIZE   1024
#define BCAST_IP  "255.255.255.255"
#define TIMEOUT   60
#define TOKEN_DEILIMITER "\n"
#define MAX_SUPPLIER 100
#define MAX_ORDER 100
#define MAX_SALE 100
#define MAX_INGREDIENT 100
#define FILE_MAX_READ 128

typedef struct {
    char id[ID_SIZE];
    char command[BUF_CLI];
    unsigned short port;
    int socket;
} BroadcastData;

typedef struct {
    int max;
    fd_set master;
    fd_set working;
} FdSet;

typedef struct {
    int fd;
    struct sockaddr_in addr;
} BroadcastInfo;

typedef enum {
    OPEN = 0,
    CLOSED = 1
} RestaurantState;

typedef enum {
    PENDING = 0,
    ACCEPTED = 1,
    DENIED = 2
} OrderResult;

typedef struct {
    int result;
    char buffer[BUFFER_SIZE];
} CLIResult;

typedef struct {
    char username[ID_SIZE];
    unsigned short port;
} SupplierInfo;

typedef struct {
    char username[ID_SIZE];
    unsigned short port;
} RestaurantInfo;

typedef struct {
    char username[ID_SIZE];
    unsigned short port;
    char food[MAX_FOOD_NAME];
    OrderResult result;
} OrderInfo;

typedef struct {
    char username[ID_SIZE];
    char food[MAX_FOOD_NAME];
    OrderResult result;
} SaleInfo;

typedef struct {
    unsigned short port;
    char name[MAX_FOOD_NAME];
    int amount;
} ReqIngredData;


typedef struct {
    char username[ID_SIZE];
    char food[MAX_FOOD_NAME];
    unsigned short port;
} OrderFoodData;

typedef struct {
    char result[10];
    unsigned short port;
} ResponseOrderData;

typedef struct {
    char name[MAX_FOOD_NAME];
    int amount;
} Ingredient;

typedef struct{
    char name[MAX_FOOD_NAME];
    Ingredient ingredients[MAX_INGREDIENT];
    int ingredsNum;
} Food;

typedef struct{
    Food** foods;
    int foodsNum;
} Menu;

#endif // __TYPES_H__