#ifndef _COMMANDS_
#define _COMMANDS_

#define SERVERADDR "34.254.242.81"
#define PORT 8080

#define REGISTER "register"
#define LOGIN "login"
#define ENTER_LIBRARY "enter_library"
#define GET_BOOKS "get_books"
#define GET_BOOK "get_book"
#define ADD_BOOK "add_book"
#define DELETE_BOOK "delete_book"
#define LOGOUT "logout"
#define EXIT "exit"

void register_user();
void login_user();
void enter_library();
void get_books();
void get_book();
void add_book();
void delete_book();
void logout();
void free_resources();

#endif