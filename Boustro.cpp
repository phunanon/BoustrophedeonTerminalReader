#include <iostream> //Terminal
#include <fstream> //IO
#include <sstream>
#include <string> //For strings
#include <thread> //For threads, and sleeping
#include <chrono> //For thread sleeping
#include "keypresses.c" //For detecting keypresses: kbhit(), pressedCh
typedef unsigned char byte;
typedef unsigned int uint;

void clearScreen() { std::cout << "\033[2J\033[1;1H"; }
bool inputted;
std::string getInput()
{
    std::string toReturn = "";
    while (!inputted)
    {
        while (kbhit())
        {
            pressedCh = getchar(); //Get the key
            std::cout << pressedCh; //Echo it to the user
            fflush(stdout);
            if (pressedCh == '\n')
            {
                inputted = true;
            } else {
                toReturn += pressedCh;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(64));
    }
    inputted = false;
    return toReturn;
}

long fsize = 0; //File size
long charRead = 0; //The character at the top of the read
long charReadEnd; //The end of the read
byte lineWidth = 20; //The width of a line (chars)
byte pageHeight = 10; //The amount of lines per page
long maxRead = 0; //Used for page boundries
bool redraw = true; //Must we redraw?
std::string cacheLine = ""; //The cache for a drawn line
long c = 0; //The index of a character in the text
byte lc = 0; //The index of a character in the line
byte l = 0; //The amount of lines
bool reflect = false; //Is this a reflected line?
char sym; //The symbol being read
bool quit = false; //Quit the program?

int main(int argc, char *argv[])
{
   if (argc != 2) { std::cout << "Must be run as ./Boustro.elf filetoread.txt" << std::endl; return 0; }
  //Load shite to listen to pressed keys
    loadKeyListen();
  //Intro
    std::cout << "Welcome to Boustro, the Boustrophedeon reader.\nBy Patrick Bowen\n\n\033[4mu\033[0m or \033[4mf\033[0m to scroll up\n\033[4mh\033[0m or \033[4mj\033[0m to scroll down\n\033[4ms\033[0m to increase line length\n\033[4ma\033[0m to decrease line length\n\033[4m/\033[0m to increase lines\n\033[4m\\\033[0m to decrease lines\n\033[4m \033[0m to jump to a character\n\033[4mq\033[0m to quit Boustro\n\nPress any key to begin reading\n";
    fflush(stdout);
    while (!kbhit()) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }

  //Load file
    std::ifstream inFile(argv[1]);
  //Calculate file size
    std::streampos begin = inFile.tellg();
    inFile.seekg(0, std::ios::end);
    std::streampos end = inFile.tellg();
    fsize = end - begin;
  //Check file is okay
    if (fsize == 0) { std::cout << "There is nothing to read here." << std::endl; return 0; }

  //Begin reading
    while (!quit)
    {
        while (kbhit())
        {
            redraw = true;
            pressedCh = getchar(); //Get the key
            switch (pressedCh)
            {
                case 'u':
                case 'f': //Scroll up
                    charRead -= lineWidth;
                    if (charRead < 0) { charRead = 0; }
                    break;
                case 'h':
                case 'j': //Scroll down
                    charRead += lineWidth;
                    maxRead = fsize - (pageHeight * lineWidth);
                    if (charRead > maxRead) { charRead = maxRead; }
                    break;
                case 's':
                    if (lineWidth < 255) { lineWidth++; }
                    break;
                case 'a':
                    if (lineWidth > 6) { lineWidth--; }
                    break;
                case '/':
                    if (pageHeight < 255) { pageHeight++; }
                    break;
                case '\\':
                    if (pageHeight > 3) { pageHeight--; }
                    break;
                case ' ':
                    std::cout << "Enter character number: ";
                    fflush(stdout);
                    charRead = std::stol(getInput());
                    break;
                case 'q': //Quit
                    quit = true;
                    break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      //Re-draw, if needed
        if (redraw)
        {
            redraw = false;
            clearScreen();
            lc = 0;
            l = 0;
            charReadEnd = charRead + (pageHeight * lineWidth);
            inFile.seekg(charRead);
            reflect = ((int)((float)charRead / (float)lineWidth) % 2); //Calculate if this is a reflected line
            for (c = charRead; c < charReadEnd; c++) //For each character
            {
                inFile.get(sym);
                if (sym == 10 || sym == 9 || sym == 13) { sym = ' '; }
                if (reflect) { cacheLine = sym + cacheLine; } else { cacheLine += sym; }
                lc++; //Increment the line-char
                if (lc == lineWidth) //If the line-char index is wider than a line, draw this line, and create a new line
                {
                    std::cout << cacheLine;
                    cacheLine = "";
                    std::cout << '\n';
                    lc = 0;
                    l++; //Increment the amount of lines
                    reflect = !reflect;
                    if (l > pageHeight) //Have we reached the page height?
                    {
                      //Stop drawing
                        break;
                    }
                }
            }
            std::cout << '\n' << charRead;
            fflush(stdout);
        }
    }
    return 0;
}
