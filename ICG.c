#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CODE_SIZE 100
#define MAX_INPUT_SIZE 1000

typedef struct {
    char op[10];
    char arg1[10];
    char arg2[10];
    char result[10];
} TAC;

TAC code[MAX_CODE_SIZE];
int codeIndex = 0;
int tempVarCount = 0;
int labelCount = 0;

char* newTemp() {
    static char temp[10];
    snprintf(temp, sizeof(temp), "t%d", tempVarCount++);
    return temp;
}

char* newLabel() {
    static char label[10];
    snprintf(label, sizeof(label), "L%d", labelCount++);
    return label;
}

void addCode(const char* op, const char* arg1, const char* arg2, const char* result) {
    if (codeIndex >= MAX_CODE_SIZE) {
        fprintf(stderr, "Error: Exceeded maximum code array size.\n");
        return;
    }
    snprintf(code[codeIndex].op, sizeof(code[codeIndex].op), "%s", op);
    snprintf(code[codeIndex].arg1, sizeof(code[codeIndex].arg1), "%s", arg1 ? arg1 : "");
    snprintf(code[codeIndex].arg2, sizeof(code[codeIndex].arg2), "%s", arg2 ? arg2 : "");
    snprintf(code[codeIndex].result, sizeof(code[codeIndex].result), "%s", result ? result : "");
    codeIndex++;
}

void printIntermediateCode() {
    printf("\nGenerated Intermediate Code (Three-Address Code):\n");
    for (int i = 0; i < codeIndex; i++) {
        if (strcmp(code[i].op, "=") == 0) {
            printf("%s = %s\n", code[i].result, code[i].arg1);
        } else if (strcmp(code[i].op, "label") == 0) {
            printf("%s:\n", code[i].result);
        } else if (strcmp(code[i].op, "goto") == 0) {
            printf("goto %s\n", code[i].result);
        } else if (strcmp(code[i].op, "if") == 0) {
            printf("if %s %s %s goto %s\n", code[i].arg1, code[i].result, code[i].arg2);
        } else if (strcmp(code[i].op, "printf") == 0) {
            printf("printf(%s)\n", code[i].arg1);
        } else {
            printf("%s = %s %s %s\n", code[i].result, code[i].arg1, code[i].op, code[i].arg2);
        }
    }
}

void processInput(char* input) {
    char* line = strtok(input, "\n");
    char condArg1[10], condArg2[10], condOp[3];
    char var[10], op[5], arg1[10], arg2[10];
    char* endLabel;
    char* trueLabel;

    while (line != NULL) {
        // Trim whitespace
        while (*line == ' ') line++;

        // Match declarations like "int x = 10;" or "int x;"
        if (sscanf(line, "int %s = %s;", var, arg1) == 2) {
            addCode("=", arg1, NULL, var);
        } else if (sscanf(line, "int %s;", var) == 1) {
            addCode("=", "0", NULL, var); // Default to 0 for uninitialized variables
        }
        // Match if condition with or without parentheses like "if (x > 0) {"
        else if (sscanf(line, "if %s %s %s {", condArg1, condOp, condArg2) == 3 ||
                 sscanf(line, "if (%s %s %s {", condArg1, condOp, condArg2) == 3) {
            trueLabel = newLabel();
            endLabel = newLabel();
            addCode("if", condArg1, condOp, trueLabel);      // Conditional jump to true label
            addCode("goto", endLabel, NULL, NULL);           // Skip the true block if condition fails
            addCode("label", NULL, NULL, trueLabel);         // True label
        }
        // Handle printf statements
        else if (sscanf(line, "printf(\"%[^\"]\");", arg1) == 1) {
            addCode("printf", arg1, NULL, NULL);
        }
        // Handle single assignments like "x = 5;"
        else if (sscanf(line, "%s = %s;", var, arg1) == 2) {
            addCode("=", arg1, NULL, var);
        } 
        // Handle arithmetic operations like "x = x + 1;"
        else if (sscanf(line, "%s = %s %s %s;", var, arg1, op, arg2) == 4) {
            char* temp = newTemp();
            addCode(op, arg1, arg2, temp); // Perform operation
            addCode("=", temp, NULL, var);  // Assign result
        }

        // Ignore unrecognized patterns silently
        line = strtok(NULL, "\n");
    }
}

int main() {
    char input[MAX_INPUT_SIZE] = ""; // Increased input size
    char line[100];

    printf("Enter your code as a whole block (type 'END' on a new line to finish):\n");

    while (1) {
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break; // Handle EOF or read error
        }
        if (strcmp(line, "END\n") == 0) {
            break;
        }
        // Check for buffer overflow while concatenating
        if (strlen(input) + strlen(line) < MAX_INPUT_SIZE) {
            strcat(input, line);
        } else {
            fprintf(stderr, "Error: Input exceeds maximum size.\n");
            break;
        }
    }

    processInput(input);
    printIntermediateCode();

    return 0;
}