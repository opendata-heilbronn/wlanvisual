#include <Arduino.h>
#include <stddef.h>

class MenuItem
{
public:
  MenuItem(void(*action)(void), const char *name);
  void(*forward)();
  const char *name;
};

struct MenuOutputItem
{
  bool selected;
  const char *name;
};

class Menu
{
public:
  Menu() {};
  Menu(const int defaultIndex, MenuItem items[], size_t itemCount);
  MenuItem* items;

  int selectedItem;
  size_t count;
};

class MenuSystem
{
private:
  Menu *currentMenu;
  int _displaySize;
public:
  MenuSystem(Menu *defaultMenu, const int displaySize);
  MenuSystem(Menu *defaultMenu);
  void updown(int x);
  int getDisplaySize();
  void forward();
  void goToMenu(Menu *toMenu);
  void outputMenu(struct MenuOutputItem itm[]); //Outputs current menu into the given MenuOutputItem array (size must be displaysize)
};
