/*
 * Tekstura.h
 *
 *  Created on: 8 sie 2015
 *      Author: Grzegorz
 */

#ifndef SRC_LOGGER_H_
#define SRC_LOGGER_H_

class Logger {
private:
	static ofstream outfile;
	static Logger* logger;
	ostringstream buffer;

	string constructTime(time_t time) {
		struct tm * now = localtime(&time);
		ostringstream ss;
		ss << '[';
		if (now->tm_mday >= 10) {
			ss << now->tm_mday;
		} else {
			ss << 0 << now->tm_mday;
		}
		ss << '-';
		if (now->tm_mon + 1 >= 10) {
			ss << now->tm_mon + 1;
		} else {
			ss << 0 << now->tm_mon + 1;
		}
		ss << '-' << (now->tm_year + 1900) << ' ';
		if (now->tm_hour >= 10) {
			ss << now->tm_hour;
		} else {
			ss << 0 << now->tm_hour;
		}
		ss << ':';
		if (now->tm_min >= 10) {
			ss << now->tm_min;
		} else {
			ss << 0 << now->tm_min;
		}
		ss << ':';
		if (now->tm_sec >= 10) {
			ss << now->tm_sec;
		} else {
			ss << 0 << now->tm_sec;
		}
		ss << "] - ";
		return ss.str();
	}
	Logger() {
	}

public:
	static string LINE;
	static string ERR;
	static void log(string text, bool newline = true, bool showTime = true) {
		if (!logger) {
			logger = new Logger();
		}
		string output = text;
		if (showTime) {
			output = logger->constructTime((long long unsigned) time(0)) + output;
		}
		if (newline) {
			output += "\n";
		}
		outfile << output;
		outfile.flush();
	}
	~Logger() {
		outfile.close();
	}
};
Logger* Logger::logger = NULL;
ofstream Logger::outfile("log.txt", ios::out);
string Logger::LINE = "=========================";
string Logger::ERR = "ERROR: ";

#endif /* SRC_TEKSTURA_H_ */
