#pragma once
#include <fstream>
#include <string>
#include <exception>
#include <vector>

#define LOGPATH           "debug.log"
#define OUTPUTPATH        "result.txt"
#define ARGWIDTH          10

class Quadruple;

class LogHandler
{
  static LogHandler* instance;
  std::ofstream logFile;
  LogHandler();
  LogHandler(LogHandler& right) = delete;
public:
  static std::ofstream& getInstance();
  ~LogHandler();
};

class OutputHandler
{
  std::ofstream outFile;
public:
  OutputHandler();
  void writeOutput(const std::vector<Quadruple>& code);
  ~OutputHandler();
};

class OutputException : public std::exception
{
  std::string _msg;

public:
  OutputException(const std::string &msg = "输入异常");
  const char *what();
};