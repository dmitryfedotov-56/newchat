	#pragma once
	/***********************************************************/
/*
	message codes
*/
	#include "definitions.h"

	/***********************************************************/
/*
	request codes
*/
	#define SIGN_IN				1
	#define SIGN_UP				2
	#define CHANGE_PASSWORD		3
	#define FIND_USER			4
	#define CONTACT_LIST		5
	#define CONTACT_MESSAGES	6
	#define TO_CONTACT			7
	#define GROUP_LIST			8
	#define GROUP_MEMBERS		9
	#define JOIN_GROUP			10
	#define LEAVE_GROUP			11
	#define NEW_GROUP			12
	#define GROUP_MESSAGES		13
	#define TO_GROUP			14
	#define NEXT_MESSAGE		15
	#define UNKNOWN_REQUEST		16

	/***********************************************************/
/*
	reply codes
*/
	#define OK					0
	#define AUTHENTICATED		1
	#define PASSCHANGED			2
	#define MESSAGE				3
	#define END					4
	#define LINE_LIST			5
	#define UNKNOWN_REPLY		6

	/***********************************************************/

