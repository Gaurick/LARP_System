/* Micro controller based larp system. 
 * Attacks are sent through IR LEDs and processed by the micro controller.
 * Includes option to set character's stats by using the Arduino's serial input.
 * Made possible by the adafruit store and swearing off video games for lent.
 *
** Libraries **
 * Uses the IRLib from here,
 * https://github.com/cyborg5/IRLib/
 * (need to update to IRLib2 eventually since IRLib is depreciated.)
 * Adafruit NeoPixel (or is it Neo Pixel?) library from Adafruit,
 * https://github.com/adafruit/Adafruit_NeoPixel
 * Also the Arduino EEPROM library that comes with the Ardunio program.
 * 
 * See readme.txt in this directory for more information and stuff.
*/

#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include <IRLib.h>
//libraries required to make the neopixels and IR receiver work.

IRrecv My_Receiver(11);
//set IR receiver to work on pin 11.
IRdecode My_Decoder;
//sets the My_Decoder to be the IR receiver object.
IRsend My_Sender;
//sets the My_Sender to be the IR sender object.

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, 6);
//initializes the neopixel strip object.

//character stats.
byte soak = EEPROM.read(0);
//hitpoints for the "character".
byte attack = EEPROM.read(1);
//stat for attack value.
byte damage = EEPROM.read(2);
//stat for character's damage.


//character's armor and resistances.
byte mitigate = EEPROM.read(3);
//armor value.
bool light = EEPROM.read(4);
bool dark = EEPROM.read(5);
bool fire = EEPROM.read(6);
bool water = EEPROM.read(7);
bool earth = EEPROM.read(8);
bool air = EEPROM.read(9);
//resistances that the characters may or may not have.
//they either have it or not.

byte attack1skillElement = EEPROM.read(10);
byte attack1effect = EEPROM.read(11);
byte attack2skillElement = EEPROM.read(12);
byte attack2effect = EEPROM.read(13);
byte attack3skillElement = EEPROM.read(14);
byte attack3effect = EEPROM.read(15);
//variables for the 3 attacks ability, element, effect, and material used.

byte deception = 0;
byte persuasion = 0;
byte dot = 0;
byte blind = 0;
byte slow = 0;
byte stun = 0;
byte enrage = 0;
byte attackWait = 0;
//placeholders for various effects that aren't just hurting you.

int counter = 0;
//counter to count.  
//used to not delay all the time, but pass time.
byte textBuffer = 0;
//buffer variable to hold inputs for setting the character's stats.

void setup(){
  Serial.begin(9600);
  //start serial communications for debugging.
  strip.begin();
  strip.show();
  //starts the neopixels.
  My_Receiver.enableIRIn();
  //starts the IR receiver.
  strip.setBrightness(64);
  //reduces the brightness of the neopixels.
  show();
  //run the function to show the character's hp.

  pinMode(13, OUTPUT);
  //built in led to make sure attacks are sending (i still can't see ir...).
  //also tied a buzzer motor to that pin so players can get buzzes when things happen.
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  //set button pins to be inputs and use the internal pullup resistors.

  //Serial.println("If you can read this and want to adjust the character, please input the data.");
  Serial.println("ready.");
}

void loop(){
  //did the IR receiver hear that?
  if (My_Receiver.GetResults(&My_Decoder)){
    My_Decoder.decode();
    if(My_Decoder.decode_type== RC5){
      //is the signal protocol correct?

      Serial.print("IRCode="); Serial.println(My_Decoder.value);

      sort();
      counter = 0;
      //gets the attack categories to attack the character.
    }
    My_Receiver.resume();
    //restarts the receiving of ir signals.
    //stopped when decoding to keep the micro controller from melting.
  }

  if(Serial.available() > 0){
    gmSet();
  }

  effects();
  //sorts the effects from other attacks.
  
  int attackState = digitalRead(9);
  if(soak > 0){
    if(attackState == LOW){
      //attack 1 done.
      myAttack(damage, attack, attack1skillElement, attack1effect);
    }
    
    attackState = digitalRead(10);
    if(attackState == LOW){
      //attacking with number 2.
      myAttack(damage, attack, attack2skillElement, attack2effect);
    }
    
    attackState = digitalRead(12);
    if(attackState == LOW){
      //third attack option choosen.
      myAttack(damage, attack, attack3skillElement, attack3effect);
    }
  }
  if(counter == 500){
    counter = 0;
    delay(10);
  }
  else{
    counter ++;
    delay(10);
  }
}

void show(){
  //if you're decieved, show blue health until you're no longer decieved.
  if(deception > 0){
    for(byte w = 0; w < soak; w++){
      strip.setPixelColor(w, 0, 0, 255);
      strip.setBrightness(64);
    }
  }

  //if you're persuaded, show purple until you're no longer persuaded.
  else if(persuasion > 0){
    for(byte x = 0; x < soak; x++){
      strip.setPixelColor(x, 255, 0, 255);
      strip.setBrightness(64);
    }
  }

  //if you're blind, then darken the health until you can see again.
  else if(blind > 0){
      strip.setBrightness(1);
  }

  else if(slow > 0){
    for(byte y = 0; y < soak; y++){
      strip.setPixelColor(y, 255, 255, 0);
      strip.setBrightness(64);
    }
  }

  else if(enrage > 0){
    for(byte z = 0; z < soak; z++){
      strip.setPixelColor(z, 255, 0, 0);
      strip.setBrightness(64);
    }
  }

  //if your health isn't too bad, show green.
  else if(soak > 5){
    for(byte a = 0; a < soak; a++){
      strip.setPixelColor(a, 0, 255, 0);
      strip.setBrightness(64);
    }
  }

  //yellow health to show you're hurting.
  else if(soak > 3){
    for(byte b = 0; b < soak; b++){
      strip.setPixelColor(b, 255, 255, 0);
      strip.setBrightness(64);
    }
  }

  //red health to show you're almost dead.
  else{
    for(byte c = 0; c < soak; c++){
      strip.setPixelColor(c, 255, 0, 0);
      strip.setBrightness(64);
    }
  }

  //darken any lost health neopixels.
  for(byte d = soak; d < 8; d++){
    strip.setPixelColor(d, 0, 0, 0);
  }
  
  strip.show();
  //actually make the colors show up.
}

void myAttack(int a, int b, byte c, byte d){
  if(attackWait == 0){
    digitalWrite(13, HIGH);
    a = a * 1000;
    b = b * 100;
    c = c * 10;
    int e = a + b + c + d;

    My_Sender.send(RC5, e, 14);
    Serial.println(e);
    if(c > 1){
      if(c < 4){
        attackWait = attackWait + 1;
      }

      else{
        attackWait = attackWait + 2;
      }
    }

    if(d > 1){
      if(d <7){
        attackWait = attackWait + 1; 
      }
      
      else{
        attackWait = attackWait + 2;
      }
    }
    delay(250);
    digitalWrite(13, LOW);
    My_Receiver.enableIRIn();
  }
}

void effects(){
  if(deception > 0){
    //deception mechanics sets damage and attack values to zero for the duration.
    attack = 0;
    damage = 0;
    deception --;
  }
      
  if(persuasion > 0){
    //persuasion cancels out the character's damage for the duration.
    damage = 0;
    persuasion --;
  }
    
  if(blind > 0){
    //blinded!
    attack = 0;
    blind = blind - 1;
  }

  if(stun > 0){
    //stunned!
    mitigate = 0;
    stun = stun - 1;
  }
    
  if(dot > 0){
    if(counter == 500){
      //DoT damage to hurt over time.
      soak = soak - 1;
      dot = dot - 1;
    }
  }
  
  if(slow > 0){
    if(counter == 0){
      //slooooow.
      if(attackWait > 0){
        attackWait = attackWait + 1;
      } 
      slow = slow - 1;
    }
  }
  
  if(enrage > 0){
    if(counter == 0){
      //full of rage!!!1!!
      damage = damage + enrage;
      enrage = enrage - 1;
      soak = soak - enrage;
    }
  }
  
  if(attackWait > 0){
    if(counter == 250){
      //delay for limiting the rate of attacks.
      attackWait = attackWait - 1;
      if(attackWait == 0){
        digitalWrite(13, HIGH);
        delay(250);
        digitalWrite(13, LOW);
      }
    }
  }
  
  if(deception == 0){
    if(blind == 0){
      attack = EEPROM.read(1);
    }
  }
  
  if(deception == 0){
    if(persuasion == 0){
      if(enrage == 0){
        damage = EEPROM.read(2);
      }
    }
  }

  if(stun == 0){
    mitigate = EEPROM.read(3);
  }
    
  show();
  
  /*Serial.print("soak="); Serial.print(soak);
  Serial.print(" attack="); Serial.print(attack);
  Serial.print(" damage="); Serial.print(damage);
  Serial.print(" mitigate="); Serial.print(mitigate);
  Serial.print(" deception="); Serial.print(deception);
  Serial.print(" persuasion="); Serial.print(persuasion);
  Serial.print(" dot="); Serial.print(dot);
  Serial.print(" slow="); Serial.print(slow);
  Serial.print(" blind="); Serial.print(blind);
  Serial.print(" stun="); Serial.print(stun);
  Serial.print(" enrage="); Serial.print(enrage);
  Serial.print(" attackWait="); Serial.print(attackWait);
  Serial.print(" count="); Serial.println(counter);*/
}

void sort(){
  byte foeDamage = ((My_Decoder.value / 1000)% 10);
  byte foeAttack = ((My_Decoder.value / 100)% 10);
  byte skillElement = ((My_Decoder.value / 10) %10);
  byte effect = (My_Decoder.value %10);
  //sort the signal received into the categories to do things.

  if(foeDamage == 9){
    if(foeAttack == 9){
      if(skillElement == 9){
        if(effect == 9){
          //GM autokill.
          soak = 0;
          attack = 0;
          damage = 0;
          mitigate = 0;
          light = 0;
          dark = 0;
          fire = 0;
          water = 0;
          earth = 0;
          air = 0;
        }
      }
    }
  }

  if(skillElement > 0){
    if(skillElement == 1){
      //normal attack.
      if((foeAttack) >= mitigate){
      }
      else{
        foeDamage = 0;
      }
    }

    if(skillElement == 2){
      //persuade.
      persuasion = (foeDamage + persuasion) * 500;
      foeDamage = 0;
      //doesn't even hurt.
    }

    if(skillElement == 3){
      //deception.
      //the deception skill used to deceive you.
      deception = (foeDamage + deception) * 500;
      foeDamage = 0;
      //deception only hurts your feelings.
    }

    if(skillElement == 4){
      //earth.
      if(earth == 1){
        foeDamage = foeDamage / 2;
      }
    }

    if(skillElement == 5){
      //air.
      if(air == 1){
        foeDamage = foeDamage / 2;
      }
    }

    if(skillElement == 6){
      //fire.
      if(fire == 1){
        foeDamage = foeDamage / 2;
      }
    }

    if(skillElement == 7){
      //water.
      if(water == 1){
        foeDamage = foeDamage / 2;
      }
    }

    if(skillElement == 8){
      //dark.
      if(dark == 1){
        foeDamage = foeDamage / 2;
      }
    }

    if(skillElement == 9){
      //light.
      if(light == 1){
        foeDamage = foeDamage / 2;
      }
    }
  }

  if(effect > 0){
    if(effect == 1){
      //fast attack.
      if((foeAttack - 1) >= mitigate){
      }
      else{
        foeDamage = 0;
      }
    }

    if(effect == 2){
      //heal.
      //instead of being attacked you're healed.
      soak = soak + foeDamage;
      foeDamage = 0;
      //feels lovely.
    }

    if(effect == 3){
      //DoT.
      //damage over time (DoT).
      dot = foeDamage + dot;
      foeDamage = 0;
      //damage happens over time, not right now.
    }

    if(effect == 4){
      //slow.
       //reduces your speed.
      slow = foeDamage + slow;
      foeDamage = foeDamage / 2;
      //doesn't hurt much, just makes you slooooow.
    }

    if(effect == 5){
      //blind.
      //I CAN'T SEE!
      blind = (foeDamage + blind) * 500;
      foeDamage = foeDamage / 2;
    }

    if(effect == 6){
      //stun.
      stun = (foeDamage + stun) * 500;
      foeDamage = foeDamage / 2;
    }

    if(effect == 7){
      //enrage.
      enrage = foeDamage + enrage;
      foeDamage = 0;
    }

    if(effect == 8){
      //accurate attack.
      if((foeAttack + 2) >= mitigate){
      }
      else{
        foeDamage = 0;
      }
    }

    if(effect == 9){
      //cure.
      //remove all current negative effect.
      foeDamage = 0;
      dot = 0;
      deception = 0;
      persuasion =0;
      slow = 0;
      blind = 0;
      //it's more like being cuddled by puppies than hit with a truck.
    }
  }
  
  soak = soak - foeDamage;
  //do the damage to the character after figuring it and any other side effects.
  show();
}

void gmSet(){
  //not working currently.  taking offline until i have the patience to mess with it.
  //use eeprom write sketch to directly write the stuff for now.
 /* //function to take serial inputs and offer to save them to eeprom if gm wants.
  //might eventually find a way to make more effecient.
  //or at least comment each step better...
  switch (counter){
    case 0:
    soak = Serial.read() - 48;
    counter ++;
    break;

    case 1:
    attack = Serial.read() - 48;
    counter ++;
    break;

    case 2:
    damage = Serial.read() - 48;
    counter ++;
    break;

    case 3:
    mitigate = Serial.read() - 48;
    counter ++;
    break;

    case 4:
    textBuffer = Serial.read() - 48;
    if(textBuffer == 1){
      light = 1;
    }
    else{
      light = 0;
    }
    counter ++;
    break;

    case 5:
    textBuffer = Serial.read() - 48;
    if(textBuffer == 1){
      dark = 1;
    }
    else{
      dark = 0;
    }
    counter ++;
    break;

    case 6:
    textBuffer = Serial.read() - 48;
    if(textBuffer == 1){
      fire = 1;
    }
    else{
      fire = 0;
    }
    counter ++;
    break;

    case 7:
    textBuffer = Serial.read() - 48;
    if(textBuffer == 1){
      water = 1;
    }
    else{
      water = 0;
    }
    counter ++;
    break;

    case 8: 
    textBuffer = Serial.read() - 48;
    if(textBuffer == 1){
      earth = 1;
    }
    else{
      earth = 0;
    }
    counter ++;
    break;

    case 9:
    textBuffer = Serial.read() - 48;
    if(textBuffer == 1){
      air = 1;
    }
    else{
      air = 0;
    }
    counter ++;
    break;

    case 10:
    attack1skillElement = Serial.read() - 48;
    counter ++;
    break;

    case 11:
    attack1effect = Serial.read() - 48;
    counter ++;
    break;

    case 12:
    attack2skillElement = Serial.read() - 48;
    counter ++;
    break;

    case 13:
    attack2effect = Serial.read() - 48;
    counter ++;
    break;

    case 14:
    attack3skillElement = Serial.read() - 48;
    counter ++;
    break;

    case 15:
    attack3effect = Serial.read() - 48;

    Serial.println();
    Serial.print("soak="); Serial.print(soak);
    Serial.print(" attack="); Serial.print(attack);
    Serial.print(" damage="); Serial.print(damage);
    Serial.print(" mitigate="); Serial.println(mitigate);
    Serial.print("light="); Serial.print(light);
    Serial.print(" dark="); Serial.print(dark);
    Serial.print(" fire="); Serial.print(fire);
    Serial.print(" water="); Serial.print(water);
    Serial.print(" earth="); Serial.print(earth);
    Serial.print(" air="); Serial.println(air);
    Serial.println(); Serial.println("attack 1");
    Serial.print("skill/element="); Serial.println(attack1skillElement);
    Serial.print("effect="); Serial.println(attack1effect);
    Serial.println(); Serial.println("attack 2");
    Serial.print("skill/element="); Serial.println(attack2skillElement);
    Serial.print("effect="); Serial.println(attack2effect);
    Serial.println(); Serial.println("attack 3");
    Serial.print("skill/element="); Serial.println(attack3skillElement);
    Serial.print("effect="); Serial.println(attack3effect);

    Serial.println();
    Serial.println("Save?  Send 1 for yes or 0 for no.");
    counter ++;
    break;

    case 16:
    textBuffer = (Serial.read() - 48);
    if(textBuffer == 1){
      Serial.println("Saving character.");
      EEPROM.write(0, soak);
      EEPROM.write(1, attack);
      EEPROM.write(2, damage);
      EEPROM.write(3, mitigate);
      EEPROM.write(4, light);
      EEPROM.write(5, dark);
      EEPROM.write(6, fire);
      EEPROM.write(7, water);
      EEPROM.write(8, earth);
      EEPROM.write(9, air);
      EEPROM.write(10, attack1skillElement);
      EEPROM.write(11, attack1effect);
      EEPROM.write(12, attack2skillElement);
      EEPROM.write(13, attack2effect);
      EEPROM.write(14, attack3skillElement);
      EEPROM.write(15, attack3effect);
      Serial.println("Saved.");      
    }

    else{
      Serial.println("Current stats,");
      Serial.println("Send any character to re-input.");
    }

    Serial.println();
    Serial.print("soak="); Serial.print(EEPROM.read(0));
    Serial.print(" attack="); Serial.print(EEPROM.read(1));
    Serial.print(" damage="); Serial.print(EEPROM.read(2));
    Serial.print(" mitigate="); Serial.println(EEPROM.read(3));
    Serial.print("light="); Serial.print(EEPROM.read(4));
    Serial.print(" dark="); Serial.print(EEPROM.read(5));
    Serial.print(" fire="); Serial.print(EEPROM.read(6));
    Serial.print(" water="); Serial.print(EEPROM.read(7));
    Serial.print(" earth="); Serial.print(EEPROM.read(8));
    Serial.print(" air="); Serial.println(EEPROM.read(9));
    Serial.println(); Serial.println("attack 1");
    Serial.print("skill/element="); Serial.println(EEPROM.read(10));
    Serial.print("effect="); Serial.println(EEPROM.read(11));
    Serial.println(); Serial.println("attack 2");
    Serial.print("skill/element="); Serial.println(EEPROM.read(12));
    Serial.print("effect="); Serial.println(EEPROM.read(13));
    Serial.println(); Serial.println("attack 3");
    Serial.print("skill/element="); Serial.println(EEPROM.read(14));
    Serial.print("effect="); Serial.println(EEPROM.read(15));

    counter = 0;
    break;   
  }*/
}
