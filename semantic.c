#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_SYMBOLS 100
#define MAX_LINES 100
#define MAX_LINE_LENGTH 100

typedef struct {
    char name[30];
    char type[10];
    int scopeDepth;
} Symbol;

Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;
int currentScope = 0;

char programLines[MAX_LINES][MAX_LINE_LENGTH];
int lineCount = 0;

const char *keywords[] = {"int", "float", "char", "if", "else", "for", "while"};
int isKeyword(const char *word) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(word, keywords[i]) == 0) return 1;
    }
    return 0;
}

// Function to add a symbol to the symbol table
void addSymbol(char *name, char *type) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0 && symbolTable[i].scopeDepth == currentScope) {
            printf("Error: Variable '%s' is already declared in this scope\n", name);
            return;
        }
    }
    Symbol newSymbol;
    strcpy(newSymbol.name, name);
    strcpy(newSymbol.type, type);
    newSymbol.scopeDepth = currentScope;
    symbolTable[symbolCount++] = newSymbol;
    printf("declared variable: %s\n", name);
}

// Function to check if a variable is declared
int isDeclared(char *name) {
    if (isKeyword(name)) return 1; // Skip keywords as they are not variables
    for (int i = symbolCount - 1; i >= 0; i--) {
        if (strcmp(symbolTable[i].name, name) == 0 && symbolTable[i].scopeDepth <= currentScope) {
            return 1; // Found the variable
        }
    }
    return 0; // Variable not declared
}

// Function to handle variable declaration
void declareVariable(char *line) {
    char type[10], name[30];
    if (sscanf(line, "%s %s =", type, name) == 2) {
        if (isKeyword(type)) {
            addSymbol(name, type);
        }
    }
}

// Function to handle variable assignment and usage
void useVariable(char *line) {
    char name[30];
    if (sscanf(line, "%s =", name) == 1) {
        if (!isDeclared(name)) {
            printf("undeclared variable: %s\n", name);
        }
    }
}

// Function to analyze each line based on conditionals, loops, and variable handling
void analyzeLine(char *line) {
    if (strstr(line, "int ") == line || strstr(line, "float ") == line || strstr(line, "char ") == line) {
        declareVariable(line);
    } else if (strstr(line, "=") != NULL) {
        useVariable(line);
    } else if (strstr(line, "if") == line || strstr(line, "for") == line || strstr(line, "while") == line) {
        printf("Entering new scope (conditional/loop)\n");
        currentScope++;
    } else if (strcmp(line, "{") == 0) {
        printf("Entering new block scope\n");
        currentScope++;
    } else if (strcmp(line, "}") == 0) {
        printf("Exiting block scope\n");
        currentScope--;
    } else {
        printf("Unknown command: %s\n", line);
    }
}

// Function to analyze the stored program lines
void analyzeProgram() {
    for (int i = 0; i < lineCount; i++) {
        char *line = programLines[i];
        analyzeLine(line);
    }
}

// Function to read program lines until "END"
void readProgram() {
    printf("Enter your program (type 'END' to finish):\n");
    while (1) {
        fgets(programLines[lineCount], sizeof(programLines[lineCount]), stdin);
        
        // Remove newline character
        programLines[lineCount][strcspn(programLines[lineCount], "\n")] = 0;

        if (strcmp(programLines[lineCount], "END") == 0) break;
        
        lineCount++;
    }
}

int main() {
    printf("Starting Semantic Analysis...\n");
    readProgram();
    analyzeProgram();
    printf("Semantic Analysis Completed.\n");
    return 0;
}
