#ifndef DEBUGTEXT_CPP
#define DEBUGTEXT_CPP
#include "debugText.hpp"
#include <sstream>
#include <iostream>
#include <base2.0/graphics/graphics.hpp>

std::stringstream DebugText::ss;

void DebugText::addEntry(const char* key, int value)
{
    ss << key << value << "\n";
}
void DebugText::addEntry(const char* key, bool value)
{
    ss << key << value << "\n";
}
void DebugText::addEntry(const char* key, float value)
{
    ss << key << value << "\n";
}
void DebugText::addEntry(const char* key, unsigned char value)
{
    ss << key << value << "\n";
}
void DebugText::clear()
{
    ss.str(std::string());
    ss.clear();
}
void DebugText::render(window* win, text* textToRender)
{
    textToRender->setText(ss.str());
    win->draw(textToRender);
}
#endif
