#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
void app_init(void);

/***************************************************************************//**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void);

#ifdef __cplusplus
}
#endif

#endif  // APP_H
