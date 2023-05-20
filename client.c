#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

#define MAXSIZE 1024
#define SERVERADDR "34.254.242.81"

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;

    sockfd = open_connection(SERVERADDR, 8080, AF_INET, SOCK_STREAM, 0);
        
    // Ex 1.1: GET dummy from main server
    // message = compute_get_request(SERVERADDR, "/api/v1/dummy", NULL, NULL, 0);
    // send_to_server(sockfd, message);
    // response = receive_from_server(sockfd);
    // printf("%s\n", response);
    // Ex 1.2: POST dummy and print response from main server
    // char *buf[MAXSIZE] = {"prop='Mama ta este femeie frumoasa'"};
    // message = compute_post_request(SERVERADDR, "/api/v1/dummy", "application/x-www-form-urlencoded", buf, 1, NULL, 0);
    // send_to_server(sockfd, message);
    // response = receive_from_server(sockfd);
    // printf("%s\n", response);
    // Ex 2: Login into main server
    // char *buf[MAXSIZE] = {"username=student", "password=student"};
    // message = compute_post_request(SERVERADDR, "/api/v1/auth/login", "application/x-www-form-urlencoded", buf, 2, NULL, 0);
    // send_to_server(sockfd, message);
    // response = receive_from_server(sockfd);
    // printf("%s\n", response);
    // Ex 3: GET weather key from main server
    // char *cookies_buf[MAXSIZE] = {"connect.sid=s%3Ap_uRlAL7yp5ger44sHiZm9vWAge17Hjj.2rWEQFygb7%2F6L7UR8LkrxJ864LeyJcX9NhfCMlqAynw"};
    // message = compute_get_request(SERVERADDR, "/api/v1/weather/key", NULL, cookies_buf, 1);
    // printf("%s\n\n", message);
    // send_to_server(sockfd, message);
    // response = receive_from_server(sockfd);
    // printf("%s\n", response);
    // Ex 4: GET weather data from OpenWeather API
    // struct hostent *host = gethostbyname("api.openweathermap.org");
    // char *owaddr = inet_ntoa(*(struct in_addr *)host->h_addr_list[0]);
    // sockfd = open_connection(owaddr, 80, AF_INET, SOCK_STREAM, 0);
    // char *query_params = "lat=44.73988&lon=22.2767&appid=b912dd495585fbf756dc6d8f415a7649";
    // message = compute_get_request(SERVERADDR, "https://api.openweathermap.org/data/2.5/weather", query_params, NULL, 1);
    // printf("%s\n\n", message);
    // send_to_server(sockfd, message);
    // response = receive_from_server(sockfd);
    // printf("%s\n", response);
    // Ex 5: POST weather data for verification to main server
    char *cookies_buf[MAXSIZE] = {"connect.sid=s%3Ap_uRlAL7yp5ger44sHiZm9vWAge17Hjj.2rWEQFygb7%2F6L7UR8LkrxJ864LeyJcX9NhfCMlqAynw"};
    // message = compute_post_request(SERVERADDR, "/api/v1/weather/44.7398/22.2767", "application/x-www-form-urlencoded", NULL, 0, cookies_buf, 1);
    // printf("%s\n\n", message);
    // send_to_server(sockfd, message);
    // response = receive_from_server(sockfd);
    // printf("%s\n", response);
    // Ex 6: Logout from main server
    message = compute_get_request(SERVERADDR, "/api/v1/auth/logout", NULL, cookies_buf, 1);
    printf("%s\n\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    // BONUS: make the main server return "Already logged in!"

    // free the allocated data at the end!

    return 0;
}
