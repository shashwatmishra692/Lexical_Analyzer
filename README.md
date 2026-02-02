## Project Overview

This project implements a Lexical Analyzer (Tokenizer) for a C-like programming language using the C programming language.
The lexer reads a source file character by character and breaks it into meaningful tokens such as keywords, identifiers, operators, constants, literals, comments, and preprocessors.

It also performs basic syntax validation, such as:

* Matching parentheses ()

* Matching curly braces { }

* Matching square brackets [ ]

* Properly closed string and character literals

* Missing semicolon detection

---

## Features

* Tokenizes C source code into:
     * Keywords

     * Identifiers

     * Operators

     * Special characters

     * Constants

     * String literals

* Detects common lexical errors

* Tracks line numbers for each token

* Validates
   * Unclosed quotes

   * Unmatched brackets

   * Missing semicolons (basic check)

* Modular design using multiple source and header files

---

## Project Structure

```

Lexical-Analyzer/
│
├── main.c        # Entry point of the program
├── lexer.c       # Core lexical analysis logic
├── lexer.h       # Token definitions and function declarations
├── README.md     # Project documentation

```

## Token Types

The lexer identifies the following token types:

```

Token Type	                      Description

KEYWORD	                          C language keywords (int, float, return, etc.)
IDENTIFIER	                      Variable and function names
OPERATOR	                      Arithmetic and logical operators
SPECIAL_CHARACTER	              Symbols like { } ( ) [ ] , ;
CONSTANT	                      Numeric constants
LITERAL	                          String literals
PREPROCESSOR	                  Preprocessor directives (#include, #define)
COMMENTED_LINE	                  Single-line (//) and multi-line (/* */) comments
UNKNOWN	                          End of file or invalid token

```

---

## Compilation and Execution

## Compile 

`gcc main.c lexer.c -o lexer`

## Run

`./lexer <source_file.c>`

## Example

`./lexer sample.c`

---

## Sample Output

```

Type: KEYWORD              ------> Token: int                    line 1
Type: IDENTIFIER           ------> Token: main                   line 1
Type: SPECIAL CHARACTER    ------> Token: (                      line 1
Type: SPECIAL CHARACTER    ------> Token: )                      line 1
Type: SPECIAL CHARACTER    ------> Token: {                      line 2
Type: KEYWORD              ------> Token: return                 line 3
Type: CONSTANT             ------> Token: 0                      line 3
Type: SPECIAL CHARACTER    ------> Token: ;                      line 3
Type: SPECIAL CHARACTER    ------> Token: }                      line 4

```
Preprocessor directives

Comments (single-line and multi-line)

