# Main Game Mechanics
To win the game you must last 2 minutes as that is the time the potion needs to fully brew.

If any flyer enters the cauldron the potion becomes unstable, and you lose.

Every 10 seconds the difficulty increases until difficulty level 10 after 1 minute 40 seconds. This increases flyer spawn rate and timer meaning more flyers will need defending off as you play the game. With each increase in difficulty there is a 20% chance an additional bomb will spawn.

Main game metrics are Time survived, Score and High Score.

Time survived = How long since you started playing.

Score = Receive score from defending flyers.
High Score = Highest noted score within the highscores.txt file.

Strawberries give 20 score points when intercepted.

Bananas give 10 score points when intercepted.

Pineapples give 30 score points when intercepted.

Bombs give -20 score points when intercepted.

You want to avoid intercepting bombs, when possible, but sometimes they will fall into the cauldron or will be near the ingredient you need to intercept. This adds difficulty as you need to extrapolate whether the bomb needs to be intercepted or not.

Within the game, the ingredients will go towards the cauldron (homing behavior), but it is not guaranteed that they will end up hitting it. This is intentional as if they all homed into the cauldron the player would just move 1cm each way and win each time. The added spread and uncertainty mean the players must move more and only target the ones that they think will end up making it to the cauldron. On the other hand, bombs only move downwards in a straight line but can still spawn on top of the cauldron and end up in it meaning an interception is needed. Generally, most flyers will end up making the cauldron, so it is intended for the player to intercept as many as possible but if they do not they reset back to the top.

The probability of a flyer being:
-	Strawberry: 30%
-	Banana: 30%
-	Pineapple: 30%
-	Bomb: 10%

(results may vary, as accurate as srand() function in cpp)
