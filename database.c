#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "database.h"
#include "rates.h"
typedef struct{
    char station1[4];
    char station2[4];
    float distance;
} Connection;

void add(int s1,int s2,float dist);
int checkIfAlreadyPresent(int s1,int s2);
void overwritePair(int s1,int s2,float new_dist);
void appendEntry(int s1,int s2,float dist);
void deleteentry(int s1,int s2);
void optimizeDatabase();
void printDatabase();
double** returnDatabase();
void addStation(char *stationName);
void deleteStation(int stationID);

int checkIfAlreadyPresent(int s1,int s2){
    char s1_str[4],s2_str[4];
    snprintf(s1_str,4,"%03d",s1);
    snprintf(s2_str,4,"%03d",s2);

    FILE *fptr=fopen("database.txt","r");
    if(!fptr){
        return 0;
    }

    char line[20];
    while(fgets(line,sizeof(line),fptr)){
        char station1[4]={0},station2[4]={0};
        float distance;
        if(sscanf(line,"%3s %3s %f",station1,station2,&distance)==3){
            int is_pair=(strcmp(station1,s1_str)==0&&strcmp(station2,s2_str)==0)||
                        (strcmp(station1,s2_str)==0&&strcmp(station2,s1_str)==0);
            if(is_pair){
                fclose(fptr);
                return 1;
            }
        }
    }
    fclose(fptr);
    return 0;
}

void overwritePair(int s1,int s2,float new_dist){
    FILE *fptr=fopen("database.txt","r");
    FILE *temp=fopen("temp.txt","w");
    if(!fptr||!temp){
        if(fptr){
            fclose(fptr);
        }
        if(temp){
            fclose(temp);
        }
        return;
    }

    char s1_str[4],s2_str[4];
    snprintf(s1_str,4,"%03d",s1);
    snprintf(s2_str,4,"%03d",s2);

    char line[20];
    while(fgets(line,sizeof(line),fptr)){
        char station1[4]={0},station2[4]={0};
        float existing_dist;
        if(sscanf(line,"%3s %3s %f",station1,station2,&existing_dist)==3){
            int is_pair=(strcmp(station1,s1_str)==0&&strcmp(station2,s2_str)==0)||
                        (strcmp(station1,s2_str)==0&&strcmp(station2,s1_str)==0);
            fprintf(temp,"%s %s %.2f\n",station1,station2,is_pair?new_dist:existing_dist);
        }else{
            fputs(line,temp);
        }
    }

    fclose(fptr);
    fclose(temp);
    remove("database.txt");
    rename("temp.txt","database.txt");
}

void appendEntry(int s1,int s2,float dist){
    FILE *fptr=fopen("database.txt","a");
    if(fptr){
        fprintf(fptr,"%03d %03d %.2f\n%03d %03d %.2f\n",s1,s2,dist,s2,s1,dist);
        fclose(fptr);
    }
}

void add(int s1,int s2,float dist){
    if(s1==s2){
        return;
    }
    checkIfAlreadyPresent(s1,s2)?overwritePair(s1,s2,dist):appendEntry(s1,s2,dist);
    optimizeDatabase();
}

void deleteentry(int s1,int s2){
    FILE *fptr=fopen("database.txt","r");
    if(!fptr){
        return;
    }

    FILE *temp=fopen("temp.txt","w");
    if(!temp){
        fclose(fptr);
        return;
    }

    char s1_str[4],s2_str[4];
    snprintf(s1_str,4,"%03d",s1);
    snprintf(s2_str,4,"%03d",s2);

    char line[20];
    while(fgets(line,sizeof(line),fptr)){
        char station1[4]={0},station2[4]={0};
        float distance;
        if(sscanf(line,"%3s %3s %f",station1,station2,&distance)==3){
            int is_pair=(strcmp(station1,s1_str)==0&&strcmp(station2,s2_str)==0)||
                       (strcmp(station1,s2_str)==0&&strcmp(station2,s1_str)==0);
            if(!is_pair){
                fputs(line,temp);
            }
        }else{
            fputs(line,temp);
        }
    }

    fclose(fptr);
    fclose(temp);
    remove("database.txt");
    rename("temp.txt","database.txt");
}

void optimizeDatabase(){
    FILE *fptr=fopen("database.txt","r");
    if(!fptr){
        return;
    }

    Connection connections[1000];
    int count=0;
    char stations[1000][4];
    int stationCount=0;
    char line[20];

    while(fgets(line,sizeof(line),fptr)){
        char s1[4],s2[4];
        float dist;
        if(sscanf(line,"%3s %3s %f",s1,s2,&dist)==3){
            strcpy(connections[count].station1,s1);
            strcpy(connections[count].station2,s2);
            connections[count].distance=dist;
            count++;

            bool hasS1=false,hasS2=false;
            for(int i=0;i<stationCount;i++){
                if(strcmp(stations[i],s1)==0){
                    hasS1=true;
                }
                if(strcmp(stations[i],s2)==0){
                    hasS2=true;
                }
            }
            if(!hasS1){
                strcpy(stations[stationCount++],s1);
            }
            if(!hasS2){
                strcpy(stations[stationCount++],s2);
            }
        }
    }
    fclose(fptr);

    bool mark[count];
    memset(mark,0,sizeof(mark));

    for(int i=0;i<count;i++){
        if(mark[i])continue;

        char *A=connections[i].station1;
        char *B=connections[i].station2;
        float directDist=connections[i].distance;

        for(int cIdx=0;cIdx<stationCount;cIdx++){
            char *C=stations[cIdx];
            if(strcmp(C,A)==0||strcmp(C,B)==0)continue;

            float d1=-1,d2=-1;
            for(int j=0;j<count;j++){
                if((strcmp(connections[j].station1,A)==0&&strcmp(connections[j].station2,C)==0)||
                   (strcmp(connections[j].station1,C)==0&&strcmp(connections[j].station2,A)==0)){
                    d1=connections[j].distance;
                    break;
                }
            }
            if(d1==-1)continue;

            for(int j=0;j<count;j++){
                if((strcmp(connections[j].station1,C)==0&&strcmp(connections[j].station2,B)==0)||
                   (strcmp(connections[j].station1,B)==0&&strcmp(connections[j].station2,C)==0)){
                    d2=connections[j].distance;
                    break;
                }
            }
            if(d2==-1)continue;

            if(d1+d2==directDist){
                for(int j=0;j<count;j++){
                    if((strcmp(connections[j].station1,A)==0&&strcmp(connections[j].station2,B)==0)||
                       (strcmp(connections[j].station1,B)==0&&strcmp(connections[j].station2,A)==0)){
                        mark[j]=true;
                    }
                }
                break;
            }
        }
    }

    FILE *temp=fopen("temp.txt","w");
    for(int i=0;i<count;i++){
        if(!mark[i]){
            fprintf(temp,"%s %s %.2f\n",connections[i].station1,connections[i].station2,connections[i].distance);
        }
    }
    fclose(temp);
    remove("database.txt");
    rename("temp.txt","database.txt");
}

void printDatabase(){
    FILE *fptr=fopen("database.txt","r");
    if(!fptr){
        return;
    }

    char line[20];
    while(fgets(line,sizeof(line),fptr)){
        printf("%s",line);
    }
    fclose(fptr);
    printf("\n");
}

double** returnDatabase(){
    FILE *fptr=fopen("database.txt","r");
    if(!fptr)return NULL;
    FILE *fptrnew=fopen("specs.txt","r");
    if(!fptrnew)return NULL;
    int lines = 1;
    char ch;
    while ((ch = fgetc(fptrnew)) != EOF) {
        if (ch == '\n') {
            lines++;
        }
    }
    double** returndata=(double**)malloc(3*sizeof(double*));
    double* linkdata = malloc(lines * 10 * 2 * sizeof(double));
    if (!linkdata) return NULL;
    double* kiticount = malloc(lines *  sizeof(double));
    if (!kiticount) return NULL;
    memset(kiticount,0,lines*sizeof(double));
    Connection connections[1000];
    int count=0;
    char stations[1000][4];
    int stationCount=0;
    char line[20];

    while(fgets(line,sizeof(line),fptr)){
        char s1[4],s2[4];
        float dist;
        if(sscanf(line,"%3s %3s %f",s1,s2,&dist)==3){
            strcpy(connections[count].station1,s1);
            strcpy(connections[count].station2,s2);
            connections[count].distance=dist;
            count++;

            bool hasS1=false,hasS2=false;
            for(int i=0;i<stationCount;i++){
                if(strcmp(stations[i],s1)==0)hasS1=true;
                if(strcmp(stations[i],s2)==0)hasS2=true;
            }
            if(!hasS1)strcpy(stations[stationCount++],s1);
            if(!hasS2)strcpy(stations[stationCount++],s2);
        }
    }
    fclose(fptr);
    fclose(fptrnew);
    for(int i=0;i<count;i++){
        int station1id=atoi(connections[i].station1);
        int station2id=atoi(connections[i].station2);
        double dist=connections[i].distance;
        linkdata[(int)((station1id - 1)*10*2 + kiticount[station1id - 1]*2 + 0)] = station2id;
        linkdata[(int)((station1id - 1)*10*2 + kiticount[station1id - 1]*2 + 1)] = dist;
        kiticount[station1id-1]++;
    }
    double* total = calloc(1,sizeof(double));
    *total = lines;
    returndata[0]=linkdata;
    returndata[1]=kiticount;
    returndata[2]=total;;
    //printf("\nvir  %f\n",*total);
    return returndata;
}

void addStation(char *stationName){
    FILE *fptr = fopen("specs.txt","a+");
    if(!fptr){
        return;
    }
    rewind(fptr);
    char line[20];
    int id=0;
    while(fgets(line,sizeof(line),fptr)){
        int id_current;
        if(sscanf(line,"%d",&id_current)==1){
            id = id_current;
        }
    }
    id++;
    fprintf(fptr,"%03d %s\n", id, stationName);
    fclose(fptr);   
}

void changeStationName(int stationID, char *newName){
    FILE *fptr = fopen("specs.txt","r");
    FILE *temp = fopen("temp.txt","w");
    if(!fptr || !temp){
        return;
    }
    char line[20];
    int id;
    char name[20];
    while(fgets(line,sizeof(line),fptr)){
        if(sscanf(line,"%d %19[^\n]",&id,name)==2){
            if(id==stationID){
                fprintf(temp,"%03d %s\n",id,newName);
            }
            else{
                fprintf(temp,"%s",line);
            }
        }
    }
    fclose(fptr);
    fclose(temp);
    remove("specs.txt");
    rename("temp.txt","specs.txt");  
}

void deleteStation(int stationID){
    FILE *fptr = fopen("specs.txt","r");
    FILE *temp = fopen("temp.txt","w");
    if(!fptr || !temp){
        return;
    }

    char line[20];
    int oldID, newID = 1;
    char name[20];
    while (fgets(line, sizeof(line), fptr)) {
        if (sscanf(line, "%d %19[^\n]", &oldID, name) == 2) {
            if (oldID != stationID) {
                fprintf(temp, "%03d %s\n", newID++, name);
            }
        }
    }

    fclose(fptr);
    fclose(temp);
    remove("specs.txt");
    rename("temp.txt", "specs.txt");
}
void printStations(){
    FILE *fptr=fopen("specs.txt","r");
    if(!fptr){
        return;
    }

    char line[20];
    while(fgets(line,sizeof(line),fptr)){
        printf("%s",line);
    }
    fclose(fptr);
    printf("\n");
}