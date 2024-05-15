///============================================================================
// Name        : MQTTClient.h
// Author      : Giuseppe Triggiani - Masmec Biomed S.p.A
// Description : header file della classe C++ per la gestione del client MQTT
//============================================================================

#ifndef MQTTCLIENT_H_
#define MQTTCLIENT_H_

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include "mosquittopp.h"

#include <cstring> //memset memcpy


#define SERIAL_NUMBER 	"NANT001/serial_number/status";
#define CULTURE_ID 		"NANT001/culture_id/status";
#define START_DATE 		"NANT001/start_date/status";
#define TIME_ELAPSED 	"NANT001/time_elapsed/status";
#define CELL_DENSITY 	"NANT001/cell_density/status";
#define PH 				"NANT001/ph/status";
#define TEMPERATURE 	"NANT001/temperature/status";
#define PHASE 			"NANT001/phase/status"
#define PHASE_DESCRIPTION "NANT001/phase_desc/status"
#define PHASE_IMAGE 	"NANT001/phase_image/status"

// Eventi
#define SYSTEMSTART 		"NANT001/system_start/status"
#define CARTDRIDGE_A_B 		"NANT001/cartdridge_A_B/status"
#define CELL_INJECTION 		"NANT001/cell_injection/status"
#define CARTDRIDGE_C 		"NANT001/cartdridge_C/status"
#define CARTDRIDGE_D 		"NANT001/cartdridge_D/status"
#define BOTTLE_REMOVED 		"NANT001/bottle_removed/status"
#define NUMBER_OF_EVENTS 5

// Other Events
#define PAUSE 				"NANT001/pause/status"
#define OPEN_DOOR_NOT_PERM 	"NANT001/open_door_not_permitted/status"
#define NEXT_PHASE 			"NANT001/next_phase/status"
#define PREVIOUS_PHASE 		"NANT001/previous_phase/status"
#define ABORT 				"NANT001/abort/status"
#define SAMPLING 			"NANT001/sampling/status"
#define NEXT_PHASE 			"NANT001/next_phase/status"

// Warning
#define OPEN_DOOR_NOT_PERMITTED_OVER_TIME 	"NANT001/open_door_not_permitted_over_time/status"
#define OUT_PH_DOWN 						"NANT001/out_ph_down/status"
#define OUT_PH_UP 							"NANT001/out_ph_up/status"
#define OUT_TEMP_DOWN 						"NANT001/out_temp_down/status"
#define OUT_TEMP_UP 						"NANT001/out_temp_up/status"
#define ERR_PH_MOUNT_TUBE 					"NANT001/err_ph_mounting_tube/status"
#define ERR_PH_LIQUID_NOT_STANDARD 			"NANT001/lerr_ph_iquid_not_standard/status"
#define ERR_PH_RANGE_SENSOR 				"NANT001/err_ph_out_range_sensor/status"
#define ERR_PH_HARD_ERROR_SENSOR 			"NANT001/err_ph_hard_error_sensor/status"
#define ERR_FLOWMETER 						"NANT001/err_flowmeter/status"


using namespace std;

class IMyMessageHandler {
public:
	virtual void onNewMessage(const struct mosquitto_message* a_message) = 0;
};

class MQTTClient : public mosqpp::mosquittopp
{
public:
	// costruttori e distruttori
	 MQTTClient(const char* a_pchid, const char* a_pchhost, int a_intport);
	 virtual ~MQTTClient();

public:
	 /*bool SetStatus(const SCurrentState& a_rSCurrentState,
			 const vector<SEventCycle>& a_rvectSEventCycle,
			 const char* a_pchCultureId = NULL,
			 const char* a_pchSerialNumber= NULL,
			 float* a_pfMaxTemperatureValue = NULL,
			 float* a_pfMinTemperatureValue = NULL,
			 float* a_pfMaxPHValue = NULL,
			 float* a_pfMinPHValue = NULL);*/

	/* bool SetStatus(const SCurrentState& a_rSCurrentState,
	 			 const vector<SEventCycle>& a_rvectSEventCycle,
	 			 const char* a_pchCultureId = NULL,
	 			 const char* a_pchSerialNumber= NULL);*/

	 bool IsConnectedToServer() { return m_bConnectedToServer; }

	 bool Disconnect();

	 void setListener(IMyMessageHandler* a_pListener) {
		 m_pListener = a_pListener;
	 }

private:

	 //callback
	 IMyMessageHandler*	m_pListener;

	 bool								m_bConnectedToServer;
	 const char*				m_pch_host;
	 const char*				m_pch_id;
	 int								m_int_port;
	 int								m_int_keepalive;

	 bool send_message(const char* a_pchmessage, const char* a_pchtopic);

	 template <class myType>
	 string ToString(myType a_myType);

//	void SendNotification(const vector<SEventCycle>& a_rvectSEventCycle);

	void on_connect(int rc);
	void on_disconnect(int rc);
	void on_publish(int mid);
	void on_message(const struct mosquitto_message* message);

};


#endif /* MQTTCLIENT_H_ */
