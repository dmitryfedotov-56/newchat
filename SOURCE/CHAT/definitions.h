	#pragma once
	/***********************************************************/
	/*                       Message Structure                 */
	/***********************************************************/

	struct Message
	{
		const char* time = nullptr;
		const char* sender = nullptr;
		const char* recipient = nullptr;
		const char* text = nullptr;
	};

	/***********************************************************/
	/*
	list of codes
	*/
	/***********************************************************/

	#define DONE			 0
	#define USER_NOT_FOUND	-1
	#define EXISTING_USER	-2
	#define WRONG_PASSWORD	-3
	#define GROUP_NOT_FOUND	-4
	#define EXISTING_GROUP	-5
	#define DATABASE_ERROR	-6
	#define PROTOCOL_ERROR	-7
	#define CONFDATA_ERROR  -8
	#define EXCHANGE_ERROR	-9 
	#define UNKNOWN_ERROR	-10

	/***********************************************************/
