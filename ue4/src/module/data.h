#ifndef UE4_DATA_H
#define UE4_DATA_H

#include "module.h"

// dev management
int data_device_create(unsigned int id, unsigned int size, char *key);

void data_device_destroy(unsigned int id);

void __init data_device_setup_all();

void data_device_destroy_all();

// ctl only operations

void data_clear(unsigned int device_id);

void data_update_key(unsigned int id, char *key);

unsigned int data_size(unsigned int id);

bool data_user_has_access(unsigned int device_id);


#endif //UE4_DATA_H
