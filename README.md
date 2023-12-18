# PixelBlaster
Here I'm publishing my robotics project, an 8x8 matrix game called Pixel Blaster, developed on Arduino Uno.

#Introduction
Hello adventurer, and welcome to my little project! Embark on a quest unlike any other, in a realm where light and strategy intertwine to create an enthralling adventure. In this world, your agility and reflexes are your greatest allies. As you navigate through the luminous labyrinth of an 8x8 LED Matrix, each glowing pixel presents a challenge to overcome and a mystery to unravel. Pixel Blaster is not just a game; it's a test of courage, a journey of discovery, and a dazzling dance of light. So, brave adventurer, are you ready to blaze a trail through this matrix of mystery and emerge as a legendary Pixel Blaster? The quest awaits!

Okay, now on a more serious note, this little project is my take on creating a fun and challenging game, using an LED Matrix. It sums up most of the things I learned throughout my robotics course, all bundled up in one game that not only should be fun to play, but also intuitive and easy to use. Creating the game itself was only a small part of the challenge, since I had to also take into account the display implementations and overall usability of my project. One of the questions I asked myself while implementing the game was "would a random person know how to use this?" and if the answer was no, that meant I had to redo it.

#Physical components
I know it looks pretty complicated since there are a lot of wires, but I didn't find a more aesthetically pleasing sollution just yet. Maybe changing the wires to ones that can be shaped and pressed against the breadboard would be a better idea, but I didn't want to risk taking apart my whole project in order to switch the wires, since it had to be ready for presenting and I could not afford a faulty connection before my deadline. However, after I get my grade on it, I would like to change the wires and find a cuter way to display it, but for now the most important part for me was making it actually work. I'll walk you through what the components for this project are and how I connected them all together.
- Arduino Uno: Self explainitory
- 8x8 LED Matrix: This is where the game "will take place". Here I'm going to be displaying the player and its actions
- LCD: Used for game-user interractions. Here will be displayed all settings and information about the game
- Joystick: Used to navigate the menus and make selections, and also for actually playing the game and moving through the matrix
- Potentionmeter: For adjusting the contrast on the LCD in order to make it readable. Also added a resistor accordingly
- Buzzer: For making sounds depending on the actions of the user (navigating the menu, shooting an obstacle, level up etc)
- Driver: Maximizes LED matrix efficiency and performance, ensuring smooth gameplay and dynamic visual effects
- 2 Condensators: Stabilize voltage and power flow, crucial for maintaining consistent performance and preventing fluctuations

#Code
I tried to keep the code for this project as simple as possible, since I might want to come back to it later in order to add extra stuff, and it would make my job so much easier actually undersatnding what I wrote. I feel like the code splits up into 3 parts: the actual game, the menu and handling the menus in order to have effects on the game (basically putting the two together, usually represented by functions with a name similar to handleCertainMenu()).
The game itself is pretty straight forward and it was fun to implement, since it's pretty basic. The idea is that the player has to choose a difficulty (1-3) and enter its name if desired, and then the game starts. You have to shoot the LEDs from the obstacles displayed, but depending on the difficulty chosen, the LEDs dissaper one by one at a certain rate, so you have to be fast and pay attention. Firstly, I created a function that would interpret the movements of my joystick, so that the player can move in all 4 directions and also shoot lasers towards the obstacle by pressing the joystick button, then I have a function that makes the LEDs dissapear depending on the difficulty. There's also a function for moving to the next obstacle when the current one was destroyed, as well as one that checks if at least one LED from the current obstacle was shot (if not, the player loses and the game ends). When the player loses, a sad face is displayed on the matrix and the highscore is updated if needed. I put all the functons together in a big one that makes the game playing experience smooth and fun. 

The menu is what I found to be most difficult to implement. The menu and all the possible settings are displayed on the LCD, so that the game can be played without a laptop, you can just connect it to any power source and play. I used the same function as above for interpreting the joystick movements, and then depending on if I moved left or right, I can navigate through the menu and the selections are made by pressing the joystick button. In order to keep track of what I'm choosing, I made an array for each menu, and used an index to interpret my selections. Using a switch-case structure, I called specific functions, depending on what I selected. 
With this implementation, here's what I can do:
-Select the difficulty ( from 1 to 3)
-Adjust the LCD brightness - saved to EEPROM
-Adjust the matrix brightness - saved to EEPROM
-Enter your name (max 3 characters)
-Sound ON/OFF 
-Reset the highscore - saved to EEPROM
-Display the "About" section and scroll through the info up and down
-Display the highscore

What I found very useful throughout implementing this menu, was using a buttonPressed function that checks if the joystick button was pressed once. You may find it funny (since it's so basic), but writing this function gave me some trouble and mastering it was a very important step, since it was essential for navigating the menu. 
The loop() function is a switch-case structure that handles the state of the program. I used an enum for the states, consisting of  MainMenu, DifficultySelection, GamePlay, Settings, GameSettings and depending on what the state is, the program enters the specific menu/submenu or it starts the game.

#What I learned from this project
This project was more of a programming challange for me than a robotics one, since we did the setup in our lab and we got instrunctions on it, so what was left for me was to write the code. Initially I was overwhelmed since there were a lot of functionalities to implement and I didn't know where to start, but eventually I built up the courage to begin. Starting with a very basic version of the game was a good idea, after which I implemented a very basic version of the menu, adding more options and improvements along the way. Organizing my code was also somewhat of a challange, since I wanted to keep it as clean as possible, making it easy to understand by someone else. Switch-case structures were a very good option, since that made it easy to keep my logic cleanly structured.

#What I wish I knew before I started
Take a deep breath! I usually get anxious when I am faced with a big project and this was no exception, but creating a skeleton for my code by implementing the basic functions and then adding more was a very good tactic. By doing so, I wasn't distracted by a random game bug for example, and was able to develop the whole structure of the program and then fix the bugs and make it better. A pen and paper also really helped, since it really helps me when I draw the functionality of my project using arrows and states. This might just be a me thing, but thinking about the big picture in simple terms is what ensures a smooth sailing through writing the code. 

#What I will do differently next time
While I am proud of myself in terms of time management (believe it or not, I finished this project 2 days before my deadline and I didn't lose any sleep), I wish I made my project more interesting. I feel like I did the bare minimum in order for it to meet the requirements, which is not a bad thing, but this course got me hooked and I have bigger expectations from myself. My plan is to continue developting this game after I present it as my homework, since I discovered a lot of things I can improve while making the final adjustments. Some of the things I wish to add are:
-making the obstacles move horizontally on the matrix for added difficulty
-having a leaderboard with the best 3 highscores instead of 1


