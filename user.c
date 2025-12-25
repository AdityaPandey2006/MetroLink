#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include"database.h"
#include"rates.h"
#define MAX_STATIONS 1000

typedef struct {
    float distance;
    int path[MAX_STATIONS];
    int length;
} RouteResult;
typedef struct station{
    int id;
    int kiti;
    struct station* links[10];
    float dist[10];
}station;
station* stationData[100];//stationData[id-1]is the station with id id
int explored[100];

void createStations(int total){
    for(int i=0;i<total;i++){
        station* newStation=(station*)malloc(sizeof(station));
        stationData[i]=newStation;
        newStation->id=i+1;
    } 
}
void linkStations(double* linkData,int total,double* kiticount){
    for(int i=0;i<total;i++){
        station* currentStation=stationData[i];
        currentStation->kiti=kiticount[i];
        for(int j=0;j<currentStation->kiti;j++){
            int linkId=(int)linkData[(i)*10*2 + j*2 + 0];
            currentStation->links[j]=stationData[linkId-1];
            currentStation->dist[j]=linkData[(i)*10*2 + j*2 + 1];;
        }
    }
}
RouteResult findRoute(int start, int prev, int end, int path[], int pathlen) {
    RouteResult result;
    result.distance = -1;
    result.length = 0;

    if (start == end) {
        path[pathlen] = start;
        result.distance = 0;
        result.length = pathlen + 1;
        memcpy(result.path, path, sizeof(int) * result.length);
        return result;
    }

    explored[start - 1] = 1;
    float mindist = 9999999;
    int bestPath[MAX_STATIONS];
    int bestLength = 0;

    path[pathlen] = start;

    for (int i = 0; i < stationData[start - 1]->kiti; i++) {
        int next = stationData[start - 1]->links[i]->id;
        if (explored[next - 1] == 0 && next != prev) {
            RouteResult temp = findRoute(next, start, end, path, pathlen + 1);
            if (temp.distance >= 0) {
                float totalDist = temp.distance + stationData[start - 1]->dist[i];
                if (totalDist < mindist) {
                    mindist = totalDist;
                    memcpy(bestPath, temp.path, sizeof(int) * temp.length);
                    bestLength = temp.length;
                }
            }
        }
    }

    explored[start - 1] = 0;

    if (mindist < 9999999) {
        result.distance = mindist;
        result.length = bestLength;
        memcpy(result.path, bestPath, sizeof(int) * bestLength);
    }

    return result;
}

typedef struct metroUser {
    int usercode;
    char name[100];
    char username[100];
    char password[100];
    int balance;
    int entry;
    int exit;
} user;

user** usercollect = NULL;
int* codecollect = NULL;
int usercount = 0;

void saveUserCount() {
    FILE* fptr = fopen("usercounter.txt", "w");
    if (fptr != NULL) {
        fprintf(fptr, "%d", usercount);
        fclose(fptr);
    }
}

void loadUserData() {
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
}

void signup() {
    user* newperson = malloc(sizeof(user));
    newperson->balance = 0;
    printf("Sign Up Now\n");
    printf("Enter Your Name: ");
    getchar(); 
    scanf("%[^\n]", newperson->name);
    printf("What would you like us to call you? ");
    scanf("%s", newperson->username);

    char dummy[100];
    do {
        printf("Create a new pass: ");
        scanf("%s", dummy);
        printf("Reverify password: ");
        scanf("%s", newperson->password);
    } while (strcmp(dummy, newperson->password) != 0);

    usercount++;
    codecollect = realloc(codecollect, (usercount + 1) * sizeof(int));
    usercollect = realloc(usercollect, (usercount + 1) * sizeof(user*));

    newperson->usercode = usercount;
    codecollect[usercount] = usercount;
    usercollect[usercount] = newperson;

    printf("Account created successfully! Your unique code is: %d\n", newperson->usercode);

    FILE* userFile = fopen("users.txt", "a");
    if (userFile != NULL) {
        fprintf(userFile, "%d\n%s\n%s\n%d\n%d\n%d\n", newperson->usercode, newperson->username, newperson->password, newperson->balance, newperson->entry, newperson->exit);
        fclose(userFile);
    }

    saveUserCount();

    char buffer[100];
    sprintf(buffer, "%d.txt", newperson->usercode);
    FILE* historyFile = fopen(buffer, "w");
    fputs("Account created.\n", historyFile);
    fclose(historyFile);
}
void addRecord(user* user1,int amt){
    time_t now;
    time(&now);
    char *timeStr = ctime(&now);
    timeStr[strcspn(timeStr, "\n")] = 0;
    char buffer[100];
    sprintf(buffer, "%d.txt", user1->usercode);
    user1->balance += amt;
        FILE* fptr1 = fopen(buffer, "a");
        if(amt>0){
            fprintf(fptr1, "[%s] Deposited %d | Balance: %d\n", timeStr, amt, user1->balance);
        }else{
            fprintf(fptr1, "[%s] Spent %d | Balance: %d\n", timeStr, -1*amt, user1->balance);
        }
        fclose(fptr1);

        // update users.txt
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
}
void depositMoney(user* user1) {
    printf("Enter money to deposit: ");
    int amt;
    scanf("%d", &amt);
    if (amt > 0) {
        addRecord(user1,amt);
    } else {
        printf("Invalid amount entered.\n");
    }
}

void viewHistory(user* user1) {
    char buffer[100];
    sprintf(buffer, "%d.txt", user1->usercode);
    FILE* fptr = fopen(buffer, "r");
    if (fptr != NULL) {
        printf("----- Hey, %s, here is your transaction history:\n", user1->username);
        char hist[300];
        while (fgets(hist, sizeof(hist), fptr)) {
            printf("%s", hist);
        }
        printf("---- That's all :)\n");
        fclose(fptr);
    } else {
        printf("No history found!\n");
    }
}

void login() {
    printf("Please enter your unique code: ");
    int code;
    scanf("%d", &code);

    int flag = 0;
    user* useraddress = NULL;

    for (int i = 1; i <= usercount; i++) {
        if (usercollect[i]->usercode == code) {
            useraddress = usercollect[i];
            printf("Account Found!\nEnter password to login: ");
            char valid[100];
            scanf("%s", valid);

            if (strcmp(valid, useraddress->password) == 0) {
                flag = 1;
                break;
            } else {
                for (int j = 2; j >= 1; j--) {
                    printf("Wrong password. Tries left: %d\nRe-enter: ", j);
                    scanf("%s", valid);
                    if (strcmp(valid, useraddress->password) == 0) {
                        flag = 1;
                        break;
                    }
                }
            }
            break;
        }
    }

    if (flag >= 1) {
        printf("Hey, %s! What do you wish to do?\n", useraddress->username);
        while (1) {
            printf("Select: 0 to exit, 1 to book (todo), 2 to check account balance, 3 to deposit cash, 4 to check transaction history\n");
            int option;
            scanf("%d", &option);
            if (option == 0) {
                printf("Bye!\n");
                return;
            } else if (option == 1) {
                for(int i=0;i<100;i++){
                    explored[i] = 0;
                  };
                  double ** returneddata=returnDatabase();
                  double* linkdata=returneddata[0];
                  double* kiticount=returneddata[1];
                  double* tmppp=returneddata[2];
                  int totalStations=*tmppp;
                  
                  createStations(totalStations);
                  linkStations(linkdata,totalStations,kiticount);
                  printStations();
                  int start,end;
                  printf("\nEnter start and end station ids: ");
                  scanf("%d %d",&start,&end);
                  printf("Route from station %d to station %d:\n",start,end);
                  //printf("%p\n%p\n%f\n",linkdata,kiticount,*tmppp);
                  int path[MAX_STATIONS];
                  RouteResult res = findRoute(start, -1, end, path, 0);
                  float distance=res.distance;
                  int price=-1;
                  if(distance>=0){
                        price=get_price_for_distance(distance);
                        printf("Distance : %f, Price : %d\n",distance,price);
                        printf("Path: ");
                        for (int i = 0; i < res.length; i++) {
                            if(i!=0){
                                printf(" - ");
                            }
                            printf("%d", res.path[i]);
                        }
                        printf("\n");
                        printf("Do you want to confirm booking? (1:YES, 0:NO) : ");
                        int response=0;
                        scanf("%d",&response);
                        if(response){
                            if(useraddress->balance>=price){
                                
                                useraddress->entry=start;
                                useraddress->exit=end;
                                addRecord(useraddress,-1*price);
                                printf("\nBooking confirmed\n");
                            }else{
                                printf("\nInsufficient Balance :(\n");
                            }
                        }
                      
                  }else{
                      printf("No route found\n");
                  }
                  free(linkdata);
                  free(kiticount);
                  free(tmppp);
                  free(returneddata);
            } else if (option == 2) {
                printf("Your balance is: %d\n", useraddress->balance);
            } else if (option == 3) {
                depositMoney(useraddress);
            } else if (option == 4) {
                viewHistory(useraddress);
            } else {
                printf("Invalid choice!\n");
            }
        }
    } else {
        printf("Login failed.\n");
    }
}

int main() {
    codecollect = calloc(1, sizeof(int));
    usercollect = calloc(1, sizeof(user*));

    loadUserData();

    int choice;
    printf("Welcome!\n1: Sign Up\n2: Login\nChoice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        signup();
    } else if (choice == 2) {
        login();
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}