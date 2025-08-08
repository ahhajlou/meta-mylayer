#ifndef RAUC_GDBUS_H
#define RAUC_GDBUS_H

#ifdef __cplusplus
extern "C" {
#endif

int cmd_info(const char* bundle_path);
int cmd_install(const char* bundle_path);
int cmd_status(void);
int cmd_monitor(void);

#ifdef __cplusplus
}
#endif

#endif // RAUC_GDBUS_H
