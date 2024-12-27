#include "CodeGenerator.h"
#include "OutputHandler.h"
#include <iostream>
#include <iomanip>
using namespace std;

LogHandler* LogHandler::instance = nullptr;

LogHandler::LogHandler()
{
}

ofstream& LogHandler::getInstance()
{
    if (!instance)
    {
        instance = new LogHandler();
        instance->logFile.open(LOGPATH);
        if (!instance->logFile.is_open())
        {
            throw OutputException("打开日志文件 " + string(LOGPATH) + " 失败");
        }
    }
    return instance->logFile;
}

LogHandler::~LogHandler()
{
    if (instance)
    {
        instance->logFile.close();
        delete instance;
    }
}

OutputHandler::OutputHandler()
{
    outFile.open(OUTPUTPATH);
    if (!outFile.is_open())
    {
        throw OutputException("打开输出文件 " + string(OUTPUTPATH) + " 失败");
    }
}

void OutputHandler::writeOutput(const vector<Quadruple>& code)
{
    outFile << setiosflags(ios::left);
    for (int i = 0; i < code.size(); i++)
    {
        outFile << setw(4) << i << setw(ARGWIDTH) << code[i].op << setw(ARGWIDTH) << code[i].arg1 << setw(ARGWIDTH) << code[i].arg2 << setw(ARGWIDTH);
        string result = code[i].result;
        if (code[i].result == "")
            result = "$L" + to_string(code[i].resultQuad);
        outFile << result << endl;
    }
}

OutputHandler::~OutputHandler()
{
    outFile.close();
}

OutputException::OutputException(const string& msg) : _msg(msg)
{
}

const char* OutputException::what()
{
    return _msg.c_str();
}