//============================================================================
// Name        : TestMQTT.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "MQTTClient.h"
#include <unistd.h>
#include <math.h>
//#include "json.hpp"
#include "json/json.h"
#define JSON_IS_AMALGAMATION

using namespace std;

#define SIGNAL_PARAMETERS		"SignalGenerator/config/+"
#define SIGNAL_AMPLITUDE 		"SignalGenerator/config/amplitude"
#define SIGNAL_FREQUENCY 		"SignalGenerator/config/frequency"
#define SIGNAL_PHASE 				"SignalGenerator/config/phase"
#define SIGNAL_VALUE 				"SignalGenerator/status/value"

#define SIGNAL_PARAMS				"SignalGenerator/config/params"


struct Point2d {
	double x;
	double y;
	bool isValid;
	Point2d() { x = 0; y = 0; isValid = false; }
};

void point2dToJSON(std::string* a_string, Point2d a_point);
void point2dArrayToJSON(std::string* a_string, Point2d* a_point, int a_iSize);

class SignalGenerator: public IMyMessageHandler {
	double	m_dAmplitude;
	double	m_dFrequency;
	double	m_dPhase;
	int			m_iTime;

public:
	SignalGenerator();
	virtual ~SignalGenerator();

	double readValue();
	virtual void onNewMessage(const struct mosquitto_message* a_message);
};


int main() {
	MQTTClient mqtt("prova", "localhost", 8881);
	mqtt.subscribe(NULL, SIGNAL_PARAMETERS, 1);
	SignalGenerator l_signal;
	mqtt.setListener(&l_signal);

	const int SIZE_BUFFER = 100;
	Point2d l_arrBuffer[SIZE_BUFFER];
	int l_iIndex = 0;

	int c = 0;
	int publishCounter = 0;
	while(1) {
		Point2d l_point;
		l_point.x = c++;
		l_point.y = l_signal.readValue();
		l_point.isValid = true;
		l_arrBuffer[l_iIndex] = l_point;

		l_iIndex = ++l_iIndex % SIZE_BUFFER;

		if(l_iIndex%50 == 0) { //invia ogni 5
			string str;
			point2dArrayToJSON(&str, l_arrBuffer, SIZE_BUFFER);
			cout << "publish di "<< str.c_str() << endl;
			mqtt.publish(NULL, SIGNAL_VALUE, str.length(), str.c_str(), 1);
		}

		/*
		string str;
		point2dToJSON(&str, l_point);
		//cout << "publish di "<< str.c_str() << endl;

		mqtt.publish(&c, SIGNAL_VALUE, str.length(), str.c_str(), 1);
		*/
		usleep(1);
	}
	return 0;
}

SignalGenerator::SignalGenerator()
{
	m_iTime = 0;
	m_dAmplitude = 1;
	m_dFrequency = 0.0001;
	m_dPhase = 0;
}
SignalGenerator::~SignalGenerator()
{
	m_iTime = 0;
	m_dAmplitude = 1;
	m_dFrequency = 1;
	m_dPhase = 0;
}
#define PI 3.14159265
double SignalGenerator::readValue()
{
	return m_dAmplitude*sin(2*PI*m_dFrequency*m_iTime++ + m_dPhase);
}

void SignalGenerator::onNewMessage(const struct mosquitto_message* a_message)
{
	if(!strcmp(a_message->topic, SIGNAL_PARAMS))
	{
		Json::Value root(Json::objectValue);
    Json::Reader reader;
    bool parsingSuccessful = reader.parse((char*)a_message->payload, root);
    if ( !parsingSuccessful )
    {
        cerr << "Error parsing the string" << endl;
        return;
    }
    m_dAmplitude = root["amplitude"].asDouble();
    m_dFrequency = root["frequency"].asDouble();
    m_dPhase = root["phase"].asDouble();
	}
	/*
	if(!strcmp(a_message->topic, SIGNAL_AMPLITUDE))
	{
		double l_dValue = atof((char*)a_message->payload);
		m_dAmplitude = l_dValue;
	}
	else if(!strcmp(a_message->topic, SIGNAL_FREQUENCY))
	{
		double l_dValue = atof((char*)a_message->payload);
		m_dFrequency = l_dValue;
	}
	else if(!strcmp(a_message->topic, SIGNAL_PHASE))
	{
		double l_dValue = atof((char*)a_message->payload);
		m_dPhase = l_dValue;
	}*/
}

void point2dToJSON(std::string* a_string, Point2d a_point)
{
	if(!a_string)
		return;

	Json::Value root(Json::objectValue);   // 'root' will contain the root value after parsing.
	//std::cin >> root;
	root["x"] = a_point.x;
	root["y"] = a_point.y;

	Json::StreamWriterBuilder wbuilder;
	wbuilder.settings_["precision"] = 7;
	// Configure the Builder, then ...
	*a_string = Json::writeString(wbuilder, root);
}

void point2dArrayToJSON(std::string* a_string, Point2d* a_pointArray, int a_iSize)
{
	if(!a_string)
		return;
	Json::Value root(Json::arrayValue);   // 'root' will contain the root value after parsing.
	for(int i = 0; i < a_iSize; i++) {
		Json::Value element(Json::objectValue);
		element["x"] = a_pointArray[i].x;
		element["y"] = a_pointArray[i].y;
		root[i] = element;
	}

	Json::StreamWriterBuilder wbuilder;
	wbuilder.settings_["precision"] = 9;
	wbuilder.settings_["indentation"] = "";
	// Configure the Builder, then ...
	*a_string = Json::writeString(wbuilder, root);
}

