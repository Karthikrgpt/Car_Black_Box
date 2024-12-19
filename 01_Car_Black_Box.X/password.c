
#include <xc.h>
#include <string.h>
#include "internal_eeprom.h"
#include "password.h"
#include "clcd.h"
#include "matrix_keypad.h"
#include "uart.h"

void reset_initial_password_data(void) {
    pass_index = 0;
    pass_count = 0;
    memset(user_password, '\0', 4);
    memset(star_arr, '_', 4);
    CLEAR_DISP_SCREEN;
}

void init_password(void) {

    static unsigned char verify[10];

    reset_initial_password_data();

    for (pass_index = 0; pass_index < 9; pass_index++) {
        verify[pass_index] = read_internal_eeprom(pass_index);
    }

    if (strcmp((const char *) verify, PASS_CODE)) {
        puts("\nPassword doesn't found in internal EEPROM memory, accessing the default password i,e. 1010\n\r");
        strcpy((char *) password, DEFAULT_PASSWORD);
    } else {
        puts("\nPassword found in internal EEPROM memory i,e. ");
        for (pass_index = 9; pass_index < 9 + 4; pass_index++)
            password[pass_index - 9] = read_internal_eeprom(pass_index);
        puts((const char *) password);
        puts("\n\r");
    }

}

unsigned char check_password(void) {

    reset_initial_password_data();

    while (1) {

        detected_key = read_switches(STATE_CHANGE);

        clcd_print((const unsigned char *) "Enter Password", LINE1(1));
        clcd_print((const unsigned char *) "Ch Left", LINE2(0));
        clcd_putch((const unsigned char) ('0' + (3 - chance)), LINE2(8));
        clcd_print((const unsigned char *) ": ", LINE2(9));
        clcd_print(star_arr, LINE2(11));

        //cursur handling
        if (pass_delay++ < 500) star_arr[pass_count] = '_';
        else if (pass_delay <= 1000) star_arr[pass_count] = ' ';
        else pass_delay = 0;

        if (detected_key == MK_SW11) {
            user_password[pass_index++] = '1';
            star_arr[pass_count] = '*';
            pass_count++;
        } else if (detected_key == MK_SW12) {
            user_password[pass_index++] = '0';
            star_arr[pass_count] = '*';
            pass_count++;
        }

        //password verifying
        if (pass_index == 4 && !strcmp((const char *) password, (const char *) user_password)) {
            chance = 0;
            reset_initial_password_data();
            puts("\nEntered to Dashboard Mode\n\n\r");
            return 1;
        } else if (pass_index == 4) {
            reset_initial_password_data();
            chance++;
        } else if (chance == MAX_CHANCE) {
            reset_initial_password_data();
            chance = 0;
            puts("\nMaximum chance finished wait for 60 second and try again later....\n\n\r");
            clcd_print((const unsigned char *) "Chance finished!", LINE1(0));
            clcd_print((const unsigned char *) "Wait for ", LINE2(0));
            clcd_print((const unsigned char *) "Sec.", LINE2(12));
            for (unsigned long int wait_pass = 600; wait_pass--;) {
                clcd_putch((const unsigned char) ('0' + (wait_pass / 100)), LINE2(9));
                clcd_putch((const unsigned char) ('0' + ((wait_pass / 10) % 10)), LINE2(10));
                for (unsigned long int hold_pass = 50000; hold_pass--;);
            }
            CLEAR_DISP_SCREEN;
            return 0;
        }
    }

    return 0;

}

void reset_password(void) {

    reset_initial_password_data();

    memset(password, '_', 4);
    memset(user_password, '_', 4);

    //storing passcord
    write_internal_eeprom(pass_index++, 'P');
    write_internal_eeprom(pass_index++, 'A');
    write_internal_eeprom(pass_index++, 'S');
    write_internal_eeprom(pass_index++, 'S');
    write_internal_eeprom(pass_index++, 'W');
    write_internal_eeprom(pass_index++, 'O');
    write_internal_eeprom(pass_index++, 'R');
    write_internal_eeprom(pass_index++, 'D');
    write_internal_eeprom(pass_index++, ':');

    while (1) {

        if (pass_count <= MAX_CHANCE) {

            clcd_print((const unsigned char *) "Enter Password", LINE1(1));
            clcd_print(password, LINE2(6));

            //cursur handling
            if (pass_delay++ < 500) password[pass_count] = '_';
            else if (pass_delay <= 1000) password[pass_count] = ' ';
            else pass_delay = 0;

            detected_key = read_switches(STATE_CHANGE);

            if (detected_key == MK_SW11) password[pass_count++] = '1';
            else if (detected_key == MK_SW12) password[pass_count++] = '0';

            if (pass_count == MAX_CHANCE + 1) CLEAR_DISP_SCREEN;

        } else {

            clcd_print((const unsigned char *) "Comform Password", LINE1(0));
            clcd_print(user_password, LINE2(6));

            //cursur handling
            if (pass_delay++ < 500) user_password[pass_count - 4] = '_';
            else if (pass_delay <= 1000) user_password[pass_count - 4] = ' ';
            else pass_delay = 0;

            detected_key = read_switches(STATE_CHANGE);

            if (detected_key == MK_SW11) user_password[pass_count++ -4] = '1';
            else if (detected_key == MK_SW12) user_password[pass_count++ -4] = '0';
        }

        if (pass_count >= 2 * (MAX_CHANCE + 1)) {

            if (!strcmp((const char *) user_password, (const char *) password)) {

                for (pass_count = 0; pass_count <= MAX_CHANCE; pass_count++)
                    write_internal_eeprom(pass_index++, user_password[pass_count]);

                puts("\nPassword Reseted as ");
                puts((const char *) password);
                puts("\n\n\r");

                clcd_print((const unsigned char *) "Password Updated", LINE1(0));
                clcd_print((const unsigned char *) "Successfully", LINE2(2));

            } else {
                
                puts("\nPassword Reseting is unsuccessful, try again \n\n\r");
                clcd_print((const unsigned char *) "Pasword Mismatch", LINE1(0));
                clcd_print((const unsigned char *) "Try Again", LINE2(4));

            }

            for (unsigned long int delay = 500000; delay--;);
            CLEAR_DISP_SCREEN;

            break;
        }
    }

}