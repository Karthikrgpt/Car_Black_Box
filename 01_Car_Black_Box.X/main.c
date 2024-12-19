/* 
 * File:   main.c
 * Author: Karthik R
 *
 * Created on 27 November, 2024, 3:39 PM
 */

#include "main.h"

void int_config(void) {

    init_uart();
    init_adc();
    init_matrix_keypad();
    init_clcd();
    init_i2c();
    init_ds1307();
    init_password();

    puts("Initialization successful...\n\n\r");

    get_logs();

    clcd_print((const unsigned char *) "Welcome", LINE1(4));
    clcd_print((const unsigned char *) "Car Black Box", LINE2(1));
    for (delay = 500000; delay--;);
    CLEAR_DISP_SCREEN;

}

int main() {

    int_config();

    while (1) {

        key = read_switches(STATE_CHANGE);

        if (key == MK_SW12) {
            if (!menu_flag) menu_flag = 1;
            CLEAR_DISP_SCREEN;
        }

        if (menu_flag == 1 && !password_flag) password_flag = check_password();
        else if (menu_flag && password_flag) display_main_menu(key);
        else view_dashboard(key);

        if (!password_flag || !menu_flag) {
            password_flag = 0;
            menu_flag = 0;
        }

    }

}
