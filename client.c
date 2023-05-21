#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */

#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "commands.h"
#include "parson.h"

int main(int argc, char *argv[])
{
    char command[LINELEN];

    while (1) {
        scanf("%s", command);
        if (!strncmp(command, REGISTER, 9)) {
            register_user();
        } else if (!strncmp(command, LOGIN, 6)) {
            login_user();
        } else if (!strncmp(command, ENTER_LIBRARY, 14)) {
            enter_library();
        } else if (!strncmp(command, GET_BOOKS, 10)) {
            get_books();
        } else if (!strncmp(command, GET_BOOK, 9)) {
            get_book();
        } else if (!strncmp(command, ADD_BOOK, 10)) {
            add_book();
        } else if (!strncmp(command, DELETE_BOOK, 12)) {
            delete_book();
        } else if (!strncmp(command, LOGOUT, 7)) {
            logout();
        } else if (!strncmp(command, EXIT, 5)) {
            break;
        } else {
            printf("Unknown command\n");
        }
    }

    // free the allocated data at the end!
    free_resources();

    return 0;
}
