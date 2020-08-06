#pragma once
/*
 * File:   Log.h
 * Author: Alberto Lepe <dev@alepe.com>
 *
 * Created on December 1, 2015, 6:00 PM
 */

#ifndef LOG_H
#define LOG_H

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

enum typelog {
	DEBUG,
	INFO,
	WARN,
	ERR
};

struct structlog {
	bool headers = false;
	typelog level = WARN;
};

extern structlog LOGCFG;

class LOG {
public:
	LOG() {}
	LOG(typelog type) {
		std::cout.unsetf(std::ios::floatfield);
		std::cout.precision(2);
		msglevel = type;
		if (LOGCFG.headers) {
			operator << ("[" + getLabel(type) + "]");
		}
	}
	~LOG() {
		if (opened) {
			cout << endl;
		}
		opened = false;
	}
	template<class T>
	LOG &operator<<(const T &msg) {
		if (msglevel >= LOGCFG.level) {
			cout << msg;
			opened = true;
		}
		return *this;
	}

	LOG& operator<<(const glm::vec2 &msg) {
		std::cout.unsetf(std::ios::floatfield);
		std::cout.precision(2);
		if (msglevel >= LOGCFG.level) {
			cout << "(" << msg.x << ", " << msg.y <<  ")" << "\n";
			opened = true;
		}
		return *this;
	}

	LOG& operator<<(const glm::vec3 &msg) {
		std::cout.unsetf(std::ios::floatfield);
		std::cout.precision(2);
		if (msglevel >= LOGCFG.level) {
			cout << "(" << msg.x << ", " << msg.y << ", " << msg.z << ")" << "\n";
			opened = true;
		}
		return *this;
	}


	LOG &operator<<(const glm::vec4 &msg) {
				std::cout.unsetf(std::ios::floatfield);
		std::cout.precision(2);
		if (msglevel >= LOGCFG.level) {
			cout << "(" << msg.x << ", " << msg.y << ", " << msg.z << ", " << msg.w << ")" << "\n";
			opened = true;
		}
		return *this;
	}

	LOG &operator<<(const glm::mat4 &msg) {
		if (msglevel >= LOGCFG.level) {
			cout << "[";
			*this << msg[0];
			*this << msg[1];
			*this << msg[2];
			cout << "(" << msg[3].x << ", " << msg[3].y << ", " << msg[3].z << ", " << msg[3].w << ")" << "]";
			opened = true;
		}
		return *this;
	}

	LOG &operator<<(const glm::mat3 &msg) {
		if (msglevel >= LOGCFG.level) {
			cout << "[";
			*this << msg[0];
			*this << msg[1];
			cout << "(" << msg[2].x << ", " << msg[2].y << ", " << msg[3].z << ")" << "]";
			opened = true;
		}
		return *this;
	}
	
private:
	bool opened = false;
	typelog msglevel = DEBUG;
	inline string getLabel(typelog type) {
		string label;
		switch (type) {
		case DEBUG: label = "DEBUG"; break;
		case INFO:  label = "INFO "; break;
		case WARN:  label = "WARN "; break;
		case ERR: label = "ERROR"; break;
		}
		return label;
	}
};

#endif