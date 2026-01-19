# Adventure-game-in-C++
Project: The Greatest Maze (2-Player Co-op Game)
------------------------------------------------
General Description:
 Two players must work together to navigate through a series of rooms, solve riddles, avoid enemies, and manipulate obstacles to reach the "Winning Room".
 the game parses external layout files to generate rooms, logic, and puzzles.

Design Decisions & Implementation Details:
1. Game Loop & Time Management (The 'Tick' System):
   - Since the game runs in a single thread, we implemented a 'tick' counter in the main loop ('Room::run').
   - Movement updates are modulo-based ('tick % speed == 0'). This allows different entities (Players vs. Enemies) to move at different speeds without using complex multi-threading or sleep timers that would freeze input.

2. Enemy- Breadth-First Search (BFS):
   - The enemy (defined in 'Enemy.cpp') uses the BFS algorithm to calculate the shortest path to the nearest player.
   - One of the players must stand on the 'x' symbol to summon the enemy.
   - If the enemy reaches one of the players the game will be over, and you lose.
   - The enemy doesn't switch rooms with the players, it always stays in the same room.

3. Obstacles Mechanics:
   - We implemented a BFS-based "Flood Fill" algorithm in 'Obstacle.cpp'. This allows the game to detect adjacent obstacle characters ('*') and group them into a single physical object that moves as one unit.

4. Rendering & Screen Management:
   -We utilize a partial redraw system. The 'Drawer' class updates only specific characters that changed rather than redrawing the entire map every frame, preventing flickering.
	
5. Lighting System:
   - Some rooms are initialized as 'dark'.
   - If a player holds a torch item, the 'isVisible' function calculates a radius around them, rendering only the tiles within that circle.
   - In order to light the entire room, the player should look for a light switch - 's'. 

6. Colors:
	- The console support colors, In order to play without colors press 3 at the menu section.

7. Riddle:
	- When one of the players steps on a riddle ('?'), a question will appear. Until the question is answered correctly, the player won't be able to pass   

8. Survival:
    - A Spike ('<') reduces the players health by one point (heart)
	- A Bomb reduces the players health by one point (heart)
    - Collect Hearts ('H') to recover lost lives (up to a max of 3)
	
9.  Breakable and dynamic walls:  
	- Dynamic Walls: Walls that appear/disappear based on their respective pressure plate
	- Strategic Bombing: Some walls ('|') are reinforced, You will need to blast them twice to clear the path (they turn to '-' after the first hit)
	- A "W" wall can not be destroyed by bomb but breakable and dynamic wall can (Explanation of the walls in the line above)

10. Players navigation:
	- The game always follows the last player to switch rooms.
	- To switch rooms, the player must use the relevant key to the door.

11. Winning Condition:
   - To enter the winning room, BOTH players must have at least 50 points (Score is gained by solving riddles).

12. File Parsing & Robustness:
   - The game utilizes 'ScreenManager' to parse '.screen.txt' files.
   - It supports a "Visual Map" section followed by a metadata "Elements" section.
   - The parser includes validation logic (e.g., ensuring the Legend 'L' doesn't overlap with walls or go out of bounds).
	
	File Formats:
	A. Screen Files (*.screen.txt)
	The file is split into two parts:
	1. The Map (Rows 0-24): Visual representation (Walls 'W', Legend 'L', etc.).
	2. The Metadata: Starts with the line "Elements".
	   Format: <Keyword> <X> <Y> [Extra Params]
	   
	   Supported Keywords:
	   - Entities: Player1, Player2, Enemy
	   - Items: Key, Torch, Heart, Bomb
	   - Logic: Door_switch <target_door_id>, Pressure_plate <target_wall_id>
	   - Objects: Obstacle, Spring <DIRECTION>, Breakable_wall <hits>, Dynamic_wall <id>
	   - Global: Dark (sets room to dark), Locked (locks doors)

	B. Riddles File (riddles.txt)
	Uses a tag-based format:
	   Q: <Question Text>
	   A: <Option A>
	   B: <Option B>
	   C: <Option C>
	   D: <Option D>
	   ANS: <Correct char a/b/c/d>
	   
	   
13. Command Line Interface & Playback Mode (Exercise 3):
   The game supports running from the command line for recording and replaying sessions.
   
   Modes:
   - save:
     Runs the game normally (with menu)
     Records all user inputs (steps) to a file named 'adv-world.steps'.
     Records expected game events (screen changes, score updates, etc.) to 'adv-world.result'.
     
   - load:
     Replays a game session based on an existing 'adv-world.steps' file.
     In this mode, the menu is skipped, and user input (keyboard) is ignored.
     
   - silent:
     Valid only with '-load'.
     Runs the playback without rendering the map to the console.
     At the end, it compares the actual run with 'adv-world.result' and prints "Test Passed" or "Test Failed".

   * Note: For detailed file structure, please refer to 'files_format.txt'.


14. Provided Test Examples:
   In accordance with the requirements, we have included a folder named 'tests_for_ex3' containing 3 complete test scenarios.
   
   How to run the examples:
   1. Open the 'tests_for_ex3' folder.
   2. Choose a test case (e.g. files named like 'adv-world-test1...').
   3. Copy the step and result files to the main game directory.
   4. Rename the files to the default required names:
      - Rename your chosen steps file to -> 'adv-world.steps'
      - Rename your chosen result file to -> 'adv-world.result'
	  
Controls:
Player 1:  D/A/X/W (Move), S (Stay), E (Drop Item)
Player 2:  L/J/M/I (Move), K (Stay), O (Drop Item)
Menu:      1 (Start), 8 (Instructions), 9 (Exit), 3 (Play without colors)
Pause:     ESC
Exit:      ESC and then h 


Symbols & Items:
&    | Player 1                                           
$    | Player 2                                    
E    | Enemy            
K    | Key                               
!    | Torch
@    | Bomb 
#    | Spring         
S    | Light Switch  
/    | Door Switch on 
\    | Door Switch off  
L    | Pressure plate    
X    | Summon Enemy   
?    | Riddle      
*    | Obstacle      
W    | Wall
[    | Dynamic wall         
1-9  | Door 
H    | Heart
<    | Spike           
