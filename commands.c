#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <ctype.h>
#include "commands.h"
#include "parson.h"
#include "requests.h"
#include "helpers.h"

#define CONNECT_COOKIE_LEN 126
#define TOKEN_LEN 278

static char *connect_cookie = NULL;
static char *token = NULL;

char *trim_string(char *str)
{
    char *end;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';

    return str;
}

int get_res_code(char *response) {
    return atoi(response + 9);
}

int read_string_as_number(char *name) {
    int invalid_str = 1;
    char line[LINELEN];
    char *p;
    while (invalid_str) {
        invalid_str = 0;
        printf("%s", name);
        fgets(line, LINELEN, stdin);
        trim_string(line);
        p = line;
        while (*p != '\0') {
            if (*p < 48 || *p > 57) {
                printf("Please enter a valid number!\n");
                invalid_str = 1;
                break;
            }
            p++;
        }
    }
    return atoi(line);
}

void register_user() {
    char *message = NULL, *response = NULL;
    char username[LINELEN], password[LINELEN];
    printf("username=");
    scanf("%s", username);
    printf("password=");
    scanf("%s", password);
    // create json for user data
    JSON_Value *user_value = json_value_init_object();
    JSON_Object *user_object = json_value_get_object(user_value);
    json_object_set_string(user_object, "username", username);
    json_object_set_string(user_object, "password", password);
    char *serialized_string = json_serialize_to_string(user_value);

    message = compute_post_request(SERVERADDR, "/api/v1/tema/auth/register", "application/json", NULL, serialized_string, NULL, 0);
    int sockfd = open_connection(SERVERADDR, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);

    int res_code = get_res_code(response);
    char *json_str_response = basic_extract_json_response(response);
    JSON_Value *json_response = json_parse_string(json_str_response);
    switch (res_code) {
    case 201:
        printf("%d - Account succesfully created\n", res_code);
        break;
    case 400:
        printf("%d - %s\n", res_code, json_object_get_string(json_object(json_response), "error"));
        break;
    default:
        printf("%d - Unknown code\n", res_code);
        break;
    }

    json_free_serialized_string(serialized_string);
    json_value_free(user_value);
    json_value_free(json_response);
    free(message);
    free(response);
}

void login_user() {
    char *message = NULL, *response = NULL;
    char username[LINELEN], password[LINELEN];
    printf("username=");
    scanf("%s", username);
    printf("password=");
    scanf("%s", password);
    // create json for user data
    JSON_Value *user_value = json_value_init_object();
    JSON_Object *user_object = json_value_get_object(user_value);
    json_object_set_string(user_object, "username", username);
    json_object_set_string(user_object, "password", password);
    char *serialized_string = json_serialize_to_string(user_value);

    message = compute_post_request(SERVERADDR, "/api/v1/tema/auth/login", "application/json", NULL, serialized_string, NULL, 0);
    // printf("%s\n", message);
    int sockfd = open_connection(SERVERADDR, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    // printf("%s\n", response);
    int res_code = get_res_code(response);
    char *json_str_response = basic_extract_json_response(response);
    JSON_Value *json_response = json_parse_string(json_str_response);
    switch (res_code) {
    case 200:
        printf("%d - Successful login\n", res_code);
        char *cookie = strstr(response, "Set-Cookie");
        if (connect_cookie != NULL) {
            free(connect_cookie);
            connect_cookie = NULL;
        }
        connect_cookie = calloc(CONNECT_COOKIE_LEN + 1, sizeof(char));
        memcpy(connect_cookie, cookie + 12, CONNECT_COOKIE_LEN);
        strtok(connect_cookie, ";");
        if (token != NULL) {
            free(token);
            token = NULL;
        }
        // printf("%s\n", connect_cookie);
        break;
    case 400:
        printf("%d - %s\n", res_code, json_object_get_string(json_object(json_response), "error"));
        break;
    default:
        printf("%d - Unknown code\n", res_code);
        break;
    }

    json_free_serialized_string(serialized_string);
    json_value_free(user_value);
    json_value_free(json_response);
    free(message);
    free(response);
}

void enter_library() {
    if (connect_cookie == NULL) {
        printf("Cannot access library - you need to login first!\n");
        return;
    }
    char *message = compute_get_request(SERVERADDR, "/api/v1/tema/library/access", NULL, NULL, &connect_cookie, 1);
    char *response;
    // printf("%s\n", message);
    int sockfd = open_connection(SERVERADDR, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    // printf("%s\n", response);
    int res_code = get_res_code(response);
    char *json_str_response = basic_extract_json_response(response);
    JSON_Value *json_response = json_parse_string(json_str_response);
    switch (res_code) {
    case 200:
        printf("%d - Granted access to library\n", res_code);
        if (token != NULL) {
            free(token);
            token = NULL;
        }
        token = calloc(TOKEN_LEN + 1, sizeof(char));
        strcpy(token, "Bearer ");
        strcat(token, json_object_get_string(json_object(json_response), "token"));
        // printf("%s\n", token);
        break;
    case 401:
        printf("%d - %s\n", res_code, json_object_get_string(json_object(json_response), "error"));
        break;
    default:
        printf("%d - Unknown code\n", res_code);
        break;
    }

    json_value_free(json_response);
    free(message);
    free(response);
}

void get_books() {
    if (token == NULL) {
        printf("You do not have library access - use enter_library command!\n");
        return;
    }

    char *message = NULL, *response = NULL;
    message = compute_get_request(SERVERADDR, "/api/v1/tema/library/books", NULL, token, &connect_cookie, 1);
    // printf("%s\n", message);
    int sockfd = open_connection(SERVERADDR, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    // printf("%s\n", response);

    int res_code = get_res_code(response);
    char *json_str_response = basic_extract_json_response(response);
    // printf("%s\n", json_str_response);
    JSON_Value *json_response = json_parse_string(json_str_response);

    switch (res_code) {
    case 200:
        printf("%d - Retrieved books successfully\n", res_code);
        // read books from json
        JSON_Array *book_array = json_value_get_array(json_response);
        int books_count = json_array_get_count(book_array);
        for (int i = 0; i < books_count; i++) {
            JSON_Value *book = json_array_get_value(book_array, i);
            int book_id = json_object_get_number(json_object(book), "id");
            const char *book_title = json_object_get_string(json_object(book), "title");
            printf("%d.\tid: %d\t title: %s\n", i + 1, book_id, book_title);
        }
        break;
    case 401:
        printf("%d - %s\n", res_code, json_object_get_string(json_object(json_response), "error"));
        break;
    default:
        printf("%d - Unknown code\n", res_code);
        break;
    }

    free(message);
    free(response);
    json_value_free(json_response);
}

void get_book() {
    if (token == NULL) {
        printf("You do not have library access - use enter_library command!\n");
        return;
    }

    // read newline
    getc(stdin);
    int id = read_string_as_number("id=");
    char path[LINELEN];
    sprintf(path, "/api/v1/tema/library/books/%d", id);

    char *message = NULL, *response = NULL;
    message = compute_get_request(SERVERADDR, path, NULL, token, &connect_cookie, 1);
    // printf("%s\n", message);
    int sockfd = open_connection(SERVERADDR, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    // printf("%s\n", response);

    int res_code = get_res_code(response);
    char *json_str_response = basic_extract_json_response(response);
    // printf("%s\n", json_str_response);
    JSON_Value *json_response = json_parse_string(json_str_response);

    switch (res_code) {
    case 200:
        printf("%d - Retrieved book successfully\n", res_code);
        // read book from json
        printf("id:\t\t%d\n", (int)json_object_get_number(json_object(json_response), "id"));
        printf("title:\t\t%s\n", json_object_get_string(json_object(json_response), "title"));
        printf("author:\t\t%s\n", json_object_get_string(json_object(json_response), "author"));
        printf("publisher:\t%s\n", json_object_get_string(json_object(json_response), "publisher"));
        printf("genre:\t\t%s\n", json_object_get_string(json_object(json_response), "genre"));
        printf("page_count:\t%d\n", (int)json_object_get_number(json_object(json_response), "page_count"));
        break;
    case 404:
        printf("%d - %s\n", res_code, json_object_get_string(json_object(json_response), "error"));
        break;
    default:
        printf("%d - Unknown code\n", res_code);
        break;
    }

    free(message);
    free(response);
    json_value_free(json_response);
}

void add_book() {
    if (token == NULL) {
        printf("You do not have library access - use enter_library command!\n");
        return;
    }

    char *message = NULL, *response = NULL;
    char title[LINELEN], author[LINELEN], genre[LINELEN], publisher[LINELEN];
    int page_count;
    // read newline left over after add_book command
    getc(stdin);
    printf("title=");
    fgets(title, LINELEN, stdin);
    trim_string(title);
    printf("author=");
    fgets(author, LINELEN, stdin);
    trim_string(author);
    printf("genre=");
    fgets(genre, LINELEN, stdin);
    trim_string(genre);
    page_count = read_string_as_number("page_count=");
    printf("publisher=");
    fgets(publisher, LINELEN, stdin);
    trim_string(publisher);
    // create json for book data
    JSON_Value *book_value = json_value_init_object();
    JSON_Object *book_object = json_value_get_object(book_value);
    json_object_set_string(book_object, "title", title);
    json_object_set_string(book_object, "author", author);
    json_object_set_string(book_object, "genre", genre);
    json_object_set_number(book_object, "page_count", page_count);
    json_object_set_string(book_object, "publisher", publisher);
    char *serialized_string = json_serialize_to_string(book_value);

    message = compute_post_request(SERVERADDR, "/api/v1/tema/library/books", "application/json", token, serialized_string, &connect_cookie, 1);
    // printf("%s\n", message);
    int sockfd = open_connection(SERVERADDR, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    // printf("%s\n", response);

    int res_code = get_res_code(response);
    char *json_str_response = basic_extract_json_response(response);
    JSON_Value *json_response = json_parse_string(json_str_response);
    switch (res_code) {
    case 200:
        printf("%d - Book added successfully\n", res_code);
        break;
    case 400:
        printf("%d - %s\n", res_code, json_object_get_string(json_object(json_response), "error"));
        break;
    default:
        printf("%d - Unknown code\n", res_code);
        break;
    }

    json_free_serialized_string(serialized_string);
    json_value_free(book_value);
    json_value_free(json_response);
    free(message);
    free(response);
}

void delete_book() {
    if (token == NULL) {
        printf("You do not have library access - use enter_library command!\n");
        return;
    }

    // read newline
    getc(stdin);
    int id = read_string_as_number("id=");
    char path[LINELEN];
    sprintf(path, "/api/v1/tema/library/books/%d", id);

    char *message = NULL, *response = NULL;
    message = compute_delete_request(SERVERADDR, path, "application/json", token, &connect_cookie, 1);
    // printf("%s\n", message);
    int sockfd = open_connection(SERVERADDR, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    // printf("%s\n", response);

    int res_code = get_res_code(response);
    char *json_str_response = basic_extract_json_response(response);
    // printf("%s\n", json_str_response);
    JSON_Value *json_response = json_parse_string(json_str_response);

    switch (res_code) {
    case 200:
        printf("%d - Deleted book successfully\n", res_code);
        break;
    case 404:
        printf("%d - %s\n", res_code, json_object_get_string(json_object(json_response), "error"));
        break;
    default:
        printf("%d - Unknown code\n", res_code);
        break;
    }

    free(message);
    free(response);
    json_value_free(json_response);
}

void logout() {
    if (connect_cookie == NULL) {
        printf("You are not logged in, nothing happened!\n");
        return;
    }

    char *message = NULL, *response = NULL;
    message = compute_get_request(SERVERADDR, "/api/v1/tema/auth/logout", NULL, NULL, &connect_cookie, 1);
    // printf("%s\n", message);
    int sockfd = open_connection(SERVERADDR, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    // printf("%s\n", response);

    int res_code = get_res_code(response);
    char *json_str_response = basic_extract_json_response(response);
    JSON_Value *json_response = json_parse_string(json_str_response);
    switch (res_code) {
    case 200:
        printf("%d - Logout successful\n", res_code);
        break;
    case 400:
        printf("%d - %s\n", res_code, json_object_get_string(json_object(json_response), "error"));
        break;
    default:
        printf("%d - Unknown code\n", res_code);
        break;
    }

    free(connect_cookie);
    connect_cookie = NULL;
    if (token != NULL) {
        free(token);
        token = NULL;
    }
    json_value_free(json_response);
    free(message);
    free(response);
}

void free_resources() {
    free(connect_cookie);
    free(token);
}