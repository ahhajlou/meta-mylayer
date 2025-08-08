#ifndef BLOCK_DEV_MONITOR_H
#define BLOCK_DEV_MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*callback_with_data_t)(void);
void register_callback_with_data(callback_with_data_t cb);
int dev_mon_main_loop(void);

#ifdef __cplusplus
}
#endif

#endif // BLOCK_DEV_MONITOR_H
