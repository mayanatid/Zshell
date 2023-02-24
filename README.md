# Welcome to My Zsh
***

## Task
To construct a shell which can execute binary files as well as built-in commands.

## Description
The shell interface displays a prompt and, based on user input, attempts to execute commands.
The main cases to consider are 1.) executable binary files 2.) built-in commands and 3.) unknown commands.

For 1.) the program searches through the ```$PATH``` environment variable to see if a matching executable exists. If it does
the program forks and has a child process execute the binary. This allows the program to persist after the execution.

For 2.) the program executes the function directly from the source code (mostly in ```HelperFns/HelperFns.c```).

For 3.) the program throws an error.

From a data structure prospective, a Shell struct is used to handle I/O and most of the program logic.

## Installation
Run ```make```

## Usage

```
./my_zsh
```

and then use the prompt. Enter ```quit``` to end the program.
### The Core Team


<span><i>Made at <a href='https://qwasar.io'>Qwasar SV -- Software Engineering School</a></i></span>
<span><img alt='Qwasar SV -- Software Engineering School's Logo' src='https://storage.googleapis.com/qwasar-public/qwasar-logo_50x50.png' width='20px'></span>
