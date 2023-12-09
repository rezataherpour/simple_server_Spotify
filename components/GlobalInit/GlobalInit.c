#include "GlobalInit.h"


static const char *TAG = "Global init";
extern QueueHandle_t BufQueue1;
extern SemaphoreHandle_t GetResponseSemaphore;
extern SemaphoreHandle_t FinishWifiConfig;

#ifdef SpotifyEnable
SemaphoreHandle_t WifiParamExistenceCheckerSemaphore = NULL;
SemaphoreHandle_t IsSpotifyAthurizedSemaphore = NULL;
#endif
/**
 * in this function we init hardware or variable that need them
 *  globally
 */
void GlobalInit()
{
    GetResponseSemaphore = xSemaphoreCreateBinary();
    BufQueue1 = xQueueCreate(1, sizeof(char) * sizeof(char[2500]));
    FinishWifiConfig = xSemaphoreCreateBinary();
    WifiParamExistenceCheckerSemaphore = xSemaphoreCreateBinary();

#ifdef SpotifyEnable
    IsSpotifyAthurizedSemaphore = xSemaphoreCreateBinary();
#endif

    ESP_LOGI(TAG, "Eventloop create");
    ESP_ERROR_CHECK(esp_event_loop_create_default());
}