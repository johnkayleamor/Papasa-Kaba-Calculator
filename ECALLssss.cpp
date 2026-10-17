#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define SUBJECTS 7
#define FACTORS 11
#define MAX_STUDENTS 100
#define PASSING_SCORE 60.0

struct Student {
    char name[50];
   	char studID[50];
    int attempts;
    char customSubjects[SUBJECTS][50];
    float factors[SUBJECTS][FACTORS];
    float subjectSCORES[SUBJECTS];
    float total;
    float weights[FACTORS];
};

char subjects[SUBJECTS][50] = {
    "Computer Programming 2",
    "PATHFIT 2",
    "Understanding the Self",
    "Differential Calculus",
    "PKF",
    "STS",
    "Discrete Structure 1"
};

char subjectShort[SUBJECTS][15] = {
    "Comp Prog 2",
    "PATHFIT 2",
    "Under Self",
    "Diff Calc",
    "PKF",
    "STS",
    "Discrete 1"
};

char questions[FACTORS][100] = {
    "Gaano ka kasipag mag-review para sa exam?",
    "Gaano mo naiintindihan ang lesson?",
    "Gaano karami at ka-epektibo ang practice mo para sa exam?",
    "Gaano kaganda ang tulog at pahinga mo bago ang exam?",
    "Gaano ka ka-stress bago ang exam?",
    "Gaano kahirap sa tingin mo ang exam?",
    "Gaano ka kagaling mag-manage ng oras sa pagre-review at habang nag-eexam?",
    "Gaano ka ka-confident na papasa ka?",
    "Gaano ka kampante sa paraan ng teacher sa paggawa ng exam?",
    "Gaano ka kaswerte sa pakiramdam mo?",
    "Gaano kalakas ang panalangin o spiritual readiness mo para sa exam?"
};

char factors[FACTORS][30] = {
    "Study",
    "Understanding",
    "Practice",
    "Sleep",
    "Stress",
    "Difficulty",
    "Time Management",
    "Confidence",
    "Teacher",
    "Luck",
    "Prayer"
};

float subjectBoost[SUBJECTS][FACTORS] = {
    {1.0, 1.2, 1.3, 1.0, 1.0, 1.1, 1.1, 1.0, 1.0, 1.0, 1.0},
    {1.0, 0.8, 1.0, 1.4, 1.1, 1.0, 1.2, 1.0, 1.0, 1.0, 1.0},
    {1.0, 1.3, 0.9, 1.0, 1.1, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
    {1.1, 1.2, 1.5, 1.0, 1.0, 1.3, 1.1, 1.0, 1.0, 1.0, 1.0},
    {1.0, 1.1, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
    {1.0, 1.3, 0.8, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
    {1.1, 1.2, 1.4, 1.0, 1.0, 1.2, 1.1, 1.0, 1.0, 1.0, 1.0}
};

float defaultWeights[FACTORS] = {
    0.25, 0.20, 0.15, 0.05, 0.05, 0.05, 0.10, 0.10, 0.03, 0.01, 0.01
};

float bonusWeights[FACTORS] = {
    0, 0, 0, 0, 0, 0, 0, 0.40, 0.25, 0.20, 0.15
};

const char* getSubjectName(int i, int mode, struct Student *s);
float cap100(float score);
float calculateProbability(float predictedScore, float passingScore);
void pauseScreen(void);
const char* getStatus(float probability);
const char* getResultComment(float overallProbability);
const char* getInsight(float overallProbability);
void sortData(struct Student students[], int allSelected[][SUBJECTS], int allModes[], int count);
void printLine(int width);
void displaySubjectResultsMatrix(struct Student *student, int selected[], int mode);
void displayOverallResultMatrix(struct Student *student, float *overallProbability);
void displayAnalysisMatrix(struct Student *student, int selected[], int mode, int bestSub, int worstSub, int used, int strongestFactor);
void displayCommentInsightMatrix(float overallProbability);
void displayResults(struct Student *student, int selected[], int count, int mode, int bestSub, int worstSub, int used, int strongestFactor);

void displaySummaryReport(struct Student students[], int count, int selected[][SUBJECTS], int modes[]);

void saveRecord(struct Student *student, int selected[], int count, int mode, float overallProbability);

const char* getSubjectName(int i, int mode, struct Student *s) {
	if (i < 0 || i >= SUBJECTS) return "N/A";
    if (mode == 3 && i == 0) return s->customSubjects[0];
    return subjects[i];
}

float cap100(float score) {
    if (score > 100.0) return 100.0;
    if (score < 0.0) return 0.0;
    return score;
}

float calculateProbability(float predictedScore, float passingScore) {
    float diff = predictedScore - passingScore;
    float sigmoid = 1.0 / (1.0 + exp(-0.25 * diff));
    float probability = 0.1 + sigmoid * 99.8;

    if (predictedScore < 40) {
        if (probability > 65.0) probability = 65.0;
    } else if (predictedScore < 50) {
        if (probability > 85.0) probability = 85.0;
    } else if (predictedScore < 60) {
        if (probability > 95.0) probability = 95.0;
    }

    if (probability > 99.9) probability = 99.9;
    if (probability < 0.1) probability = 0.1;

    return probability;
}

void pauseScreen(void) {
    printf("\n\n");
    printf("  ========================================================\n");
    printf("  Press ENTER to continue...\n");
    printf("  ========================================================\n");
    fflush(stdout);
    getchar();
    getchar();
}

const char* getStatus(float probability) {
    if (probability >= 80.0f) return "VERY HIGH CHANCE";
    if (probability >= 60.0f) return "GOOD CHANCE";
    if (probability >= 40.0f) return "FIFTY-FIFTY";
    return "DANGER";
}

const char* getResultComment(float overallProbability) {
    if (overallProbability >= 80.0f) return "SIGURADONG PASADO - Relax ka nalang ";
    if (overallProbability >= 60.0f) return "MATAAS ANG CHANCE - Konting review pa ";
    if (overallProbability >= 40.0f) return "FIFTY-FIFTY - Kailangan mag-aral ";
    if (overallProbability >= 20.0f) return "MAHIRAP - Serious review ang needed ";
    return "BAGSAK - LOCK IN MODE ";
}

const char* getInsight(float overallProbability) {
    if (overallProbability >= 80.0f) return "Very high overall chance of passing";
    if (overallProbability >= 60.0f) return "Good overall standing";
    if (overallProbability >= 40.0f) return "Borderline performance";
    return "Critical performance level";
}

void clearScreen(void) {
    system("cls");
}

void printSeparator(void) {
    printf("  =====================================================\n");
}

void printHeader(const char *title) {
    printf("\n\n");
    printSeparator();
    printf("    %s\n", title);
    printSeparator();
}

void printSubHeader(const char *title) {
    printf("\n\n  ------------------------------------------------------------------------------\n");
    printf("    %s\n", title);
    printf("  ------------------------------------------------------------------------------\n");
}

void printLine(int width) {
    printf("+");
    for (int i = 0; i < width - 2; i++) printf("-");
    printf("+\n");
}

void displaySubjectResultsMatrix(struct Student *student, int selected[], int mode) {
    float subjectProbability[SUBJECTS] = {0};
    float probabilityTotal = 0;
    int count = 0;

    printf("\n\n+============================== SUBJECT RESULTS =============================+\n");
    printf("| %-25s | %10s | %12s | %-18s |\n", 
           "Subject", "Score", "Probability", "Status");
    printLine(78);

    for (int i = 0; i < SUBJECTS; i++) {
        if (selected[i] == 1) {
            subjectProbability[i] = calculateProbability(student->subjectSCORES[i], PASSING_SCORE);
            probabilityTotal += subjectProbability[i];
            count++;

            printf("| %-25s | %9.2f%% | %11.2f%% | %-18s |\n",
                   getSubjectName(i, mode, student),
                   student->subjectSCORES[i],
                   subjectProbability[i],
                   getStatus(subjectProbability[i]));
        }
    }
    printLine(78);

    if (count > 0) {
        printf("| %-25s | %9.2f%% | %11.2f%% | %-18s |\n",
               "AVERAGE", 
               probabilityTotal / count,
               probabilityTotal / count,
               getStatus(probabilityTotal / count));
        printLine(78);
    }
}

void displayOverallResultMatrix(struct Student *student, float *overallProbability) {
    *overallProbability = calculateProbability(student->total, PASSING_SCORE);

    printf("\n\n+==================== OVERALL RESULT ===================+\n");
    printf("| %-30s | %20s |\n", "Metric", "Value");
    printLine(57);
    printf("| %-30s | %19.2f%% |\n", "Overall Predicted Score", student->total);
    printf("| %-30s | %19.2f%% |\n", "Passing Score Requirement", PASSING_SCORE);
    printf("| %-30s | %19.2f%% |\n", "Overall Probability", *overallProbability);
    printf("| %-30s | %-20s |\n", "Status", getStatus(*overallProbability));
    printLine(57);
}

void displayAnalysisMatrix(struct Student *student, int selected[], int mode, int bestSub, int worstSub, int used, int strongestFactor) {
    printf("\n\n+========================== ANALYSIS ==========================+\n");
    printf("| %-20s | %-37s |\n", "Category", "Details");
    printLine(64);
    printf("| %-20s | %-37s |\n", "Best Subject", getSubjectName(bestSub, mode, student));
    
    if (used == 1) {
        char detail[80];
        snprintf(detail, sizeof(detail), "%s (only subject)", getSubjectName(worstSub, mode, student));
        printf("| %-20s | %-37s |\n", "Worst Subject", detail);
    } else {
        printf("| %-20s | %-37s |\n", "Worst Subject", getSubjectName(worstSub, mode, student));
    }
    
    printf("| %-20s | %-37s |\n", "Strongest Factor", factors[strongestFactor]);
    printf("| %-20s | %-37d |\n", "Subjects Taken", used);
    printLine(64);
}

void displayCommentInsightMatrix(float overallProbability) {
    printf("\n\n+===================== COMMENT & INSIGHT ======================+\n");
    printf("| %-15s | %-42s |\n", "Type", "Message");
    printLine(64);
    printf("| %-15s | %-42s |\n", "COMMENT", getResultComment(overallProbability));
    printf("| %-15s | %-42s |\n", "INSIGHT", getInsight(overallProbability));
    printLine(64);
}

void displayResults(struct Student *student, int selected[], int count, int mode,
                    int bestSub, int worstSub, int used, int strongestFactor) {
    float overallProbability;

    displaySubjectResultsMatrix(student, selected, mode);
    displayOverallResultMatrix(student, &overallProbability);
    displayAnalysisMatrix(student, selected, mode, bestSub, worstSub, used, strongestFactor);
    displayCommentInsightMatrix(overallProbability);

    saveRecord(student, selected, count, mode, overallProbability);
}

void displaySummaryReport(struct Student students[], int count, int selected[][SUBJECTS], int modes[]) {	
    system("cls");
    
    	sortData(students, selected, modes, count);
    
    if (count == 0) {
        printf("\n\n+=================== SUMMARY REPORT ===================+\n");
        printf("| %-52s |\n", "No student records found.");
        printLine(56);
        pauseScreen();
        return;
    }

    printf("\n\n+=========================================================== SUMMARY REPORT =================================================================================+\n");
    printf("| %-3s | %-20s | %-20s | %8s | %12s | %-38s | %-35s |\n", 
           "No.", "Student Name", "Student ID", "Score", "Probability", "Comment", "Insight");
    printLine(158);

    for (int s = 0; s < count; s++) {
        float overallProb = calculateProbability(students[s].total, PASSING_SCORE);
        
        printf("| %3d | %-20s | %-20s | %7.2f%% | %11.2f%% | %-38s | %-35s |\n",
               s + 1,
               students[s].name,
               students[s].studID,
               students[s].total,
               overallProb,
               getResultComment(overallProb),
               getInsight(overallProb));
    }
    printLine(158);

    printf("\n\n+====================================================== SUBJECT BREAKDOWN ====================================================+\n");
    printf("| %-18s |", "Student");
    for (int i = 0; i < SUBJECTS; i++) {
        printf(" %12s |", subjectShort[i]);
    }
    printf("\n");
    printLine(22 + SUBJECTS * 15);

    for (int s = 0; s < count; s++) {
        printf("| %-18s |", students[s].name);
        for (int i = 0; i < SUBJECTS; i++) {
            if (selected[s][i] == 1 && modes[s] != 3) {
                float prob = calculateProbability(students[s].subjectSCORES[i], PASSING_SCORE);
                printf(" %11.1f%% |", prob);
            } else if (selected[s][i] == 1 && modes[s] == 3 && i != 0) {
                float prob = calculateProbability(students[s].subjectSCORES[i], PASSING_SCORE);
                printf(" %11.1f%% |", prob);
            } else {
                printf("      ---     |");
            }
        }
        printf("\n");
    }
    printLine(22 + SUBJECTS * 15);

    int hasCustom = 0;
    for (int s = 0; s < count; s++) {
        if (modes[s] == 3) {
            hasCustom = 1;
            break;
        }
    }

    if (hasCustom) {
        printf("\n\n+======================== CUSTOM SUBJECTS ===============================================+\n");
        printf("| %-20s | %-20s | %-25s | %12s |\n", "Student", "Student ID", "Custom Subject", "Probability");
        printLine(90);

        for (int s = 0; s < count; s++) {
            if (modes[s] == 3 && selected[s][0] == 1) {
                float prob = calculateProbability(students[s].subjectSCORES[0], PASSING_SCORE);
                printf("| %-20s | %-20s | %-25s | %11.1f%% |\n",
                       students[s].name,
                       students[s].studID,
                       students[s].customSubjects[0],
                       prob);
            }
        }
        printLine(90);
    }

    pauseScreen();
}

void saveRecord(struct Student *student, int selected[], int count, int mode,
                float overallProbability) {
    FILE *fp = fopen("records.txt", "a");

    if (fp) {
        fprintf(fp, "[STUDENT_RECORD]\n");
        fprintf(fp, "NAME|%s\n", student->name);
        fprintf(fp, "STUDENT ID|%s\n", student->studID);
        fprintf(fp, "MODE|%d\n", mode);
        fprintf(fp, "TOTAL|%.2f\n", student->total);
        
        for (int i = 0; i < SUBJECTS; i++) {
            if (selected[i] == 1) {
                if (mode == 3 && i == 0) {
                    fprintf(fp, "CUSTOM|%s|%.2f\n", student->customSubjects[0], student->subjectSCORES[i]);
                } else {
                    fprintf(fp, "SUBJECT_%d|%.2f\n", i, student->subjectSCORES[i]);
                }
            }
        }
        
        fprintf(fp, "[END_RECORD]\n");
        fclose(fp);
    } else {
        printf("\n  ERROR: Could not save record!");
    }
}

int loadRecords(struct Student students[], int allSelected[][SUBJECTS], int allModes[], int *count) {
    FILE *fp = fopen("records.txt", "r");
    if (!fp) return 0;

    char line[300];
    *count = 0;

    memset(students, 0, sizeof(struct Student) * MAX_STUDENTS);
    memset(allSelected, 0, sizeof(int) * MAX_STUDENTS * SUBJECTS);
    memset(allModes, 0, sizeof(int) * MAX_STUDENTS);

    while (fgets(line, sizeof(line), fp) && *count < MAX_STUDENTS) {
        line[strcspn(line, "\r\n")] = '\0';

        if (strcmp(line, "[STUDENT_RECORD]") == 0) {
            struct Student *s = &students[*count];
            int mode = 1;

            while (fgets(line, sizeof(line), fp)) {
                line[strcspn(line, "\r\n")] = '\0';

                if (strcmp(line, "[END_RECORD]") == 0) {
                    break;
                }

                char *pipe = strchr(line, '|');
                if (!pipe) continue;

                char key[50];
                char value[200];
                strncpy(key, line, pipe - line);
                key[pipe - line] = '\0';
                strcpy(value, pipe + 1);

                if (strcmp(key, "NAME") == 0) {
                    strncpy(s->name, value, 49);
                    s->name[49] = '\0';
                } else if (strcmp(key, "STUDENT ID") == 0) {
					strncpy(s->studID, value, 49);
                    s->studID[49] = '\0';
				}else if (strcmp(key, "MODE") == 0) {
                    mode = atoi(value);
                    allModes[*count] = mode;
                } else if (strcmp(key, "TOTAL") == 0) {
                    s->total = atof(value);
                } else if (strcmp(key, "CUSTOM") == 0) {
                    char *pipe2 = strchr(value, '|');
                    if (pipe2) {
                        *pipe2 = '\0';
                        strncpy(s->customSubjects[0], value, 49);
                        s->customSubjects[0][49] = '\0';
                        s->subjectSCORES[0] = atof(pipe2 + 1);
                        allSelected[*count][0] = 1;
                    }
                } else if (strncmp(key, "SUBJECT_", 8) == 0) {
                    int subjectIdx = atoi(key + 8);
                    if (subjectIdx >= 0 && subjectIdx < SUBJECTS) {
                        s->subjectSCORES[subjectIdx] = atof(value);
                        allSelected[*count][subjectIdx] = 1;
                    }
                }
            }

            (*count)++;
        }
    }

    fclose(fp);
    return *count;
}

void sortData(struct Student students[], int allSelected[][SUBJECTS], int allModes[], int count) {
	struct Student temp;
	int tempMode;
	int tempSelected[SUBJECTS];
	
	for(int i = 0; i < count - 1; i++){
		
		for(int j = 0; j < count - i - 1; j++){
			if(strcmp(students[j].name, students[j+1].name) > 0){
				temp = students[j];
				students[j] = students[j+1];
				students[j+1] = temp;
				
				for(int k = 0; k < SUBJECTS; k++){
					tempSelected[k] = allSelected[j][k];
					allSelected[j][k] = allSelected[j + 1][k];
					allSelected[j+1][k] = tempSelected[k];
				}
				
				tempMode = allModes[j];
				allModes[j] = allModes[j+1];
				allModes[j+1] = tempMode;
			}	
			
		}
	}
	
}

int main(void) {
    struct Student students[MAX_STUDENTS];
    int count = 0;
    int choice;

    int allSelected[MAX_STUDENTS][SUBJECTS] = {0};
    int allModes[MAX_STUDENTS] = {0};

    loadRecords(students, allSelected, allModes, &count);

    do {
        clearScreen();

        printHeader("      EXAM AND QUIZ PROBABILITY CALCULATOR");

        printf("\n");
        printf("  MAIN MENU\n");
        printf("\n");
        printf("    1.  New Student\n");
        printf("    2.  View All Records\n");
        printf("    3.  View Summary Report\n");
        printf("    4.  Search Student\n");
        printf("    5.  Delete Record\n");
        printf("    6.  Exit\n");
        printf("\n");
        printSeparator();
        printf("\n  Enter choice (1-6): ");

        while (scanf("%d", &choice) != 1 || choice < 1 || choice > 6) {
            printf("  ERROR: Invalid input! Enter 1-6: ");
            while (getchar() != '\n');
        }

        switch (choice) {
            case 1: {
                clearScreen();

                int mode;
                int selected[SUBJECTS] = {0};

                if (count >= MAX_STUDENTS) {
                    printHeader("ERROR - LIMIT REACHED");
                    printf("\n  Maximum student capacity (%d students) reached.\n\n", MAX_STUDENTS);
                    pauseScreen();
                    break;
                }

                float bonus = 0;
                float coreTotal = 0;

                for (int j = 0; j < FACTORS; j++) {
                    students[count].weights[j] = defaultWeights[j];
                }

                int bestSub = -1, worstSub = -1;
                float bestScore = -1, worstScore = 999;

                printHeader("NEW STUDENT ENTRY");
                printf("\n  Student Name:  ");
                while (getchar() != '\n');
                scanf(" %49[^\n]", students[count].name);

                while (strlen(students[count].name) == 0) {
                    printf("\n  ERROR: Name cannot be empty. Try again:  ");
                    scanf(" %49[^\n]", students[count].name);
                }
                
                printf("\n  Student ID:  ");
                while (getchar() != '\n');
                scanf(" %49[^\n]", students[count].studID);
                
                 while (strlen(students[count].studID) == 0) {
                    printf("\n  ERROR: Student ID cannot be empty. Try again:  ");
                    scanf(" %49[^\n]", students[count].studID);
                }

                printSubHeader("SELECT MODE");
                printf("\n");
                printf("    1.  All Subjects\n");
                printf("    2.  Choose Specific Subjects\n");
                printf("    3.  Add Your Own Subject\n");
                printf("\n  Enter choice (1-3): ");

                while (scanf("%d", &mode) != 1 || mode < 1 || mode > 3) {
                    printf("  ERROR: Invalid input! Enter 1-3: ");
                    while (getchar() != '\n');
                }

                printf("\n");

                if (mode == 1) {
                    for (int i = 0; i < SUBJECTS; i++)
                        selected[i] = 1;
                } else if (mode == 2) {
                    printSubHeader("SELECT YOUR SUBJECTS");
                    printf("\n");
                    printf("    0 = NO     1 = YES\n");
                    printf("\n");
					
					int anySelected = 0;
					do{
                    for (int i = 0; i < SUBJECTS; i++) {
                        printf("    %d.  %-40s: ", i + 1, subjects[i]);
                        int response;
                        while (scanf("%d", &response) != 1 || (response != 0 && response != 1)) {
                            printf("    ERROR: Enter 0 (NO) or 1 (YES): ");
                            while (getchar() != '\n');
                        }
                        selected[i] = response;
                        if (response == 1) anySelected = 1;
                    } if (!anySelected){
                    	printf("\n  ERROR: Please select at least one subject!\n\n");
					}
                }while (!anySelected);
                } else {
                    for(int i = 0; i < SUBJECTS; i++)
                        selected[i] = 0;

                    printf("  Enter Subject Name: ");
                    while (getchar() != '\n');
                    scanf(" %49[^\n]", students[count].customSubjects[0]);

                    while (strlen(students[count].customSubjects[0]) == 0) {
                        printf("  ERROR: Subject name cannot be empty. Enter again: ");
                        scanf(" %49[^\n]", students[count].customSubjects[0]);
                    }

                    selected[0] = 1;
                }

                for (int i = 0; i < SUBJECTS; i++) {
                    if (selected[i] == 1) {
                        printf("\n\n  --- %s ---\n", getSubjectName(i, mode, &students[count]));
                        for (int j = 0; j < FACTORS; j++) {
                            do {
                                printf("  %s [%s] (1-10): ", questions[j], factors[j]);
                                while (scanf("%f", &students[count].factors[i][j]) != 1 ||
                                       students[count].factors[i][j] < 1 ||
                                       students[count].factors[i][j] > 10) {
                                    printf("  ERROR: Invalid input! (1-10): ");
                                    while (getchar() != '\n');
                                }
                            } while (students[count].factors[i][j] < 1 ||
                                     students[count].factors[i][j] > 10);
                        }
                    }
                }

                int used = 0;

                for (int i = 0; i < SUBJECTS; i++) {
                    if (selected[i] == 1) {
                        float score = 0;

                        for (int j = 0; j < 7; j++) {
                            float value = students[count].factors[i][j];

                            if (j == 4 || j == 5)
                                value = 11 - value;

                            score += (value / 10.0) * students[count].weights[j] * subjectBoost[i][j];
                        }

                        students[count].subjectSCORES[i] = cap100(score * 100);

                        if (bestSub == -1 || students[count].subjectSCORES[i] > bestScore) {
                            bestScore = students[count].subjectSCORES[i];
                            bestSub = i;
                        }

                        if (worstSub == -1 || students[count].subjectSCORES[i] < worstScore) {
                            worstScore = students[count].subjectSCORES[i];
                            worstSub = i;
                        }

                        coreTotal += students[count].subjectSCORES[i];
                        used++;
                    } else {
                        students[count].subjectSCORES[i] = 0;
                    }
                }

                if (used == 1) {
                    worstSub = bestSub;
                }

                for (int i = 0; i < SUBJECTS; i++) {
                    if (selected[i] == 1) {
                        float b = 0;
                        for (int j = 7; j < FACTORS; j++) {
                            b += (students[count].factors[i][j] / 10.0) * bonusWeights[j];
                        }
                        bonus += b * 100;
                    }
                }

                if (used > 0) {
                    coreTotal /= used;
                    bonus /= used;
                }

                coreTotal = cap100(coreTotal);
                bonus = cap100(bonus);
                students[count].total = cap100((coreTotal * 0.75) + (bonus * 0.25));

                int strongestFactor = 0;
                float strongestValue = -1;

                for (int j = 0; j < 7; j++) {
                    float ave = 0;
                    for (int i = 0; i < SUBJECTS; i++) {
                        if (selected[i] == 1) {
                            ave += students[count].factors[i][j];
                        }
                    }

                    if (used > 0)
                        ave /= used;

                    if (ave > strongestValue) {
                        strongestValue = ave;
                        strongestFactor = j;
                    }
                }

                printf("\n\n  Student: %s\n  Student ID: %s", students[count].name, students[count].studID);

                for (int i = 0; i < SUBJECTS; i++) {
                    if (selected[i] == 1) {
                        printf("\n  %s: %.2f%%", getSubjectName(i, mode, &students[count]), students[count].subjectSCORES[i]);
                    }
                }

                printf("\n\n  Passing Score: %.0f%% (Fixed)\n", PASSING_SCORE);

                for (int i = 0; i < SUBJECTS; i++) {
                    allSelected[count][i] = selected[i];
                }
                allModes[count] = mode;

                displayResults(&students[count], selected, count, mode,
                              bestSub, worstSub, used, strongestFactor);

                count++;
                pauseScreen();
                break;
            }

            case 2: {
                clearScreen();
                sortData(students, allSelected, allModes, count);

                if (count == 0) {
                    printHeader("NO RECORDS");
                    printf("\n  No records yet.\n\n");
                } else {
                    printHeader("ALL RECORDS");

                    printf("\n\n+=========================================================== STUDENT RECORDS ================================================================================+\n");
                    printf("| %-3s | %-20s | %-20s | %8s | %12s | %-38s | %-35s |\n", 
                           "No.", "Student Name", "Student ID", "Score", "Probability", "Comment", "Insight");
                    printLine(158);

                    for (int s = 0; s < count; s++) {
                        float overallProb = calculateProbability(students[s].total, PASSING_SCORE);
                        
                        printf("| %3d | %-20s | %-20s | %7.2f%% | %11.2f%% | %-38s | %-35s |\n",
                               s + 1,
                               students[s].name,
                               students[s].studID,
                               students[s].total,
                               overallProb,
                               getResultComment(overallProb),
                               getInsight(overallProb));
                    }
                    printLine(158);
                }

                pauseScreen();
                break;
            }

            case 3: {
                displaySummaryReport(students, count, allSelected, allModes);
                break;
            }

            case 4: {
                clearScreen();
				
                char searchID[50];
                int found = -1;

                printHeader("SEARCH STUDENT");
                printf("\n  Enter Student ID to search: ");
                while (getchar() != '\n');
                scanf(" %49[^\n]", searchID);

                if (count == 0) {
                    clearScreen();
                    printHeader("NO RECORDS");
                    printf("\n  No records yet.\n\n");
                } else {
                    for (int s = 0; s < count; s++) {
                        if (strstr(students[s].studID, searchID) != NULL) {
                            found = s;
                            break;
                        }
                    }

                    clearScreen();

                    if (found == -1) {
                        printHeader("SEARCH RESULTS");
                        printf("\n  No Student ID '%s' found.\n\n", searchID);
                    } else {
                        int tempSelected[SUBJECTS];
                        for (int i = 0; i < SUBJECTS; i++) {
                            tempSelected[i] = allSelected[found][i];
                        }
					
                        displaySubjectResultsMatrix(&students[found], tempSelected, allModes[found]);
                        
                        float foundProb = calculateProbability(students[found].total, PASSING_SCORE);
                        displayOverallResultMatrix(&students[found], &foundProb);

                        int bestSub = -1, worstSub = -1;
                        float bestScore = -1, worstScore = 999;
                        int used = 0;

                        for (int i = 0; i < SUBJECTS; i++) {
                            if (tempSelected[i] == 1) {
                                if (bestSub == -1 || students[found].subjectSCORES[i] > bestScore) {
                                    bestScore = students[found].subjectSCORES[i];
                                    bestSub = i;
                                }
                                if (worstSub == -1 || students[found].subjectSCORES[i] < worstScore) {
                                    worstScore = students[found].subjectSCORES[i];
                                    worstSub = i;
                                }
                                used++;
                            }
                        }

                        if (used == 1) {
                            worstSub = bestSub;
                        }

                        int strongestFactor = 0;
                        float strongestValue = -1;
                        for (int j = 0; j < 7; j++) {
                            float ave = 0;
                            for (int i = 0; i < SUBJECTS; i++) {
                                if (tempSelected[i] == 1) {
                                    ave += students[found].factors[i][j];
                                }
                            }
                            if (used > 0) ave /= used;
                            if (ave > strongestValue) {
                                strongestValue = ave;
                                strongestFactor = j;
                            }
                        }

                        displayAnalysisMatrix(&students[found], tempSelected, allModes[found], bestSub, worstSub, used, strongestFactor);
                        displayCommentInsightMatrix(foundProb);
                    }
                }

                pauseScreen();
                break;
            }

            case 5: {
                clearScreen();

                char deleteID[50];
                int found = -1;

                printHeader("DELETE RECORD");
                printf("\n  Enter Student ID to delete: ");
                while (getchar() != '\n');
                scanf(" %49[^\n]", deleteID);

                for (int s = 0; s < count; s++) {
                    if (strcmp(students[s].studID, deleteID) == 0) {
                        found = s;
                        break;
                    }
                }

                clearScreen();

                if (found == -1) {
                    printHeader("DELETE RESULT");
                    printf("\n  No Student ID '%s' found.\n\n", deleteID);
                } else {
                    FILE *fp = fopen("records.txt", "r");
                    FILE *temp = fopen("temp_records.txt", "w");

                    if (!fp || !temp) {
                        printHeader("ERROR");
                        printf("\n  ERROR: Could not access files.\n\n");
                        if (fp) fclose(fp);
                        if (temp) fclose(temp);
                        pauseScreen();
                        break;
                    }

                    char line[300];
					char record[5000];
					int deletedRecordCount = 0;
					
					while (fgets(line, sizeof(line), fp)) {
					
					    if (strcmp(line, "[STUDENT_RECORD]\n") == 0) {
					
					        strcpy(record, line);
					
					        char recordID[50] = "";
					
					        while (fgets(line, sizeof(line), fp)) {
					
					            strcat(record, line);
					
					            if (strncmp(line, "STUDENT ID|", 11) == 0) {
					                sscanf(line + 11, "%49[^\n]", recordID);
					            }
					
					            if (strcmp(line, "[END_RECORD]\n") == 0) {
					                break;
					            }
					        }
					
					        if (strcmp(recordID, deleteID) != 0) {
					            fputs(record, temp);
					        } else {
					            deletedRecordCount++;
					        }
					    }
					}

                    fclose(fp);
                    fclose(temp);

                    remove("records.txt");
                    rename("temp_records.txt", "records.txt");

                    printHeader("DELETE RESULT");
                    if (deletedRecordCount > 0)
					    printf("\n  Record '%s' deleted successfully.\n\n", deleteID);
					else
					    printf("\n  Record '%s' was not found in records.txt.\n\n", deleteID);

                    memset(students, 0, sizeof(students));
                    memset(allSelected, 0, sizeof(allSelected));
                    memset(allModes, 0, sizeof(allModes));
                    count = 0;

                    int reloaded = loadRecords(students, allSelected, allModes, &count);
                    printf("  Reloaded %d record(s).\n\n", reloaded);
                }

                pauseScreen();
                break;
            }

            case 6:
                clearScreen();
                printf("\n\n  Goodbye!\n\n");
                break;

            default:
                printf("\n  ERROR: Invalid choice! Please enter 1-6.\n\n");
                pauseScreen();
                break;
        }

    } while (choice != 6);

    return 0;
}
