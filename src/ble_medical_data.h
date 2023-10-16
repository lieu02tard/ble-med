#ifndef BLE_MEDICAL_DATA_H
#define BLE_MEDICAL_DATA_H

#include <stdint.h>
#include <stdlib.h>

#define PACKAGE_SIZE 46
#define PACKAGE_INTERVAL (1.0/120.0)

typedef uint8_t* ble_pack_t;
typedef int64_t ble_time_t;
typedef ble_time_t ble_ela_t;

typedef struct _ble_pack_inf {
        uint8_t t1;
        uint8_t t2;
        uint16_t rvalue[10];
        uint16_t irvalue[10];
        int32_t beatAvg;
        // uint64_t time;
        // uint32_t millis;
} ble_pack_inf;

typedef struct _point_t {
        double x;
        double y;
} point_t;

typedef enum _value_type {
        RED_VALUE,
        IR_VALUE
} value_t;

typedef struct _t_pack {
        ble_pack_t      data;
        ble_time_t      time;
        int             fileWritten;
        int             plotWritten;
} t_pack;

void pack_from_data(ble_pack_inf*, ble_pack_t);
ble_ela_t elapsed_time(ble_time_t first, ble_time_t second);
double toSecond(ble_ela_t);
void pack_to_point(point_t*, ble_time_t, t_pack*);
extern inline uint8_t ble_pack_get_t1(ble_pack_t);
extern inline uint8_t ble_pack_get_t2(ble_pack_t);
extern inline uint16_t* ble_pack_get_rvalue(ble_pack_t);
extern inline uint16_t* ble_pack_get_irvalue(ble_pack_t);
extern inline int32_t ble_pack_get_beat(ble_pack_t);


#endif