#include <Arduino.h>
#include "MenuLib.h"

MenuItem::MenuItem(void(*action)(void), const char *name)
{
  this->forward = action;
  this->name = name;
}

Menu::Menu(const int defaultIndex, MenuItem items[], size_t itemCount)
{
  this->selectedItem = defaultIndex;
  this->count = itemCount;
  this->items = items;
}

MenuSystem::MenuSystem(Menu *defaultMenu, const int displaySize)
{
  this->currentMenu = defaultMenu;
  this->_displaySize = displaySize;
}

MenuSystem::MenuSystem(Menu *defaultMenu)
{
  this->currentMenu = defaultMenu;
  this->_displaySize = 0;
}

int MenuSystem::getDisplaySize()
{
  return this->_displaySize;
}

void MenuSystem::updown(int x)
{
  if (x == 0)
    return;

  int len = currentMenu->count;
  int i = currentMenu->selectedItem;

  i += x;

  if (i < 0){ i = len - 1; }
  if (i > len - 1){ i = 0; }

  currentMenu->selectedItem = i;
}

void MenuSystem::forward()
{
  currentMenu->items[currentMenu->selectedItem].forward();
}

void MenuSystem::goToMenu(Menu *toMenu)
{
  currentMenu = toMenu;
}

const char *emptyString = "";

void MenuSystem::outputMenu(MenuOutputItem itm[])
{
  int len = _displaySize;
  static int _displayIndex;

  if (_displaySize == 0)
  {
    len = currentMenu->count;
    _displayIndex = 0;
  }
  else
  {
    while ((currentMenu->selectedItem - _displayIndex) > (_displaySize - 1)) { _displayIndex++; }
    while ((currentMenu->selectedItem - _displayIndex) < 0) { _displayIndex--; }
  }

  for (int i = 0; i < len; i++)
  {
    itm[i].selected = ((i + _displayIndex) == currentMenu->selectedItem);
    if(i + _displayIndex >= currentMenu->count){
      itm[i].name = emptyString;
    }
    else
    {
      itm[i].name = currentMenu->items[i + _displayIndex].name;
    }
  }
}
