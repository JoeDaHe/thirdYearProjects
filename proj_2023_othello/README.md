NB The IngeniousFramework.jar and the `Logs/` directory should be inside the `IngeniousFrame/` directory 
--------------------------------------------------------------------------------------------------------
The IngeniousFrame Jar uses relative paths to find the `Logs/` directory inside the `IngeniousFrame/` directory, i.e. the `Logs/` directory and the IngeniousFramework.jar file should reside inside the `IngeniousFrame/` directory.

Building players and running matches
------------------------------------ 
Executing: `python3 run_rr.py` 
1. builds the players by calling make inside the `src_random_player/` and `src_my_player/` directories respectively
 - Note that the executables are stored in the `players/` directory 
 - and the output files in `src_random_player/obj/` and `player_min/obj/` are deleted
2. and runs a tournament where `my_player` plays two matches against every other player (one where it makes the first move and one where it makes the second move) 

Random player
-------------
- The `src_random_player/` directory contains an Othello player that simply makes random moves 

Developing your own player
--------------------------
- The `src_my_player/` directory currently contains a copy of the random player
- Change this code to develop your own player
