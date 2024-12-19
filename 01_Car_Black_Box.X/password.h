/* 
 * File:   password.h
 * Author: user
 *
 * Created on 6 December, 2024, 7:42 PM
 */

#ifndef PASSWORD_H
#define	PASSWORD_H

#define MAX_CHANCE              3
#define PASS_CODE               "PASSWORD:"
#define DEFAULT_PASSWORD        "1010"

unsigned char password[5], user_password[5];
unsigned char pass_index, cursur, pass_count = 0, detected_key;
unsigned char star_arr[] = "____";

unsigned short pass_delay = 0, chance = 0;

void init_password(void);
void reset_initial_password_data(void);
unsigned char check_password(void);
void reset_password(void);

#endif	/* PASSWORD_H */

