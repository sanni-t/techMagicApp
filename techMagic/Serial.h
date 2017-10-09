#pragma once

class Serial
{
private:
	//Serial comm handler
	HANDLE hSerial;
	//Connection status
	bool connected;
	//Get various information about the connection
	COMSTAT status;
	//Keep track of last error
	DWORD errors;

	//Read data in a buffer, if nbChar is greater than the
	//maximum number of bytes available, it will return only the
	//bytes available. The function return -1 when nothing could
	//be read, the number of bytes actually read.
	int ReadData(char *buffer, unsigned int nbChar);
	//Writes data from a buffer through the Serial connection
	//return true on success.
	bool WriteData(const char *buffer, unsigned int nbChar);
	//Check if we are actually connected
	bool IsConnected();

public:
	//Initialize Serial communication with the given COM port
	Serial(const char *portName);
	
	bool sendCommand(SERIAL_CMD cmd);

	//Close the connection
	~Serial();
	
};

