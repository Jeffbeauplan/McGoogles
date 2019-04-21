#include "McGoogles.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


bool IsEmpty(McGoogles* mcg);
bool IsFull(McGoogles* mcg);
bool IsLastOrder(McGoogles* mcg);
bool IsOrdersLeft(McGoogles* mcg);
void AddOrderToBack(Order **orders, Order *order);


MenuItem McGooglesMenu[] = { 
    "GoogMac", 
    "GoogDouble", 
    "GoogChicken", 
    "GoogQuarterPounder", 
    "GoogNuggets",
    "GoogCrispyChicken",
    "GoogHamburger",
    "GoogCheeseBurger",
    "GoogSmokeHouseBurger",
    "GoogFiletOFish",
};
int McGooglesMenuLength = 10;

MenuItem PickRandomMenuItem() {
    int randNum = (rand() % McGooglesMenuLength);
    return McGooglesMenu[randNum];
}

McGoogles* OpenRestaurant(int max_size, int expected_num_orders) {
    McGoogles *mcg = (McGoogles*) malloc(sizeof(McGoogles));
    mcg->orders = NULL;
    mcg->current_size = 0;
    mcg->next_order_number = 0;
    mcg->max_size = max_size;
    mcg->orders_handled = 0;
    mcg->expected_num_orders = expected_num_orders;
    pthread_mutex_init(&mcg->mutex, NULL);
    pthread_cond_init(&(mcg->can_add_orders), NULL);
    pthread_cond_init(&(mcg->can_get_orders), NULL);
    printf("Restaurant is open!\n");
    return mcg;
}

void CloseRestaurant(McGoogles* mcg) {
    if (mcg->orders_handled == mcg->expected_num_orders) {
        pthread_mutex_destroy(&(mcg->mutex));
        pthread_cond_destroy(&(mcg->can_add_orders));
        pthread_cond_destroy(&(mcg->can_get_orders));
        free(mcg);
        printf("Restaurant is closed!\n");
    }
    else {
        printf("%d orders not fulfilled\n", mcg->expected_num_orders - mcg->orders_handled);
    }
}

int AddOrder(McGoogles* mcg, Order* order) {
    pthread_mutex_lock(&(mcg->mutex));
    
    while(IsFull(mcg)) {
        pthread_cond_wait(&(mcg->can_add_orders), &(mcg->mutex));
    }
    
    order->order_number = mcg->next_order_number;
    
    if(IsEmpty(mcg)) {
        mcg->orders = order;
    }
    else {
        AddOrderToBack(&(mcg->orders), order);
    }
    
    mcg->next_order_number++;
    mcg->current_size++;
    
    pthread_cond_signal(&(mcg->can_get_orders));
    pthread_mutex_unlock(&(mcg->mutex));
   
    return order->order_number;
}

Order *GetOrder(McGoogles* mcg) {
    pthread_mutex_lock(&(mcg->mutex));
    
    while(IsEmpty(mcg) && !IsLastOrder(mcg) ) {
        pthread_cond_wait(&(mcg->can_get_orders), &(mcg->mutex));
    }
    
    // Ensure that cooks that receive final broadcast don't add to the orders_handled count
    if(IsOrdersLeft(mcg)) mcg->orders_handled++;
    
    if(mcg->orders_handled >= mcg->expected_num_orders) {
        //notify cooks no orders left
        printf("No more orders\n");
        pthread_cond_broadcast(&(mcg->can_get_orders));
        pthread_mutex_unlock(&(mcg->mutex));
        return NULL;
    }
    
    Order* ord = mcg->orders;
    mcg->orders = mcg->orders->next;
    mcg->current_size--;
    
    if (!IsFull(mcg) && IsOrdersLeft(mcg)) {
        pthread_cond_signal(&(mcg->can_add_orders));
    }
    
    pthread_mutex_unlock(&(mcg->mutex));
    return ord;
}

// Optional helper functions (you can implement if you think they would be useful)
bool IsEmpty(McGoogles* mcg) {
    return (mcg->orders == NULL);
}

bool IsFull(McGoogles* mcg) {
  return mcg->current_size >= mcg->max_size;
}

bool IsLastOrder(McGoogles* mcg) {
    return mcg->orders && (mcg->orders->order_number >= mcg->max_size-1);
}

bool IsOrdersLeft(McGoogles* mcg) {
    return mcg->orders_handled < mcg->expected_num_orders;
}

void AddOrderToBack(Order **orders, Order *order) {
    Order *temp = *orders;
    while(temp->next != NULL) temp = temp->next;  
    temp->next = order;
}
