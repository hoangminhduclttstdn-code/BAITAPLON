#include "water_seven.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////
//Task 0
int clamp(int value, int minVal, int maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

bool readInput(
    const string &filename,
    char character[FIXED_CHARACTER][MAX_NAME], int hp[FIXED_CHARACTER], 
    int skill[FIXED_CHARACTER], int &shipHP, int &repairCost){

    ifstream inputFile(filename);
    if (!inputFile.is_open()) return false;

    string name;
    int val1, val2;
    int numCharacters = 0;

    
    for(int i = 0; i < FIXED_CHARACTER; i++) character[i][0] = '\0';
    
    while (inputFile >> name) {
        if (inputFile >> val1 >> val2) {
            if (name == "GOING_MERRY") {
                shipHP = clamp(val1, 0, 1000);
                repairCost = clamp(val2, 0, 3000);
            } else {
                int index = -1;
                for (int i = 0; i < numCharacters; i++) {
                    if (strcmp(character[i], name.c_str()) == 0) {
                        index = i;
                        break;
                    }
                }
                
                if (index != -1) {
                    hp[index] = clamp(val1, 0, 1000);
                    skill[index] = clamp(val2, 0, 100);
                } else if (numCharacters < FIXED_CHARACTER) {
                    strncpy(character[numCharacters], name.c_str(), MAX_NAME - 1);
                    character[numCharacters][MAX_NAME - 1] = '\0';
                    hp[numCharacters] = clamp(val1, 0, 1000);
                    skill[numCharacters] = clamp(val2, 0, 100);
                    numCharacters++;
                }
            }
        }
    }
    inputFile.close();
    return true;
}



// Task 1
 bool isPerfect (int n) {
        if (n<6) return false;

        int sum = 1;
        for (int i=2; i*i<=n; i++) {
            if (n % i == 0) {
                if (i*i != n) 
                    sum = sum + i + n/i;
                else 
                    sum = sum +i;
            }
        }
        return sum==n;
    }

bool SumOfDigit (int n) {
        int digitSum = 0;
        int temp = (n < 0) ? -n : n;

        while (temp > 0) {
            digitSum += temp % 10;
            temp /= 10;
        }

        return isPerfect(digitSum);

}


int damageEvaluation(int shipHP, int repairCost){
    if (shipHP<455 && SumOfDigit(shipHP)) {
        repairCost = static_cast<int>(ceil(repairCost * 1.5));
    }
    
    return repairCost;
}

// Task 2
int conflictSimulation(
    char character[FIXED_CHARACTER][MAX_NAME], int hp[FIXED_CHARACTER], int skill[FIXED_CHARACTER],
    int shipHP, int repairCost){
    
        int skillLuffy = 0, skillUsopp = 0;

        for (int i = 0; i < FIXED_CHARACTER; i++) {
            if (strcmp(character[i], "LUFFY") == 0) {
                skillLuffy = skill[i];
            } else if (strcmp(character[i], "USOPP") == 0) {
                skillUsopp = skill[i];
            }
        }

        int conflictIndex = skillLuffy - skillUsopp + ((repairCost + 99) / 100) + ((500 - shipHP + 49) / 50);

        int steps = 0;
        while (steps < 10 && conflictIndex < 255) {
            int id = conflictIndex % 6;
            if (id < 0) id += 6;
            
            switch (id) {
                case 0: conflictIndex += 255; break;
                case 1: conflictIndex += 20;  break;
                case 2: conflictIndex += 50;  break;
                case 3: conflictIndex += 70;  break;
                case 4: conflictIndex += 90;  break;
                case 5: conflictIndex += 100; break;
            }

            steps++;
        }

        
        return conflictIndex;
    }

// Task 3
void resolveDuel(
    char character[FIXED_CHARACTER][MAX_NAME], int hp[FIXED_CHARACTER], int skill[FIXED_CHARACTER],
    int conflictIndex, int repairCost, char duel[FIXED_CHARACTER][MAX_NAME]){
        //TODO: Output assign to duel parameter
       
    int luffyIdx = -1, usoppIdx = -1;
    for (int i = 0; i < 7; i++) {
        if (strcmp(character[i], "LUFFY") == 0) luffyIdx = i;
        if (strcmp(character[i], "USOPP") == 0) usoppIdx = i;
    }

    int U = skill[usoppIdx] + 
        static_cast<int>(ceil(static_cast<double>(conflictIndex) / 20)) + 
        static_cast<int>(ceil(static_cast<double>(repairCost) / 500));

    int members[5]; 
    int mCount = 0;
    for (int i = 0; i < 7; i++) {
        if (i != luffyIdx && i != usoppIdx) {
            members[mCount++] = i;
        }
    }

    int bestMask = -1;
    int minTotalCost = INT_MAX; 
    int minTeamSize = INT_MAX;

    for (int mask = 0; mask < 32; mask++) {
        int currentSupport = 0;
        int currentCost = 0;
        int currentSize = 0;

        for (int i = 0; i < 5; i++) {
            if ((mask >> i) & 1) { 
                int idx = members[i];
                currentSupport += skill[idx]; 
                currentCost += (hp[idx] % 10) + 1; 
                currentSize++;
            }
        }

    
        if (skill[luffyIdx] + currentSupport >= U) {
            if (currentCost < minTotalCost) {
                minTotalCost = currentCost;
                minTeamSize = currentSize;
                bestMask = mask;
            } else if (currentCost == minTotalCost && currentSize < minTeamSize) {
                minTeamSize = currentSize;
                bestMask = mask;
            }
        }
    }

    int countDuel=0;

    if (bestMask != -1) {
        for (int i=0; i<5; i++) {
            if ((bestMask>>i) & 1) {
                int charIdx = members[i];
                strcpy(duel[countDuel], character[charIdx]);
                countDuel++;
            }
        }
    }
    
    //Null Termination
    for (int i= countDuel; i<7; i++) {
        duel[i][0] = '\0';
    }
}

// Task 4
void decodeCP9Message(char character[FIXED_CHARACTER][MAX_NAME], 
    int hp[FIXED_CHARACTER], int skill[FIXED_CHARACTER], int conflictIndex, 
    int repairCost, char cipherText[], char resultText[]){
        //TODO: Output assign to resultText parameter

        
    int pos = -1;
    int n = strlen(cipherText);
    for (int i = 0; i < n; i++) {
        if (cipherText[i] == '#') { pos = i; break; }
    }


    char message[MAX_NAME]; 
    int sum = 0;
    for (int i = 0; i < pos; i++) {
        message[i] = cipherText[i];
        sum += (int)message[i];
    }
    message[pos] = '\0'; 


    int XY = (cipherText[pos+1] - '0') * 10 + (cipherText[pos+2] - '0');

    if (sum % 100 != XY) {
        resultText[0] = '\0'; 
        return;
    }


    int key = (conflictIndex + repairCost) % 26;
    int B = (key % 5) + 4;
   

    for (int i = 0; i < pos; i += B) {
        int start = i;
        int end = (i + B - 1 < pos - 1) ? i + B - 1 : pos - 1;
        while (start < end) {
            char temp = message[start];
            message[start] = message[end];
            message[end] = temp;
            start++; end--;
        }
    }

    for (int i = 0; i < pos; i++) {
    if (message[i] >= 'A' && message[i] <= 'Z') {
        message[i] = (message[i] - 'A' - key % 26 + 26) % 26 + 'A';
    } else if (message[i] >= 'a' && message[i] <= 'z') {
        message[i] = (message[i] - 'a' - key % 26 + 26) % 26 + 'a';
    } else if (message[i] >= '0' && message[i] <= '9') {
        message[i] = (message[i] - '0' - key % 10 + 10) % 10 + '0';
    }
}

    bool isValid = (strstr(message, "CP9") != NULL || strstr(message, "ENIESLOBBY") != NULL);

    if (isValid) {
        strcpy(resultText, message);
        strcat(resultText, "_TRUE");
    } else {
        strcpy(resultText, message);
        strcat(resultText, "_FALSE");
    }
    
}

// Task 5
int analyzeDangerLimit(int grid[MAX_GRID][MAX_GRID], int rows, int cols){
    
    int maxRowSum = INT_MIN;
    int maxCell = INT_MIN; 

    for (int i = 0; i < rows; i++) {
        int currentRowSum = 0;
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] != -1) {
                currentRowSum += grid[i][j];
            }
            if (grid[i][j] > maxCell) maxCell = grid[i][j];
        }
        if (currentRowSum > maxRowSum) maxRowSum = currentRowSum;
    }
    return maxRowSum + maxCell;
    
}


bool evaluateRoute(int grid[MAX_GRID][MAX_GRID], int rows, int cols, int dangerLimit){
    
    if (grid[0][0] == -1 || grid[rows-1][cols-1] == -1) return false;

    int dp[MAX_GRID][MAX_GRID];
    const int inf = 100000000;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] == -1) {
                dp[i][j] = inf;
                continue;
            }
            if (i == 0 && j == 0) dp[i][j] = grid[i][j];
            else {
                int up = (i > 0) ? dp[i-1][j] : inf;
                int left = (j > 0) ? dp[i][j-1] : inf;
                
                int minPrev = (up < left) ? up : left;
                if (minPrev == inf) dp[i][j] = inf; 
                else dp[i][j] = grid[i][j] + minPrev;
            }
        }
    }

    int finalDanger = dp[rows-1][cols-1];
    return (finalDanger != inf && finalDanger <= dangerLimit);
}


////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////
