LARP system readme.
So this uses the trinket pro from the Adafruit store to create an IR beam shooting LARP system.
The rules are pretty simple.
Requires a computer with the Arduino IDE installed to use the serial output to set the character's stats.
Will also require an FTDI cable to send the serial stuff over correctly.

**Rules**
Character's stats are Soak, Mitigate, Attack, Damage, Light, Dark, Fire, Water, Earth, and Air resistances, and Attacks 1, 2, and 3.
Soak is how much damage characters can take (HP).
Mitigate is the attribute to resist normal attacks (armor).
Attack is how likely a normal attack is to hit.
Damage is how much you hurt when you do hit.
Resistances automatically halve damage if you have the resistance to an element that hits you or reduce the duration of effects by half if you have the resistance.
Attacks are your 3 attacks that you have.

Effects
Heal damage is added to character's soak instead of removed from their soak up to the soak stat.
Persuasion damage is set to zero for the persuasion duration.
Deception attack and damage are set to zero for deception duration.
Elemental attack (earth, air, fire, water, dark, light) bypass mitigate and only check the resistance value.  If resisted then only does half damage.
Fast Attack attack is a speedier attack but is more prone to miss.  Attack value - 1 vs mitigate.  If "misses" no damage at all.
Normal Attack for standard attacking.  Attack value vs mitigate.  If "misses" no damage at all.
Accurate Attack hits more often.  Attack value +2 vs mitigate.  If "misses" no damage at all.
Dammage Over Time deals the damage one point at a time over a little time instead of all at once.
Slow increases attackWait time when you attack.
Blind sets attack to zero.
Stun sets armor to zero.
Enrage increases damage to enrage ammount then reduces the bonus slowly until it's back to normal.  Damages you when the duration is over.
Cure cancels any negative effects on target (DoT, deception, persuasion, slow, and blind).

** Character Input Value **
Serial input of the character's attributes is required through serial input from an FTDI cable and the Arduino IDE.

Key for what the character's number is, 0123456789abcdef
(values are 0-9)
0 = soak
1 = attack
2 = damage
3 = mitigation
(values are 0 for have and 1 for have not)
4 = light
5 = dark
6 = fire
7 = water
8 = earth
9 = air
(values are 0-9 table for what they are is below.)
a = attack 1 skill/element
b = attack 1 effect
c = attack 2 skill/element
d = attack 2 effect
e = attack 3 skill/element
f = attack 3 effect
skill/element         Effect
0 = none              0 = none
1 = normal attack     1 = fast attack
2 = persuasion        2 = heal
3 = deception         3 = damage over time
4 = earth             4 = slow
5 = air               5 = blind
6 = fire              6 = stun
7 = water             7 = enrage
8 = dark              8 = accurate attack
9 = light             9 = cure
Using the serial input 8212000000026310 is the standard stats set of new characters.

** Hardware **
5v Trinket pro,
https://www.adafruit.com/products/2000
Trinket charging backpack,
https://www.adafruit.com/products/2124
LiIon battery,
https://www.adafruit.com/products/1781
IR receiver,
https://www.adafruit.com/products/157
IR LED,
https://www.adafruit.com/products/387
NeoPixel stick,
https://www.adafruit.com/products/1426
Buzzing motor (stole one from an old controller, but here's one to by),
https://www.adafruit.com/products/1201
Half perma-proto board,
https://www.adafruit.com/products/1609
Buttons,
https://www.adafruit.com/products/367
Ethernet cord
https://www.adafruit.com/products/730
Ethernet jack
https://www.adafruit.com/products/2912
metal tin
https://www.adafruit.com/products/97

**Wiring**
This is a bit complicated, but i'll try to explain as best as i can.
There should also be a .jpg file in this directory with a picture of the wiring setup.

Firstly the two ethernet jacks are showing that the two units are connected by an ethernet cord.
So connect those with a standard RJ-45 cord and jack.

Trinket pro connections
BAT+ to positive rail.
GND to ground rail.
Pin 6 to the neopixel strip.
Pin 3 to a 4.7k ohm resistor that goes to the middle pin of the 2222 transistor.
Pin 9 to the solid blue ethernet jack connection.
Pin 10 to the blue stripe etherent jack connection.
Pin 11 to the left pin of the IR receiver.
Pin 12 to the brown stripe ethernet jack connection.
Pin 13 to the solid orange ethernet jack connection.

IR Receiver (rounded side facing you)
Left pin to pin 11 on the trinket pro.
Middle pin to the ground rail.
Right pin to the positive rail.

Transistor (rounded side away from you)
Left pin to the orange stripe ethernet jack connection.
Middle pin to the 4.7k ohm resistor that connects to pin 3 on the trinket pro.
Right pin to the ground rail. 

Ethernet jack
Orange stripe to the left pin of the transistor.
Orange  to pin 13 on the trinket pro.
Green stripe to the ground rail.
Green to the positive rail.
Blue stripe to pin 10 on the trinket pro.
Blue to pin 9 on the trinket pro.
Brown stripe to pin 12 on the trinket pro.

Ethernet cord (pictured as another ethernet jack)
Orange stripe to the negative lead on the led.
Orange  to the positive lead of the vibrational motor.
Green stripe to the ground rail.
Green to the positive lead on the led.
Blue stripe to button 2.
Blue to button 1.
Brown stripe to button 3.

IR led
Negative lead to the orange stripe ethernet wire.
Positive lead to the green ethernet wire.

Vibrational motor
Positive lead to the orange ethernet wire.
Negative lead to the ground rail.

Button 1
First pin to ground rail.
Pin on same side as the first pin to blue ethernet wire.

Button 2
First pin to ground rail.
Pin on same side as the first pin to blue stripe ethernet wire.

Button 3
First pin to ground rail.
Pin on same side as the first pin to brown stripe ethernet wire.

** To Do **
have ir range, need to create bread board layouts and explanations for the three options.
put in attackWait for each attack and give each a unique buzz.
adjust how effects show up on the lights. (ideal would be flash debuff 1 sec, health 1 sec, other debuff 1 sec, new debuff displays as soon as you get it).

set setup to check eeprom for values and set them to defaults if they're weird or completly wrong (factory value was 255 so anything over 9?)
update to IRLib2, since the original is depreciated.
write IR code for IRLib2 to prevent remote controller hax.

add in magic stuff (steal/mod from casting jacket thing elsewhere in my git account.)
put in 2 separate buzzers.  first in the main body to buzz on receiving an attack.  second on the attacky thing to buzz when an attack is launched and when the attackWait is finished.
