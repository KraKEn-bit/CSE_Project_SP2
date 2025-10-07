#include "raylib.h"
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include<string.h>



//Two Player
//Verison Control
//Updated Port Connections
//Game start



/*
country struct:
population, dead, size(sqKm), position vector, tech, infectivity[air, water, touch, animals]

portConnected[MAX_COUNTRY][MAX_COUNTRY] and landConnected[MAX_COUNTRY][MAX_COUNTRY] = countries connected
*/

int INIT;

typedef long long ll;
typedef double db;

int mode = 0; //display mode (stats / connections) (press space)
int ff = 0; //fast forward (press F)
int adj = 0; //for balancing purposes (Show/Hide) (press Q)

int FPS = 60;
int INFECT_POINT = 0;
int CURE_POINT = 0;
double univK = 10000.0; //speed of infection, default: 500000.0

#define USA 1
#define CAN 2
#define MEX 3
#define BRA 4
#define ARG 5
#define UK 6
#define W_EU 7  
#define SCA 8   
#define C_EU 9  
#define E_EU 10 
#define RUS 11  
#define TUR 12  
#define M_E 13  
#define N_AF 14 
#define W_AF 15 
#define C_AF 16 
#define E_AF 17 
#define S_AF 18 
#define BAN 19 
#define PAK 20  
#define CN 21   
#define SE_A 22 
#define JAP 23  
#define AUS 24  
#define GRE 25  
#define CAR 26  
#define C_A 27  
#define MON 28  
#define SIB 29  
#define COL 30  
#define UKR 31  
#define EGY 32  
#define NIG 33  
#define ETH 34  
#define DRC 35  
#define IRN 36  
#define PAL 37  
#define CAU 38  
#define THI 39  
#define VIE 40  

typedef struct country{
    int number; // number of the country
    int population;
    int infected;
    int dead;
    int size; // square Km
    Vector2 position;
    int technology;
    int infectivity[4]; // air, water, touch, animals (1 to 5)
    int temperature; // avg temperature, 1 = very cold, 5 = very warm
}country;

int portConnections[40][40];
int landConnections[40][40];
int airConnections[40][40];

typedef struct FlightRoute{
    int path[5];
    int length;
}FlightRoute;

FlightRoute routes[] = {
    // South Asia to Europe via Middle East (forced transit example)
    { { BAN, M_E, UK, 0, 0 }, 3 },
    { { BAN, M_E, W_EU, 0, 0 }, 3 },

    // South Asia to Oceania/SE Asia
    { { BAN, SE_A, AUS, 0, 0 }, 3 },
    { { BAN, SE_A, VIE, 0, 0 }, 3 },

    // SE Asia to Europe via Middle East
    { { SE_A, M_E, UK, 0, 0 }, 3 },
    { { SE_A, M_E, W_EU, 0, 0 }, 3 },

    // China / East Asia corridors
    { { CN, JAP, USA, 0, 0 }, 3 },
    { { CN, RUS, USA, 0, 0 }, 3 },
    { { CN, SE_A, AUS, 0, 0 }, 3 },
    { { CN, JAP, W_EU, 0, 0 }, 3 },
    { { JAP, USA, UK, 0, 0 }, 3 },

    // Transatlantic major corridors
    { { USA, UK, 0, 0, 0 }, 2 },
    { { USA, W_EU, 0, 0, 0 }, 2 },
    { { UK, W_EU, 0, 0, 0 }, 2 },

    // Middle East as a global connector
    { { M_E, UK, 0, 0, 0 }, 2 },
    { { M_E, W_EU, 0, 0, 0 }, 2 },
    { { M_E, USA, 0, 0, 0 }, 2 },

    // Africa connections via Middle East and Europe
    { { EGY, M_E, W_EU, 0, 0 }, 3 },
    { { N_AF, M_E, UK, 0, 0 }, 3 },
    { { S_AF, M_E, UK, 0, 0 }, 3 },

    // Oceania links
    { { AUS, SE_A, JAP, 0, 0 }, 3 },
    { { AUS, SE_A, USA, 0, 0 }, 3 },
};
int routeCount = sizeof(routes) / sizeof(routes[0]);
Sound pop, appear, cureApp, curePop;
typedef struct {
    Vector2 pos;
    Vector2 start;
    Vector2 end;
    int active;
    float progress;
} Airplane;

Airplane planes[10];
Texture2D planeTex; 

float Lerp(float a, float b, float t){
    return a + (b - a) * t;
}

extern country countries[40];

void spawnPlane(int from, int to){
    // prevent duplicate 
    for(int i=0;i<10;i++){
        if(planes[i].active){
            if(planes[i].start.x == countries[from].position.x && planes[i].start.y == countries[from].position.y &&
               planes[i].end.x == countries[to].position.x && planes[i].end.y == countries[to].position.y){
                return;
            }
        }
    }
    for(int i=0;i<10;i++){
        if(planes[i].active == 0){
            planes[i].start = countries[from].position;
            planes[i].end = countries[to].position;
            planes[i].pos = planes[i].start;
            planes[i].progress = 0.0f;
            planes[i].active = 1;
            break;
        }
    }
}


void updateAndDrawPlanes(){
    float delta = GetFrameTime();
    const float baseSpeed = 800.0f;
    float speedMultiplier = 1.0f + (float)ff * 3.0f; 

    for(int i=0;i<10;i++){
        if(planes[i].active){
            float dx = planes[i].end.x - planes[i].start.x;
            float dy = planes[i].end.y - planes[i].start.y;
            float distance = sqrtf(dx*dx + dy*dy);
            if(distance < 1.0f) distance = 1.0f;

            float progressDelta = (delta * baseSpeed * speedMultiplier) / distance;
            planes[i].progress += progressDelta;
            if(planes[i].progress > 1.0f) planes[i].progress = 1.0f;

            planes[i].pos.x = Lerp(planes[i].start.x, planes[i].end.x, planes[i].progress);
            planes[i].pos.y = Lerp(planes[i].start.y, planes[i].end.y, planes[i].progress);

            float angle = atan2f(dy, dx);
            float angleDeg = angle * (180.0f / PI);
            Rectangle src = {0.0f, 0.0f, (float)planeTex.width, (float)planeTex.height};
            float scale = 0.07f; 
            float destW = planeTex.width * scale;
            float destH = planeTex.height * scale;
            Rectangle dst = {planes[i].pos.x - destW/2.0f, planes[i].pos.y - destH/2.0f, destW, destH};
            Vector2 origin = {destW/2.0f, destH/2.0f};
            DrawTexturePro(planeTex, src, (Rectangle){planes[i].pos.x, planes[i].pos.y, destW, destH}, origin, angleDeg, WHITE);

            if(planes[i].progress >= 1.0f){
                int toIndex = -1, fromIndex = -1;

                // Find destination country
                for(int c=0;c<40;c++){
                    if(countries[c].position.x == planes[i].end.x && countries[c].position.y == planes[i].end.y){
                        toIndex = c;
                    }
                    if(countries[c].position.x == planes[i].start.x && countries[c].position.y == planes[i].start.y){
                        fromIndex = c;
                    }
                }

                if(toIndex >= 0 && fromIndex >= 0){
                   
                    #define PLANE_INFECTION_PERCENT 0.001f // 0.1% of population
                    double outbreakThreshold = countries[fromIndex].population * PLANE_INFECTION_PERCENT;

                    if(countries[fromIndex].infected > outbreakThreshold){
                        if(countries[toIndex].infected == 0){
                            countries[toIndex].infected = 1; 
                        }
                    }
    
                    for(int r=0;r<routeCount;r++){
                        for(int leg=0; leg<routes[r].length-1; leg++){
                            int legFrom = routes[r].path[leg] - 1;
                            int legTo = routes[r].path[leg+1] - 1;
                            if(legFrom == -1 || legTo == -1) continue;
                            if(countries[legFrom].position.x == planes[i].start.x && countries[legFrom].position.y == planes[i].start.y &&
                               countries[legTo].position.x == planes[i].end.x && countries[legTo].position.y == planes[i].end.y){
                                if(leg+1 < routes[r].length-1){
                                    int nextFrom = routes[r].path[leg+1] - 1;
                                    int nextTo = routes[r].path[leg+2] - 1;
                                    if(nextFrom >= 0 && nextTo >= 0) spawnPlane(nextFrom, nextTo);
                                }
                            }
                        }
                    }
                }
                planes[i].active = 0;
            }
        }
    }
}



country countries[40]={
    {USA, 331000000, 0, 0, 9800000, (Vector2){300, 390}, 5, {5, 3, 4, 3}, 2},
    {CAN, 38000000, 0, 0, 9980000, (Vector2){260, 260}, 5, {4, 2, 3, 4}, 1},
    {MEX, 150000000, 0, 0, 2800000, (Vector2){300, 475}, 3, {4, 4, 4, 3}, 4},
    {BRA, 215000000, 0, 0, 8500000, (Vector2){525, 610}, 4, {5, 4, 4, 4}, 5},
    {ARG, 45000000, 0, 0, 2780000, (Vector2){465, 710}, 4, {4, 3, 3, 4}, 3},
    {UK, 67000000, 0, 0, 314000, (Vector2){720, 360}, 5, {5, 3, 4, 3}, 2},
    {W_EU, 195000000, 0, 0, 4500000, (Vector2){745, 420}, 5, {5, 3, 4, 4}, 2},
    {SCA, 27000000, 0, 0, 1030000, (Vector2){805, 285}, 5, {4, 2, 3, 4}, 1},
    {C_EU, 162000000, 0, 0, 1500000, (Vector2){820, 375}, 5, {5, 4, 4, 3}, 2},
    {E_EU, 55000000, 0, 0, 2000000, (Vector2){845, 420}, 5, {5, 3, 4, 4}, 2},
    {RUS, 144000000, 0, 0, 17000000, (Vector2){1120, 275}, 5, {4, 2, 3, 4}, 1},
    {TUR, 82000000, 0, 0, 1000000, (Vector2){915, 445}, 4, {4, 3, 4, 3}, 3},
    {M_E, 191000000, 0, 0, 3500000, (Vector2){955, 525}, 4, {5, 4, 4, 4}, 4},
    {N_AF, 103000000, 0, 0, 5000000, (Vector2){775, 490}, 3, {4, 4, 4, 3}, 4},
    {W_AF, 215000000, 0, 0, 5000000, (Vector2){710, 550}, 2, {4, 5, 4, 4}, 5},
    {C_AF, 62000000, 0, 0, 3300000, (Vector2){830, 570}, 2, {4, 5, 4, 4}, 5},
    {E_AF, 400000000, 0, 0, 3000000, (Vector2){900, 555}, 2, {5, 5, 4, 5}, 5},
    {S_AF, 154000000, 0, 0, 1220000, (Vector2){845, 730}, 3, {4, 4, 3, 4}, 4},
    {BAN, 1630000000, 0, 0, 3583000, (Vector2){1100, 515}, 3, {5, 5, 5, 4}, 5},
    {PAK, 282000000, 0, 0, 880000, (Vector2){1045, 475}, 3, {5, 4, 4, 3}, 4},
    {CN, 1439000000, 0, 0, 9600000, (Vector2){1200, 465}, 4, {5, 4, 4, 4}, 2},
    {SE_A, 650000000, 0, 0, 4300000, (Vector2){1240, 615}, 3, {5, 4, 5, 4}, 5},
    {JAP, 201000000, 0, 0, 600000, (Vector2){1350, 460}, 5, {5, 3, 4, 3}, 3},
    {AUS, 31000000, 0, 0, 7700000, (Vector2){1325, 735}, 5, {4, 3, 3, 4}, 4},
    {GRE, 56000, 0, 0, 2160000, (Vector2){615, 215}, 5, {3, 2, 2, 2}, 1},
    {CAR, 44000000, 0, 0, 300000, (Vector2){440, 490}, 3, {5, 4, 4, 3}, 5},
    {C_A, 72000000, 0, 0, 4300000, (Vector2){1040, 420}, 3, {4, 3, 3, 4}, 3},
    {MON, 3300000, 0, 0, 1560000, (Vector2){1250, 400}, 3, {4, 3, 3, 3}, 1},
    {SIB, 27000000, 0, 0, 13000000, (Vector2){1310, 275}, 4, {3, 2, 2, 3}, 1},
    {COL, 51000000, 0, 0, 1140000, (Vector2){425, 550}, 3, {4, 4, 4, 3}, 4},
    {UKR, 44000000, 0, 0, 600000, (Vector2){885, 390}, 4, {4, 3, 4, 3}, 2},
    {EGY, 161000000, 0, 0, 2863000, (Vector2){850, 505}, 3, {5, 4, 4, 3}, 4},
    {NIG, 200000000, 0, 0, 923000, (Vector2){775, 575}, 2, {5, 5, 4, 4}, 5},
    {ETH, 115000000, 0, 0, 1100000, (Vector2){935, 595}, 2, {5, 4, 4, 4}, 5},
    {DRC, 90000000, 0, 0, 2340000, (Vector2){845, 645}, 2, {4, 5, 4, 5}, 5},
    {IRN, 83000000, 0, 0, 1640000, (Vector2){985, 475}, 4, {5, 4, 4, 4}, 4},
    {PAL, 5000000, 0, 0, 6000, (Vector2){895, 490}, 4, {5, 4, 4, 3}, 4},
    {CAU, 15000000, 0, 0, 450000, (Vector2){990, 435}, 4, {4, 3, 4, 3}, 3},
    {THI, 70000000, 0, 0, 513000, (Vector2){1190, 540}, 3, {5, 4, 5, 4}, 5},
    {VIE, 97000000, 0, 0, 513000, (Vector2){1230, 550}, 3, {5, 4, 5, 4}, 5}
};
const char* countryNames[]={
    "United States",
    "Canada",
    "Mexico",
    "Brazil",
    "Argentina",
    "United Kingdom",
    "Western Europe",
    "Scandinavia",
    "Central Europe",
    "Eastern Europe",
    "Russia",
    "Turkey",
    "Middle East",
    "North Africa",
    "West Africa",
    "Central Africa",
    "East Africa",
    "South Africa",
    "Bangladesh/India",
    "Pakistan/Afghanistan",
    "China",
    "Southeast Asia",
    "Japan/Korea",
    "Australia/New Zealand",
    "Greenland",
    "Caribbean",
    "Central Asia",
    "Mongolia",
    "Siberia/Far East",
    "Colombia",
    "Ukraine",
    "Egypt/Sudan",
    "Nigeria",
    "Ethiopia",
    "DR Congo",
    "Iran",
    "Palestine",
    "Caucasus",
    "Thailand",
    "Vietnam/Cambodia"
};





Color circleColor[11] = {
    {255, 0, 0, 0},
    {255, 0, 0, 25},
    {255, 0, 0, 50},
    {255, 0, 0, 75},
    {255, 0, 0, 100},
    {255, 0, 0, 125},
    {255, 0, 0, 150},
    {255, 0, 0, 175},
    {255, 0, 0, 200},
    {255, 0, 0, 225},
    {255, 0, 0, 255}
};

Color box = {75, 75, 255, 210};
Color bgcol = {75, 75, 255, 255};

int gameActive = 0;

int display(){ //updates circles for every country + displays & updates sidebar
    float rad;
    for(int num=0;num<40;num++){
        int opacity=0, ind = 0;
        if(countries[num].infected == 0){
            opacity = 0;
        }else{
            ind = (int)((((db)countries[num].infected / (db)(countries[num].population - countries[num].dead + 1))) * 205.0);
            if(ind > 205) ind = 205;
            //if(num == 0) printf("%d - ", ind);
            opacity++;
            while(ind > 0){
                ind -= (opacity * opacity);
                opacity++;
            }
            if(countries[num].population == countries[num].dead)opacity = 10;
        }

        rad = ((float)countries[num].size / 17000000.0) * 75.0 + 15.0;
        DrawCircleV(countries[num].position, rad, circleColor[opacity]);
    }
    long long init1 = 0, init2 = 0, init3 = 0;
    for(int i=0;i<40;i++){
        init1 += (long long)countries[i].population;
        init2 += (long long)countries[i].infected;
        init3 += (long long)countries[i].dead;
    }
    int init4 = (int)(((double)((init1 - init2 - init3) * 100.0) / (double)(init1)) * 100.0);
    char pop[100] = "    Population:              ";
    char inf[100] = "      Infected:             0";
    char dead[100] = "           Dead:             0";
    char healthy[100] = "Healthy:   0.00%";
    for(int i=14;init1 != 0;i--){
        if((i+1)%4 == 0 && init1 !=0)pop[14+i] = ',';
        else{
            pop[14+i] = init1 % 10 + '0';
            init1 /= 10;
        }
    }
    for(int i=14;init2 != 0;i--){
        if((i+1)%4 == 0 && init2 !=0)inf[14+i] = ',';
        else{
            inf[14+i] = init2 % 10 + '0';
            init2 /= 10;
        }
    }
    for(int i=14;init3 != 0;i--){
        if((i+1)%4 == 0 && init3 !=0)dead[15+i] = ',';
        else{
            dead[15+i] = init3 % 10 + '0';
            init3 /= 10;
        }
    }
    healthy[14] = init4%10 + '0';
    init4 /= 10;
    healthy[13] = init4%10 + '0';
    init4 /= 10;
    healthy[11] = init4%10 + '0';
    init4 /= 10;
    if(init4 > 0)healthy[10] = init4%10 + '0';
    init4 /= 10;
    if(init4 > 0)healthy[9] = init4%10 + '0';
    
    int shift = 255;
    DrawRectangle(shift - 10, 860, 1145, 40, BLUE);
    DrawText(pop, shift, 870, 20, BLACK);
    DrawText(inf, 300+shift, 870, 20, BLACK);
    DrawText(dead, 600+shift, 870, 20, BLACK);
    DrawText(healthy, 950+shift, 870, 20, BLACK);


    //sidebar
    //infect
    int pts = INFECT_POINT;
    char ptss[4] = "  0";
    ptss[2] = pts % 10 + '0';
    pts/=10;
    if(pts != 0)ptss[1] = pts % 10 + '0';
    pts/=10;
    if(pts != 0)ptss[0] = pts % 10 + '0';
    DrawRectangle(1620, 20, 160, 60, (Color){200, 0, 200, 255});
    DrawRectangleLines(1620, 20, 160, 60, BLACK);
    DrawText(ptss, 1670, 32, 40, BLACK);
    //cure
    pts = CURE_POINT;
    char ptss1[4] = "  0";
    ptss1[2] = pts % 10 + '0';
    pts/=10;
    if(pts != 0)ptss1[1] = pts % 10 + '0';
    pts/=10;
    if(pts != 0)ptss1[0] = pts % 10 + '0';
    DrawRectangle(1620, 480, 160, 60, (Color){0, 255, 0, 255});
    DrawRectangleLines(1620, 480, 160, 60, BLACK);
    DrawText(ptss1, 1670, 492, 40, BLACK);
    
}

int printMousePos(Vector2 mousePos){
    char mPos[10] = "0000, 000";
    int iPos[2] = {(int)mousePos.x, (int)mousePos.y};
    for(int i=0;i<4;i++){
        mPos[3-i] = iPos[0] % 10 + '0';
        mPos[8-i] = iPos[1] % 10 + '0';
        iPos[0] /= 10;
        iPos[1] /= 10;
    }
    DrawText(mPos, mousePos.x - 50, mousePos.y - 6, 10, BLACK);
}

int displayConnections(Vector2 mousePos){
    for(int num=0;num<40;num++){
        Vector2 diff;
        diff.x = countries[num].position.x - mousePos.x;
        diff.y = countries[num].position.y - mousePos.y;
        double len = sqrt(diff.x*diff.x + diff.y*diff.y);

        float rad = ((float)countries[num].size / 17098242.0) * 75.0 + 15.0;

        if(len < rad){
            for(int to=0;to<40;to++){
                if(to == num)continue;
                else{
                    if(landConnections[num][to] == 1 || portConnections[num][to] == 1 || airConnections[num][to] == 1){
                        if(landConnections[num][to] == 1) DrawLineV(countries[num].position, countries[to].position, RED);
                        else if(portConnections[num][to] == 1) DrawLineV(countries[num].position, countries[to].position, BLUE);
                        else DrawLineV(countries[num].position, countries[to].position, (Color){150,0,200,255});
                    }
                }
            }
        }
    }
}

int hover(Vector2 mousePos){
    for(int num=0;num<40;num++){
        Vector2 diff;
        diff.x = countries[num].position.x - mousePos.x;
        diff.y = countries[num].position.y - mousePos.y;
        double len = sqrt(diff.x*diff.x + diff.y*diff.y);
        float rad = ((float)countries[num].size / 17000000.0) * 75.0 + 15.0;
        if(len < rad){
            char pop[100] = "    Population:              ";
            char inf[100] = "      Infected:             0";
            char dead[100] = "           Dead:             0";
            long long init1 = countries[num].population, init2 = countries[num].infected, init3 = countries[num].dead;
            for(int i=14;init1 != 0;i--){
                if((i+1)%4 == 0 && init1 !=0)pop[14+i] = ',';
                else{
                    pop[14+i] = init1 % 10 + '0';
                    init1 /= 10;
                }
            }
            for(int i=14;init2 != 0;i--){
                if((i+1)%4 == 0 && init2 !=0)inf[14+i] = ',';
                else{
                    inf[14+i] = init2 % 10 + '0';
                    init2 /= 10;
                }
            }
            for(int i=14;init3 != 0;i--){
                if((i+1)%4 == 0 && init3 !=0)dead[15+i] = ',';
                else{
                    dead[15+i] = init3 % 10 + '0';
                    init3 /= 10;
                }
            }
            int nameShift = ( 21 - strlen(countryNames[num]) ) * 6;
            int leftShift = 100;
            int upShift = 35;
            DrawRectangle((int)countries[num].position.x - 10 - leftShift, (int)countries[num].position.y - 95 - upShift, 255, 105, box);
            DrawText(countryNames[num], (int)countries[num].position.x - leftShift + nameShift, (int)countries[num].position.y - 85 - upShift, 20, BLACK);
            DrawText(pop, (int)countries[num].position.x - leftShift, (int)countries[num].position.y - 60 - upShift, 15, BLACK);
            DrawText(inf, (int)countries[num].position.x - leftShift, (int)countries[num].position.y - 40 - upShift, 15, BLACK);
            DrawText(dead, (int)countries[num].position.x - leftShift, (int)countries[num].position.y - 20 - upShift, 15, BLACK);

            INIT = num;
            if(!gameActive && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                countries[num].infected = 1;
                gameActive = 1;
            }
        }
    }
}





typedef struct virus{
    int infectionPotential[4]; // Infection through [air, water, touch, animals] in (1-3)
    int temperatureAdapt[2]; // [Cold, Hot] weather in (1-5)
    int killPotential; // (1-5)
    int cureResist; // (1-5)
}virus;

typedef struct cure{
    float progress; //(0-10000), will be displayed as (0-100)%
    int effort; //speed of progress and cureProgress; (0-5), where 0 = 1x speed and 5 = 2x speed
    int combatInfection; //(0-5), will only be effective after "progress" reaches 100%
    int combatDeath; //(0-5), will only be effective after "progress" reaches 100%
    float cureProgress; //(0-30000) progress of cure, once it reaches 100%, it's game over, can only start after "progress" has reached 100%
}cure;

virus v1 = {{1, 1, 1, 1}, {1, 1}, 1, 1};
cure cure1 = {0, 0, 0, 0, 0};
//virus upgrade costs: 2x4 -> 3x4 -> 4x4 ......7x4 (total upgrades = 24)(points required = 108)
//cure upgrade costs: 5x3-> 6x3 -> 7x3 ..... 9x3 (total upgrades = 15)(points required = 105)
int vUpgradeNum = 0, vNextCost;
int cureUpgradeNum = 0, cureNextCost;

int button(int x, int y, int text1, int text2, int num, Color col){ //displays button on the sidebar, if clicked, returns "num", else returns 0
    //Draw
    DrawRectangle(x-30, y-15, 60, 30, col);
    DrawRectangleLines(x-30, y-15, 60, 30, BLACK);
    char c[7] = "0 (0)";
    c[0] = text1 + '0';
    c[3] = text2 + '0';
    DrawText(c, x-27, y-12, 24, BLACK);

    //input check
    Vector2 p = GetMousePosition();
    int mul = 0;
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        if(p.x>=(x-30) && p.x<=(x+30) && p.y>=(y-15) && p.y<=(y+15)){
            mul = 1;
        }
    }

    return mul * num;
}

int currentKey = 0;
int currentKeyTime = 0;
int generateNextKey = 1;
int randCountry=-1;
//generateKey: 1=yes, 2-6 = (next key to be generated) - 1;
int cureButton(int x, int y, int text1, int text2, int num, Color col){ //displays button on the sidebar, if clicked, returns "num", else returns 0
    //Draw
    DrawRectangle(x-30, y-15, 60, 30, col);
    DrawRectangleLines(x-30, y-15, 60, 30, BLACK);
    char c[7] = "0 (0)";
    c[0] = text1 + '0';
    c[3] = text2 + '0';
    DrawText(c, x-27, y-12, 24, BLACK);

    //input check
    char key = -1;
    if(IsKeyPressed(KEY_KP_1) || IsKeyPressed(KEY_ONE)) key = 1;
    if(IsKeyPressed(KEY_KP_2) || IsKeyPressed(KEY_TWO)) key = 2;
    if(IsKeyPressed(KEY_KP_3) || IsKeyPressed(KEY_THREE)) key = 3;
    if(IsKeyPressed(KEY_KP_4) || IsKeyPressed(KEY_FOUR)) key = 4;
    if(IsKeyPressed(KEY_KP_5) || IsKeyPressed(KEY_FIVE)) key = 5;
    if(key == currentKey){
        PlaySound(curePop);
        currentKey = 0;
        currentKeyTime = 0;
        generateNextKey = 1;
        CURE_POINT++;
    }
    key = 0;
    if(IsKeyPressed(KEY_KP_7) || IsKeyPressed(KEY_SEVEN)) key = 7;
    if(IsKeyPressed(KEY_KP_8) || IsKeyPressed(KEY_EIGHT)) key = 8;
    if(IsKeyPressed(KEY_KP_9) || IsKeyPressed(KEY_NINE)) key = 9;
    if(key > 1) return key;
    else return -1;
}

int upgradeManage(){
    int vNextCost = 2 + (vUpgradeNum / 4);
    int cureNextCost = 5 + (cureUpgradeNum / 3);
    int temp=0, x;
    int tempMax, tempV[8] = {
        v1.infectionPotential[0],
        v1.infectionPotential[1],
        v1.infectionPotential[2],
        v1.infectionPotential[3],
        v1.temperatureAdapt[0],
        v1.temperatureAdapt[1],
        v1.killPotential,
        v1.cureResist
    };
    char text[20][20] = {
        "     Air:",
        " Water:",
        " Touch:",
        "Animals:",
        "   Cold:",
        "    Hot:",
        "     Kill:",
        " Resist:"
    };
    
    //display
    char v[10] = "Cost = 0";
    v[7] = vNextCost + '0';
    char c[10] = "Cost = 0";
    c[7] = cureNextCost + '0';
    DrawText(v,1630,90,30,BLACK);
    DrawText(c,1630,550,30,BLACK);

    //update v
    for(int i=0;i<8;i++){
        if(i<=3) tempMax = 3;
        else tempMax = 5;
        DrawText(text[i], 1620, 130+35*i, 25, BLACK);
        if((x = button(1750, 140+35*i, tempV[i], tempMax, i+1, (Color){200,0,200,255}))){
            temp = x;
        }
        if(temp == i+1 && tempV[i] == tempMax) temp = 0;
    }
    if(INFECT_POINT >= vNextCost && temp){
        INFECT_POINT -= vNextCost;
        vUpgradeNum++;
    }
    else temp = 0;
    if(temp == 0);
    else if(temp <= 4)v1.infectionPotential[temp-1]++;
    else if(temp <= 6)v1.temperatureAdapt[temp-5]++;
    else if(temp == 7)v1.killPotential++;
    else v1.cureResist++;


    //For Cure
    temp = 0;
    int tempC[3]={
        cure1.effort,
        cure1.combatInfection,
        cure1.combatDeath
    };
    char text2[3][20]={
        "[7] Effort:",
        "[8] Infect:",
        "[9] Death:"
    };
    for(int i=0;i<3;i++){
        DrawRectangle(1620, 595+35*i, 19, 17, (Color){0,255,0,255});
        DrawText(text2[i], 1620, 595+35*i, 19, BLACK);
        if((x = cureButton(1750, 605+35*i, tempC[i], 5, i+1, (Color){0,255,0,255}))){
            temp = x;
        }
    }
    if(x > 0 && CURE_POINT >= cureNextCost){
        if(x == 7 && cure1.effort < 5){
            cure1.effort++;
            CURE_POINT -= cureNextCost;
            cureUpgradeNum++;
        }
        if(x == 8 && cure1.combatInfection < 5){
            cure1.combatInfection++;
            CURE_POINT -= cureNextCost;
            cureUpgradeNum++;
        }
        if(x == 9 && cure1.combatDeath < 5){
            cure1.combatDeath++;
            CURE_POINT -= cureNextCost;
            cureUpgradeNum++;
        }
    }
}


int inc = 1;
int updateStats(){ //for debugging purposes
    //display
    DrawRectangle(45, 795, 425, 105, (Color){0,0,255,100});
    DrawText("Mediums: 1=air, 2=water, 3=touch, 4=anim", 50, 800, 20, BLACK);
    DrawText("Temp: 5=cold, 6=Warm", 50, 820, 20, BLACK);
    DrawText("7=kill, 8=resist, 9=Toggle", 50, 840, 20, BLACK);
    char text[100] = "[0,0,0,0],[0,0],0,0,(+)";
    int st[8] = {
        v1.infectionPotential[0],
        v1.infectionPotential[1],
        v1.infectionPotential[2],
        v1.infectionPotential[3],
        v1.temperatureAdapt[0],
        v1.temperatureAdapt[1],
        v1.killPotential,
        v1.cureResist,
    };
    text[1] = st[0] + '0';
    text[3] = st[1] + '0';
    text[5] = st[2] + '0';
    text[7] = st[3] + '0';
    text[11] = st[4] + '0';
    text[13] = st[5] + '0';
    text[16] = st[6] + '0';
    text[18] = st[7] + '0';
    if(inc == 1)text[21] = '+';
    if(inc == -1)text[21] = '-';
    DrawText(text, 50, 860, 20, BLACK);

    //update
    if(IsKeyPressed(KEY_KP_1)) v1.infectionPotential[0] += inc;
    if(IsKeyPressed(KEY_KP_2)) v1.infectionPotential[1] += inc;
    if(IsKeyPressed(KEY_KP_3)) v1.infectionPotential[2] += inc;
    if(IsKeyPressed(KEY_KP_4)) v1.infectionPotential[3] += inc;
    if(IsKeyPressed(KEY_KP_5)) v1.temperatureAdapt[0] += inc;
    if(IsKeyPressed(KEY_KP_6)) v1.temperatureAdapt[1] += inc;
    if(IsKeyPressed(KEY_KP_7)) v1.killPotential += inc;
    if(IsKeyPressed(KEY_KP_8)) v1.cureResist += inc;
    if(IsKeyPressed(KEY_KP_9)) inc = (-1) * inc;

    for(int i=0;i<4;i++){
        if(v1.infectionPotential[i] == 0)v1.infectionPotential[i] = 1;
        if(v1.infectionPotential[i] == 4)v1.infectionPotential[i] = 3;
    }
    for(int i=0;i<2;i++){
        if(v1.temperatureAdapt[i] == 0)v1.temperatureAdapt[i] = 1;
        if(v1.temperatureAdapt[i] == 6)v1.temperatureAdapt[i] = 5;
    }
    if(v1.killPotential == 0) v1.killPotential = 1;
    if(v1.killPotential == 6) v1.killPotential = 5;
    if(v1.cureResist == 0) v1.cureResist = 1;
    if(v1.cureResist == 6) v1.cureResist = 5;
}




int infTime = 45, updateTimers=0, x1=0;
int activationOrder[130], timeOrder[130];
int currentlyActive = 0;
int current = 0;
int infPoints[40][8]; // 0-2 = points remaining from a country = {1st Infection, time, 70%}
//3 = delay before the next bubble can be active (15 x FPS)
//4 = timer for "time" point, starts as soon as first inf (infTime x FPS)
//5-7 = can bubbles 0-2 be activated? can be only be activated if 3 is 0

int displayBubble(int countryNum, Texture2D x, int current){ // only for infection points
    float rad = 25.0;
    DrawCircleV(countries[countryNum].position, rad, (Color){200,0,200,255});
    // Center and scale skull to fit neatly inside the circle
    float texW = (float)x.width;
    float texH = (float)x.height;
    float diameter = rad * 2.0f;
    float maxSide = (texW > texH) ? texW : texH;
    float scale = (diameter * 0.85f) / maxSide; // 85% of circle diameter for padding
    float destW = texW * scale;
    float destH = texH * scale;
    Rectangle src = {0.0f, 0.0f, texW, texH};
    Rectangle dst = {countries[countryNum].position.x - destW/2.0f, countries[countryNum].position.y - destH/2.0f, destW, destH};
    Vector2 origin = {0.0f, 0.0f};
    DrawTexturePro(x, src, dst, origin, 0.0f, WHITE);
    DrawCircleLinesV(countries[countryNum].position, rad, BLACK);

    Vector2 mousePos = GetMousePosition(), diff;
    diff.x = countries[countryNum].position.x - mousePos.x;
    diff.y = countries[countryNum].position.y - mousePos.y;
    double len = sqrt(diff.x*diff.x + diff.y*diff.y);
    if(len < rad && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        timeOrder[current] = 2 * FPS;
        INFECT_POINT++;
        PlaySound(pop);
    }
}

int addIndex = 0;
void add(int num){ //used by "pointsUpdate", adds a country to the queue
    activationOrder[addIndex] = num;
    timeOrder[addIndex++] = (6 + 2) * FPS; //a 2-second delay between two bubbles
    infPoints[num][3] = 15*FPS;
}

int pointsUpdate(){ //keeps track of points remaing and popping up from each country
    int addTime = 1 + ff*3;
    for(int num=0;num<40;num++){
        //can (point) be activated?
        double ratio = (db)(countries[num].infected + countries[num].dead) / (double)countries[num].population;
        if(countries[num].infected > 0)infPoints[num][5] = 1;
        if(infPoints[num][4] >= infTime*FPS)infPoints[num][6] = 1;
        if(ratio >= 0.7)infPoints[num][7] = 1;
        
        //activate
        if(infPoints[num][3] <= 0 && infPoints[num][0] && infPoints[num][5]){
            infPoints[num][0] = 0;
            add(num);
        }
        if(infPoints[num][3] <= 0 && infPoints[num][1] && infPoints[num][6]){
            infPoints[num][1] = 0;
            add(num);
        }
        if(infPoints[num][3] <= 0 && infPoints[num][2] && infPoints[num][7]){
            infPoints[num][2] = 0;
            add(num);
        }

        //update timers
        if(infPoints[num][3] > 0)infPoints[num][3]-=addTime;
        if(infPoints[num][4] < infTime*FPS && infPoints[num][5] == 1 )infPoints[num][4]+=addTime;
    }

    //For cure
    if(currentKeyTime > 0) currentKeyTime-=addTime;
    if(currentKeyTime <= 0){
        currentKey = 0;
        generateNextKey = 1;
    }
    if(generateNextKey == 1 && currentKey == 0 && currentKeyTime <= 0){
        currentKeyTime = (25 + (rand() % 11)) * FPS;
        generateNextKey = (rand() % 5) + 2;
        //generate random country
        randCountry = -1;
        int totalTech=0;
        for(int n=0;n<40;n++){
            totalTech += countries[n].technology;
        }
        int temp = (rand() % totalTech) + 1;
        while(temp >= 0){
            temp -= countries[randCountry + 1].technology;
            randCountry++;
        }
    }
    if(currentKeyTime <= 2 * FPS * addTime){
        if(currentKey != generateNextKey - 1) PlaySound(cureApp);
        currentKey = generateNextKey - 1;
    }

}

int trackBubble(Texture2D x){
    int temp;
    for(int i=0;i<3;i++){
        temp = i + current;
        if(timeOrder[temp] == FPS * 8)PlaySound(appear);
        if(timeOrder[temp] > FPS * 2){
            displayBubble(activationOrder[temp], x, temp);
        }
        if(timeOrder[temp] > 0)timeOrder[temp]--;
    }
    while(timeOrder[current] <= 0 && current < addIndex){
        current++;
    }
}


int cureBubble(int cur){
    if(cur > 0){
        char textB[10] = "0";
        textB[0] = cur + '0';
        int x = countries[randCountry].position.x, y=countries[randCountry].position.y - 55;
        Vector2 v1 = {x-15, y};
        Vector2 v2 = {x+15, y};
        Vector2 v3 = countries[randCountry].position;
        DrawTriangle(v2,v1,v3,WHITE);
        DrawCircle(x, y, 25, (Color){0,255,0,255});
        DrawCircleLines(x, y, 25, BLACK);
        DrawText(textB, x-5, y-12, 30, BLACK);
    }
}


int updateInfection(){
    //update array [add to "infected", add to "dead"];
    ll tempArray[40][2];
    cure vaccine = cure1;
    if(vaccine.progress < 10000.0){
        vaccine.combatInfection = 0;
        vaccine.combatDeath = 0;
    }

    for(int num=0; num<40; num++){

        int randInt = rand() % 101;
        db infK = ((db)(400 - randInt) / 400.0) + 1.0; // 1.75 - 2.00

        db infMedium = 0;
        db totalIP=0;
        for(int type=0; type<4; type++){
            infMedium += (db)(countries[num].infectivity[type] * v1.infectionPotential[type]);
            totalIP+=v1.infectionPotential[type];
        }
        infMedium = (infMedium / 60.0) + 1.0; //1.0-2.0
        totalIP /= 12;
        infMedium = infMedium + (2.0-infMedium)*totalIP;
        infMedium *= infMedium * infMedium;

        db infTemperature = ((db)(v1.temperatureAdapt[0] / 5.0) * (6 - countries[num].temperature));
        infTemperature += ((db)(v1.temperatureAdapt[1] / 5.0) * countries[num].temperature);
        infTemperature = (infTemperature / 6.0) + 1.0; // 1=cold, 5=warm, 1.0-2.0
        infTemperature *= infTemperature * infTemperature;

        db healthy = (db)(countries[num].population - countries[num].infected - countries[num].dead) / 10000.0;
        db infected = (db)countries[num].infected / 10000.0;
        db alive = (db)(countries[num].population - countries[num].dead) / 10000.0;
        db pop = (countries[num].population) / 10000.0;

        db density = (alive * 10000) / (db)countries[num].size; // density of people that are still alive
        density = (density / 100.0) + 10.0;

        //db mul = (infected * healthy) / univK;
        db mul = (infected * healthy) / (pop * pow(pop, 0.6));
        if(countries[num].infected >= 1) mul += 1.0;
        mul = mul * mul * ((pop / 75000.0) + 1.0);
        
        db cure = 2.25 - ( ((db)vaccine.combatInfection / (db)v1.cureResist) + 5.0 ) * 0.25;


        tempArray[num][0] = (ll)(infK * infMedium * infTemperature * density * mul * cure * 1.4);
        //if(num == INIT) printf("\nk%lf M%lf T%lf d%lf m%lf c%lf pop%.2lf tot%d\n", infK, infMedium, infTemperature, density, mul, cure, pop, tempArray[num][0]);


        db totalInfected = (db)(countries[num].infected + tempArray[num][0]);
        if((totalInfected + countries[num].dead) > countries[num].population){
            totalInfected = (db)(countries[num].population - countries[num].dead);
            tempArray[num][0] = countries[num].population - countries[num].dead - countries[num].infected;
        }

        //calculate death
        db death = 0;
        db d = v1.killPotential;
        d *= d*d;
        pop = (pop / 50000.0) + 1.0;
        if(countries[num].infected == 0) d = 0;
        d = d * (1 - (healthy / (countries[num].population / 10000.0)));
        db k = ( ((db)countries[num].population - totalInfected) / (db)countries[num].population ) * 0.5 + 0.5;
        if(randInt <= ((v1.killPotential - vaccine.combatDeath) * 4 + 10)){
            death = (k * totalInfected * 0.0001 * (((db)v1.killPotential / (db)(vaccine.combatDeath+1)) * 4.0 + 5.0)) + d*pop;
        }

        //Check for spread
        int chance = (int)(100.0 * (totalInfected / countries[num].population));
        int checkCond = 0;
        if(countries[num].infected > 0) checkCond = 1;
        for(int check = 0; check < 40; check++){
            if(checkCond && (landConnections[num][check] == 1 || portConnections[num][check] == 1)){
                int randF = (rand() % 51) + (rand() % 51);
                if(randF <= chance){
                    countries[check].infected += 1;
                }
            }
            if(checkCond && airConnections[num][check] == 1){
                int randF = (rand() % 51) + (rand() % 51);
                if(randF <= chance){
                    spawnPlane(num, check);
                }
            }
        }

        tempArray[num][1] = (ll)death;
        tempArray[num][0] = tempArray[num][0] - tempArray[num][1];
    }

    //Add & Adjust
    for(int num=0; num<40; num++){
        countries[num].infected += tempArray[num][0];
        countries[num].dead += tempArray[num][1];
        if(countries[num].infected + countries[num].dead > countries[num].population){
            countries[num].infected = countries[num].population - countries[num].dead;
        }
        if(countries[num].dead > countries[num].population) countries[num].dead = countries[num].population;
        if(countries[num].infected < 0){
            countries[num].infected = 0;
        }
    }
}

int temp = 0;
void displayCure(){
    DrawRectangle(500, 0, 600, 50, box);
    char prog[100] = "Cure Progress:   0%";
    char prog2[100] = "Cure Distributed:   0%";

    if(temp > 0){
        temp = 0;
    }

    int p = (int)cure1.progress / 100;
    prog[17] = p % 10 + '0';
    p /= 10;
    if(p != 0)prog[16] = p % 10 + '0';
    p /= 10;
    if(p != 0)prog[15] = p % 10 + '0';
    int p2 = (int)cure1.cureProgress / 300;
    prog2[20] = p2 % 10 + '0';
    p2 /= 10;
    if(p2 != 0)prog2[19] = p2 % 10 + '0';
    p2 /= 10;
    if(p2 != 0)prog2[18] = p2 % 10 + '0';

    if(cure1.progress < 10000){
        DrawText(prog, 605, 5, 40, BLACK);
    }
    else{
        DrawText(prog2, 605, 5, 40, BLACK);
    }
}



int CONNECT_LAND(int a, int b){
    landConnections[a-1][b-1] = 1;
    landConnections[b-1][a-1] = 1;
}
int CONNECT_PORT(int a, int b){
    portConnections[a-1][b-1] = 1;
    portConnections[b-1][a-1] = 1;
}
int CONNECT_AIR(int a, int b){
    airConnections[a-1][b-1] = 1;
    airConnections[b-1][a-1] = 1;
}


int pause = 0;


int main(){
    //LAND
    CONNECT_LAND(USA,CAN);
    CONNECT_LAND(USA,MEX);
    CONNECT_LAND(MEX,COL);
    CONNECT_LAND(BRA,ARG);
    CONNECT_LAND(BRA,COL);
    CONNECT_LAND(UK,W_EU);
    CONNECT_LAND(W_EU,C_EU);
    CONNECT_LAND(W_EU,E_EU);
    CONNECT_LAND(SCA,RUS);
    CONNECT_LAND(C_EU,UKR);
    CONNECT_LAND(C_EU,E_EU);
    CONNECT_LAND(C_EU,RUS);
    CONNECT_LAND(E_EU,UKR);
    CONNECT_LAND(RUS,SIB);
    CONNECT_LAND(RUS,MON);
    CONNECT_LAND(RUS,C_A);
    CONNECT_LAND(RUS,CN);
    CONNECT_LAND(RUS,TUR);
    CONNECT_LAND(TUR,IRN);
    CONNECT_LAND(TUR,M_E);
    CONNECT_LAND(TUR,PAL);
    CONNECT_LAND(M_E,IRN);
    CONNECT_LAND(N_AF,W_AF);
    CONNECT_LAND(N_AF,EGY);
    CONNECT_LAND(N_AF,NIG);
    CONNECT_LAND(N_AF,C_AF);
    CONNECT_LAND(W_AF,NIG);
    CONNECT_LAND(C_AF,EGY);
    CONNECT_LAND(C_AF,E_AF);
    CONNECT_LAND(C_AF,DRC);
    CONNECT_LAND(C_AF,NIG);
    CONNECT_LAND(E_AF,ETH);
    CONNECT_LAND(E_AF,EGY);
    CONNECT_LAND(S_AF,DRC);
    CONNECT_LAND(BAN,PAK);
    CONNECT_LAND(BAN,CN);
    CONNECT_LAND(BAN,THI);
    CONNECT_LAND(PAK,C_A);
    CONNECT_LAND(PAK,CAU);
    CONNECT_LAND(CN,MON);
    CONNECT_LAND(CN,C_A);
    CONNECT_LAND(SE_A,THI);
    CONNECT_LAND(JAP,SIB);
    CONNECT_LAND(C_A,CAU);
    CONNECT_LAND(MON,SIB);
    CONNECT_LAND(EGY,PAL);
    CONNECT_LAND(NIG,DRC);
    CONNECT_LAND(ETH,DRC);
    CONNECT_LAND(IRN,PAL);
    CONNECT_LAND(IRN,CAU);
    CONNECT_LAND(IRN,PAK);
    CONNECT_LAND(THI,VIE);

    //PORT
    CONNECT_PORT(USA,S_AF);
    CONNECT_PORT(USA,AUS);
    CONNECT_PORT(USA,JAP);
    CONNECT_PORT(USA,E_EU);
    CONNECT_PORT(CAN,SCA);
    CONNECT_PORT(CAN,ARG);
    CONNECT_PORT(CAN,VIE);
    CONNECT_PORT(MEX,IRN);
    CONNECT_PORT(MEX,UK);
    CONNECT_PORT(MEX,NIG);
    CONNECT_PORT(BRA,W_EU);
    CONNECT_PORT(BRA,DRC);
    CONNECT_PORT(ARG,EGY);
    CONNECT_PORT(ARG,GRE);
    CONNECT_PORT(UK,GRE);
    CONNECT_PORT(UK,CN);
    CONNECT_PORT(UK,CAR);
    CONNECT_PORT(W_EU,AUS);
    CONNECT_PORT(W_EU,ETH);
    CONNECT_PORT(SCA,NIG);
    CONNECT_PORT(SCA,BAN);
    CONNECT_PORT(SCA,COL);
    CONNECT_PORT(C_EU,W_AF);
    CONNECT_PORT(C_EU,RUS);
    CONNECT_PORT(E_EU,BAN);
    CONNECT_PORT(RUS,BAN);
    CONNECT_PORT(RUS,BRA);
    CONNECT_PORT(RUS,CAN);
    CONNECT_PORT(TUR,M_E);
    CONNECT_PORT(M_E,S_AF);
    CONNECT_PORT(M_E,CAR);
    CONNECT_PORT(N_AF,AUS);
    CONNECT_PORT(N_AF,JAP);
    CONNECT_PORT(W_AF,CAU);
    CONNECT_PORT(C_AF,MEX);
    CONNECT_PORT(C_AF,COL);
    CONNECT_PORT(E_AF,VIE);
    CONNECT_PORT(E_AF,CN);
    CONNECT_PORT(S_AF,PAK);
    CONNECT_PORT(BAN,AUS);
    CONNECT_PORT(CN,USA);
    CONNECT_PORT(CN,JAP);
    CONNECT_PORT(CN,CAR);
    CONNECT_PORT(SE_A,PAK);
    CONNECT_PORT(SE_A,COL);
    CONNECT_PORT(JAP,BRA);
    CONNECT_PORT(JAP,E_EU);
    CONNECT_PORT(AUS,UK);
    CONNECT_PORT(CAR,USA);
    CONNECT_PORT(C_A,NIG);
    CONNECT_PORT(C_A,THI);
    CONNECT_PORT(UKR,MEX);
    CONNECT_PORT(EGY,IRN);
    CONNECT_PORT(EGY,THI);
    CONNECT_PORT(IRN,RUS);
    CONNECT_PORT(IRN,ARG);
    CONNECT_PORT(IRN,PAL);
    CONNECT_PORT(CAU,VIE);
    CONNECT_PORT(THI,CAR);
    CONNECT_PORT(VIE,C_EU);

    InitAudioDevice();

    appear = LoadSound("bubble_ind.ogg");
    pop = LoadSound("bubble_pop.ogg");
    cureApp = LoadSound("cure_ind.ogg");
    curePop = LoadSound("cure_pop.ogg");
    // AIR (multi-leg routes with required transits)
    for(int r=0; r<routeCount; r++){
        for(int i=0; i<routes[r].length-1; i++){
            int from = routes[r].path[i];
            int to = routes[r].path[i+1];
            if(from > 0 && to > 0){
                CONNECT_AIR(from, to);
            }
        }
    }

    for(int i=0;i<40;i++){ //Initialize infPoints
        for(int j=0;j<8;j++){
            if(j <= 2)infPoints[i][j] = 1;
            else infPoints[i][j] = 0;
        }
    }

    srand(time(NULL));
    InitWindow(1800, 900, "Outbreak");
    Texture2D skull = LoadTexture("skull.png");
    GenTextureMipmaps(&skull);
    SetTextureFilter(skull, TEXTURE_FILTER_BILINEAR);
    planeTex = LoadTexture("plane.png");
    GenTextureMipmaps(&planeTex);
    SetTextureFilter(planeTex, TEXTURE_FILTER_BILINEAR);
    Texture2D map = LoadTexture("worldMap.png");
    SetTargetFPS(FPS);
    int i = 0;
    while(!WindowShouldClose()){
        if(IsKeyPressed(KEY_SPACE) && gameActive == 1) pause = 1 - pause;
        if(cure1.cureProgress >= 30000) gameActive = 2;
        ll totalP = 0, totalD = 0;
        for(int n=0;n<40;n++){
            totalP += countries[n].population;
            totalD += countries[n].dead;
        }
        if(totalP == totalD) gameActive = 3;
        BeginDrawing();
        if(pause == 0 && gameActive < 2){
            ClearBackground(bgcol);
            DrawTexture(map, 0, 0, WHITE);
            DrawRectangle(1610, 10, 180, 880, (Color){200,200,255,255});
            DrawRectangle(1610, 460, 180, 10, bgcol);
            display();
            
            if(!gameActive){
                DrawRectangle(575, 15, 450, 60, box);
                DrawText("Select a Country", 580, 20, 50, BLACK);
                hover(GetMousePosition());
            }
            else if(gameActive == 1){
                displayCure();
                float add = 3.0 * (1.0 + cure1.effort*0.2);
                if(i >= FPS/(3 + ff*9)){
                    updateInfection();
                    if(!temp && cure1.progress < 10000)cure1.progress += add;
                    else if(!temp)cure1.cureProgress += add;
                    temp++;
                    i=0;
                }
                pointsUpdate();
                for(int r=0; r<routeCount; r++){
                    int from = routes[r].path[0] - 1;
                    int to = routes[r].path[1] - 1; // first leg only
                    if(from >= 0 && from < 40 && to >= 0 && to < 40){
                        if(countries[from].infected > 100){
                            if((rand() % 1000) < 5){
                                spawnPlane(from, to);
                            }
                        }
                    }
                }
                i++;
                if(mode == 0)hover(GetMousePosition());
                else displayConnections(GetMousePosition());
            }
            if(ff && gameActive){
                Vector2 p1[3] = {{750,50},{750,110},{795,80}};
                Vector2 p2[3] = {{805,50},{805,110},{850,80}};
                DrawTriangle(p1[0], p1[1], p1[2], WHITE);
                DrawTriangle(p2[0], p2[1], p2[2], WHITE);
            }
            trackBubble(skull);
            if(gameActive == 1) updateAndDrawPlanes();
            cureBubble(currentKey);
            upgradeManage();
            if(adj)updateStats();
            if(IsKeyPressed(KEY_Q))adj = 1 - adj;
            printMousePos(GetMousePosition());
            if(IsKeyPressed(KEY_C) && gameActive) mode = 1 - mode;
            if(IsKeyPressed(KEY_F) && gameActive) ff = 1 - ff;
        }else if(pause == 1 && gameActive < 2){
            ClearBackground((Color){0,0,80,255});
            DrawText("GAME PAUSED", 1100, 200, 60, WHITE);
            DrawText("HEALTHY:", 20, 20, 25, (Color){100,255,100,255});
            DrawText("INFECTED:", 350, 20, 25, (Color){255,100,100,255});
            int l=0, r=0, font = 18;
            for(int n=0;n<40;n++){
                if(countries[n].infected == 0 && countries[n].dead == 0){
                    DrawText(countryNames[n], 20, 55+21*l, font, WHITE);
                    l++;
                }else{
                    DrawText(countryNames[n], 350, 55+21*r, font, WHITE);
                    r++;
                }
            }
        }else{
            if(gameActive == 2){
                ClearBackground((Color){0,0,80,255});
                DrawText("CURER HAS WON!", 1100, 200, 60, WHITE);
            }else{
                ClearBackground((Color){0,0,80,255});
                DrawText("INFECTER HAS WON!", 1100, 200, 60, WHITE);
            }
        }
        EndDrawing();
    }
    UnloadSound(pop);
    UnloadSound(appear);
    UnloadSound(cureApp);
    UnloadSound(curePop);
    UnloadTexture(map);
    CloseWindow();
}