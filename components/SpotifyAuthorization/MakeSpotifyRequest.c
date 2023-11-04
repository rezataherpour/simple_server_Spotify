

#include "SpotifyAuthorization.h"
#include <stdio.h>
#include <stdlib.h>
#include "HttpsRequest_.h"
#include "cJSON.h"
#include "main.h"

extern struct Token_ TokenParam;
extern struct UserInfo_ UserInfo;

/**
*@brief This function searches for specific patterns ('code' and 'state') within a character array and returns a boolean value indicating if either pattern was found.
*@param[in] Res The character array to search within, and Res is response from first stage from spotify athurisiation
*@param[in] SizeRes The size of the character array.
*@return Returns true if either the 'code' or 'state' pattern was found, and false otherwise.
*/
bool FindCode(char *Res, uint16_t SizeRes)
{

    uint8_t flg_findCode = 0;
    uint8_t flg_findState = 0;
    for (uint16_t i = 0; i < SizeRes; i++)
    {
        if (Res[i] == 'c')
        {
            if (Res[i + 1] == 'o' && Res[i + 2] == 'd' && Res[i + 3] == 'e')
            {
                printf("\twe find CODE !\n");
                flg_findCode = 1;
            }
        }
        if (Res[i] == 's')
        {
            if (Res[i + 1] == 't' && Res[i + 2] == 'a' && Res[i + 3] == 't' && Res[i + 3] == 'e')
            {
                printf("\twe find State !\n");
                flg_findState = 1;
            }
        }
    }
    if (flg_findCode == 1 || flg_findState == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
*@brief This function searches for a token within a character array and extracts the corresponding JSON object.
*@param[in] Res The character array to search within. and Res is response from first stage from spotify athurisiation
*@param[in] SizeRes The size of the character array.
*@return Returns true if the token is found and the corresponding JSON object is successfully extracted, otherwise returns false.
*/
bool FindToken(char *Res, uint16_t SizeRes)
{
    uint8_t flg_findToken = 0;
    uint32_t SizeOfJson = 0;
    char json[1024] = {0};
    for (uint16_t i = 0; i < SizeRes; i++)
    {
        if (Res[i] == '{')
        {
            if (Res[i + 1] == '"' && Res[i + 2] == 'a' && Res[i + 3] == 'c' && Res[i + 4] == 'c' && Res[i + 5] == 'e' && Res[i + 6] == 's')
            {
                flg_findToken = 1;
                SizeOfJson = i;
            }
        }
        if (Res[i] == '}')
        {
            for (uint16_t j = SizeOfJson; j <= i; j++)
            {
                json[j - SizeOfJson] = Res[j];
            }
            memset(Res, 0x000, SizeRes);
            for (uint16_t j = 0; j < sizeof(json); j++)
            {
                Res[j] = json[j];
            }
        }
    }
    if (flg_findToken == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**

*@brief This function sends a request to the Spotify login API to exchange an authorization code for an access token.
*@param[in,out] Buf The character buffer to store the request and receive the response.
*@param[in] SizeBuf The size of the character buffer.
*@param[in] code is parameter that we give it before .
*@param[in] SizeCode The size of the authorization code.
*@return This function does not return a value.
*/
void SendRequest_AndGiveToken(char *Buf, size_t SizeBuf, char *code, size_t SizeCode)
{
    // static const char *ReDirectUri = "http%3A%2F%2Fdeskhub.local%2Fcallback%2F";
    char grand[1024] = {0};
    printf("\n\n\n\n%s\n\n\n", code);
    sprintf(grand, "grant_type=authorization_code&redirect_uri=%s&%s", ReDirectUri, code);
    memset(Buf, 0x0, SizeBuf);
    sprintf(Buf, "POST /api/token HTTP/1.1 \r\n"
                 "Host: accounts.spotify.com\r\n"
                 "Authorization: Basic NTViYjk3NGEwNjY3NDgxYWIwYjJhNDlmZDBhYmVhNmQ6ZDgwYmQ3ZThjMWIwNGJmY2FjZGI1ZWNmNmExNTUyMTU=\r\n"
                 "Content-Length: %d\r\n"
                 "Cookie: __Host-device_id=AQAwmp7jxagopcWw89BjSDAA530mHwIieOZdJ9Im8nI0-70oEsSInx3jkeSO09YQ7sPgPaIUyMEvZ-tct7I6OlshJrzVYOqcgo0; sp_tr=false\r\n"
                 "Content-Type: application/x-www-form-urlencoded\r\n"
                 "\r\n"
                 "%s\r",
            strlen(grand), grand);
    // printf("\n\n\nmake request for give token : %s\n\n\n", Buf);
    char url[256] = "https://accounts.spotify.com/api/token";
    char server[256] = "accounts.spotify.com";
    MyHttpsHandler(Buf, SizeBuf, url, sizeof(url), server, sizeof(server));
}
/**

*@brief This function sends a request to the Spotify API to perform a player command.
*@param[in] Method_ The HTTP method to be used in the request (e.g., "POST", "PUT").
*@param[in] Command_ The specific player command to be executed (e.g., "next", "previous", "play", "pause").
*@param[out] Buf The character buffer to store the request and receive the response.
*@param[in] SizeBuf The size of the character buffer.
*@return This function does not return a value.
*/
void MakePlayerCommand_AndSendIt(const char *Method_, const char *Command_, char *Buf, size_t SizeBuf)
{
    // we copy to of them because ,we dont sent size of Method and command
    char Method[50] = {0};
    char Command[50] = {0};
    strcpy(Method, Method_);
    strcpy(Command, Command_);
    memset(Buf, 0x00, SizeBuf);
    sprintf(Buf, "%s /v1/me/player/%s HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Content-Length: 0\r\n"
                 "Connection: close\r\n\r\n",
            Method, Command, TokenParam.access_token);
    printf("\n\nrequest for getting now playing\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char url[256] = "https://api.spotify.com";
    char server[256] = "api.spotify.com";
    MyHttpsHandler(Buf, SizeBuf, url, sizeof(url), server, sizeof(server));
}
/**

*@brief This function sends a request to the Spotify API to skip to the next track in the player.
*@return This function does not return a value.
*/
void SendRequestForNext()
{
    char Buf[2500];
    MakePlayerCommand_AndSendIt("POST", "next", Buf, 2500);
}
/**

*@brief This function sends a request to the Spotify API to skip to the previous track in the player.
*@return This function does not return a value.
*/
void SendRequestForPrevious()
{
    char Buf[2500];
    MakePlayerCommand_AndSendIt("POST", "previous", Buf, 2500);
}
/**

*@brief This function sends a request to the Spotify API to play the current track in the player.
*@return This function does not return a value.
*/
void SendRequestForPlay()
{
    char Buf[2500];
    MakePlayerCommand_AndSendIt("PUT", "play", Buf, 2500);
}
/**

*@brief This function sends a request to the Spotify API to pause the current track in the player.
*@return This function does not return a value.
*/
void SendRequestForPause()
{
    char Buf[2500];
    MakePlayerCommand_AndSendIt("PUT", "pause", Buf, 2500);
}
/**

*@brief This function sends a request to the Spotify API to retrieve the user's profile information.
*@return This function does not return a value.
*/
void GetUserStatus()
{
    char Buf[2500];
    sprintf(Buf, "GET /v1/me/ HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Content-Length: 0\r\n"
                 "Connection: close\r\n\r\n",
            TokenParam.access_token);
    printf("\n\nrequest for getting now playing\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char url[256] = "https://api.spotify.com";
    char server[256] = "api.spotify.com";
    MyHttpsHandler(Buf, sizeof(Buf), url, sizeof(url), server, sizeof(server));
}
/**

*@brief This function sends a request to the Spotify API to retrieve the user's current status.
*@return This function does not return a value.
*/
void GetUserTopItems()
{
    char Buf[2500];
    sprintf(Buf, "GET /v1/me/top/artists HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Content-Length: 0\r\n"
                 "Connection: close\r\n\r\n",
            TokenParam.access_token);
    printf("\n\nrequest for getting now playing\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char url[256] = "https://api.spotify.com";
    char server[256] = "api.spotify.com";
    MyHttpsHandler(Buf, sizeof(Buf), url, sizeof(url), server, sizeof(server));
}
/**
*@brief This function sends a request to the Spotify API to retrieve the profile information of a specific user.
*@param[in] UserId_ The ID of the user whose profile information is to be retrieved.
*@return This function does not return a value.
*/
void GetUserProfile(char *UserId_)
{
    char Buf[2500];
    char UserId[50];
    strcpy(UserId, UserId_);
    sprintf(Buf, "GET /v1/users/%s HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Content-Length: 0\r\n"
                 "Connection: close\r\n\r\n",
            UserId, TokenParam.access_token);

    printf("\n\nrequest for getting now playing\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char url[256] = "https://api.spotify.com";
    char server[256] = "api.spotify.com";
    MyHttpsHandler(Buf, sizeof(Buf), url, sizeof(url), server, sizeof(server));
}
/**
*@brief This function sends a request to the Spotify API to give Curent playing information 
*/

void GetCurrentPlaying()
{
    char Buf[4000];
    sprintf(Buf, "GET /v1/me/player/currently-playing HTTP/1.1\r\n"
                 "Host: api.spotify.com\r\n"
                 "Authorization: Bearer %s\r\n"
                 "Connection: close\r\n\r\n",
            TokenParam.access_token);
    printf("\n\nrequest for getting now playing\n%s\nand size it =%d\n\n", Buf, strlen(Buf));
    char url[256] = "https://api.spotify.com";
    char server[256] = "api.spotify.com";
    MyHttpsHandler(Buf, sizeof(Buf), url, sizeof(url), server, sizeof(server));
}

/**
 *@brief This function sends a request to the Spotify login API to exchange an authorization code for an access token.
 *@param[in,out] Buf The character buffer to store the request and receive the response.
 *@param[in] SizeBuf The size of the character buffer.
 *@param[in] code is parameter that we give it before .
 *@return This function does not return a value.
 */
void SendRequest_AndGiveTokenWithRefreshToken(char *Buf, size_t SizeBuf, char *RefreshToken_)
{
    char RefreshToken[400];
    strcpy(RefreshToken, RefreshToken_);
    char grand[1024] = {0};
    sprintf(grand, "grant_type=refresh_token&refresh_token=%s&redirect_uri=%s", RefreshToken, ReDirectUri);
    memset(Buf, 0x0, SizeBuf);
    sprintf(Buf, "POST /api/token HTTP/1.1 \r\n"
                 "Host: accounts.spotify.com\r\n"
                 "Authorization: Basic NTViYjk3NGEwNjY3NDgxYWIwYjJhNDlmZDBhYmVhNmQ6ZDgwYmQ3ZThjMWIwNGJmY2FjZGI1ZWNmNmExNTUyMTU=\r\n"
                 "Content-Length: %d\r\n"
                 "Content-Type: application/x-www-form-urlencoded\r\n"
                 "\r\n"
                 "%s\r",
            strlen(grand), grand);
    printf("\n\n\n%s\n\n\n", Buf);
    char url[256] = "https://accounts.spotify.com/api/token";
    char server[256] = "accounts.spotify.com";
    MyHttpsHandler(Buf, SizeBuf, url, sizeof(url), server, sizeof(server));
}