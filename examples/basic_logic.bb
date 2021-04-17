# This is a comment
# All comments are ignored by the compiler.

let one = 1; # A variable declaration with an assignment
let two; # You can also declare a variable without an assignment

# Here's an example of a while loop
let counter = 1;
while (counter <= 10)
{
    print("Counter is: ", counter, "\n");
    counter = counter + 1;
}

# You can also use the dotimes loop to replicate the above while loop
counter = 1;
dotimes(10)
{
    print("Counter is: ", counter, "\n");
    counter = counter + 1;
}

# The Bare Bones Language also supports reading values from the command line
let input;
print("Enter a number: ");
read(input);
print("\nYou entered: ", input, "\n");