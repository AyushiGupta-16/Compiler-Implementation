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

int pos = 0;

void skipWhitespace(char* input);
void addSymbol(char* name, char* type);
void lexicalAnalysis(char* input);

void skipWhitespace(char* input) {
    while (isspace(input[pos])) {
        pos++;
    }
}

void addSymbol(char* name, char* type) {
    strcpy(symbolTable[symbolCount].name, name);
    strcpy(symbolTable[symbolCount].type, type);
    symbolCount++;
}

void lexicalAnalysis(char* input) {
    printf("\nLexical Analysis:\n");
    while (input[pos] != '\0') {
        skipWhitespace(input);

        // Ensure we are not at the end of input before processing
        if (input[pos] == '\0') break;

        // Handle single-line comments
        if (input[pos] == '/' && input[pos + 1] == '/') {
            while (input[pos] != '\n' && input[pos] != '\0') {
                pos++;  // Skip the comment
            }
            continue;  // Move to the next iteration
        }

        // Handle multi-line comments
        if (input[pos] == '/' && input[pos + 1] == '*') {
            pos += 2;  // Skip the "/*"
            while (input[pos] != '*' || input[pos + 1] != '/') {
                if (input[pos] == '\0') {
                    printf("Error: Unclosed multi-line comment\n");
                    exit(1);
                }
                pos++;  // Skip the comment
            }
            pos += 2;  // Skip the "*/"
            continue;  // Move to the next iteration
        }

        if (isalpha(input[pos])) {  // Check for identifiers or keywords
            char identifier[MAX_TOKEN_LENGTH];
            int i = 0;
            while (isalnum(input[pos]) || input[pos] == '_') {
                identifier[i++] = input[pos++];
            }
            identifier[i] = '\0';

            // Check if it's a keyword or an identifier
            if (strcmp(identifier, "int") == 0 || strcmp(identifier, "float") == 0 || strcmp(identifier, "char") == 0 ||
                strcmp(identifier, "if") == 0 || strcmp(identifier, "else") == 0 || 
                strcmp(identifier, "while") == 0 || strcmp(identifier, "for") == 0 || 
                strcmp(identifier, "do") == 0 || strcmp(identifier, "return") == 0 ||
                strcmp(identifier, "break") == 0 || strcmp(identifier, "continue") == 0 ||
                strcmp(identifier, "switch") == 0 || strcmp(identifier, "case") == 0) {
                printf("Token: %s, Type: Keyword\n", identifier);
            } else {
                printf("Token: %s, Type: Identifier\n", identifier);
                addSymbol(identifier, "unknown");  // Add to symbol table with unknown type
            }
        } else if (isdigit(input[pos]) || (input[pos] == '.' && isdigit(input[pos + 1]))) {  // Check for numbers
            char number[MAX_TOKEN_LENGTH];
            int i = 0;
            while (isdigit(input[pos]) || input[pos] == '.') {
                number[i++] = input[pos++];
            }
            number[i] = '\0';
            printf("Token: %s, Type: Number\n", number);
        } else if (input[pos] == '=') {
            if (input[pos + 1] == '=') {
                printf("Token: ==, Type: Comparison Operator\n");
                pos += 2;
            } else {
                printf("Token: =, Type: Assignment Operator\n");
                pos++;
            }
        } else if (input[pos] == '+') {
            if (input[pos + 1] == '=') {
                printf("Token: +=, Type: Compound Assignment Operator\n");
                pos += 2;
            } else {
                printf("Token: +, Type: Operator\n");
                pos++;
            }
        } else if (input[pos] == '-') {
            if (input[pos + 1] == '=') {
                printf("Token: -=, Type: Compound Assignment Operator\n");
                pos += 2;
            } else {
                printf("Token: -, Type: Operator\n");
                pos++;
            }
        } else if (input[pos] == '*') {
            if (input[pos + 1] == '=') {
                printf("Token: *=, Type: Compound Assignment Operator\n");
                pos += 2;
            } else {
                printf("Token: *, Type: Operator\n");
                pos++;
            }
        } else if (input[pos] == '/') {
            if (input[pos + 1] == '=') {
                printf("Token: /=, Type: Compound Assignment Operator\n");
                pos += 2;
            } else {
                printf("Token: /, Type: Operator\n");
                pos++;
            }
        } else if (input[pos] == ';') {
            printf("Token: ;, Type: Semicolon\n");
            pos++;
        } else if (input[pos] == '(') {
            printf("Token: (, Type: Left Parenthesis\n");
            pos++;
        } else if (input[pos] == ')') {
            printf("Token: ), Type: Right Parenthesis\n");
            pos++;
        } else if (input[pos] == '{') {
            printf("Token: {, Type: Left Brace\n");
            pos++;
        } else if (input[pos] == '}') {
            printf("Token: }, Type: Right Brace\n");
            pos++;
        } else if (input[pos] == '[') {
            printf("Token: [, Type: Left Bracket\n");
            pos++;
        } else if (input[pos] == ']') {
            printf("Token: ], Type: Right Bracket\n");
            pos++;
        } else if (input[pos] == '<') {
            if (input[pos + 1] == '=') {
                printf("Token: <=, Type: Comparison Operator\n");
                pos += 2;
            } else {
                printf("Token: <, Type: Comparison Operator\n");
                pos++;
            }
        } else if (input[pos] == '>') {
            if (input[pos + 1] == '=') {
                printf("Token: >=, Type: Comparison Operator\n");
                pos += 2;
            } else {
                printf("Token: >, Type: Comparison Operator\n");
                pos++;
            }
        } else if (input[pos] == '!') {
            if (input[pos + 1] == '=') {
                printf("Token: !=, Type: Comparison Operator\n");
                pos += 2;
            } else {
                printf("Token: !, Type: Logical Operator\n");
                pos++;
            }
        } else if (input[pos] == '&') {
            if (input[pos + 1] == '&') {
                printf("Token: &&, Type: Logical Operator\n");
                pos += 2;
            } else {
                printf("Token: &, Type: Bitwise Operator\n");
                pos++;
            }
        } else if (input[pos] == '|') {
            if (input[pos + 1] == '|') {
                printf("Token: ||, Type: Logical Operator\n");
                pos += 2;
            } else {
                printf("Token: |, Type: Bitwise Operator\n");
                pos++;
            }
        } else if (input[pos] == ',') {
            printf("Token: ,, Type: Comma\n");
            pos++;
        } else if (input[pos] == '.') {
            printf("Token: ., Type: Dot\n");
            pos++;
        } else if (input[pos] == ':') {
            printf("Token: :, Type: Colon\n");
            pos++;
        } else if (input[pos] == '?') {
            printf("Token: ?, Type: Question Mark\n");
            pos++;
        } else if (input[pos] == '"') {  // Handle string literals
            pos++; // skip the opening quote
            char stringLiteral[MAX_INPUT_LENGTH];
            int i = 0;
            while (input[pos] != '"' && input[pos] != '\0') {
                stringLiteral[i++] = input[pos++];
            }
            if (input[pos] == '"') {
                stringLiteral[i] = '\0'; // close the string
                printf("Token: \"%s\", Type: String Literal\n", stringLiteral);
                pos++; // skip the closing quote
            } else {
                printf("Error: Unclosed string literal\n");
                exit(1);
            }
        } else if (input[pos] < 32 || input[pos] > 126) {  // Check for control characters
            // Skip control characters but don't output
            pos++;
        } else {
            printf("Unknown token: %c\n", input[pos]);
            pos++;
        }
    }
}

int main() {
    char input[MAX_INPUT_LENGTH] = ""; // Initialize input to an empty string
    char line[MAX_INPUT_LENGTH];

    printf("Enter a program (end with EOF (Ctrl+D on Unix or Ctrl+Z on Windows)): \n");

    // Read multiple lines of input until EOF
    while (fgets(line, sizeof(line), stdin)) {
        strncat(input, line, MAX_INPUT_LENGTH - strlen(input) - 1);
    }

    lexicalAnalysis(input);

    return 0;
}