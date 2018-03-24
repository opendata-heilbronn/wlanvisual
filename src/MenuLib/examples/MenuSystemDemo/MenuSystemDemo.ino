#include "MenuLib.h"
#include "Rotary.h"


Rotary rotary(6, 7, 5, true);

//MenuItem(pointer Callback, char* NAME)
//Menu(int initialIndex, int number_of_menu_points, MenuItem* MenuItems)
//MenuSystem(Menu menu, int displaySize)

MenuItem items[] = {
  MenuItem(&gotoUntermenue, "Untermenue")
};
Menu defaultMenu = Menu(0, 1, items);

MenuItem items2[] = {
  MenuItem(&gotoMainmenue, "Back"), 
  MenuItem(&ledAus, "LED 13 AUS"), 
  MenuItem(&ledAn, "LED 13 AN"),
  MenuItem(&emptyFunction, "NICHTS TUN 1"),
  MenuItem(&emptyFunction, "NICHTS TUN 2"),
  MenuItem(&emptyFunction, "NICHTS TUN 3"),
  MenuItem(&emptyFunction, "NICHTS TUN 4")
};
Menu unterMenue = Menu(0, 7, items2);

MenuSystem s = MenuSystem(defaultMenu, 4);

void gotoMainmenue(){s.goToMenu(defaultMenu);}
void gotoUntermenue(){s.goToMenu(unterMenue);}

void emptyFunction(){}

void ledAn(){digitalWrite(13, true);}
void ledAus(){digitalWrite(13, false);}

void setup(){
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
}

void loop(){
  rotary.update();
  
  char rotaryChange = rotary.getChange();
  if(rotaryChange > 0) {s.updown(1); displayMenu();}
  if(rotaryChange < 0) {s.updown(-1); displayMenu();}
  if(rotary.buttonClicked()) {s.forward(); displayMenu();}
}

void displayMenu()
{
  clear();
  int displaySize = s.getDisplaySize();
  MenuOutputItem itm[displaySize];
  s.outputMenu(itm);
  for(int i = 0; i < displaySize; i++)
  {
    setCursor(0,i);
    print(const_cast<char*>(itm[i].selected ? "-> " : "   "));
    print(itm[i].name);
  }
}

void clear()
{
  Serial.write(27);
  Serial.print("[2J");
  Serial.write(27);
  Serial.print("[1;1H");
}

void setCursor(int x, int y)
{
  //<ESC>[1;1H
  Serial.write(27);
  Serial.print("[");
  Serial.print(y+1);
  Serial.print(";");
  Serial.print(x+1);
  Serial.print("H");
}

void print(int i)
{
  Serial.print(i);
}

void print(char* i)
{
  Serial.print(i);
}

void printLine(char* out){
  Serial.println(out);
}
