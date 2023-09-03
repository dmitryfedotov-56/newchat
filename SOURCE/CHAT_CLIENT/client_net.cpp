	/***********************************************************/
	/*                      Client Networking                  */
	/***********************************************************/	
	
	#include <fstream>
	#include <iostream>
	#include <stdio.h>
	#include <vector>
	#include <conio.h>

	#include "chat_client.h"
	#include "../CHAT/buffer.h"
	#include "../CHAT/network.h"

	/***********************************************************/

	#define DEBUG	0		// debugging output

	#define TIMEOUT	0		// time to wait

	#define NOT_FOUND		"not found"
	#define IP_REQUIRED		"ip address required"
	#define PORT_REQUIRED	"port number required"
	#define INVALID_IP		"invalid ip address"
	#define INVALID_PORT	"invalid port number"
	#define NETWORK_ERROR	"communication error"

	void network_Error()
	{
		error_Message();
	};

	void init_Error(const char* s) 
	{
		cout << s << endl;
		cout << "press any key" << endl;
 		while (!_kbhit()) {};
	};

	/***********************************************************/

	#define CONFIG	"chat_client_conf"

	string server_addr = "";	// server IP address
	int server_port = 0;		// server port

	/***********************************************************/

	int get_Config()						// read config file
	{
		string s;
		ifstream config(CONFIG);			// open config
		if (!config.is_open()) {
			s = CONFIG;
			s += " ";
			s += NOT_FOUND;
			init_Error(s.c_str());
			return -1;
		};
		config >> server_addr;				// get ip address
		if (config.fail()) {
			init_Error(IP_REQUIRED);
			return -1;
		};
		config >> server_port;				// get port number
		if (config.fail()) {
			init_Error(PORT_REQUIRED);
			return -1;
		};
		if ((server_port <= 1024) || (server_port >= 49151)) {
			init_Error(INVALID_PORT);
			return -1;
		};
		return 0;
	};

	/***********************************************************/
	
	int open_Client() 
	{
		int status;
		status = get_Config();
		if (status < 0)return status;
		status = open_Network(server_addr.c_str(), server_port);
		if (status < 0) 
		{
			if (status == BADADDR)
				init_Error(INVALID_IP);
			else
				init_Error(NETWORK_ERROR);
		};
		return status;
	};

	void close_Client() { close_Network(); };

	/***********************************************************/
	/*                      Exchange Cycle                     */
	/***********************************************************/

	void send_Request() 
	{
		int status = 0;
		bool finish = false;

		status = open_Connection();
		if (status < 0 ){
			network_Error();
			close_Connection();
			return;
		};
		
		short packet_size = 0;												

		while (true) {

			// send message to server 
			packet_size = send_Packet(request.data(), request.size());
			if (packet_size < 0) {
				network_Error();
				close_Connection();
				return;
			};
#if DEBUG
			cout << "SENT:" << packet_size << endl;
			show_Buffer(request, request.size());
#endif

			// receive reply
			clear_Buffer(reply);
			packet_size = wait_Packet(reply.data(), BUFF_SIZE, TIMEOUT);
			if (packet_size < 0) {
				network_Error();
				close_Connection();
				return;
			};
#if DEBUG
			cout << "RCVD:" << packet_size << endl;
			show_Buffer(reply, packet_size);
#endif
			// reply processing
			client_Processing(finish);

			// if finish close connection
			if (finish) {
				close_Connection();
				return;
			};
		};

		close_Connection();
	};

	/***********************************************************/


