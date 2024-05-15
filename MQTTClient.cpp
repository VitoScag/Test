///============================================================================
// Name        : MQTTClient.cpp
// Author      : Giuseppe Triggiani - Masmec Biomed S.p.A
// Description : source file della classe C++ per la gestione del client MQTT
//============================================================================

#include "MQTTClient.h"
#include <unistd.h>


MQTTClient::MQTTClient(const char* a_pchid, const char* a_pchhost, int a_intport) : mosquittopp(a_pchid)
{
	m_bConnectedToServer = false;
	m_pListener = NULL;

	mosqpp::lib_init();        				// Mandatory initialization for mosquitto library
	this->m_int_keepalive = 60;    				// Basic configuration setup for myMosq class
	this->m_pch_id = a_pchid;
	this->m_int_port = a_intport;
	this->m_pch_host = a_pchhost;
	username_pw_set("prova","prova");
	connect_async(m_pch_host, m_int_port, m_int_keepalive); 	// non blocking connection to broker request
	loop_start();            				// Start thread managing connection / publish / subscribe
};


MQTTClient::~MQTTClient()
{
	loop_stop();            		// Kill the thread
	mosqpp::lib_cleanup();    		// Mosquitto library cleanup
}



bool MQTTClient::Disconnect()
{
	/*const char* l_pchMachineSerial = "---";
	const char* l_pchCultureId = "---";
	SCurrentState l_SCurrentState;
	vector<SEventCycle> l_vectSEventCycle;
	this->SetStatus(l_SCurrentState,l_vectSEventCycle,l_pchCultureId,l_pchMachineSerial);
	 */
	sleep(2);

	this->disconnect();

	m_bConnectedToServer = false;
	return true;
}

bool MQTTClient::send_message(const char* a_pchmessage, const char* a_pchtopic)
 {
	 // Send message - depending on QoS, mosquitto lib managed re-submission this the thread
	 //
	 // * NULL : Message Id (int *) this allow to latter get status of each message
	 // * topic : topic to be used
	 // * lenght of the message
	 // * message
	 // * qos (0,1,2)
	 // * retain (boolean) - indicates if message is retained on broker or not
	 // Should return MOSQ_ERR_SUCCESS
	 int ret = publish(NULL,a_pchtopic,strlen(a_pchmessage),a_pchmessage,1,true);
	 return ( ret == MOSQ_ERR_SUCCESS );
 }

/*string MQTTClient::ToString(short a_shParam)
{
	ostringstream l_ostringstream;
	l_ostringstream << a_shParam;
	return l_ostringstream.str();
}*/

template <class myType>
string MQTTClient::ToString(myType a_myType)
{
	ostringstream l_ostringstream;
	l_ostringstream << a_myType;
	return l_ostringstream.str();
}

void MQTTClient::on_connect(int rc)
 {
	 if ( rc == 0 )
	 {
		 std::cout << ">> MQTTClient - connected with server" << std::endl;
		 m_bConnectedToServer = true;
	 }
	 else
	 {
		 std::cout << ">> MQTTClient - Impossible to connect with server(" << rc << ")" << std::endl;
		 std::cout << ">> MQTTClient - " << strerror(rc) << std::endl;

		 m_bConnectedToServer = false;
	 }
 }

void MQTTClient::on_disconnect(int rc)
{
	std::cout << ">> MQTTClient - disconnection(" << rc << ")" << std::endl;
	std::cout << ">> MQTTClient - " << strerror(rc) << std::endl;
	m_bConnectedToServer = false;
}

void MQTTClient::on_publish(int mid)
{
	//std::cout << ">> MQTTClient - Message (" << mid << ") succeed to be published " << std::endl;
}

void MQTTClient::on_message(const struct mosquitto_message* message) {
	std::cout << ">> MQTTClient - on_message" << ((char*) message->payload) << std::endl;
	if(m_pListener)
		m_pListener->onNewMessage(message);
}


/*
void MQTTClient::SendNotification(const vector<SEventCycle>& a_rvectSEventCycle)
{
	const char* l_pch_topic = NULL;


	string l_strStringToSend;

	if(a_rvectSEventCycle.size() == 0 )
	{
		l_pch_topic = SYSTEMSTART;
		l_strStringToSend = string("---")+"*"+string("---");
		this->send_message(l_strStringToSend.c_str(),l_pch_topic);
	}
	else
	{
		bool l_bnotFind = false;
		for(size_t i = 0; i < a_rvectSEventCycle.size(); i++)
		{
			if(a_rvectSEventCycle.at(i).typeEvent != ETypeEvent_SystemStart)
				l_bnotFind &= true;
		}

		if( l_bnotFind )
		{
			l_pch_topic = SYSTEMSTART;
			l_strStringToSend = string("---")+"*"+string("---");
			this->send_message(l_strStringToSend.c_str(),l_pch_topic);
		}
	}

	if(a_rvectSEventCycle.size() == 0 )
	{
		l_pch_topic = CARTDRIDGE_A_B;
		l_strStringToSend = string("---")+"*"+string("---");
		this->send_message(l_strStringToSend.c_str(),l_pch_topic);
	}
	else
	{
		bool l_bnotFind = false;
		for(size_t i = 0; i < a_rvectSEventCycle.size(); i++)
		{
			if(a_rvectSEventCycle.at(i).typeEvent != ETypeEvent_LoadCartridge_B)
				l_bnotFind &= true;
		}

		if( l_bnotFind )
		{
			l_pch_topic = CARTDRIDGE_A_B;
			l_strStringToSend = string("---")+"*"+string("---");
			this->send_message(l_strStringToSend.c_str(),l_pch_topic);
		}
	}

	if(a_rvectSEventCycle.size() == 0 )
	{
		l_pch_topic = CELL_INJECTION;
		l_strStringToSend = string("---")+"*"+string("---");
		this->send_message(l_strStringToSend.c_str(),l_pch_topic);
	}
	else
	{
		bool l_bnotFind = false;
		for(size_t i = 0; i < a_rvectSEventCycle.size(); i++)
		{
			if(a_rvectSEventCycle.at(i).typeEvent != ETypeEvent_CellInjection)
				l_bnotFind &= true;
		}

		if( l_bnotFind )
		{
			l_pch_topic = CELL_INJECTION;
			l_strStringToSend = string("---")+"*"+string("---");
			this->send_message(l_strStringToSend.c_str(),l_pch_topic);
		}
	}

	if(a_rvectSEventCycle.size() == 0 )
	{
		l_pch_topic = CARTDRIDGE_C;
		l_strStringToSend = string("---")+"*"+string("---");
		this->send_message(l_strStringToSend.c_str(),l_pch_topic);
	}
	else
	{
		bool l_bnotFind = false;
		for(size_t i = 0; i < a_rvectSEventCycle.size(); i++)
		{
			if(a_rvectSEventCycle.at(i).typeEvent != ETypeEvent_LoadCartridge_C)
				l_bnotFind &= true;
		}

		if( l_bnotFind )
		{
			l_pch_topic = CARTDRIDGE_C;
			l_strStringToSend = string("---")+"*"+string("---");
			this->send_message(l_strStringToSend.c_str(),l_pch_topic);
		}
	}

	if(a_rvectSEventCycle.size() == 0 )
	{
		l_pch_topic = CARTDRIDGE_D;
		l_strStringToSend = string("---")+"*"+string("---");
		this->send_message(l_strStringToSend.c_str(),l_pch_topic);
	}
	else
	{
		bool l_bnotFind = false;
		for(size_t i = 0; i < a_rvectSEventCycle.size(); i++)
		{
			if(a_rvectSEventCycle.at(i).typeEvent != ETypeEvent_LoadCartridge_D)
				l_bnotFind &= true;
		}

		if( l_bnotFind )
		{
			l_pch_topic = CARTDRIDGE_D;
			l_strStringToSend = string("---")+"*"+string("---");
			this->send_message(l_strStringToSend.c_str(),l_pch_topic);
		}
	}

	if(a_rvectSEventCycle.size() == 0 )
	{
		l_pch_topic = BOTTLE_REMOVED;
		l_strStringToSend = string("---")+"*"+string("---");
		this->send_message(l_strStringToSend.c_str(),l_pch_topic);
	}
	else
	{
		bool l_bnotFind = false;
		for(size_t i = 0; i < a_rvectSEventCycle.size(); i++)
		{
			if(a_rvectSEventCycle.at(i).typeEvent != ETypeEvent_BottleRemoved)
				l_bnotFind &= true;

			if( l_bnotFind )
			{
				l_pch_topic = BOTTLE_REMOVED;
				l_strStringToSend = string("---")+"*"+string("---");
				this->send_message(l_strStringToSend.c_str(),l_pch_topic);
			}
		}
	}

	for(size_t i = 0; i < a_rvectSEventCycle.size(); i++)
	{
		if( a_rvectSEventCycle.at(i).typeEvent == ETypeEvent_SystemStart ||
			a_rvectSEventCycle.at(i).typeEvent == ETypeEvent_LoadCartridge_B ||
			a_rvectSEventCycle.at(i).typeEvent == ETypeEvent_CellInjection ||
			a_rvectSEventCycle.at(i).typeEvent == ETypeEvent_LoadCartridge_C ||
			a_rvectSEventCycle.at(i).typeEvent == ETypeEvent_LoadCartridge_D ||
			a_rvectSEventCycle.at(i).typeEvent == ETypeEvent_BottleRemoved )
		{
			string l_Day, l_Month, l_Hour, l_Minutes;
			if( *(&(a_rvectSEventCycle.at(i).sTime_Start[1])) < 10)
				l_Day = "0"+ToString(a_rvectSEventCycle.at(i).sTime_Start[1]);
			else
				l_Day = ToString(a_rvectSEventCycle.at(i).sTime_Start[1]);

			if( *(&(a_rvectSEventCycle.at(i).sTime_Start[2])) < 10)
				l_Month = "0"+ToString(a_rvectSEventCycle.at(i).sTime_Start[2]);
			else
				l_Month = ToString(a_rvectSEventCycle.at(i).sTime_Start[2]);

			if( *(&(a_rvectSEventCycle.at(i).sTime_Start[3])) < 10)
				l_Hour = "0"+ToString(a_rvectSEventCycle.at(i).sTime_Start[3]);
			else
				l_Hour = ToString(a_rvectSEventCycle.at(i).sTime_Start[3]);

			if( *(&(a_rvectSEventCycle.at(i).sTime_Start[4])) < 10)
				l_Minutes = "0"+ToString(a_rvectSEventCycle.at(i).sTime_Start[4]);
			else
				l_Minutes = ToString(a_rvectSEventCycle.at(i).sTime_Start[4]);

			//if (m_CLanguageManager.GetCurrentLanguage() == LNG_ENGLISH)
			l_strStringToSend = ToString(a_rvectSEventCycle.at(i).sTime_Start[0])+"-"+
									l_Day+"-"+
									l_Month+", "+
									l_Hour+":"+
									l_Minutes+"*"+string(a_rvectSEventCycle.at(i).sBarCodeRead_User);

			// Cartdridge A_B_C
			if (a_rvectSEventCycle.at(i).typeEvent == ETypeEvent_SystemStart)
				l_pch_topic = SYSTEMSTART;
			else if( a_rvectSEventCycle.at(i).typeEvent == ETypeEvent_LoadCartridge_B )
				l_pch_topic = CARTDRIDGE_A_B;
			else if( a_rvectSEventCycle.at(i).typeEvent == ETypeEvent_CellInjection )
				l_pch_topic = CELL_INJECTION;
			else if( a_rvectSEventCycle.at(i).typeEvent == ETypeEvent_LoadCartridge_C )
				l_pch_topic = CARTDRIDGE_C;
			else if( a_rvectSEventCycle.at(i).typeEvent == ETypeEvent_LoadCartridge_D )
				l_pch_topic = CARTDRIDGE_D;
			else if( a_rvectSEventCycle.at(i).typeEvent == ETypeEvent_BottleRemoved )
				l_pch_topic = BOTTLE_REMOVED;

			this->send_message(l_strStringToSend.c_str(),l_pch_topic);
		}
	}

	return;
}
*/

// bool SetStatus(const SCurrentState& a_rSCurrentState, const vector<SEventCycle>& a_rvectSEventCycle, const char* a_pchCultureId = NULL, const char* a_pchSerialNumber= NULL);
/*
bool MQTTClient::SetStatus(const SCurrentState& a_rSCurrentState, const vector<SEventCycle>& a_rvectSEventCycle, const char* a_pchCultureId, const char* a_pchSerialNumber)
{
	const char* l_pch_topic = NULL;

	l_pch_topic = SERIAL_NUMBER;
	// Serial Number
	if( a_pchSerialNumber != NULL )
		this->send_message(a_pchSerialNumber,l_pch_topic);
	else
		this->send_message("---",l_pch_topic);

	l_pch_topic = CULTURE_ID;
	// Culture ID
	if( a_pchCultureId != NULL )
		this->send_message(a_pchCultureId,l_pch_topic);
	else
		this->send_message("---",l_pch_topic);

	string l_strStringStartDate;
	if( a_rSCurrentState.sDateStart[0] == 0 &&
			a_rSCurrentState.sDateStart[1] == 0 &&
			a_rSCurrentState.sDateStart[2] == 0 &&
			a_rSCurrentState.sDateStart[3] == 0 &&
			a_rSCurrentState.sDateStart[4] == 0 )
	{
		l_strStringStartDate = "---";
	}
	else
	{
		// Start Date
		string l_Day, l_Month, l_Hour, l_Minutes;
		if( *(&(a_rSCurrentState.sDateStart[1])) < 10)
			l_Day = "0"+ToString<short>(a_rSCurrentState.sDateStart[1]);
		else
			l_Day = ToString<short>(a_rSCurrentState.sDateStart[1]);

		if( *(&(a_rSCurrentState.sDateStart[2])) < 10)
			l_Month = "0"+ToString<short>(a_rSCurrentState.sDateStart[2]);
		else
			l_Month = ToString<short>(a_rSCurrentState.sDateStart[2]);

		if( *(&(a_rSCurrentState.sDateStart[3])) < 10)
			l_Hour = "0"+ToString<short>(a_rSCurrentState.sDateStart[3]);
		else
			l_Hour = ToString<short>(a_rSCurrentState.sDateStart[3]);

		if( *(&(a_rSCurrentState.sDateStart[4])) < 10)
			l_Minutes = "0"+ToString<short>(a_rSCurrentState.sDateStart[4]);
		else
			l_Minutes = ToString<short>(a_rSCurrentState.sDateStart[4]);

		//if (m_CLanguageManager.GetCurrentLanguage() == LNG_ENGLISH)
		l_strStringStartDate = ToString<short>(a_rSCurrentState.sDateStart[0])+"-"+
								l_Day+"-"+
								l_Month+", "+
								l_Hour+":"+
								l_Minutes;
	}

	l_pch_topic = START_DATE;
	this->send_message(l_strStringStartDate.c_str(),l_pch_topic);

	// TIME ELAPSED
	int l_iMinute, l_iDay, l_iHour;
	l_iMinute = a_rSCurrentState.lElapsedTotal_sec / 60;
	l_iHour = l_iMinute / 60;
	l_iMinute= l_iMinute % 60;
	l_iDay = l_iHour / 24;
	l_iHour = l_iHour % 24;

	string l_strStringElapsedTime;
	if( l_iMinute == 0 &&
		l_iDay == 0 &&
		l_iHour == 0 )
	{
		l_strStringElapsedTime = "---";
	}
	else
	{
		//if (m_CLanguageManager.GetCurrentLanguage() == LNG_ENGLISH)
		l_strStringElapsedTime = ToString<int>(l_iDay)+"d"+
				ToString<int>(l_iHour)+"h"+
				ToString<int>(l_iMinute)+"m";
	}

	l_pch_topic = TIME_ELAPSED;
	this->send_message(l_strStringElapsedTime.c_str(),l_pch_topic);

	// CELL DENSITY
	string l_stringCellDensity;
	if( a_rSCurrentState.fAct_PercCellDensity <= 0.0 )
	{
		l_stringCellDensity = "---";
		l_pch_topic = CELL_DENSITY;
		this->send_message(l_stringCellDensity.c_str(),l_pch_topic);
	}
	else
	{
		char l_cvectTemp[50];
		memset(l_cvectTemp, 0, sizeof(l_cvectTemp));
		sprintf(l_cvectTemp, "%.0f", a_rSCurrentState.fAct_PercCellDensity);
		l_stringCellDensity = l_cvectTemp;

		l_pch_topic = CELL_DENSITY;
		this->send_message(l_stringCellDensity.c_str(),l_pch_topic);
	}


	string l_stringTemperature;
	if( a_rSCurrentState.fAct_Temperature == 0.0 )
	{
		l_stringTemperature = "---";
		l_pch_topic = TEMPERATURE;
		this->send_message(l_stringTemperature.c_str(),l_pch_topic);
	}
	else
	{
		char l_cvectTemp[50];
		memset(l_cvectTemp, 0, sizeof(l_cvectTemp));
		sprintf(l_cvectTemp, "%.1f", a_rSCurrentState.fAct_Temperature);
		l_stringTemperature = l_cvectTemp;

		if(a_rSCurrentState.bErrorTemperatureInProgress)
		{
			bool l_bool = true;
			l_stringTemperature += "-" + ToString<bool>(l_bool);
		}
		else
		{
			bool l_bool = false;
			l_stringTemperature += "-" + ToString<bool>(l_bool);
		}

		l_pch_topic = TEMPERATURE;
		this->send_message(l_stringTemperature.c_str(),l_pch_topic);
	}



	string l_stringPHCulture;
	if( a_rSCurrentState.fAct_PH == 0 )
	{
		l_stringPHCulture = "---";
		l_pch_topic = PH;
		this->send_message(l_stringPHCulture.c_str(),l_pch_topic);
	}
	else
	{
		char l_cvectTemp[50];
		memset(l_cvectTemp, 0, sizeof(l_cvectTemp));
		sprintf(l_cvectTemp, "%.1f", a_rSCurrentState.fAct_PH);
		l_stringPHCulture = l_cvectTemp;


		if(a_rSCurrentState.bErrorPHInProgress)
		{
			bool l_bool = true;
			l_stringPHCulture += "-" + ToString<bool>(l_bool);
		}
		else
		{
			bool l_bool = false;
			l_stringPHCulture += "-" + ToString<bool>(l_bool);
		}

		l_pch_topic = PH;
		this->send_message(l_stringPHCulture.c_str(),l_pch_topic);
	}

	// PHASE NUMBER
	string l_stringPhase = "";
	if( a_rSCurrentState.sNumTotMacroPhase != 0 )
	{
		l_stringPhase = a_rSCurrentState.cvectProcedureNumberInRun;
		l_stringPhase += "/";
		l_stringPhase += ToString<short>(a_rSCurrentState.sNumTotMacroPhase);

		//Calocolo string
		//l_stringPhase = ToString<short>(a_rSCurrentState.sProcedureNumberInRun) + "/" + ToString<short>(a_rSCurrentState.sNumTotMacroPhase);
	}
	else
		l_stringPhase = string("---") + string("/") + string("---");
	l_pch_topic = PHASE;
	this->send_message(l_stringPhase.c_str(),l_pch_topic);

	// PHASE DESCRIPTION
	l_pch_topic = PHASE_DESCRIPTION;
	string l_stringPhaseDesc = "---";
	string str(a_rSCurrentState.cvect_Phase_WebServer);
	if( str != "" )
		this->send_message(a_rSCurrentState.cvect_Phase_WebServer,l_pch_topic);
	else
		this->send_message(l_stringPhaseDesc.c_str(),l_pch_topic);

	// PHASE IMAGE
	l_pch_topic = PHASE_IMAGE;
	this->send_message(a_rSCurrentState.cvect_Image_WebServer,l_pch_topic);

	// Notifications
	SendNotification(a_rvectSEventCycle);

	return true;
}*/

