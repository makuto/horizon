#ifndef DEBUGTEXT_HPP
#define DEBUGTEXT_HPP
#include <sstream>
#include <base2.0/graphics/graphics.hpp>

/* --DebugText--
 * Debug text takes key-value pairs globally and puts them in the provided text for
 * display on the screen.
 * */
class DebugText
{
    private:
        static std::stringstream ss;
    public:
        static void addEntry(const char* key, int value);
        static void addEntry(const char* key, bool value);
        static void addEntry(const char* key, float value);
        static void addEntry(const char* key, unsigned char value);
        static void clear();
        static void render(window* win, text* textToRender);
};
#endif
