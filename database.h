#ifndef DATABASE_H
#define DATABASE_H
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
void changeStationName(int stationID, char *newName);
void printStations();
#endif