#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT_LENGTH 1024
#define MAX_TOKEN_LENGTH 20
#define MAX_SYMBOLS 100

typedef struct {
    char name[MAX_TOKEN_LENGTH];
    char type[MAX_TOKEN_LENGTH];
} Symbol;

Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

void addSymbol(char* name, char* type);
void semanticAnalysis(char* input);
void checkForErrors(char* line, int lineNumber);
int isDeclared(char* name);
int isNumericLiteral(char* token);
void trimWhitespace(char* str);
char* getType(char* name);
int isCompatible(char* op1, char* op2);
void checkCondition(char* condition, int lineNumber);

void addSymbol(char* name, char* type) {
    if (symbolCount < MAX_SYMBOLS) {
        strcpy(symbolTable[symbolCount].name, name);
        strcpy(symbolTable[symbolCount].type, type);
        symbolCount++;
    }
}

int isDeclared(char* name) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            return 1; // Variable is declared
        }
    }
    return 0; // Variable is not declared
}

int isNumericLiteral(char* token) {
    for (int i = 0; token[i] != '\0'; i++) {
        if (!isdigit(token[i]) && token[i] != '.' && token[i] != '-') {
            return 0; // Not a numeric literal
        }
    }
    return 1; // Is a numeric literal
}

void trimWhitespace(char* str) {
    char *end;
    while (isspace(*str)) str++;
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;
    end[1] = '\0';
}

char* getType(char* name) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            return symbolTable[i].type;
        }
    }
    return NULL;
}

int isCompatible(char* op1, char* op2) {
    // Implement more sophisticated type compatibility checks here
    if (strcmp(op1, "int") == 0 && strcmp(op2, "int") == 0) {
        return 1;
    } else if (strcmp(op1, "float") == 0 && strcmp(op2, "float") == 0) {
        return 1;
    } else if (strcmp(op1, "int") == 0 && strcmp(op2, "float") == 0) {
        return 1; // Implicit conversion from int to float
    } else if (strcmp(op1, "float") == 0 && strcmp(op2, "int") == 0) {
        return 1; // Implicit conversion from int to float
    } else {
        return 0;
    }
}

void checkCondition(char* condition, int lineNumber) {
    char* token = strtok(condition, " > < =+*/-");
    while (token != NULL) {
        if (!isNumericLiteral(token) && !isDeclared(token)) {
            printf("Error: Variable '%s' not declared in condition on line %d.\n", token, lineNumber);
        }
        token = strtok(NULL, " > < =+*/-");
    }
}

void checkForErrors(char* line, int lineNumber) {
    char type[MAX_TOKEN_LENGTH];
    char name[MAX_TOKEN_LENGTH];

    // Remove leading and trailing whitespace
    trimWhitespace(line);

    // Check for variable declaration
    if (strstr(line, ";") != NULL) {
        size_t len = strlen(line);

        // Remove the semicolon for processing
        if (line[len - 1] == ';') {
            line[len - 1] = '\0';
        }

        // Extract type and variable name
        if (sscanf(line, "%s %s", type, name) == 2) {
            // Validate data type and add to symbol table
            if (strcmp(type, "int") == 0 || strcmp(type, "float") == 0 ||
                strcmp(type, "char") == 0 || strcmp(type, "string") == 0) {
                addSymbol(name, type);
                printf("Declared variable: %s of type %s\n", name, type);
            } else {
                printf("Error: Invalid type '%s' on line %d.\n", type, lineNumber);
            }
        }
    }
    // Check for assignment
    else if (strchr(line, '=') != NULL) {
        char lhs[MAX_TOKEN_LENGTH], rhs[MAX_INPUT_LENGTH];
        if (sscanf(line, "%s = %[^\n]", lhs, rhs) == 2) {
            // Check if the left-hand side variable is declared
            if (!isDeclared(lhs)) {
                printf("Error: Variable '%s' not declared on line %d.\n", lhs, lineNumber);
            } else {
                // Check right-hand side for undeclared variables and invalid expressions
                char* token = strtok(rhs, " +-*/");
                char* prevType = NULL;
                while (token != NULL) {
                    if (!isNumericLiteral(token)) {
                        if (!isDeclared(token)) {
                            printf("Error: Variable '%s' not declared on line %d.\n", token, lineNumber);
                        } else {
                            char* currentType = getType(token);
                            if (prevType != NULL && !isCompatible(prevType, currentType)) {
                                printf("Error: Type mismatch in expression on line %d.\n", lineNumber);
                            }
                            prevType = currentType;
                        }
                    }
                    token = strtok(NULL, " +-*/");
                }
            }
        }
    }
    // Check for conditional statements and loops
    else if (strncmp(line, "if", 2) == 0 || strncmp(line, "for", 3) == 0 ||
               strncmp(line, "while", 5) == 0) {
        char condition[MAX_INPUT_LENGTH];
        sscanf(line, "%*s (%[^)])", condition);
        checkCondition(condition, lineNumber);
    }
    // Handle unrecognized statements
    else if (strlen(line) > 0) {
        printf("Error: Unrecognized statement on line %d: %s\n", lineNumber, line);
    }
}

void semanticAnalysis(char* input) {
    printf("Semantic Analysis:\n");

    char* line = strtok(input, "\n");
    int lineNumber = 1;

    while (line != NULL) {
        trimWhitespace(line);
        if (strlen(line) > 0) {
            checkForErrors(line, lineNumber);
        }

        line = strtok(NULL, "\n");
        lineNumber++;
    }
}

int main() {
    char input[MAX_INPUT_LENGTH] = "";
    char line[MAX_INPUT_LENGTH];

    printf("Enter a program (end with 'END' on a new line):\n");

    while (fgets(line, sizeof(line), stdin) != NULL) {
        if (strcmp(line, "END\n") == 0) {
            break;
        }
        strcat(input, line);
    }

    semanticAnalysis(input);

    return 0;
}