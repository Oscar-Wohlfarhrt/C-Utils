#include <iostream>
#include <sstream>
#include <cstring>
#include <signal.h>

using namespace std;

bool fexist(const char *path);
bool Compile(string path, bool cpp = false);
void Run(string path, int argc, char **args, bool rm, bool cpp = false);
void Del(string path,bool message=false);
void Create(string path, bool cpp = false);
void Help();

#ifdef __unix__
string ext = "";
#elif _WIN32
string ext = ".exe";
#endif

string version = "1.2";
string delPath = "";
int state=0;

void ExitHandler(int signal){
    printf("\n");
    if(state == 1)
        Del(delPath,false);
    
    printf("Ctrl + C\n");
}

//"\033[31;44m"
int main(int argc, char **args)
{
    signal(2,ExitHandler);
    if ((argc == 2 && !strcmp(args[1], "--help")) || argc == 1)
    {
        Help();
        return 0;
    }
    else if (argc >= 3)
    {
        if (!strcmp(args[1], "-r"))
        {
            Run(args[2], argc, args, true);
        }
        else if (!strcmp(args[1], "-r++"))
        {
            Run(args[2], argc, args, true, true);
        }
        else if (!strcmp(args[1], "-rc"))
        {
            Run(args[2], argc, args, false);
        }
        else if (!strcmp(args[1], "-rc++"))
        {
            Run(args[2], argc, args, false, true);
        }
        else if (!strcmp(args[1], "-c"))
        {
            Compile(args[2]);
        }
        else if (!strcmp(args[1], "-c++"))
        {
            Compile(args[2], true);
        }
        else if (!strcmp(args[1], "--create"))
        {
            Create(args[2]);
        }
        else if (!strcmp(args[1], "--create++"))
        {
            Create(args[2], true);
        }
        else
        {
            printf("\033[1;41mUnknown command\033[0m\n");
            printf("\033[1mRun cutils --help for more information\033[0m\n");
        }
    }
    else
    {
        printf("\033[1;41mInsufficient arguments\033[0m\n");
        printf("\033[1mRun cutils --help for more information\033[0m\n");
    }
}

bool Compile(string path, bool cpp)
{
    if (fexist(path.c_str()))
    {
        stringstream cmd = stringstream();
        int lastPoint = path.find_last_of(".");
        string name = path.substr(0, lastPoint);

        if (cpp)
        {
            cmd << "g++ " << path << " -o " << name << ext;
        }
        else
        {
            cmd << "gcc " << path << " -o " << name << ext;
#ifdef __unix__
            cmd << " -lm";
#endif
        }

        system(cmd.str().c_str());
        return true;
    }
    printf("\033[1;41mThe file doesn't exists\033[0m\n");
    return false;
}

void Run(string path, int argc, char **args, bool rm, bool cpp)
{
    if (Compile(path, cpp))
    {
        string filePath;
        stringstream cmd = stringstream();
        int lastPoint = path.find_last_of(".");
        string name = path.substr(0, lastPoint);

#ifdef __unix__
        cmd << "./";
#endif
        cmd << name << ext;
        filePath = cmd.str();
        for (int i = 3; i < argc; i++)
        {
            cmd << " " << args[i];
        }

        delPath=filePath;
        state=1;

        if (fexist(filePath.c_str()))
        {
            system(cmd.str().c_str());
        }

        if (rm)
        {
            Del(filePath);
        }
    }
}

void Del(string path,bool message)
{
    if (fexist(path.c_str()))
        remove(path.c_str());
    else if(message)
        printf("\033[1;41mThe file doesn't exists\033[0m\n");
}

bool fexist(const char *path)
{
    FILE *file;
    if (file = fopen(path, "r"))
    {
        fclose(file);
        return true;
    }
    else
    {
        return false;
    }
}

void Create(string path, bool cpp)
{
    if (!fexist(path.c_str()))
    {
        if (cpp)
        {
            FILE *newFile = fopen(path.c_str(), "w");
            fprintf(newFile, "#include <iostream>\n\nusing namespace std;\n\nint main(int argc, char **args)\n{\n    \n    return 0;\n}");
            fclose(newFile);
        }
        else
        {
            FILE *newFile = fopen(path.c_str(), "w");
            fprintf(newFile, "#include <stdio.h>\n#include <stdlib.h>\n\nint main(int argc, char **args)\n{\n    \n    return 0;\n}");
            fclose(newFile);
        }
    }
    else
    {
        printf("\033[1;41mExisting file\033[0m\n");
    }
}

void Help()
{
#ifdef __unix__
    printf("on linux\n");
#elif _WIN32
    printf("on windows\n");
#endif
    printf("Version: %s\n", version.c_str());
    printf("cutils is a command that use gcc/g++ compiler to compile and run C/C++ code\n\n");
    printf("cutils [command] [file] [arguments] ...\n\n");
    printf("flags:\n");
    printf("%-25s Compile, run the c file. Delete the exe file after the execution\n", "-r [file]");
    printf("%-25s Compile, run the cpp file. Delete the exe file after the execution\n", "-r++ [file]");
    printf("%-25s Compile, run the c file.Don't delete the exe file after the execution\n", "-rc [file]");
    printf("%-25s Compile, run the cpp file.Don't delete the exe file after the execution\n", "-rc++ [file]");
    printf("%-25s Compile the c file\n", "-c [file]");
    printf("%-25s Compile the cpp file\n", "-c++ [file]");
    printf("%-25s Create a c template file with the especified name\n", "--create [file]");
    printf("%-25s Create a cpp template file with the especified name\n", "--create++ [file]");
    printf("%-25s Shows this message\n\n", "--help");
}