	/***********************************************************/
	/*                        Chat_Client                      */
	/***********************************************************/

	#include <vector>

	using namespace std;

	/***********************************************************/
	/*
		This file defines intefcace betwwen chat_client and client_net
	*/
	/***********************************************************/

	int sign_In(const char* username, const char* password);

	int sign_Up(const char* username, const char* password);

	int get_Contacts(vector<const char*>& contacts);

	int get_Groups(vector<const char*>& groups);

	int contact_Messages(const char* user);

	int group_Messages(const char* group);

	int to_Contact(const char* user, const char* text);

	int to_Group(const char* user, const char* text);

	int change_Password(const char* oldpassword, const char* newpassword);

	int join_Group(const char* group);

	int leave_Group(const char* group);

	/***********************************************************/
	/*                        Network Layer                    */
	/***********************************************************/

	int open_Client();

	// open network access, it can return WINSOCK arror and IP_ADDR_ERROR

	void close_Client();		// close network access

	void send_Request();
	/*
		starts a dialog with the server, returns its result
	*/
	/***********************************************************/
	/*                        Protocol Layer                  */
	/***********************************************************/

	void client_Processing(bool& finish);
	/*
		this function is called by send_Request
		processing the response and forming the next request,
		at the end of the dialog the flag is set, and the result of the dialog is determined
	*/
	/***********************************************************/
	/*                        Indication                       */
	/***********************************************************/

	void error_Message();

	/***********************************************************/



