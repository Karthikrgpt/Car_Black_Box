
#include "main.h"

void view_dashboard(unsigned char detected_key) {

    event_reader(detected_key);

    clcd_print((const unsigned char *) "   Time   EV SP ", LINE1(0));
    clcd_print(clcd_screen_char, LINE2(1));
}

void display_main_menu(unsigned char detected_key) {

    if (detected_key == MK_SW1 || (menu_flag == 2 && detected_key != MK_SW2)) {

        if (detected_key == MK_SW1) CLEAR_DISP_SCREEN;

        switch (menu_count) {
            case 0:
                view_log(detected_key);
                menu_flag = 2;
                break;
            case 1:
                download_log();
                menu_flag = 1;
                break;
            case 2:
                clear_log();
                menu_flag = 1;
                break;
            case 3:
                set_time(detected_key);
                if (count_view == 1) menu_flag = 2;
                else {
                    menu_flag = 1;
                    count_view = 0;
                    CLEAR_DISP_SCREEN;
                }
                break;
            case 4:
                reset_password();
                menu_count = 0;
                arrow_count = 0;
                count_view = 0;
                menu_flag = 1;
        }

    } else if (detected_key == MK_SW2) {

        menu_count = 0;
        arrow_count = 0;
        time_fild = 0;
        count_view = 0;
        if (--menu_flag <= 0) menu_flag = 0;
        CLEAR_DISP_SCREEN;

    } else if (menu_flag == 1) {

        if (menu_count) clcd_print((const unsigned char*) "->", LINE2(0));
        else clcd_print((const unsigned char*) "->", LINE1(0));

        clcd_print(menu_list[menu_count - arrow_count], LINE1(3));
        clcd_print(menu_list[menu_count - arrow_count + 1], LINE2(3));

        switch (detected_key) {
            case MK_SW11:
                if (++menu_count >= 4) menu_count = 4;
                CLEAR_DISP_SCREEN;
                break;
            case MK_SW12:
                if (--menu_count <= 0) menu_count = 0;
                CLEAR_DISP_SCREEN;
                break;
        }

        if (!menu_count) arrow_count = 0;
        else arrow_count = 1;
    }

}

void event_reader(unsigned char detected_key) {

    get_time();

    speed = read_adc(CHANNEL4);
    speed = (speed * 99) / 1023;

    switch (detected_key) {
        case MK_SW1:
            gear = COLUSION;
            clcd_screen_data();
            event_store();
            break;
        case MK_SW2:
            if (++gear > MAX_GEAR) gear = MAX_GEAR;
            clcd_screen_data();
            event_store();
            break;
        case MK_SW3:
            if (--gear < MIN_GEAR) gear = MIN_GEAR;
            clcd_screen_data();
            event_store();
            break;
    }

    clcd_screen_data();

}

void view_log(unsigned char detected_key) {

    if (detected_key == MK_SW11) {
        if (++count_view >= 9) count_view = 9;
    } else if (detected_key == MK_SW12) {
        if (--count_view <= 0) count_view = 0;
    }

    clcd_print((const unsigned char *) "L   Time   EV SP", LINE1(0));
    clcd_putch((const unsigned char) ('0' + (unsigned int) count_view), LINE2(0));
    clcd_print(log_data[count_view], LINE2(2));

}

void get_time(void) {

    clock_reg[0] = read_ds1307(HOUR_ADDR);
    clock_reg[1] = read_ds1307(MIN_ADDR);
    clock_reg[2] = read_ds1307(SEC_ADDR);

    if (clock_reg[0] & 0x40) {
        time[0] = '0' + ((clock_reg[0] >> 4) & 0x01);
        time[1] = '0' + (clock_reg[0] & 0x0F);
    } else {
        time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);
        time[1] = '0' + (clock_reg[0] & 0x0F);
    }
    time[2] = ':';
    time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F);
    time[4] = '0' + (clock_reg[1] & 0x0F);
    time[5] = ':';
    time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F);
    time[7] = '0' + (clock_reg[2] & 0x0F);
    time[8] = '\0';
}

void set_time(unsigned char detected_key) {

    clcd_print((const unsigned char*) "Set Time", LINE1(4));

    if (detected_key == MK_SW12 && ++time_fild == 3) time_fild = 0;
    else if (detected_key == MK_SW11 && time_fild == 0 && ++hur_count >= 24) hur_count = 0;
    else if (detected_key == MK_SW11 && time_fild == 1 && ++min_count >= 60) min_count = 0;
    else if (detected_key == MK_SW11 && time_fild == 2 && ++sec_count >= 60) sec_count = 0;
    else {
        switch (time_fild) {
            case 0:
                if (delay++ < 500) clcd_print((const unsigned char*) "  ", LINE2(4));
                else if (delay < 1000) clcd_print(time, LINE2(4));
                else delay = 0;
                break;
            case 1:
                if (delay++ < 500) clcd_print((const unsigned char*) "  ", LINE2(7));
                else if (delay < 1000) clcd_print(time, LINE2(4));
                else delay = 0;
                break;
            case 2:
                if (delay++ < 500) clcd_print((const unsigned char*) "  ", LINE2(10));
                else if (delay < 1000) clcd_print(time, LINE2(4));
                else delay = 0;
                break;
        }
    }

    time[0] = '0' + (hur_count / 10);
    time[1] = '0' + (hur_count % 10);
    time[3] = '0' + (min_count / 10);
    time[4] = '0' + (min_count % 10);
    time[6] = '0' + (sec_count / 10);
    time[7] = '0' + (sec_count % 10);

    if (detected_key == MK_SW1) {

        write_ds1307(HOUR_ADDR, ((unsigned char) ((hur_count / 10) << 4) | (hur_count % 10)));
        write_ds1307(MIN_ADDR, ((unsigned char) ((min_count / 10) << 4) | (min_count % 10)));
        write_ds1307(SEC_ADDR, ((unsigned char) ((sec_count / 10) << 4) | (sec_count % 10)));

        puts("\nTime of Black box is modified as ");
        puts((const char *) time);
        puts("\n\n\r");

        count_view++;
    }

}

void download_log(void) {

    clcd_print((const unsigned char*) "Log download", LINE1(2));
    clcd_print((const unsigned char*) "View in Teraterm", LINE2(0));

    puts("\nThe Event logs are :\n\r");
    puts("----------------------\n\r");
    puts(" Logs    Time   EV SP \n\r");
    puts("----------------------\n\r");
    for (i = 0; i < MAX_LOG; i++) {
        puts("  ");
        putch('0' + i);
        puts(".   ");
        puts((const char *) log_data[i]);
        puts("\n\r");
    }
    puts("----------------------\n\n\r");

    for (delay = 500000; delay--;);

    menu_count = 0;
    arrow_count = 0;
    CLEAR_DISP_SCREEN;

}

void clear_log(void) {

    clcd_print((const unsigned char *) "Clear Logs   ", LINE1(3));
    clcd_print((const unsigned char *) "Clear Successful", LINE2(0));

    strcpy((char *) clcd_screen_char, (const char *) "HH:MM:SS NA NA");

    eeprom_address = INIT_LOG_ADDRESS; //updating the address

    for (i = 0; i < MAX_LOG; i++)
        for (j = 0; j < LOG_LEN; j++)
            write_external_eeprom(eeprom_address++, clcd_screen_char[j]);

    for (i = 0; i < MAX_LOG; i++)
        strcpy((char *) log_data[i], (const char *) clcd_screen_char);

    puts("\nLogs are Cleared from the External EEPROM memory\n\n\r");

    for (delay = 500000; delay--;);

    menu_count = 0;
    arrow_count = 0;
    CLEAR_DISP_SCREEN;

}

void event_store(void) {

    for (i = 0; i < MAX_LOG_ADDRESS; i++) // Updating the EEPROM data
        write_external_eeprom(i, read_external_eeprom(i + LOG_LEN));

    eeprom_address = NINEth_LOG_ADDRESS; //updating the address

    for (i = 0; i < MAX_LOG - 1; i++)
        strcpy((char *) log_data[i], (const char *) log_data[i + 1]);

    strcpy((char *) log_data[MAX_LOG - 1], (const char *) clcd_screen_char);

    for (i = 0; i < FIRST_LOG_ADDRESS; i++) // storing the new data to EEPROM
        write_external_eeprom(eeprom_address++, clcd_screen_char[i]);

    puts("Event detected -> "); //Updating the stored data to the UART
    puts((const char *) clcd_screen_char);
    puts("\n\r");

}

void get_logs(void) {

    eeprom_address = INIT_LOG_ADDRESS;

    for (i = 0; i < MAX_LOG; i++)
        for (j = 0; j < LOG_LEN; j++)
            log_data[i][j] = read_external_eeprom(eeprom_address++);
}

void clcd_screen_data(void) {

    clcd_screen_char[0] = time[0];
    clcd_screen_char[1] = time[1];
    clcd_screen_char[2] = time[2];
    clcd_screen_char[3] = time[3];
    clcd_screen_char[4] = time[4];
    clcd_screen_char[5] = time[5];
    clcd_screen_char[6] = time[6];
    clcd_screen_char[7] = time[7];
    clcd_screen_char[8] = ' ';
    clcd_screen_char[9] = Events[gear][0];
    clcd_screen_char[10] = Events[gear][1];
    clcd_screen_char[11] = ' ';
    clcd_screen_char[12] = '0' + (unsigned char) (speed / 10);
    clcd_screen_char[13] = '0' + (unsigned char) (speed % 10);

    sec_count = (time[6] - '0') * 10 + (time[7] - '0');
    min_count = (time[3] - '0') * 10 + (time[4] - '0');
    hur_count = (time[0] - '0') * 10 + (time[1] - '0');

}