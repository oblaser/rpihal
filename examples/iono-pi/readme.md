# Example - Iono Pi

In this example an Iono Pi from Sferalabs is used to implement a simple staircase and basement light controller.

### Staircase
A push button (or several in parallel) turns on the light and triggers a timer, which turns off the light after a given period of time.

| input | output |
|:---:|:---:|
| DI3 | O2 |

### Basement Rooms
A push button is used to switch the light on and off.

| room | input | output |
|:---:|:---:|:---:|
| 1 | DI4 | O3 |
| 2 | DI5 | O4 |
