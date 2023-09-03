	#pragma once
	/***********************************************************/
	/*                       Chat Interface                    */
	/***********************************************************/
	/*
		chat interface definition
	*/
	/***********************************************************/
	
	#include <vector>
	#include <time.h>

	#include "../CHAT/definitions.h"

	using namespace std;

	/***********************************************************/
	/*                       Interface Part                    */
	/***********************************************************/

	void show_Result(int code);

	void show_Message(Message& message);

	/***********************************************************/
	/*                       Client Interface                  */
	/***********************************************************/

	int open_Chat();

	void close_Chat();

	/***********************************************************/
	/*                       Authentication                    */
	/***********************************************************/

	int sign_In(const char* username, const char* password);

	int sign_Up(const char* username, const char* password);

	int change_Password(const char* oldpass, const char* newpass);

	/***********************************************************/
	/*                       Contacts                          */
	/***********************************************************/

	int check_Contact(const char* peer);

	int get_Contacts(vector<const char*>& contacts);

	int contact_Messages(const char* peer);

	int to_Contact(const char* peer, const char* text);

	/***********************************************************/
	/*                          Groups                         */
	/***********************************************************/

	int get_Groups(vector<const char*>& groups);

	int get_Members(const char* group, vector<const char*>& members);

	int join_Group(const char* group);

	int leave_Group(const char* group);

	int create_Group(const char* group);

	int group_Messages(const char* group);

	int to_Group(const char* user, const char* text);

	/***********************************************************/
	




