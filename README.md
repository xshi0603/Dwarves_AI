# Dwarves_AI

## Hunter College

## Created by: Xing Tao Shi

This project displays a n x m grid of dwarves trying to collect resources and survive against zombies. You can see what each symbol represents on the top right when you run the program.


Somes goals that these dwarves are trying to fufill include:

```
Surivival (touching a zombie induces death; however, dwarves can use lumber to build walls that zombies cannot cross)
Apples (apple trees can be collected in order to gain apples)
Pumpkins (pumpkin zombies will spawn a pumpkin when they die which can be collected to gain points)
Fences (at the very end, the program calculates the longest conjoined fence)
```

## How to run it:

In order to run this program, first clone the repo onto your local machine. Then, locate and enter the folder and type the following command:

```
make
```

After doing this, the program will create a file called *dwarves*. You can then type the following command:

```
./dwarves c
```

You can also specify the number of rows/columns the map will have by instead typing the following: 

```
./dwarves c ROWS COLS
```

Afterwards, there are several keys you can press to modify the screen:

```
f - fast-foward
p - pause
q - quit
s - step
```

Sit back and watch the dwarves work!
