#include "lvglGui.h"
#include "GlobalInit.h"
#include "nvsFlash.h"
#include "WiFiConfig.h"
#include "SpotifyInterface.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Setup_GPIO.h"
#define TIMER_TIME pdMS_TO_TICKS(5000) // in millis

// ****************************** GLobal Variables ****************************** //
static const char *TAG = "Main";

SpotifyInterfaceHandler_t SpotifyInterfaceHandler;

// ****************************** GLobal Functions ****************************** //
/**
 * @brief Function to change colors based on a timer callback
 */
void SpotifyPeriodicTimer(TimerHandle_t xTimer)
{
    bool CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, GetNowPlaying);
    if (CommandResult == false)
    {
        ESP_LOGE(TAG, "Playback info update failed");
        return;
    }
    GUI_UpdateSpotifyScreen( SpotifyInterfaceHandler.PlaybackInfo->ArtistName,
                             SpotifyInterfaceHandler.PlaybackInfo->SongName,
                             SpotifyInterfaceHandler.PlaybackInfo->AlbumName,
                             SpotifyInterfaceHandler.PlaybackInfo->Duration,
                             SpotifyInterfaceHandler.PlaybackInfo->Progress);
    ESP_LOGI(TAG, "Playback info updated");
}
void IRAM_ATTR BackBottomCallBack_(void *arg, void *data)
{
    // bool IsNowPlaying = xSemaphoreTake(IsSpotifyAuthorizedSemaphore, portMAX_DELAY) == pdTRUE;
    // if (IsNowPlaying == false)
    // {
    //     ESP_LOGE(TAG, "Spotify is not authorized");
    //     return;
    // }
    // bool CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, GetNowPlaying);
    // if (CommandResult == false)
    // {
    //     ESP_LOGE(TAG, "Playback info update failed");
    //     return;
    // }
    // Spotify_SendCommand(SpotifyInterfaceHandler, PlayNext);
}
void IRAM_ATTR AcceptBottomCallBack_(void *arg, void *data)
{
    // bool IsNowPlaying = xSemaphoreTake(IsSpotifyAuthorizedSemaphore, portMAX_DELAY) == pdTRUE;
    // if (IsNowPlaying == false)
    // {
    //     ESP_LOGE(TAG, "Spotify is not authorized");
    //     return;
    // }
    // bool CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, GetNowPlaying);
    // if (CommandResult == false)
    // {
    //     ESP_LOGE(TAG, "Playback info update failed");
    //     return;
    // }
    // Spotify_SendCommand(SpotifyInterfaceHandler, PlayNext);
}
void app_main(void)
{
    GUI_TaskInit();
    GlobalInit();
    nvsFlashInit();
    SpiffsGlobalConfig();
    BottomCallBackFunctions_t BottomCallBackFunctions;
    BottomCallBackFunctions.BackBottomCallBack = BackBottomCallBack_;
    BottomCallBackFunctions.AcceptBottomCallBack = AcceptBottomCallBack_;
    GPIO_init(BottomCallBackFunctions);
#ifdef WIFI_INIT_STA_MODE
    // WifiStationMode("Hardware", "87654321");
    WifiStationMode("BELL789", "167271A164A9");
#else
    wifiConnectionModule();
#endif

#ifdef SpotifyEnable

    SpotifyInterfaceHandler.IsSpotifyAuthorizedSemaphore = &IsSpotifyAuthorizedSemaphore;
    SpotifyInterfaceHandler.ConfigAddressInSpiffs = SpotifyConfigAddressInSpiffs;
    Spotify_TaskInit(&SpotifyInterfaceHandler);

    // after this semaphore you can use playback command function in every where !
    if (xSemaphoreTake(IsSpotifyAuthorizedSemaphore, portMAX_DELAY) == pdTRUE)
    {
        bool CommandResult = false;
        CommandResult = Spotify_SendCommand(SpotifyInterfaceHandler, GetUserInfo);
        if (CommandResult == false)
        {
            ESP_LOGE(TAG, "User info update failed");
            return;
        }
        ESP_LOGI(TAG, "User info updated");
        TimerHandle_t xTimer = xTimerCreate("update", TIMER_TIME, pdTRUE, NULL, SpotifyPeriodicTimer);
        xTimerStart(xTimer, 0);
        if (xTimer != NULL)
        {
            if (xTimerStart(xTimer, 0) == pdPASS)
            {
                ESP_LOGI(TAG, "Timer getting start");
            }
        }
    }
#endif
}
