[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/xR-cYv8r)
# project1
Answer these questions please:
Tell me what this project is about?
Tell me how your thought process for completing the project?
Any issues you came across?
 
The Simple Linux Shell is a command-line interface developed and designed to perform fundamental Linux commands. Users can interact with the shell by entering commands, including built-in functionalities like /proc for accessing process-related information and exit for exiting the shell. The program also maintains a command history, enabling users to review their executed commands with the history command

The thought process was implementing a user input loop for command execution, and considering features such as command parsing, execution of both built-in and external commands, command history, and dynamic memory management. Additionally, error handling and testing were crucial to ensure robust and reliable functionality, while documentation and version control were essential for project organization and collaboration. 

I could not get history to be just the last 10 so you get the whole file. It also leaks but it doesn't break or segfault. I also cried 3 times trying to figure out how to read to the .421sh file.

Proc only works like /proc .
Ex:
/proc /1/status
/proc /cpuinfo
