# Gold-Chase-Test-scripts
1) This project is meant to create multiple test scripts on the existing Gold chase projects.
2) Though the project a few number of test cases, similar approach can be used for creating 
multiple case checking for various boundary conditions and cases.
3) The project uses forks for creating of multiple child processes which act as players.
4) Execlp is used to assign the game project executable to the child process and the parent process is the actual
testing project executable.
5) The parent process is used to create the environment and the map to be used my the child processes.
6) The communication between the parent and the child processes is done using pipes.
