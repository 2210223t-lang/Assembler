#pragma once

#include "../stack.h"

#define logger_name "log"

int logger_open();
void printlg( const char* text, ... );
void logger_close( void );
void stack_status( stack* stk );
void print_metadata( stack* stk );
void putslg_debug( const char* a );
