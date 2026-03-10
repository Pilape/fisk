# Insert language name here
A simple embeddable concatenative language

## Features
- Functions
- Different types [int, bool, nil, list]

## TODO/roadmap
- Variables
- Strings

## Syntax
Every keyword and symbol must be seperated by whitespace. With the sole exception of ~Satoru Gojo~ comments.
```FORTH
{ square ( n -- n ) dup * } (Valid code)
{ square(n--n) dup * } (Also valid code)
{square(n--n) dup *} (Invalid code)
```
The language is also case sensitive so 
```FORTH
dup
```
But not
```FORTH
DUP
```

### Functions
Functions are defined within curly brackets. The word after the first curly bracket is the function name. Everything else is the code.
```FORTH
{ func dup dup + - }
```
> [!NOTE]
> It is also common for concatenative languages to have comments describing the stack before and after the function. For eksample: ( n -- ) accepts a number and returns nothing.

Once a function is defined it can be called by simply writing its name.
````FORTH
{ double ( n -- n ) dup + }

5 double
````

### Primitives
Primitives are functions/keywords that are built into the language. For eksample dup or swap
TODO: List all primitives
