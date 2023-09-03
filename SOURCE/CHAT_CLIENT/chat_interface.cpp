	/***********************************************************/
	/*                     Chat Interface                      */
	/***********************************************************/

	#define _CRT_SECURE_NO_WARNINGS

	#include <iostream>
	using namespace std;

	#include <time.h>
	#include <conio.h>
	#include <stdlib.h>
	#include <string>

	#include "../CHAT/definitions.h"
	#include "chat_interface.h"

	/***********************************************************/
	/*
		locale data
	*/
	/***********************************************************/

	#define  RUSSIAN 1

#if RUSSIAN
						 
	#define OK				"OK"
	#define NO_USER_NAME	"пользователь не найден"
	#define USER_EXISTS		"пользователь уже существует"
	#define WRONG_PASS		"неверный пароль"
	#define UNKNOWN_ERR		"неизвестная ошибка"
	#define DATABASE_ERR	"ошибка базы данных"
	#define EXCHANGE_ERR	"ошибка обмена"
	#define PROTOCOL_ERR	"ошибка протокола"

	#define EXIT			"выход из приложения"
	#define SIGN_IN			"вход"
	#define SIGN_UP			"регистрация"
	#define USERNAME		"логин"
	#define PASSWORD		"пароль"
	#define CHANGEPASS		"изменить пароль"
	#define OLDPASSWORD		"старый пароль"
	#define NEWPASSWORD		"новый пароль "
	#define PASSCHANGED		"пароль изменен" 
	#define CHANGEUSER		"сменить пользователя"

	#define CONTACTS		"мои контакты"
	#define GROUPS			"мои группы"

	#define LISTISEMPTY		"список пуст"

	#define CANCEL			"отмена"
	#define CHOOSECONTACT	"выбрать контакт"
	#define NEWCONTACT		"новый контакт"

	#define NO_GROUP_NAME	"группа не найдена"
	#define GROUP_EXISTS	"группа уже существует"
	#define CHOOSEGROUP		"выбрать группу"
	#define JOINGROUP		"присоединиться к группе"
	#define CREATEGROUP		"создать группу"
	#define GROUPNAME		"группа"

	#define MAINMENU		"главное меню"		
	#define MESSAGES		"показать сообщения"
	#define SENDMESSAGE		"послать сообщение"	
	#define MESSAGETEXT		"текст"
	#define LEAVEGROUP		"покинуть группу"
	#define GROUPMEMBERS	"члены группы"

	#define NOMESSAGES		"нет сообщений"
	#define PEERNAME		"пользователь"
#endif

#ifdef	ENGLISH

	#define OK				"OK"
	#define NO_USER_NAME	"user not found"
	#define USER_EXISTS		"user already exists"
	#define WRONG_PASS		"wrong password"
	#define UNKNOWN_ERR		"unknown error"
	#define DATABASE_ERR	"database error"
	#define EXCHANGE_ERR	"communication error"
	#define PROTOCOL_ERR	"protocol error"

	#define EXIT			"exit"
	#define SIGN_IN			"sign in"
	#define SIGN_UP			"sign up"
	#define USERNAME		"login"
	#define PASSWORD		"password"
	#define CHANGEPASS		"change password"
	#define OLDPASSWORD		"old password"
	#define NEWPASSWORD		"new password"
	#define PASSCHANGED		"password changed" 
	#define CHANGEUSER		"сменить пользователя"

	#define CONTACTS		"my contacts"
	#define GROUPS			"my groups"

	#define LISTISEMPTY		"list is empty"

	#define CANCEL			"cancel"
	#define CHOOSECONTACT	"choose contact"
	#define NEWCONTACT		"new contact"

	#define NO_GROUP_NAME	"group not found"
	#define GROUP_EXISTS	"group already exits"
	#define CHOOSEGROUP		"choose group"
	#define JOINGROUP		"join group"
	#define CREATEGROUP		"create group"
	#define GROUPNAME		"group"

	#define MAINMENU		"main menu"		
	#define MESSAGES		"messages"
	#define SENDMESSAGE		"send message"	
	#define MESSAGETEXT		"text"
	#define LEAVEGROUP		"leave group"
	#define GROUPMEMBERS	"group members"

	#define NOMESSAGES		"no messages"
	#define PEERNAME		"user"
#endif

	/***********************************************************/

	void show_Result(int code)
	{
		cout << " ";
		if (code == 0)	return;		// normal execution?	
		if (code < 0)				// error code ?
		{
			cout << endl;
			switch (code)
			{
			case USER_NOT_FOUND:	cout << NO_USER_NAME;
				break;
			case EXISTING_USER:		cout << USER_EXISTS;
				break;
			case WRONG_PASSWORD:	cout << WRONG_PASS;
				break;
			case GROUP_NOT_FOUND:	cout << NO_GROUP_NAME;
				break;
			case EXISTING_GROUP:	cout << GROUP_EXISTS;
				break;
			case DATABASE_ERROR:	cout << DATABASE_ERR;
				break;
			case EXCHANGE_ERROR:	cout << EXCHANGE_ERR;
				break;
			case PROTOCOL_ERROR:	cout << PROTOCOL_ERR;
				break;
			default:;
				cout << UNKNOWN_ERR;
			};
		};
		cout << endl;
	};

	/***********************************************************/

	void show_Bar()
	{
		// cout << " ";
		for (unsigned i = 0; i < 60; i++)cout << "-";
		cout << endl;
	};

	/***********************************************************/

	#define BUFFSIZE	40

	static bool nomessages = true;
	static string context_name = "";

	void show_Message(Message& message)
	{
		nomessages = false;

		show_Bar();

		struct tm* local;
		char s[BUFFSIZE];
		time_t time = stoull(message.time);	// message time
		local = localtime(&time);
		strftime(s, BUFFSIZE, "%d.%m.%Y %H:%M:%S", local);
		cout << " " << s << " ";

		if(strcmp(context_name.c_str(), message.sender) == 0)
		{
			cout << " ";
			cout << message.sender;
			cout << " -> ";
			cout << message.recipient;
		};

		if (strcmp(context_name.c_str(), message.recipient) == 0)
		{
			cout << " ";
			cout << message.recipient;
			cout << " <- ";
			cout << message.sender;
		};

		cout << endl;

		cout << " ";
		cout << message.text << endl ;		// message text
	};

	/***********************************************************/
	/*                          Context                        */
	/***********************************************************/

	class Context;

	Context* current_context = nullptr;

	string prompt = "";

	class Context 
	{
	protected:
		std::vector<const char*>table;
	public:

		virtual const char* get_Prompt() { return ""; };

		virtual void show_Menu() 
		{
			char text[6] = " x : ";
			show_Bar();
			for (int i = 0; i < table.size(); i++)
			{
				text[1] = (char)(48 + i);
				cout << text << table[i] << endl;
			};
			show_Bar();
			cout << get_Prompt() << ">";
		};

		virtual void show_Choice(int code) 
		{
			if (code < table.size()) cout << table[code];
		};

		virtual void operation(int code) = 0;

		virtual void become_Current() 
		{ 
			current_context = this; 
			show_Menu();
		};

		Context(const vector<const char*>& t) : table(t) {};
	};

	/***********************************************************/

	void make_Current(Context* context) 
	{
		cout << endl;
		context->become_Current();
	};

	/***********************************************************/
	/*                       User Context                      */
	/***********************************************************/

	static string current_user = "";

	class User_Context : public Context 
	{
	protected:
	public:
		virtual const char* get_Prompt() override { return current_user.c_str(); };

		User_Context(const vector<const char*>& t) : Context(t) {};
	};

	/***********************************************************/
	/*                       User Context                      */
	/***********************************************************/

	void main_Context();
	void contact_Control();
	void group_Control();
	void auth_Context();

	void get_Text(string& text) 
	{
		cout << endl;
		show_Bar();
		cout << MESSAGETEXT << " : ";
		getline(cin, text);
	};

	/***********************************************************/
	/*                       Contact Context                   */
	/***********************************************************/

	string current_contact = "";
	string contact_prompt = " ";

	class Contact_Context : public User_Context
	{
	public:

		Contact_Context() : 
			User_Context({ MAINMENU, CONTACTS, GROUPS, MESSAGES, SENDMESSAGE }) {};

		virtual const char* get_Prompt() { return contact_prompt.c_str(); };

		void show_Messages()
		{
			nomessages = true;
			context_name = current_user;
			cout << endl;
			int code = contact_Messages(current_contact.c_str());
			if (code == DONE)
				if (nomessages) cout << NOMESSAGES << endl;
		};

		void send_Message()		// send a message to contact
		{
			string text;
			get_Text(text);
			int code = to_Contact(current_contact.c_str(), text.c_str());
		};

		virtual void become_Current()
		{
			current_context = this;
			cout << endl;
			show_Menu();
		};

		virtual void operation(int code) override 
		{
			switch (code)
			{
			case 0: main_Context();			return;
			case 1:	contact_Control();		return;
			case 2:	group_Control();		return;
			case 3:	show_Messages();		break;
			case 4:	send_Message();			break;
			default:						return;
			};
			show_Menu();
		};
	};

	static Contact_Context contact_context;

	/***********************************************************/
	/*                       Group Context                     */
	/***********************************************************/

	string current_group = "";
	string group_prompt = " ";

	class Group_Context : public User_Context
	{
	public:

		Group_Context() :
			User_Context({ MAINMENU, CONTACTS, GROUPS, MESSAGES, SENDMESSAGE, GROUPMEMBERS,LEAVEGROUP }) {};

		virtual const char* get_Prompt() { return group_prompt.c_str(); };

		void show_Messages() 
		{
			nomessages = true;
			context_name = current_group;
			cout << endl;
			int code = group_Messages(current_group.c_str());
			if (code == DONE) 
				if (nomessages) cout << NOMESSAGES << endl;
		};

		void send_Message() 
		{
			string text;
			get_Text(text);
			int code = to_Group(current_group.c_str(), text.c_str());
		};	

		virtual void become_Current()
		{
			current_context = this;
			cout << endl;
			show_Menu();
		};

		void group_Members() 
		{
			vector<const char*> list = {};
			int code = get_Members(current_group.c_str(), list);
			cout << endl;
			if (code == DONE)
			 {
				if (list.size() == 0)
					cout << " " << LISTISEMPTY;
				else {
					show_Bar();
					for (int i = 0; i < list.size(); i++)
						cout << " " << list[i] << endl;
				};
			};
		};

		void leave_User_Group() 
		{
			int code = leave_Group(current_group.c_str());
			if (code != DONE) {
				show_Menu();
				return;
			};
			main_Context();
		};	

		virtual void operation(int code) override
		{
			switch (code)
			{
			case 0: main_Context();			return;
			case 1:	contact_Control();		return;
			case 2:	group_Control();		return;
			case 3:	show_Messages();		break;
			case 4:	send_Message();			break;
			case 5: group_Members();		break;
			case 6: leave_User_Group();		return;
			default:						return;
			};
			show_Menu();
		};
	};

	static Group_Context group_context;

	/***********************************************************/
	/*                    Transitional Context                 */
	/***********************************************************/

	class Transitional_Context : public User_Context
	{
	protected:
		Context* parent_context = nullptr;
	public:
		virtual void become_Current() override
		{
			parent_context = current_context;		// store current context
			User_Context::become_Current();			// become current
		};

		virtual bool cancel_Context(int code)		// return to the parent context
		{
			if (code == 0)							// if code == 0
			{
				cout << endl;
				current_context = parent_context;	// return to the parent context
				current_context->show_Menu();
				return true;
			};
			return false;
		};

		Transitional_Context(const vector<const char*>& t) : User_Context(t) {};
	};

	/***********************************************************/
	/*                       Choice Context                   */
	/***********************************************************/

	vector<const char*> list = {};

	class Choice_Context : public Transitional_Context 
	{
	public:

		virtual void become_Current() override
		{
			table = list;								// choice list
			Transitional_Context::become_Current();		// become current
		};

		virtual void choose_Item(const char* s) = 0;

		virtual void operation(int code) override
		{

			if (cancel_Context(code)) return;
			if (code < table.size())  choose_Item(table[code]);
		};

		Choice_Context() : Transitional_Context(list) {};
	};

	/***********************************************************/
	/*                       Context Choice                    */
	/***********************************************************/

	void enter_Contact(const char* peer)
	{
		current_contact = peer;
		contact_prompt = current_user;
		contact_prompt += " | ";
		contact_prompt += current_contact;
		contact_context.become_Current();
	};

	/***********************************************************/

	class Contact_Choice : public Choice_Context 
	{
	public:

		virtual void choose_Item(const char* s) override { enter_Contact(s); };

		Contact_Choice() : Choice_Context() {};
	};

	static Contact_Choice contact_choice;

	/***********************************************************/
	/*                       Group Choice                      */
	/***********************************************************/

	void enter_Group(const char* group) 
	{
		current_group = group;
		group_prompt = current_user;
		group_prompt += " | ";
		group_prompt += current_group;
		group_context.become_Current();
	};

	/***********************************************************/

	class Group_Choice : public Choice_Context
	{
	public:

		virtual void choose_Item(const char* s) override { enter_Group(s); };

		Group_Choice() : Choice_Context() {};
	};
	
	Group_Choice group_choice;

	/***********************************************************/
	/*                      Control Context                    */
	/***********************************************************/

	class Control_Context : public Transitional_Context
	{
	public:

		virtual int get_List() = 0;					// get list

		virtual void open_Choice() = 0;				// open choice context

		virtual bool choice_Open(int code)			// if choice context open return true 
		{
			if (code != 1)return false;
			list = {CANCEL};
			if (get_List() != DONE)					// get list
			{										// if not done
				show_Menu();						// show menu
				return true; 
			};
			if (list.size() == 1)					// no list elements?
			{
				cout << endl;
				cout << " " << LISTISEMPTY << endl;		// empty list
				show_Menu();
				return true;
			}
			else
			{
				open_Choice();					// open choice context
				return true;
			};
			return true;
		};

		Control_Context(const vector<const char*>& t) : Transitional_Context(t) {};
	};

	/***********************************************************/
	/*                      Contacts Control                   */
	/***********************************************************/

	string current_peer = "";

	class Contact_Control : public Control_Context
	{
	public:
		virtual int get_List() override { return get_Contacts(list); };

		virtual void open_Choice() override { make_Current(&contact_choice); };

		virtual void new_Contact() 
		{
			string peer = "";
			cout << endl;
			show_Bar();
			cout << " " << PEERNAME << " : ";
			getline(cin, peer);
			int code = check_Contact(peer.c_str());
			if (code == DONE)
				enter_Contact(peer.c_str());
			else		
				show_Menu();
		};

		virtual void operation(int code) override
		{
			if(cancel_Context(code)) return;
			if(choice_Open(code)) return;
			if (code == 2) new_Contact();
		};

		Contact_Control() : Control_Context({ CANCEL, CHOOSECONTACT, NEWCONTACT }) {};
	};

	Contact_Control contact_control;

	void contact_Control() { make_Current(&contact_control); }

	/***********************************************************/
	/*                       Groups Control                    */
	/***********************************************************/

	class Group_Control : public Control_Context
	{
	public:
		virtual int get_List() override { return get_Groups(list); };

		virtual void open_Choice() override { make_Current(&group_choice); };

		void get_Name(string& group) 
		{
			cout << endl;
			show_Bar();
			cout << " " << GROUPNAME << " : ";
			getline(cin, group);
		};

		void join_User_Group()
		{
			string group;
			get_Name(group);
			int code = join_Group(group.c_str());
			if (code != DONE) {
				show_Menu();
				return;
			};
			enter_Group(group.c_str());
		};

		void create_User_Group() 
		{
			string group;
			get_Name(group);
			int code = create_Group(group.c_str());
			if (code != DONE) {
				show_Menu();
				return;
			};
			enter_Group(group.c_str());
		};

		virtual void operation(int code) override
		{
			if (cancel_Context(code)) return;
			if (choice_Open(code)) return;
			switch (code) 
			{
			case 2: join_User_Group();		return;
			case 3: create_User_Group();	return;
			};
		};

		Group_Control() : Control_Context({ CANCEL, CHOOSEGROUP, JOINGROUP, CREATEGROUP }) {};
	};

	Group_Control group_control;

	void group_Control() { make_Current(&group_control); };

	/***********************************************************/
	/*                       Main Context                      */
	/***********************************************************/

	class Main_Context : public User_Context
	{
	public:

		void change_Pass()
		{
			string oldpassword;
			string newpassword;

			cout << endl;
			cout << " " << OLDPASSWORD << ": ";
			getline(cin, oldpassword);
			cout << " " << NEWPASSWORD << ": ";
			getline(cin, newpassword);
			int code = change_Password(oldpassword.c_str(), newpassword.c_str());
			if (code == DONE) cout << PASSCHANGED << endl;
		};

		virtual void operation(int code) override
		{
			switch (code)
			{
			case 0: 
				close_Chat(); 
				exit(0);
			case 1: 
				contact_Control();
				return;
			case 2: 
				group_Control();
				return;
			case 3:
				change_Pass();
				show_Menu();
				return;
			case 4: 
				auth_Context();
				return;
			};
		};

		Main_Context() :
			User_Context({ EXIT, CONTACTS, GROUPS, CHANGEPASS, CHANGEUSER }) {};
	};

	Main_Context main_context;

	void main_Context() { make_Current(&main_context); };

	/***********************************************************/
	/*                 Authentication context                  */
	/***********************************************************/

	void get_User_Data(string* username, string* password)
	{
		cout << " " << USERNAME << " : ";
		getline(cin, *username);
		cout << " " << PASSWORD <<  ": ";
		getline(cin, *password);
	};

	/***********************************************************/

	void sign_In()
	{
		string username;
		string password;
		cout << endl;
		show_Bar();
		get_User_Data(&username, &password);
		int code = sign_In(username.c_str(), password.c_str());
		if (code != DONE) {
			current_context->show_Menu();
 			return;
		};
		current_user = username;
		main_context.become_Current();
	};

	/***********************************************************/

	void sign_Up() 
	{
		string username;
		string password;
		cout << endl;
		show_Bar();
		get_User_Data(&username, &password);
		int code = sign_Up(username.c_str(), password.c_str());
		if (code != DONE) {
			current_context->show_Menu();
			return;
		};
		current_user = username;
		main_context.become_Current();
	};

	/***********************************************************/

	class Auth_Context : public Context 
	{
	public:
		virtual void operation(int code) override
		{
			switch (code) 
			{
			case 0: exit(0);
			case 1: sign_In(); return;
			case 2: sign_Up(); return;
			};
		};

		Auth_Context() : Context({ EXIT, SIGN_IN, SIGN_UP }) {};
	};

	Auth_Context auth_context;

	void auth_Context() 
	{
		current_user = "";
		make_Current(&auth_context);
	};

	/***********************************************************/

	int main() 
	{
		setlocale(LC_ALL, "");
		system("chcp 1251");
 	
		if(open_Chat() != DONE)exit(0);

		auth_context.become_Current();
		char c; int code;
		while (true)	
		{
			c = _getch();
			code = c - 48;
			current_context->show_Choice(code);
			current_context->operation(code);
		};

		close_Chat();
		return 0;
	};

	/***********************************************************/



