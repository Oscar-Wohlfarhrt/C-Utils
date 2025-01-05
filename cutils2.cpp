#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <filesystem>
#include <signal.h>
#include "CLI11.hpp"
#include "rang.hpp"

#ifdef __unix__
    #include <fmt/format.h>
    using namespace fmt;
#else
    #include <format>
#endif



using namespace std;
//using namespace CLI;
using namespace rang;


void Compile(string cmd, vector<string> cmdArgs, string src, string exe);
void Run(string cmd, vector<string> args);

#ifdef __unix__
string ext = "";
#elif _WIN32
string ext = ".exe";
#endif

string version = "2.0";
string description = "C-Utils is a command that use GCC/G++ compiler to compile source files, and run the executable after compilation.\n";
string delPath = "";
int state=0;

void ExitHandler(int signal){
    printf("\nCtrl + C\n");
}

//"\033[31;44m"
int main(int argc, char **args)
{
    signal(2,ExitHandler);
    bool debug = false;
    string inFile = "";
    string outFile = "./main.o";
    string cmd = "g++";
    vector<string> cmdArgs = vector<string>();
    bool runExe = false;
    vector<string> exeArgs = vector<string>();

    string configFile = "";

    CLI::App app = CLI::App(format("Version: {}\n{}",version, description));
    args = app.ensure_utf8(args);

    CLI::Option *inFileOpt = app.add_option<string>("src-file,--src-file,-i",inFile,"The file to be compiled")->option_text("File Required")->check(CLI::ExistingFile);
    app.add_option<string>("--out-file,-o",outFile,"The resulting file after the compilation")->option_text("File");
    app.add_flag<bool>("--run,-r",runExe,"If present the compiled executable will be executed");
    app.add_option<vector<string>>("exe-args,--exe-args",exeArgs,"Arguments to be passed to the executable, if -r is present")->option_text("Text")->delimiter(',');
    app.add_flag<string>("--compiler{g++},-c{gcc}",cmd,"The compiler command to execute, default is G++")->option_text("Executable")->group("Compiler Options");
    app.add_option<vector<string>>("--compiler-args,-a",cmdArgs,"Aditional argumets for the gcc/g++ compiler")->option_text("Text")->delimiter(',')->group("Compiler Options");
    CLI::Option *configOpt = app.set_config("--config,-f","./cutils.conf");

    CLI::Option *createSrcC = app.add_flag<string>("--create-c{./main.c}",configFile,"Creates a C template file with a main function")->option_text("Path")->group("Helpers")->excludes("src-file");
    CLI::Option *createSrcCpp = app.add_flag<string>("--create-cpp{./main.cpp}",configFile,"Creates a C++ template file with a main function")->option_text("Path")->group("Helpers")->excludes("--create-c","src-file");
    CLI::Option *createConf = app.add_flag<string>("--create-config{./cutils.conf}",configFile,"Creates a configuration file using the current arguments")->option_text("Path")->group("Helpers")->excludes("--create-c","--create-cpp");

    CLI::Option *helpOpt = app.set_help_flag("--help,-h,-?","Display this information");
    CLI::Option *verOpt = app.set_version_flag("--version,-v",format("C-Utils version: {}",version));
    CLI::Option *debugOpt = app.add_flag<bool>("--debug",debug,"Shows debug information")->group("");

    std::atexit([](){cout << style::reset;});
    try {
        app.parse(argc, args);
    } catch (const CLI::ParseError &e) {
        cout << (e.get_exit_code()==0 ? fg::reset : fg::red);
        return app.exit(e);
    }

    if(debug){
        cout << inFile << endl
            << outFile << endl
            << cmd << endl;
            
        for(int i = 0;i<cmdArgs.size();i++){
            cout << cmdArgs[i] << ", ";
        }
        
        cout << endl << runExe << endl;

        for(int i = 0;i<exeArgs.size();i++){
            cout << exeArgs[i] << ", ";
        }
        cout << endl << inFileOpt->get_name(true) << endl;
    }

    if(!createSrcC->empty()){
        fstream fs = fstream(configFile,fstream::out);
        fs << "#include <stdio.h>\n#include <stdlib.h>\n\nint main(int argc, char **args)\n{\n    \n    return 0;\n}";
        fs.close();
    }
    else if (!createSrcCpp->empty()){
        fstream fs = fstream(configFile,fstream::out);
        fs << "#include <iostream>\n\nusing namespace std;\n\nint main(int argc, char **args)\n{\n    \n    return 0;\n}";
        fs.close();
    }
    else if(!createConf->empty()){
        fstream fs = fstream(configFile,fstream::out);
        auto opts = app.get_options();

        for(auto opt : opts){
            if(!opt->empty() && opt != createSrcC && opt != createSrcCpp && opt != createConf && 
               opt != helpOpt && opt != verOpt && opt != debugOpt && opt != configOpt){
                for(auto val : opt->results()){
                    fs << opt->get_name() << " = " << val << endl;
                }
            }
        }
        
        fs.close();
    }
    else if(!inFileOpt->empty()){
        Compile(cmd,cmdArgs,inFile,outFile);
        if(runExe){
            Run(outFile,exeArgs);
        }
    }
    else
        cout << app.help();
    return 0;
}

void Compile(string cmd, vector<string> cmdArgs, string src, string exe)
{
    stringstream cmdStr = stringstream();

    cmdStr << cmd << " " << src;
    if(exe != "")
        cmdStr << " -o " << exe;
    for (string arg : cmdArgs)
        cmdStr << " " << arg;

    system(cmdStr.str().c_str());
}

void Run(string cmd, vector<string> args)
{
        stringstream cmdStr = stringstream();

        #ifdef __unix__
        cmdStr << "./";
        #endif
        cmdStr << cmd;
        for (string arg : args)
            cmdStr << " " << arg;

        if (filesystem::exists(cmdStr.str()))
        {
            system(cmdStr.str().c_str());
        }
}
