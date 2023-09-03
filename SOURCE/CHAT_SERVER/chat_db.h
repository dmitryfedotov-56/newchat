	#pragma once
	/***********************************************************/
	/*                       Chat Interface                    */
	/***********************************************************/
	/*
		db interface definition
	*/
	/***********************************************************/

	#include <vector>
	#include "../CHAT/definitions.h"

	using namespace std;

	#define NOCONTACT	-11

	int open_DB();

	void close_DB();

	// authentication

	int auth_User(const char* user, const char* pass);

	int new_User(const char* user, const char* pass);

	int change_User_Password(const char* newpass);

	// groups

	int get_Group_List(vector<const char*>& groups);

	int get_Group_Members(const char* group, vector<const char*>& members);

	int join_User_Group(const char* group);

	int leave_User_Group(const char* group);

	int create_User_Group(const char* group);

	int get_Group_Messages(const char* group);

	int to_User_Group(const char* group, const char* text);

	// contacts

	int find_Peer(const char* peer);

	int get_Contact_List(vector<const char*>& contacts);

	int get_Contact_Messages(const char* peer);

	int to_User_Contact(const char* group, const char* text);

	/***********************************************************/

	int get_Message(bool& available, Message& message);

	bool messages_Available();	// message list available

	void reset_Messages();		// free up memory occupied by messages
	
	/***********************************************************/

