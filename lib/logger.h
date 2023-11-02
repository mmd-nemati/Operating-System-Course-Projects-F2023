#ifndef __LOGGER_H__
#define __LOGGER_H__

void logTerminalNormal(const char* msg);
void logTerminalInput(const char* msg);
void logTerminalMsg(const char* msg);
void logTerminalInfo(const char* msg);
void logTerminalWarning(const char* msg);
void logTerminalError(const char* msg);
void logFile(const char* msg, const char* username);
#endif // __LOGGER_H__