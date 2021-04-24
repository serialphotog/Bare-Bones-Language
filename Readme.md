# The Bare Bones Programming Language

This repository contains what I am calling the *Bare Bones Programming Language*; A bare bones programming language implementation that I wrote as a project for my *Concepts of Programming Languages* class. 

The language is extremely simple - you may even say that it's bare bones! It implements a basic tokenizer, a recursive decent parser, and a code generator that outputs the "compiled" code into valid C-language code, which can then be compiled with a C compiler of your choice.

## Language Features

The whole point of this language is to be simple. As such, you'll notice that there are a number of features, some of them rather important for doing real work, missing from the implementation. This is an intentional choice, not an oversight. Some of the include a lack of an array type, lack of subroutines, and a lack of a proper type system (all values are assumed to be integers). These choices were made simply to keep the language implementation as small and simple as possible.

In gerneral, the other features/constructs of the Bare Bones Language are:

* All statements should end with a semicolon, as in C, C++, or Java.
* All variables are assumed to be of type integer.
* All variables must be declared before use by using the *let* keyword. You may assign a value to a variable at the same time you're declaring it, as in *let foo = 5;*. It's also probably worth mentioning that the Bare Bones Language has no concept of scope for its variables. While you can declare a variable at any point in your program, they will ultimately have global scope.
* Looping is supported through a *while* loop and a *dotimes* loop. The *while* loop continues while some boolean expression is true. The *dotimes* loop takes either an integer literal or a variable and loops that number of times. For both, the condition must be contained in parenthesis after the looping-keyword and the bodies of the loops must be contained within a set of curley braces. For example: *while (i > 5) { i = i + 1; }*
* The language supports a standard *if* and *if-else* construct. The parameters for an *if* must be enclosed in parenthesis and the bodies for *if* and *else* must be contained in curley braces. 
* The language supports nesting of loops and if statements.
* You can output to the console using *print*. The paremeters to the print must be enclosed in parenthesis. Print can take either a string literal, a variable, or some combination of the two. In the event that you're passing multiple parameters to print, you separate them with a comma. For example, you could do something like this: *print("The value of i is: ", i, ". The value of foo is: ", foo, ".\n");*
* You can read a user-supplied integer value into a variable using the *read* keyword. Read takes a single paremeter: the variable to be read into. The parameter must be enclosed in parenthesis and the variable must have been previously declared with *let*. For example: *read(foo);* would read an integer value inputed by the user at the command line into the variable *foo*. 
* Anything that begins with a *#* character is treated as a comment and is ignored by the compiler.

### Debug Configuration

The file ***src/bb.h*** contains two defines that will enable various debug parameters to be enabled in the build. If the ***DEBUG*** define is uncommented then debug output will be enabled in the compiler. This will allow you to see what the lexer and the parser is doing at each stage. 

Additionally, there is a ***PRETTY_PRINT*** define. If this is uncommented, the C output generated will be much more human readable, which is useful for debugging purposes. If this is commented out, then the generated output is extremely compact and is not inteded for human consumption. 
