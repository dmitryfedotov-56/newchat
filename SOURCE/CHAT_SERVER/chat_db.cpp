	/***********************************************************/
	/*                       Chat DataBase                     */
	/***********************************************************/
	/*
		chat interface definition
	*/
	/***********************************************************/

	#define _CRT_SECURE_NO_WARNINGS

	#include <time.h>
	#include <string>

	#include "../CHAT/definitions.h"
	#include "chat_db.h"
	#include "sha1.h"
	#include <mysql.h>

	#include <iostream>
	using namespace std;

	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;

	string query = "";

	bool group_messages = false;
	bool contact_messages = false;

	/***********************************************************/

	int open_DB()
	{
		mysql_init(&mysql);

		if (&mysql == NULL) return DATABASE_ERROR;

		if (!mysql_real_connect(&mysql, "localhost", "root", "root", "chat_db", 0, NULL, 0))
			return DATABASE_ERROR;

		mysql_set_character_set(&mysql, "cp1251");
		return DONE;
	};

	void close_DB() { mysql_close(&mysql); };

	/***********************************************************/

	int sql_Query(string& query)
	{
		int code = mysql_query(&mysql, query.c_str());
		code = mysql_errno(&mysql);
		if (code != 0)return DATABASE_ERROR;
		return DONE;
	};

	int get_Result(string& query) 
	{
		int code;
		mysql_query(&mysql, query.c_str());
		code = mysql_errno(&mysql);
		if (code != 0) return DATABASE_ERROR;
		res = mysql_store_result(&mysql);
		code = mysql_errno(&mysql);
		if (code != 0) return DATABASE_ERROR;
		return DONE;
	};

	void start_Update() {
		mysql_query(&mysql, "START TRANSACTION");
	};

	void commit_Update() { mysql_query(&mysql, "COMMIT"); };

	void cancel_Update() { mysql_query(&mysql, "ROLLBACK"); };


	/***********************************************************/
	/*                       Authentication                    */
	/***********************************************************/

	char str_digest[42];

	void sha_Str(Digest& digest) 
	{
		int k = 0;
		char s[10];
		for (int i = 0; i < 5; i++) 
		{
			sprintf(s, "%08x", digest[i]);
			for (int j = 0; j < 8; j++) 
			{
				str_digest[k] = s[j];
				k++;
			};
		};
		digest[40] = 0;
	};

	/***********************************************************/

	string user_id = "";
	string user_name = "";

	int auth_User(const char* name, const char* pass) 
	{ 
		Digest digest;
		sha1(pass, strlen(pass), digest);
		sha_Str(digest);
		query = "SELECT * FROM person WHERE name = ''";
		query.insert(query.size() - 1, name);
		int code = get_Result(query);							// get result
		if (code != 0) 											// error?
			code = DATABASE_ERROR;								
		else {													// no error
			row = mysql_fetch_row(res);							// no rows?
			if (row == nullptr)code = USER_NOT_FOUND;			// not found
			else {
				const char* s = row[2];							// check password
					if (strcmp(str_digest, s) != 0)code = WRONG_PASSWORD;
				else { 
 					code = DONE;								// done 
					user_id = row[0];							// store id
					user_name = name;							// store name
				};
			};
		};
		mysql_free_result(res);
		return code;  
	};

	/***********************************************************/

	#define DUPLICATED 1062

	int new_User(const char* user, const char* pass) 
	{ 
		Digest digest;
		sha1(pass, strlen(pass), digest);
		sha_Str(digest);
		query = "INSERT INTO person(id, name, pass) values(default, '')";
		string values = user;
		values += "', '";
		values += str_digest;
		query.insert(query.size() - 2, values);
		mysql_query(&mysql, query.c_str());
		int code = mysql_errno(&mysql);
		if (code != 0)
		{
			if (code == DUPLICATED)
				return EXISTING_USER;
			else
				return DATABASE_ERROR;
		};
		return auth_User(user, pass);
	};

	/***********************************************************/

	int change_User_Password(const char* newpass) 
	{
		Digest digest;
		sha1(newpass, strlen(newpass), digest);
		sha_Str(digest);
		query = "UPDATE person SET pass = ''";
		query.insert(query.size() - 1, str_digest);
		query += " WHERE id = ";
		query += user_id;
		mysql_query(&mysql, query.c_str());
		int code = mysql_errno(&mysql);
		if (code != 0) return DATABASE_ERROR;
		return DONE;
	};

	/***********************************************************/
	/*                         Groups                          */
	/***********************************************************/

	int get_Group_List(vector<const char*>& groups) 
	{
		query = "SELECT community.name FROM person_community ";
		query += "JOIN community ON community.id = person_community.community_id ";
		query += "WHERE person_id = ";
		query.insert(query.size(), user_id);
		int code = get_Result(query);				// get result
		if (code != 0)								// error?
			code = DATABASE_ERROR;
		else {										// no error
			code = DONE;							// done
			while (row = mysql_fetch_row(res))		// fill in the list
				groups.push_back(row[0]);
		};
		mysql_free_result(res);
		return code;
	};

	/***********************************************************/

	string group_id = "";

	int find_Group(const char* group) 
	{
		int code;
		query = "SELECT id FROM community WHERE community.name = ''"; // get group id
		query.insert(query.size() - 1, group);
		code = get_Result(query);					// get result
		if (code != 0)
			code = DATABASE_ERROR;
		else {
			row = mysql_fetch_row(res);				// no rows?
			if (row == nullptr)
				code = GROUP_NOT_FOUND;				// not found
			else {									// else 
				code = DONE;						// done
				group_id = row[0];					// store id
			};
		};
		mysql_free_result(res);
		return code;
	};

	/***********************************************************/

	int get_Group_Members(const char* group, vector<const char*>& members) 
	{
		int code = find_Group(group);
		if (code != DONE) return code;
		query = "SELECT person.name FROM person_community "; 
		query += "JOIN person ON person.id = person_community.person_id ";
		query += "WHERE community_id = ";
		query += group_id;
		code = get_Result(query);					// get result
		if (code != 0)								// error?
			code = DATABASE_ERROR;
		else {										// no error
			code = DONE;							// done
			while (row = mysql_fetch_row(res))		// fill in the list
				members.push_back(row[0]);
		};
		mysql_free_result(res);
		return code;
	};

	/***********************************************************/

	int join_User_Group(const char* group) 
	{
		int code = find_Group(group);
		if (code != DONE) return code;
		query = "INSERT INTO person_community(id, person_id, community_id) ";
		query += "values(default, ";
		query += user_id;
		query += ", ";
		query += group_id;
		query += ")";
		mysql_query(&mysql, query.c_str());
		code = mysql_errno(&mysql);
		if (code == 0)code = DONE; else code = DATABASE_ERROR;
		return code;
	};

	/***********************************************************/

	int leave_User_Group(const char* group)
	{
		int code = find_Group(group);
		if (code != DONE) return code;
		query = "DELETE FROM person_community WHERE person_id = ";
		query += user_id;
		query += " AND community_id = ";
		query += group_id;
		mysql_query(&mysql, query.c_str());
		code = mysql_errno(&mysql);
		if (code == 0)code = DONE; else code = DATABASE_ERROR;
		return code;
	};

	/***********************************************************/

	int create_User_Group(const char* group) 
	{
		int code;
		query = "INSERT INTO community(id, name) values(default, '')";
		query.insert(query.size() - 2, group);
		start_Update();
		mysql_query(&mysql, query.c_str());
		code = mysql_errno(&mysql);
		if (code != 0) {
			cancel_Update();
			if (code == DUPLICATED)
				code = EXISTING_GROUP;
			else
				code = DATABASE_ERROR;
		}
		else {
			code = join_User_Group(group);
			if (code == DONE)commit_Update(); else cancel_Update();
		};
		return code;
	};

	/***********************************************************/

	int get_Group_Messages(const char* group) 
	{
		int code = find_Group(group);
		if (code != DONE) return code;
		query = "SELECT timestamp, person.name, community.name, text ";
		query += "FROM message ";
		query += "JOIN person ON message.sender_id = person.id ";
		query += "JOIN community ON message.community_id = community.id ";
		query += "WHERE community_id = ";
		query.insert(query.size(), group_id);
		code = get_Result(query);						// get result
		if (code == 0) {								// error?
			code = DONE;
			group_messages = true;
		}
		else 
			code = DATABASE_ERROR;;
		// mysql_free_result(res);
		return code;;
	};

	/***********************************************************/

	void form_Message_Query()
	{
		time_t timestamp = time(nullptr);
		string time = to_string(timestamp);
		query = "INSERT INTO message ";
		query += "(id, timestamp, sender_id, contact_id, community_id, text)";
		query += " values(default, ";
		query += time;
		query += ", ";
		query += user_id;
	};

	/***********************************************************/

	int to_User_Group(const char* group, const char* text)
	{
		int code = find_Group(group);		// find group
		if (code != DONE) return code;		// not found?

		form_Message_Query();				// query main part
		query += ", NULL, ";				// no contact
		query += group_id;					// group id
		query += ", '";
		query += text;						// message text
		query += "')";

		mysql_query(&mysql, query.c_str());
		code = mysql_errno(&mysql);
		if (code == 0)code = DONE; else code = DATABASE_ERROR;
		return code;
	};

	/***********************************************************/
	/*                         Contacts                       */
	/***********************************************************/

	string peer_id = "";
	string peer_name = "";

	int find_Peer(const char* peer)
	{
		query = "SELECT * FROM person WHERE name = ''";
		query.insert(query.size() - 1, peer);
		int code = get_Result(query);							// get result
		if (code != 0)											// error?
			code = DATABASE_ERROR;
		else {													// no error
			row = mysql_fetch_row(res);
			if (row == nullptr)
				code = USER_NOT_FOUND;							// not found
			else {
				code = DONE;									// if found
				peer_id = row[0];								// store peer id
				peer_name = peer;								// store peer name
			};						
		};
		mysql_free_result(res);
		return code;;
	};

	/***********************************************************/

	string contact_id;

	int find_User_Contact(const char* peer) 
	{
		int code = find_Peer(peer);
		if (code != DONE)return code;
		query = "SELECT contact_id FROM person_contact WHERE person_id = "; 
		query += user_id;
		query += " AND mutual_id = ";
		query += peer_id;
		code = get_Result(query);			// get result
		if (code != 0)
			code = DATABASE_ERROR;
		else 
		{
			row = mysql_fetch_row(res);
			if (row == nullptr)
				code = NOCONTACT;
			else 
			{
				code = DONE;
				contact_id = row[0];		// store contact id
			};
		};
		mysql_free_result(res);
		return code;
	};

	/***********************************************************/

	int find_Contact() 
	{
		query = "SELECT id FROM contact WHERE id1 = ";
		query += user_id;
		query += " AND id2 = ";
		query += peer_id;
		int code = get_Result(query);
		if (code != 0) {										// error?
			code = DATABASE_ERROR;
		}
		else {													// no error
			row = mysql_fetch_row(res);
			if (row == nullptr)
				code = DATABASE_ERROR;							// not found
			else {
				contact_id = row[0];							// store contact id
			};
		};
		mysql_free_result(res);
		return code;
	};

	/***********************************************************/

	void create_Contact() 
	{
		query = "INSERT INTO contact (id, id1, id2) values(default, ";
		query += user_id;
		query += ", ";
		query += peer_id;
		query += ")";
		mysql_query(&mysql, query.c_str());
		int code = mysql_errno(&mysql);
		if (code != 0) throw DATABASE_ERROR;
	};
	
	/***********************************************************/

	void create_User_Contact(string& id1, string& id2) 
	{
		query = "INSERT INTO person_contact (id, person_id, mutual_id, contact_id) ";
		query += " values(default, ";
		query += id1;
		query += ", ";
		query += id2;
		query += ", ";
		query += contact_id;
		query += ")";
		mysql_query(&mysql, query.c_str());
		int code = mysql_errno(&mysql);
		if (code != 0) throw DATABASE_ERROR;
	};

	/***********************************************************/

	void new_Contact() 
	{
		create_Contact();
		int code = find_Contact();
		if (code != DONE)throw code;
		create_User_Contact(user_id, peer_id);
		create_User_Contact(peer_id, user_id);
	};

	/***********************************************************/

	int to_User_Contact(const char* peer, const char* text) 
	{
		int code;
		start_Update();
		try {
			code = find_User_Contact(peer);
			if (code == DATABASE_ERROR)throw code;
			if (code == NOCONTACT)new_Contact();
			form_Message_Query();
			query += ", ";
			query += contact_id;				// contact id
			query += ", NULL, '";				// no group
			query += text;						// message text
			query += "')";

			mysql_query(&mysql, query.c_str());
			code = mysql_errno(&mysql);
			if (code != 0)throw DATABASE_ERROR;
		}
		catch (int error) 
		{
			cancel_Update();
			return error;
		};
		commit_Update();
		return DONE;
	};

	/***********************************************************/

	int get_Contact_Messages(const char* peer)
	{
		Message message;
		string sender = "";
		string contact = "";
		int code = find_User_Contact(peer);
		if (code == NOCONTACT)return code;
		if (code != DONE)return code;
		query = "SELECT timestamp, sender_id, text ";
		query += "FROM message WHERE contact_id = ";
		query += contact_id;
		code = get_Result(query);						// get result
		if (code == 0) {								// error?
			code = DONE;
			contact_messages = true;
		}
		else 
			code = DATABASE_ERROR;
		// mysql_free_result(res);
		return code;;
	};

	/***********************************************************/

	int get_Contact_List(vector<const char*>& contacts)
	{
		query = "SELECT person.name FROM person_contact ";
		query += "JOIN person ON person_contact.mutual_id = person.id ";
		query += "WHERE person_contact.person_id = ";
		query.insert(query.size(), user_id);
		int code = get_Result(query);					// get result
		if (code != 0)									// error?
			code = DATABASE_ERROR;
		else {											// no error
			code = DONE;								// done
			while (row = mysql_fetch_row(res))			// fill in the list
				contacts.push_back(row[0]);
		};
		return code;
	};

	/***********************************************************/
	/*                         Messages                        */
	/***********************************************************/
	
	void reset_Messages()		// free up memory occupied by messages
	{
		group_messages = false;
		contact_messages = false;
		mysql_free_result(res);
	};

	/***********************************************************/

	int get_Group_Message(bool& available, Message& message)
	{
		row = mysql_fetch_row(res);
		if (mysql_errno(&mysql) != 0)return DATABASE_ERROR;
		if (row == nullptr)
		{
			reset_Messages();
			available = false;
			return DONE;
		}
		message.time = row[0];
		message.sender = row[1];
		message.recipient = row[2];
		message.text = row[3];
		available = true;
		return DONE;
	};

	/***********************************************************/

	int get_Contact_Message(bool& available, Message& message) 
	{
		row = mysql_fetch_row(res);
		if (mysql_errno(&mysql) != 0)return DATABASE_ERROR;
		if (row == nullptr)
		{
			reset_Messages();
			available = false;
			return DONE;
		};
		message.time = row[0];
		const char* sender = row[1];
		message.text = row[2];
		if (sender == user_id) {
			message.sender = user_name.c_str();
			message.recipient = peer_name.c_str();
		}
		else {
			message.sender = peer_name.c_str();
			message.recipient = user_name.c_str();
		};
		available = true;
		return DONE;
	};

	/***********************************************************/

	int get_Message(bool& available, Message& message)
	{
		if (group_messages) {
			return get_Group_Message(available, message);
		};

		if (contact_messages) {
			return get_Contact_Message(available, message);
		};

		return DATABASE_ERROR;
	};

	/***********************************************************/
	
	bool messages_Available()	// message list available
	{
		return group_messages || contact_messages;
	};

	/***********************************************************/


