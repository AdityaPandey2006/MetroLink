#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>  
#include "database.h"
#include "rates.h"
#define MAX_ATTEMPTS 3
#define LOCKOUT_DURATION 120  
#define PASSWORD "admin123"
typedef struct metroUser {
    int usercode;
    char name[100];
    char username[100];
    char password[100];
    int balance;
    int entry;
    int exit;
} user;


    

void showLockoutCountdown(time_t lockout_start) {
    time_t now = time(NULL);
    int remaining = (int)(lockout_start + LOCKOUT_DURATION - now);

    while (remaining > 0) {
        printf("\rToo many incorrect attempts. Please wait %d seconds...", remaining);
        fflush(stdout);
        sleep(1);
        remaining--;
    }

    printf("\nLockout period over. You can try again.\n");
    remove("lockout_time.txt");
}

// Check for existing lockout
int checkAndHandleLockout() {
    FILE *f = fopen("lockout_time.txt", "r");
    if (f) {
        time_t lockout_start;
        fscanf(f, "%ld", &lockout_start);
        fclose(f);

        time_t now = time(NULL);
        double elapsed = difftime(now, lockout_start);

        if (elapsed < LOCKOUT_DURATION) {
            showLockoutCountdown(lockout_start);
        } else {
            remove("lockout_time.txt");
        }
    }
    return 0;
}

// Set the current time to lockout file
void setLockoutTime() {
    FILE *f = fopen("lockout_time.txt", "w");
    if (f) {
        fprintf(f, "%ld", time(NULL));
        fclose(f);
    }
}

// Main authentication function
int authenticateAdmin() {
    char input[50];
    int attempts = 0;

    checkAndHandleLockout();

    while (1) {
        while (attempts < MAX_ATTEMPTS) {
            printf("Enter admin password: ");
            scanf("%s", input);

            if (strcmp(input, PASSWORD) == 0) {
                printf("Access granted.\n");
                return 1;
            } else {
                printf("Incorrect password. Try again.\n");
                attempts++;
            }
        }

        // After 3 wrong attempts
        setLockoutTime();
        checkAndHandleLockout();
        attempts = 0; // Reset attempts after lockout
    }
}

int main() {
    if (!authenticateAdmin()) {
        return 1;
    }

    // Admin menu placeholder
    printf("Welcome, Admin!\n");
    //printf("Enter action to perform\nL : List of stations\nA1 : Add station\nA2 : Modify station : \nB : Delete station\nC : Add or modify connection\nD : Delete connection\nE : Change rates\nQ : Quit");
    printf("\nWelcome, Admin!\n");
    char userinp;
    while (1) {
        
        printf("Enter action to perform:\n");
        printf("L : List of stations\n");
        printf("A : Add station\n");
        printf("B : Modify station\n");
        printf("C : Delete station\n");
        printf("D : Add or modify connection\n");
        printf("E : Delete connection\n");
        printf("F : Change rates\n");
        printf("G : Validate entry\n");
        printf("H : Validate exit\n");
        printf("Q : Quit\n");
        printf("Enter choice: ");

        scanf(" %c", &userinp);

        if (userinp == 'L') {
            printStations();
        } 
        else if (userinp == 'A') {
            char stnname[50];
            printf("Enter station name to add: ");
            scanf(" %[^\n]", stnname);
            addStation(stnname);
            printf("Station added.\n");
        } 
        else if (userinp == 'B') {
            int id;
            char stnname[50];
            printf("Enter station ID to modify: ");
            scanf("%d", &id);
            printf("Enter station name to modify: ");
            
            scanf(" %[^\n]", stnname);
            changeStationName(id,stnname);
            printf("Station %d modified.\n", id);
        } 
        else if (userinp == 'C') {
            int id;
            printf("Enter station ID to delete: ");
            scanf("%d", &id);
            deleteStation(id);
            printf("Station %d deleted.\n", id);
        } 
        else if (userinp == 'D') {
            int s1, s2;
            float dist;
            printf("Enter station 1, station 2 and distance: ");
            scanf("%d %d %f", &s1, &s2, &dist);
            add(s1, s2, dist);
        } 
        else if (userinp == 'E') {
            int s1, s2;
            printf("Enter station 1 and station 2 to delete connection: ");
            scanf("%d %d", &s1, &s2);
            deleteentry(s1, s2);
            printf("Connection deleted.\n");
        } 
        else if (userinp == 'F') {
            float min_d, max_d;
            int new_price;
            printf("Enter min distance, max distance, and new price: ");
            scanf("%f %f %d", &min_d, &max_d, &new_price);
            if (update_price_range(min_d, max_d, new_price) == 0) {
                printf("Price updated for %.2f - %.2f km\n", min_d, max_d);
            } else {
                printf("Failed to update price.\n");
            }
        }
        else if (userinp == 'G') {//Entry
            user** usercollect = NULL;
            int* codecollect = NULL;
            int usercount = 0;
            codecollect = calloc(1, sizeof(int));
            usercollect = calloc(1, sizeof(user*));

            FILE* countFile = fopen("usercounter.txt", "r");
            if (countFile != NULL) {
                fscanf(countFile, "%d", &usercount);
                fclose(countFile);
            }

            codecollect = realloc(codecollect, (usercount + 1) * sizeof(int));
            usercollect = realloc(usercollect, (usercount + 1) * sizeof(user*));

            FILE* userFile = fopen("users.txt", "r");
            if (userFile != NULL) {
                for (int i = 1; i <= usercount; i++) {
                    user* loaded = malloc(sizeof(user));
                    fscanf(userFile, "%d\n%s\n%s\n%d\n%d\n%d\n", &loaded->usercode, loaded->username, loaded->password, &loaded->balance, &loaded->entry, &loaded->exit);
                    codecollect[i] = loaded->usercode;
                    usercollect[i] = loaded;
                }
                fclose(userFile);
            }
            int stnid;
            int ucode;
            printf("Enter station ID : ");
            scanf("%d",&stnid);
            printf("Enter passenger's unique code : ");
            scanf("%d",&ucode);
            int flag = 0;
            user* useraddress = NULL;

            for (int i = 1; i <= usercount; i++) {
                if (usercollect[i]->usercode == ucode) {
                    useraddress = usercollect[i];
                    flag=1;
                    if(useraddress->entry==stnid){
                        printf("\nAccess granted\n");
                        useraddress->entry=0;
                        //upload data
                        FILE* userFile = fopen("users.txt", "w");
                        for (int i = 1; i <= usercount; i++) {
                            fprintf(userFile, "%d\n%s\n%s\n%d\n%d\n%d\n",
                                usercollect[i]->usercode,
                                usercollect[i]->username,
                                usercollect[i]->password,
                                usercollect[i]->balance,
                                usercollect[i]->entry,
                                usercollect[i]->exit);
                        }
                        fclose(userFile);
                    }else{
                        printf("\nAccess denied\n");
                    }
                    break;
                }
            }
            if(flag==0){
                printf("User not found");
            }
        } 
        else if (userinp == 'H') { //Exit
            user** usercollect = NULL;
            int* codecollect = NULL;
            int usercount = 0;
            codecollect = calloc(1, sizeof(int));
            usercollect = calloc(1, sizeof(user*));

            FILE* countFile = fopen("usercounter.txt", "r");
            if (countFile != NULL) {
                fscanf(countFile, "%d", &usercount);
                fclose(countFile);
            }

            codecollect = realloc(codecollect, (usercount + 1) * sizeof(int));
            usercollect = realloc(usercollect, (usercount + 1) * sizeof(user*));

            FILE* userFile = fopen("users.txt", "r");
            if (userFile != NULL) {
                for (int i = 1; i <= usercount; i++) {
                    user* loaded = malloc(sizeof(user));
                    fscanf(userFile, "%d\n%s\n%s\n%d\n%d\n%d\n", &loaded->usercode, loaded->username, loaded->password, &loaded->balance, &loaded->entry, &loaded->exit);
                    codecollect[i] = loaded->usercode;
                    usercollect[i] = loaded;
                }
                fclose(userFile);
            }
            int stnid;
            int ucode;
            printf("Enter station ID : ");
            scanf("%d",&stnid);
            printf("Enter passenger's unique code : ");
            scanf("%d",&ucode);
            int flag = 0;
            user* useraddress = NULL;

            for (int i = 1; i <= usercount; i++) {
                if (usercollect[i]->usercode == ucode) {
                    useraddress = usercollect[i];
                    flag=1;
                    if(useraddress->exit==stnid){
                        printf("\nAccess granted\n");
                        useraddress->exit=0;
                        FILE* userFile = fopen("users.txt", "w");
                        for (int i = 1; i <= usercount; i++) {
                            fprintf(userFile, "%d\n%s\n%s\n%d\n%d\n%d\n",
                                usercollect[i]->usercode,
                                usercollect[i]->username,
                                usercollect[i]->password,
                                usercollect[i]->balance,
                                usercollect[i]->entry,
                                usercollect[i]->exit);
                        }
                        fclose(userFile);
                    }else{
                        printf("\nAccess denied\n");
                    }
                    break;
                }
            }
            if(flag==0){
                printf("User not found");
            }
        }
        else if (userinp == 'Q') {
            printf("Exiting Admin Menu.\n");
            break;
        } 
        else {
            printf("Invalid input! Please try again.\n");
        }
    }

    return 0;
}