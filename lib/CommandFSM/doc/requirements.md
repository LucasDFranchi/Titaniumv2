# Requirements

## Command Processing Finite State Machine

    1 - This module must process one command at time.

    2 - This module receives as argument a command.

    3 - A process can't be interrupted, so to receive a new command a old process should finish first.

    3 - The FSM has thee states

        1 - Setup Command: This state validates if all the requirements for the command are present.

        2 - Execute Command: This state call the commands the loop function of the command.

        3 - TearDown Command: This state clear all the leftovers from the command.

        