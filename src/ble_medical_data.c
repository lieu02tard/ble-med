#include "ble_medical_data.h"
#include "config.h"
#include <string.h>


void pack_from_data(ble_pack_inf *inf, ble_pack_t data)
{
        inf->t1 = *((uint8_t*)(data));
        inf->t2 = *((uint8_t*)(data + 1));
        memcpy(inf->rvalue, (data + 2), sizeof(uint16_t) * 10);
        memcpy(inf->irvalue, (data + 22), sizeof(uint16_t) * 10);
        inf->beatAvg = *((int32_t*)(data + 42));
}

ble_ela_t elapsed_time(ble_time_t first, ble_time_t second)
{
        return second - first;
}

double toSecond(ble_ela_t elapsed)
{
        return ((double) elapsed) / 1000000.0;
}


void pack_to_point(point_t *points, ble_time_t starting_time, t_pack *pack)
{
        double initialEla = toSecond(elapsed_time(starting_time, pack->time));

        for (size_t i = 0; i < 10; i++)
        {
                points[i].x = initialEla + PACKAGE_INTERVAL * i;
#ifdef BLE_MEDICAL_DATA_CONFIG_REDVALUE
                points[i].y = (double)(ble_pack_get_rvalue(pack->data)[i]);
#elif defined BLE_MEDICAL_DATA_CONFIG_IRVALUE
                points[i].y = (double)(ble_pack_get_rvalue(pack->data)[i]);
#endif          
        }
}

__attribute__((always_inline))
inline uint8_t ble_pack_get_t1(ble_pack_t data)
{
        return *((uint8_t*)data);
}

__attribute__((always_inline))
inline uint8_t ble_pack_get_t2(ble_pack_t data)
{
        return *((uint8_t*)(data + 1));
}

__attribute__((always_inline))
inline uint16_t* ble_pack_get_rvalue(ble_pack_t data)
{
        return (uint16_t*)(data + 2);
}

__attribute__((always_inline))
inline uint16_t* ble_pack_get_irvalue(ble_pack_t data)
{
        return (uint16_t*)(data + 22);
}

__attribute__((always_inline))
inline int32_t ble_pack_get_beat(ble_pack_t data)
{
        return *((int32_t*)(data + 42));
}