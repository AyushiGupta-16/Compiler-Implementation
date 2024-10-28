#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKENS 100

typedef struct {
    char token[50];
    char type[20];
} Token;

Token tokens[MAX_TOKENS];
int token_count = 0;

void add_token(const char *token, const char *type) {
    if (token_count < MAX_TOKENS) {
        strcpy(tokens[token_count].token, token);
        strcpy(tokens[token_count].type, type);
        token_count++;
    }
}
void tokenize(const char *code) {
    char buffer[50];
    int buf_index = 0;
    token_count = 0;
    int in_comment = 0; // Flag to track if we are inside a comment

    for (int i = 0; i < strlen(code); i++) {
        char ch = code[i];

        // Handle single-line comments
        if (ch == '/' && code[i + 1] == '/') {
            break; // Ignore the rest of the line
        }

        // Handle multi-line comments
        if (ch == '/' && code[i + 1] == '*') {
            in_comment = 1; // Start of multi-line comment
            i += 2; // Skip the "/*"
            continue; // Continue to the next character
        }
        if (in_comment) {
            if (ch == '*' && code[i + 1] == '/') {
                in_comment = 0; // End of multi-line comment
                i += 2; // Skip the "*/"
            }
            continue; // Skip characters while inside a comment
        }

        if (isspace(ch)) {
            continue; // Skip whitespace
        }

        if (isalpha(ch)) { // Handle identifiers and keywords
            buffer[buf_index++] = ch;
            while (isalnum(code[i + 1])) {
                buffer[buf_index++] = code[++i];
            }
            buffer[buf_index] = '\0';

            // Check for keywords
            if (strcmp(buffer, "int") == 0 || strcmp(buffer, "float") == 0 || 
                strcmp(buffer, "char") == 0 || strcmp(buffer, "if") == 0 || 
                strcmp(buffer, "while") == 0 || strcmp(buffer, "else") == 0) {
                add_token(buffer, "Keyword");
            } else {
                add_token(buffer, "Identifier");
            }
            buf_index = 0;
        } else if (isdigit(ch) || (ch == '.' && isdigit(code[i + 1]))) { // Handle numbers
            buffer[buf_index++] = ch;
            while (isdigit(code[i + 1]) || code[i + 1] == '.') {
                buffer[buf_index++] = code[++i];
            }
            buffer[buf_index] = '\0';
            add_token(buffer, "Number");
            buf_index = 0;
        } else { // Handle operators and symbols
            buffer[buf_index++] = ch; // Add current character to buffer
            buffer[buf_index] = '\0';

            // Check if the character is an operator or symbol
            if (ch == '=' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || 
                ch == '>' || ch == '<') {
                add_token(buffer, "Operator");
            } else if (ch == ';' || ch == '(' || ch == ')' || ch == '{' || ch == '}') {
                add_token(buffer, "Symbol");
            }
            buf_index = 0; // Reset buffer index
        }
    }
}


#define MAX_STACK_SIZE 100

typedef struct {
    char items[MAX_STACK_SIZE];
    int top;
} Stack;

void init_stack(Stack *s) {
    s->top = -1;
}

int is_empty(Stack *s) {
    return s->top == -1;
}

void push(Stack *s, char item) {
    if (s->top < MAX_STACK_SIZE - 1) {
        s->items[++(s->top)] = item;
    }
}

char pop(Stack *s) {
    if (!is_empty(s)) {
        return s->items[(s->top)--];
    }
    return '\0'; // Return null character if stack is empty
}

int syntax_analysis() {
    int expecting_identifier = 0; // Flag to check for identifier after keyword
    Stack stack;
    init_stack(&stack);
    int block_opened = 0; // Track if a block is opened

    for (int i = 0; i < token_count; i++) {
        // Check for variable declaration
        if (strcmp(tokens[i].type, "Keyword") == 0) {
            if (strcmp(tokens[i].token, "int") == 0 || 
                strcmp(tokens[i].token, "float") == 0 || 
                strcmp(tokens[i].token, "char") == 0) {
                expecting_identifier = 1; // Set to expect an identifier next
            }
        } else if (strcmp(tokens[i].type, "Identifier") == 0) {
            if (expecting_identifier) {
                expecting_identifier = 0; // Identifier found after keyword
            } else {
                printf("Syntax Error: Identifier '%s' found where it wasn't expected.\n", tokens[i].token);
                return 0; // Unexpected identifier
            }
        } else if (strcmp(tokens[i].type, "Operator") == 0) {
            // Check if an operator appears without a preceding identifier
            if (expecting_identifier) {
                printf("Syntax Error: Operator '%s' found without preceding identifier.\n", tokens[i].token);
                return 0; // Unexpected operator
            }
            // Reset expecting_identifier as an identifier should follow this operator
            expecting_identifier = 1; // Expecting an identifier after an assignment
        }

        // Check for missing semicolon: improved logic
        if (strcmp(tokens[i].type, "Symbol") == 0) {
            if (strcmp(tokens[i].token, ";") == 0) {
                // Reset expecting_identifier flag
                expecting_identifier = 0;
            } else if (strcmp(tokens[i].token, "{") == 0) {
                push(&stack, '{');
                block_opened = 1; // Mark that a block has started
            } else if (strcmp(tokens[i].token, "}") == 0) {
                if (is_empty(&stack)) {
                    printf("Syntax Error: Unmatched closing brace '}'\n");
                    return 0;
                }
                pop(&stack);
                // Reset expecting_identifier flag after closing a block
                expecting_identifier = 0; 
            } else {
                // If we encounter any other symbol not recognized as a terminator, check for errors
                if (block_opened && (strcmp(tokens[i].token, "else") != 0)) {
                    printf("Syntax Error: Missing semicolon or unexpected symbol '%s'\n", tokens[i].token);
                    return 0;
                }
            }
        }

        // Check for parentheses matching
        if (strcmp(tokens[i].token, "(") == 0) {
            push(&stack, '(');
        } else if (strcmp(tokens[i].token, ")") == 0) {
            if (is_empty(&stack)) {
                printf("Syntax Error: Unmatched closing parenthesis ')'\n");
                return 0;
            }
            pop(&stack);
        }
    }

    // Check for unmatched opening parentheses and braces
    if (!is_empty(&stack)) {
        if (stack.items[stack.top] == '(') {
            printf("Syntax Error: Unmatched opening parenthesis '('\n");
        } else if (stack.items[stack.top] == '{') {
            printf("Syntax Error: Unmatched opening brace '{'\n");
        }
        return 0;
    }

    // Check for missing semicolon at the end of the last statement if it's not empty
    if (token_count > 0 && strcmp(tokens[token_count - 1].token, ";") != 0 && 
        strcmp(tokens[token_count - 1].token, "}") != 0) {
        printf("Syntax Error: Missing semicolon at the end of the statement.\n");
        return 0;
    }

    printf("Syntax analysis completed successfully.\n");
    return 1;
}


void run_test(const char *code) {
    printf("Input:\n%s\n", code);
    tokenize(code);
    printf("Tokenized Output:\n");
    for (int i = 0; i < token_count; i++) {
        printf("Token: %s, Type: %s\n", tokens[i].token, tokens[i].type);
    }
    printf("Syntax Analysis:\n");
    syntax_analysis();
    printf("\n");
}

int main() {
    char code[1000];
    char line[100];
    code[0] = '\0'; // Initialize to an empty string
    printf("Enter your C-like code (type 'END' on a new line to finish input):\n");
    
    while (1) {
        fgets(line, sizeof(line), stdin);
        if (strcmp(line, "END\n") == 0) {
            break; // Stop input on 'END'
        }
        strcat(code, line); // Append the new line to the code string
    }
    
    run_test(code); // Run the tokenization and syntax analysis on the full input
    return 0;
}