#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "McGoogles.h"

// Feel free to play with these numbers! This is a great way to
// test your implementation.
#define MCGOOGLES_SIZE 5
#define NUM_CUSTOMERS 100
#define NUM_COOKS 5
#define ORDERS_PER_CUSTOMER 10
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

// Global variable for the restaurant.
McGoogles *mcg;

/**
 * Thread funtion that represents a customer. A customer should:
 *  - allocate space (memory) for an order.
 *  - select a menu item.
 *  - populate the order with their menu item and their customer ID.
 *  - add their order to the restaurant.
 */
void* McGooglesCustomer(void* tid) {
    int customer_id = (int)(long) tid;
    
    for(int i =0; i < ORDERS_PER_CUSTOMER; i++) {
        Order *ord = (Order *) malloc(sizeof(Order));
        ord->menu_item = PickRandomMenuItem();
        ord->customer_id = customer_id;
        ord->next = NULL;
        AddOrder(mcg, ord);
    }
	return NULL;
}

/**
 * Thread function that represents a cook in the restaurant. A cook should:
 *  - get an order from the restaurant.
 *  - if the order is valid, it should fulfill the order, and then
 *    free the space taken by the order.
 * The cook should take orders from the restaurants until it does not
 * receive an order.
 */
void* McGooglesCook(void* tid) {
    int cook_id = (int)(long) tid;
	int orders_fulfilled = 0;
    Order *ord;
    while((ord = GetOrder(mcg))) {
        orders_fulfilled++;
        free(ord);
    }
	printf("Cook #%d fulfilled %d orders\n", cook_id, orders_fulfilled);
	return NULL;
}

/**
 * Runs when the program begins executing. This program should:
 *  - open the restaurant
 *  - create customers and cooks
 *  - wait for all customers and cooks to be done
 *  - close the restaurant.
 */
int main() {
    srand(time(NULL));
    pthread_t thrCustomers[NUM_CUSTOMERS];
    pthread_t thrCooks[NUM_COOKS];
    
    mcg = OpenRestaurant(MCGOOGLES_SIZE, EXPECTED_NUM_ORDERS);
    
    for(int i = 0; i < NUM_COOKS; i++) {
        pthread_create(&thrCooks[i], NULL, McGooglesCook, (void *) (long) i);
    }
    
    for(int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_create(&thrCustomers[i], NULL, McGooglesCustomer, (void *) (long) i);
    }
    
    for(int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_join(thrCustomers[i], NULL);
    }
    
    for(int i = 0; i < NUM_COOKS; i++) {
        pthread_join(thrCooks[i], NULL);
    }
    
    CloseRestaurant(mcg);
    
    return 0;
}