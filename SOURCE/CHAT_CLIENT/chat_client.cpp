	/***********************************************************/
	/*                       Chat Client                       */
	/***********************************************************/
	
	#define TEST	0

	#include <stdlib.h>
	#include <string>

	#include "chat_interface.h"
	#include "chat_client.h"
	#include "../CHAT/message_codes.h"
	#include "../CHAT/buffer.h"

	using namespace std;

	/***********************************************************/
	/*                       Client Processing                 */
	/***********************************************************/

	int code;

	static bool finish = false;

	static string username = "";
	static string password = "";
	static string newpassw = "";

	static string current_user = "";
	static string current_pass = "";

	vector<const char*>* list;

	/***********************************************************/

	static const char* get_Parameter() 
	{
		const char* s = get_Line(reply);
		if (s == nullptr) throw 1;
		return s;
	};

	/***********************************************************/

	void get_Message() 
	{
		Message message;
		message.time = get_Parameter();
		message.sender = get_Parameter();
		message.recipient = get_Parameter();
		message.text = get_Parameter();
		show_Message(message);
	};

	/***********************************************************/

	void get_List() 
	{
		const char* s;
		for (s = get_Line(reply); strcmp("", s) != 0; s = get_Line(reply))
			list->push_back(s);
	};

	/***********************************************************/

	void send_Next();

	void client_Processing(bool& finish) 
	{
		const char* s = nullptr;
		start_Reading();
		s = get_Line(reply);
		code = stoi(s);

		if (code == MESSAGE)					// MESSAGE code?
			finish = false;						// end of dialog
		else									// else
			finish = true;						// the dialogue continues

		if ((code <= UNKNOWN_ERROR) || (code >= UNKNOWN_REPLY))
		{
			code = PROTOCOL_ERROR;
		};

		if (code < 0)							// error code?
		{
			show_Result(code);					// show result
			return;								// this is all
		};

		switch (code)							// message info			
		{
		case AUTHENTICATED:
			current_user = username;
			current_pass = password;
			code = DONE;
			return;
		case PASSCHANGED:
			current_pass = newpassw;
			code = DONE; 
			return;
		case LINE_LIST:
			get_List();
			code = DONE;
			return;
		case MESSAGE:	
			get_Message();	
			send_Next();
			return;
		case END:
			code = DONE;
			return;
		};
	};

	/***********************************************************/

	int open_Chat() { return open_Client(); };

	void close_Chat() { close_Client(); };

	void error_Message() 
	{
		show_Result(EXCHANGE_ERROR);
	};

	/***********************************************************/
	/*                       Send Request                     */
	/***********************************************************/

#if TEST

	int send_Request()
	{
		finish = false;
		while (!finish)
		{
			server_Processing();
			client_Processing();
		};
		return code;
	};

#endif

	/***********************************************************/

	int start_Dialog()
	{
		send_Request();
		return code;
	};

	/***********************************************************/

	static void put_Code(int code)
	{
		request.clear();
		string s = to_string(code);
		put_Line(request, s.c_str());
	};

	/***********************************************************/

	void send_Next()
	{
		put_Code(NEXT_MESSAGE);
	};

	/***********************************************************/
	/*                       Authentication                    */
	/***********************************************************/

	void put_Auth_Info(const char* user, const char* pass) 
	{
		username = user;
		password = pass;
		current_user = "";
		current_pass = "";
		put_Line(request, user);
		put_Line(request, pass);
	};

	/***********************************************************/

	int sign_In(const char* user, const char* pass) 
	{ 
		put_Code(SIGN_IN);
		put_Auth_Info(user, pass);
		return start_Dialog();
	};

	/***********************************************************/

	int sign_Up(const char* user, const char* pass) 
	{ 
		put_Code(SIGN_UP);
		put_Auth_Info(user, pass);
		return start_Dialog();
	};

	/***********************************************************/

	void request_Header(int code)
	{
		put_Code(code);
		put_Line(request, current_user.c_str());
		put_Line(request, current_pass.c_str());
	};

	/***********************************************************/
	
	int change_Password(const char* oldpass, const char* newpass)
	{
		put_Code(CHANGE_PASSWORD);
		put_Line(request, current_user.c_str());
		put_Line(request, oldpass);
		put_Line(request, newpass);
		newpassw = newpass;
		return start_Dialog();
	};

	/***********************************************************/
	/*                       Contacts                          */
	/***********************************************************/

	int check_Contact(const char* peer) 
	{ 
		request_Header(FIND_USER);
		put_Line(request, peer);
		return start_Dialog();
	};

	/***********************************************************/

	int get_Contacts(vector<const char*>& contacts)
	{
		list = &contacts;
		request_Header(CONTACT_LIST);
		return start_Dialog();
	};

	/***********************************************************/

	int contact_Messages(const char* peer) 
	{
		request_Header(CONTACT_MESSAGES);
		put_Line(request, peer);
		return start_Dialog();
	};

	/***********************************************************/

	int to_Contact(const char* peer, const char* text) 
	{ 
		request_Header(TO_CONTACT);
		put_Line(request, peer);
		put_Line(request, text);
		return start_Dialog();
	};

	/***********************************************************/
	/*                          Groups                         */
	/***********************************************************/

	int get_Groups(vector<const char*>& groups) 
	{
		list = &groups;
		request_Header(GROUP_LIST);
		return start_Dialog();
	};

	/***********************************************************/

	int get_Members(const char* group, vector<const char*>& members) 
	{ 
		list = &members;
		request_Header(GROUP_MEMBERS);
		put_Line(request, group);
		return start_Dialog();
	};

	/***********************************************************/

	int to_Group(const char* group, const char* text) 
	{ 
		request_Header(TO_GROUP);
		put_Line(request, group);
		put_Line(request, text);
		return start_Dialog();;
	};

	/***********************************************************/

	int join_Group(const char* group) 
	{ 
		request_Header(JOIN_GROUP);
		put_Line(request, group);
		return start_Dialog();
	};

	/***********************************************************/

	int leave_Group(const char* group) 
	{  
		request_Header(LEAVE_GROUP);
		put_Line(request, group);
		return start_Dialog();
	};

	/***********************************************************/

	int create_Group(const char* group)
	{
		request_Header(NEW_GROUP);
		put_Line(request, group);
		return start_Dialog();
	};

	/***********************************************************/

	int group_Messages(const char* group)
	{
		request_Header(GROUP_MESSAGES);
		put_Line(request, group);
		return start_Dialog();
	};

	/***********************************************************/

