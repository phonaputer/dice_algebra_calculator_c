# Dice Algebra Calculator Written in C

A dice algebra expression parser & calculator written in C. 
The purpose of writing this application is to have fun trying out C by writing a simple application.
A dice algebra calculator (including lexing, parsing, and execution of string expressions) was selected as something that is probably complex enough to be interesting yet is simple enough to do in a single afternoon.
It's also something that can be extended later if that seems like it would be fun (e.g. this application could run a GUI window in which expressions can be input & a breakdown of the results can be shown, etc.).

## Dice Algebra

This application supports dice expressions including...

#### Dice rolls
`xdy` or `xDy` (where both `x` and `y` must be integers) will roll a `y`-sided die `x` times.

For example, `3d6` will roll a 6-sided die three times and sum the results.

The leading `x` may be omitted if it is 1. For example, `d4` rolls a 4-sided die one time.

#### ...

...under construction...

## ANTLR Grammar

The above dice algebra format can be expressed as the following ANTLR grammar.

```ANTLR
// Parser

add: mult (('+' | '-') mult)* ;

mult: atom (('*' | '/') atom)* ;

atom: (roll | '(' add ')') ;

roll : (integer | longroll | shortroll) ;

longroll : integer D integer ((H integer | L integer))? ;

shortroll : D integer ; 

integer : NUMBER ;

// Lexer

WHITESPACE : ' ' -> skip ;
NUMBER     : '-'?[0-9]+ ;
D : 'd' | 'D' ;
PLUS : '+' ;
MINUS : '-' ;
MULT: '*' ;
DIV : '/' ;
OPENPAREN : '(' ;
CLOSEPAREN : ')' ;
H : 'h' | 'H' ;
L: 'l' | 'L' ;
```

## Tooling

This project uses the Meson build system. [The getting started guide](https://mesonbuild.com/Tutorial.html) on the Meson website explains how to install and use Meson.