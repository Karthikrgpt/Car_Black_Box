#ifndef MAIN_H
#define MAIN_H

#include <xc.h>
#include <string.h>
#include "clcd.h"
#include "matrix_keypad.h"
#include "adc.h"
#include "External_eeprom.h"
#include "uart.h"
#include "i2c.h"
#include "ds1307.h"
#include "password.h"
#include "internal_eeprom.h"

#define MAX_GEAR                8
#define MIN_GEAR                2
#define COLUSION                0
#define MAX_LOG                 10
#define LOG_LEN                 14
#define INIT_LOG_ADDRESS        0x00
#define MAX_LOG_ADDRESS         0x8B // 0 to 139 i,e maximum accessable EEPROM address to store complete 10 logs
#define FIRST_LOG_ADDRESS       0x0E // 14 i,e maximum accessable EEPROM address to store one log
#define NINEth_LOG_ADDRESS      0x7E // 126 i,e Starting EEPROM address to store 10th log

unsigned char key, i, j, password_flag = 0;
unsigned char time[9], clock_reg[3], sec_count = 0, min_count = 0, hur_count = 0, time_fild = 0;
unsigned long int speed = 0, delay = 0;
short gear = 1, menu_flag = 0, menu_count = 0, arrow_count = 0, count_view = 0;
unsigned char Events[][3] = {"C_", "ON", "GN", "G1", "G2", "G3", "G4", "G5", "GR"};
unsigned char clcd_screen_char[15], eeprom_address = INIT_LOG_ADDRESS, log_data[10][15];
unsigned char menu_list[][14] = {"View log", "Download log", "Clear log", "Set time", "Reset Pasword"};

void int_config(void); //configure function
void view_dashboard(unsigned char detected_key); //Dashboard function declaration
void clcd_screen_data(void);
void event_store(void); //Storing events function declaration
void get_logs(void);
void display_main_menu(unsigned char detected_key); //main menu function declaration
void view_log(unsigned char detected_key); //View log function declaration
void event_reader(unsigned char detected_key); //Reading events function declaration
void get_time(void);
void set_time(unsigned char detected_key); //Set time function declaration
void download_log(void); //Download log function declaration
void clear_log(void); //Clear log function declaration


#endif

