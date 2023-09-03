	/***********************************************************/
	/*                       Chat Client                       */
	/***********************************************************/

	#include <stdlib.h>
	#include <string>
	#include <fstream>
	#include <iostream>
	#include <string>

	#include "chat_db.h"
	#include "chat_server.h"
	#include "../CHAT/buffer.h"
	#include "../CHAT/message_codes.h"

	using namespace std;

	/***********************************************************/
	/*                          Log Messages                   */
	/***********************************************************/

	ofstream logfile;
	static LogLevel loglevel;

	int open_Log(LogLevel level)
	{
		loglevel = level;
		logfile.open("chat_server_log", ios::out);
		if (!logfile.is_open()) return -1;
		return 0;
	};

	void log_Line(const char* s)
	{
		cout << s;
		logfile << s;
	};

	void end_Line()
	{
		cout << endl;
		logfile << endl;
	};

	void close_Log()
	{
		logfile.close();
	};

	/***********************************************************/

	const char* get_Error_Code(int code) 
	{
		switch (code)
		{
		case USER_NOT_FOUND:	return "USER_NOT_FOUND";
		case EXISTING_USER:		return "EXISTING_USER";
		case WRONG_PASSWORD:	return "WRONG_PASSWORD";
		case GROUP_NOT_FOUND:	return "GROUP_NOT_FOUND";
		case EXISTING_GROUP:	return "DATABASE_ERROR";
		case DATABASE_ERROR:	return "DATABASE_ERROR";
		case PROTOCOL_ERROR:	return "PROTOCOL_ERROR";
		default:				return "UNKNOWN_ERROR";
		};
	};

	/***********************************************************/

	const char* get_Request_Code(int code)
	{
		switch (code)
		{
		case SIGN_IN:			return "SIGN_IN";
		case SIGN_UP:			return "SIGN_UP";
		case FIND_USER:			return "FIND_USER";
		case CONTACT_LIST:		return "CONTACT_LIST";
		case CONTACT_MESSAGES:	return "CONTACT_MESSAGES";
		case TO_CONTACT:		return "TO_CONTACT";
		case GROUP_LIST:		return "GROUP_LIST";
		case GROUP_MEMBERS:		return "GROUP_MEMBERS";
		case JOIN_GROUP:		return "JOIN_GROUP";
		case LEAVE_GROUP:		return "LEAVE_GROUP";
		case NEW_GROUP:			return "NEW_GROUP";
		case GROUP_MESSAGES:	return "GROUP_MESSAGES";
		case TO_GROUP:			return "TO_GROUP";
		case NEXT_MESSAGE:		return "NEXT_MESSAGE";
		default:				return "UNKNOWN REQUEST";
		};
	};

	/***********************************************************/

	const char* get_Reply_Code(int code) 
	{
		switch (code) 
		{
		case DONE:		return "DONE";
		case AUTHENTICATED:	return "AUTHENTICATED";
		case PASSCHANGED:	return "PASSCHANGED";
		case MESSAGE:		return "MESSAGE";
		case END:			return "END";
		case LINE_LIST:		return "LINE_LIST";
		default:			return "UNKNOWN REPLY";
		};
	};

	/***********************************************************/

	void log_Request(int code) 
	{
		if (loglevel != LogLevel::detaillog) return;
		log_Line("RCVD : ");
		log_Line(get_Request_Code(code));
		end_Line();
	};

	/***********************************************************/

	void log_Reply(int code) 
	{
		if (loglevel == LogLevel::nolog)return;
		log_Line("SENT : ");
		if (code < 0)
			log_Line(get_Error_Code(code));
		else
		{
			if (loglevel == LogLevel::detaillog) {
				log_Line(get_Reply_Code(code));
			};
		};
		end_Line();
	};

	/***********************************************************/
	/*                           DB access                     */
	/***********************************************************/

	int open_Chat_Access()	{ return open_DB(); };

	void close_Chat_Access() { close_DB(); }; // close chat access

	/***********************************************************/
	/*                      Message Processing                 */
	/***********************************************************/

	static bool finish = false;

	const char* username = nullptr;
	const char* password = nullptr;

	const char* name = nullptr;
	const char* text = nullptr;

	/***********************************************************/

	void reply_Code(int code)
	{
		log_Reply(code);
		reply.clear();
		string s = to_string(code);
		put_Line(reply, s.c_str());
		if (code != MESSAGE)finish = true;
	};

	/***********************************************************/

	static const char* get_Parameter()
	{
		const char* s = get_Line(request);
		if (s == "") throw 1;				// is missing
		return s;
	};

	/***********************************************************/

	int get_Auth_Data() 
	{
		try {
			username = get_Parameter();		// may be missing
			password = get_Parameter();		// may be missing
		}
		catch (int)							// if it is missing
		{ 
			return PROTOCOL_ERROR;			// protocol error
		};
		return DONE;
	};

	/***********************************************************/

	int auth_User()
	{
		int code = get_Auth_Data();				// get authentication data
		if (code != DONE) return code;			// are data available?
		return auth_User(username, password);	// try to authenticate
	};

	/***********************************************************/

	void create_User() 
	{
		int code = 0;
		code = get_Auth_Data();					// get authentication data
		if (code != DONE) {						// not available?
			reply_Code(code);					// reply with the error code
			return;
		};
		code = new_User(username, password);	// try to create user
		if (code != DONE)
			reply_Code(code);
		else
			reply_Code(AUTHENTICATED);
	};

	/***********************************************************/

	void change_Password() 
	{
		int code = 0;
		const char* username = nullptr;
		const char* password = nullptr;
		const char* newpassw = nullptr;

		// try to get parameters

		try {
			username = get_Parameter();		// may be missing
			password = get_Parameter();		// may be missing
			newpassw = get_Parameter();		// may be missing
		}
		catch (int) 
		{
			reply_Code(PROTOCOL_ERROR);		// reply with the error code
			return; 
		}

		// try to authenticate with the old password

		code = auth_User(username, password);	// try to authenticate
		if (code != DONE){						// not done?
			reply_Code(code);					// reply with the error code
			return;
		};

		// if authenticated change pasword

		code = change_User_Password(newpassw);	// try to change password
		if (code == DONE) code = PASSCHANGED;	// is it done
		reply_Code(code);						// reply with completion code
	};

	/***********************************************************/

	bool get_Name()
	{
		try {
			name = get_Parameter();			// parameter may be missing
		}
		catch (int)
		{
			reply_Code(PROTOCOL_ERROR);		// if missing protocol error
			return false;
		};
		return true;
	};
	/***********************************************************/

	void find_User() 
	{
		if (!get_Name()) return;
		reply_Code(find_Peer(name));
	};

	/***********************************************************/

	void put_List(vector<const char*>& list) 
	{
		for (int i = 0; i < list.size(); i++)
			put_Line(reply, list[i]);
		list.clear();
	};

	/***********************************************************/

	void contact_List() 
	{
		vector<const char*> contacts;
		int code = get_Contact_List(contacts);
		if (code != DONE) 
		{
			reply_Code(code);
			return;
		};
		reply_Code(LINE_LIST);
		put_List(contacts);
	};

	/***********************************************************/

	void group_List()
	{
		vector<const char*> contacts;
		int code = get_Group_List(contacts);
		if (code != DONE)
		{
			reply_Code(code);
			return;
		};
		reply_Code(LINE_LIST);
		put_List(contacts);
	};

	/***********************************************************/

	void next_Message() 
	{
		Message message;
		bool available = false;
		int code = get_Message(available, message);
		if (code != DONE)
		{
			reply_Code(code);
			return;
		};
		if (!available) {
			reply_Code(END);
		}
		else 
		{
			reply_Code(MESSAGE);
			put_Line(reply, message.time);
			put_Line(reply, message.sender);
			put_Line(reply, message.recipient);
			put_Line(reply, message.text);
		};
	};

	/***********************************************************/

	void contact_Messages() 
	{
		if (!get_Name())return;
		int code = get_Contact_Messages(name);
		if (code == NOCONTACT) 
		{
			reply_Code(END);
			return;
		};
		if (code != DONE) 
		{
			reply_Code(code);
			return;
		};
		next_Message();
	};

	/***********************************************************/

	void group_Messages()
	{
		if (!get_Name())return;
		int code = get_Group_Messages(name);
		if (code != DONE)
		{
			reply_Code(code);
			return;
		};
		next_Message();
	};

	/***********************************************************/

	void send_to_Contact() 
	{
		if (!get_Name())return;
		const char* text = get_Line(request);
		reply_Code(to_User_Contact(name, text));
	};

	/***********************************************************/

	void send_to_Group() 
	{
		if (!get_Name())return;
		const char* text = get_Line(request);
		reply_Code(to_User_Group(name, text));
	};

	/***********************************************************/

	void create_Group() 
	{
		if (!get_Name())return;
		reply_Code(create_User_Group(name));
	};

	/***********************************************************/

	void group_Members() 
	{
		if (!get_Name())return;
		vector<const char*> members;
		int code = get_Group_Members(name, members);
		if (code != DONE)
		{
			reply_Code(code);
			return;
		};
		reply_Code(LINE_LIST);
		put_List(members);
	};

	/***********************************************************/

	void join_Group() 
	{
		if (!get_Name())return;
		reply_Code(join_User_Group(name));
	};

	/***********************************************************/

	void leave_Group()
	{
		if (!get_Name())return;
		reply_Code(leave_User_Group(name));
	};

	/***********************************************************/

	void message_Processing()
	{
		const char* s = nullptr;
		int code = UNKNOWN_REQUEST;
			start_Reading();				// start request reding
		try {
			s = get_Line(request);			// get code (may be nullptr)
			code = stoi(s);					// convert into int (exception possible)
			log_Request(code);				// log request
		}
		catch (...) {
			log_Request(code);
			reply_Code(PROTOCOL_ERROR);
			return;
		};

		if ((code <= 0) || (code >= UNKNOWN_REQUEST))	// check code
		{
			reply_Code(PROTOCOL_ERROR);
			return;
		};

		if (code != NEXT_MESSAGE)				// not a NEXT_MESSAGE?
		{
			if (messages_Available())			// messages available?
			{
				reset_Messages();				// reset messsages
				reply_Code(PROTOCOL_ERROR);		// protocol error
				return;
			};
		};

		// for these codes authentication is not required

		switch (code) 
		{
		case SIGN_UP:
			create_User();					// create user 
			return;
		case CHANGE_PASSWORD:
			change_Password();				// change user password
			return;
		case NEXT_MESSAGE:					// next message in the list
			next_Message();
			return;
		};

		// for these codes authentication is required

		int reply_code = auth_User();		// try to authenticate
		if(reply_code != DONE){				// if not done
			reply_Code(reply_code); ;		// reply with the code
			return;
		};

		switch (code) 
		{
		case SIGN_IN:						// sign in?
			reply_Code(AUTHENTICATED);		// authenticated
			return;
		case FIND_USER:						// find user				
			find_User();
			return;
		case CONTACT_LIST:					// get contact list
			contact_List();	
			return;
		case GROUP_LIST:					// get group list
			group_List();
			return;
		case CONTACT_MESSAGES:				// get contact mesages
			contact_Messages();
			return;
		case GROUP_MESSAGES:				// get group messages
			group_Messages();
			return;
		case TO_CONTACT:					// send a message to a contact
			send_to_Contact();				
			return;
		case TO_GROUP:						// send a message to a group
			send_to_Group();	
			return;
		case NEW_GROUP:						// create a new group
			create_Group();
			return;
		case GROUP_MEMBERS:					// get group members
			group_Members();
			return;
		case JOIN_GROUP:					// join group
			join_Group();
			return;
		case LEAVE_GROUP:					// leave group
			leave_Group();
			return;
		default:;
		};
	};

	/***********************************************************/

	void server_Processing(bool& result) 
	{
		finish = false;
		message_Processing();

		result = finish;
	};

	/***********************************************************/



