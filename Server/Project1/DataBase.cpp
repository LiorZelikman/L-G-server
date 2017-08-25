#include "DataBase.h"

//exception type for stoi is: "invalid_argument inv_arg"

//Checks if all the databases can be opened.
DataBase::DataBase()
{
	char *zErrMsg = 0;
	int rc;
	sqlite3_free(zErrMsg);
	sqlite3* db;
	try{
		//open table
		rc = sqlite3_open(DB_PATH, &db);
		if (rc != SQLITE_OK)//if the command SQL not good :(
		{
			throw "main_db not open";
			return;
		}
		sqlite3_close(db);
		
		rc = sqlite3_open(COMMENTS_PATH, &db);
		if (rc != SQLITE_OK)//if the command SQL not good :(
		{
			throw "comments_on_posts_db not open";
			return;
		}
		sqlite3_close(db);

		rc = sqlite3_open(MESSAGGES_PATH, &db);
		if (rc != SQLITE_OK)//if the command SQL not good :(
		{
			throw "messages_of_users_db not open";
			return;
		}
		sqlite3_close(db);

		rc = sqlite3_open(POSTS_COMMENTED_PATH, &db);
		if (rc != SQLITE_OK)//if the command SQL not good :(
		{
			throw "posts_commented_db not open";
			return;
		}
		sqlite3_close(db);

		rc = sqlite3_open(POSTS_CREATED_PATH, &db);
		if (rc != SQLITE_OK)//if the command SQL not good :(
		{
			throw "posts_created_db not open";
			return;
		}
		sqlite3_close(db);

		rc = sqlite3_open(READ_LATER_POSTS, &db);
		if (rc != SQLITE_OK)//if the command SQL not good :(
		{
			throw "read_later_posts_db not open";
			return;
		}
		sqlite3_close(db);
	}
	catch (string ex)
	{
		cout << ex;
	}
}

DataBase::~DataBase()
{}

//returns true if the password of user matches to the password in the DB.
bool DataBase::isNameAndPassMatch(string user, string pass)
{
	//Creating the sqlite query
	unordered_map<string, vector<string>> my_results;
	string command = "select password from users where name=\"" + user + "\";";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(DB_PATH, command, true))
	{
		results_lock = false;
		return false;
	}
	my_results = results;
	results_lock = false;
	if (my_results.empty() == false)
	{
		if (my_results.find("password")->second.back() == pass)
		{
			return true;
		}
	}
	return false;
}

//return the ID of the user whose username is user. returns "0" upon failure.
string DataBase::getIDByName(string name)
{
	unordered_map<string, vector<string>> my_results;
	string id = "0";
	string command = "select ID from users where name=\"" + name + "\";";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(DB_PATH, command, true))
	{
		results_lock = false;
		return "0";
	}
	my_results = results;
	results_lock = false;
	if (my_results.empty() == false)
	{
		id = my_results.find("ID")->second.back();
	}
	return id;
}

//returns true if there's a user whose name is equal to the paremeter.
bool DataBase::isUserAlreadyExist(string name)
{
	unordered_map<string, vector<string>> my_results;
	//Creating the sqlite query
	string command = "select name from users";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(DB_PATH, command, true))
	{
		results_lock = false;
		return false;
	}
	my_results = results;
	results_lock = false;
	vector<string> usernames = my_results.find("name")->second;
	for (int i = 0; i < (signed)usernames.size(); i++)
	{
		if (usernames[i] == name)
			return true;
	}
	return false;
}

//returns true if there's a user whose email matches to the paremeter.
bool DataBase::isEmailAlreadyExist(string email)
{
	unordered_map<string, vector<string>> my_results;
	//Creating the sqlite query
	string command = "select email from users;";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(DB_PATH, command, true))
	{
		results_lock = false;
		return false;
	}
	my_results = results;
	results_lock = false;
	vector<string> emails = my_results.find("email")->second;
	for (int i = 0; i < (signed)emails.size(); i++)
	{
		if (emails[i] == email)
			return true;
	}
	return false;
}

//Returns the last item in the DB whose path, table and column matches the paremeter. Return "0" upon failure.
string DataBase::getLastItem(string column, string table, string path)
{
	unordered_map<string, vector<string>> my_results;
	string returnVal;
	string command = "select * from " + table + " order by " + column + " desc limit 1;";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(path, command, true))
	{
		results_lock = false;
		return "0";
	}
	my_results = results;
	results_lock = false;
	if (my_results.empty() == false)
	{
		returnVal = my_results.find(column)->second.back();
	}
	else
		returnVal = "0";
	return returnVal;
}

//Inserts a user into the DB. Returns true upon succes and false upon failure.
bool DataBase::insertNewUser(string name, string password, string email)
{
	string id = to_string(stoi(getLastItem("ID", "users", DB_PATH)) + 1);
	if (id == "0")
		return false;
	string command = "insert into users(ID, name, password, email, date_of_birth, status, hobbies, occupation, interests) values(" + id + ",\"" + name + "\",\"" + password + "\",\"" + email + "\",\"0\",\"00\",\"00\",\"00\",\"00\");";
	if (!sqliteQuery(DB_PATH, command, false))
		return false;

	command = "create table messages_of_user_" + id + "(ID INTEGER, type TEXT, message TEXT, ID_of_reciever INTEGER, ID_of_sender INTEGER, pic_link TEXT, time INTEGER);";
	if (!sqliteQuery(MESSAGGES_PATH, command, false))
		return false;

	command = "create table votes_of_" + id + "(up_or_down TEXT, post_ID INTEGER, comment_num INTEGER);";
	if (!sqliteQuery(VOTES_PATH, command, false))
		return false;
	
	command = "create table commented_posts_of_" + id + "(ID INTEGER PRIMARY KEY AUTOINCREMENT, post_ID INTEGER, num_of_comments INTEGER);";
	if (!sqliteQuery(POSTS_COMMENTED_PATH, command, false))
		return false;

	command = "create table created_posts_of_" + id + "(ID INTEGER PRIMARY KEY AUTOINCREMENT, post_ID INTEGER);";
	if (!sqliteQuery(POSTS_CREATED_PATH, command, false))
		return false;

	command = "create table read_later_posts_of_" + id + "(ID INTEGER PRIMARY KEY AUTOINCREMENT, post_ID INTEGER);";
	if (!sqliteQuery(READ_LATER_POSTS, command, false))
		return false;

	command = "create table blocks_of_" + id + "(ID INTEGER PRIMARY KEY AUTOINCREMENT, ID_of_blocked_user TEXT);";
	if (!sqliteQuery(BLOCKS, command, false))
		return false;
	
	return true;
}

//returns true if the email of user matches to the email in the DB.
bool DataBase::IsNameAndEmailMatch(string name, string email)
{
	unordered_map<string, vector<string>> my_results;
	//Creating the sqlite query
	string command = "select email from users where name=\"" + name + "\";";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(DB_PATH, command, true))
	{
		results_lock = false;
		return false;
	}
	my_results = results;
	results_lock = false;
	if (my_results.find("email")->second.back() == email)
		return true;
	else
		return false;
}

//Updates a password of a user. Returns true upon success.
bool DataBase::updatePassword(string name, string new_password)
{
	string command = "update users set password = \"" + new_password + "\" " + "where name = \"" + name + "\"";
	if(!sqliteQuery(DB_PATH, command, false))
		return false;
	return true;
	
}

//Inserts a new message into the DB. action_type is either "sender", "reciever", or "both". returns true upon success and false upon failue.
bool DataBase::insertNewMessage(string message, string type, string reciever_id, string sender_id, string pic_link, string creation_time, string action_type)//action type is either "sender", "reciever", or "both".
{
	if (action_type == "both")
	{
		string idForReciever = to_string(stoi(getLastItem("ID", "messages_of_user_" + reciever_id, MESSAGGES_PATH)) + 1);
		string idForSender = to_string(stoi(getLastItem("ID", "messages_of_user_" + sender_id, MESSAGGES_PATH)) + 1);
		if (idForReciever == "0" || idForSender == "0")
			return false;
		//Creating the sqlite query
		string command;
		command = "insert into messages_of_user_" + reciever_id + "(ID, type, message, ID_of_reciever, ID_of_sender, pic_link, time) values(" + idForReciever + ",\"" + type + "\",\"" + message + "\"," + reciever_id + "," + sender_id + ",\"" + pic_link + "\"," + creation_time + ");";
		if (!sqliteQuery(MESSAGGES_PATH, command, false))
			return false;
		command = "insert into messages_of_user_" + sender_id + "(ID, type, message, ID_of_reciever, ID_of_sender, pic_link, time) values(" + idForSender + ",\"" + type + "\",\"" + message + "\"," + reciever_id + "," + sender_id + ",\"" + pic_link + "\"," + creation_time + ");";
		if (!sqliteQuery(MESSAGGES_PATH, command, false))
			return false;
	}
	else if (action_type == "sender")
	{
		string idForSender = to_string(stoi(getLastItem("ID", "messages_of_user_" + sender_id, MESSAGGES_PATH)) + 1);
		if (idForSender == "0")
			return false;
		string command = "insert into messages_of_user_" + sender_id + "(ID, type, message, ID_of_reciever, ID_of_sender, pic_link, time) values(" + idForSender + ",\"" + type + "\",\"" + message + "\"," + reciever_id + "," + sender_id + ",\"" + pic_link + "\"," + creation_time + ");";
		if (!sqliteQuery(MESSAGGES_PATH, command, false))
			return false;
	}
	else if (action_type == "reciever")
	{
		string idForReciever = to_string(stoi(getLastItem("ID", "messages_of_user_" + reciever_id, MESSAGGES_PATH)) + 1);
		if (idForReciever == "0")
			return false;
		string command = "insert into messages_of_user_" + reciever_id + "(ID, type, message, ID_of_reciever, ID_of_sender, pic_link, time) values(" + idForReciever + ",\"" + type + "\",\"" + message + "\"," + reciever_id + "," + sender_id + ",\"" + pic_link + "\"," + creation_time + ");";
		if (!sqliteQuery(MESSAGGES_PATH, command, false))
			return false;
	}
	else
	{
		cout << "DataBase::insertNewMessage - that's wierd" << endl; //not supposed to happen.
		return false;
	}
	return true;
}

//Inserts a post into the DB. Returns true upon success, false upon failure.
bool DataBase::insertNewPost(string username, string content, string categoryID, string userID, string min_age, string max_age, string hobbies, string occupation, string interests)
{
	string id_str = to_string(stoi(getLastItem("ID", "posts", DB_PATH)) + 1);
	if (id_str == "0")
		return false;
	string help;
	if (categoryID.length() < 2)
	{
		help = categoryID;
		categoryID = "0" + help;
	}
	string time_now_ms_str = to_string(time_now());
	//Creating the sqlite query
	string command = "insert into posts(ID, num_of_comments, username, content, category, time, min_age, max_age, hobbies, occupation, interests) values(" + id_str + "," + "0" + ", \"" + username + "\",\"" + content + "\",\"" + categoryID + "\"," + time_now_ms_str + "," + min_age + "," + max_age + ",\"" + hobbies + "\",\"" + occupation + "\",\"" + interests + "\");";
	if (!sqliteQuery(DB_PATH, command, false))
		return false;
	
	//Now need to create a table for its comments.
	command = "create table comments_of_post_" + id_str + " (comment_num INTEGER, content TEXT, username TEXT, upvotes INTEGER, downvotes INTEGER, time INTEGER);";
	if (!sqliteQuery(COMMENTS_PATH, command, false))
		return false;
	command = "insert into created_posts_of_" + userID + "(post_ID) values(" + id_str + ");";
	if (!sqliteQuery(POSTS_CREATED_PATH, command, false))
		return false;
	return true;
}

//Inserts a new comment to a post whose ID is post_id. Returns the comment number or "0" upon failure.
string DataBase::insertNewComment(string post_id, string content, string username, string userID)
{
	unordered_map<string, vector<string>> my_results;
	string command = "select num_of_comments from posts where ID = " + post_id + ";";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(DB_PATH, command, true))
	{
		results_lock = false;
		return "0";
	}
	my_results = results;
	results_lock = false;
	string comment_num = to_string(stoi(my_results.find("num_of_comments")->second.back()) + 1);
	comment_num = comment_num == "-1" ? "0" : comment_num;
	string creation_time = to_string(time_now());
	command = "insert into comments_of_post_" + post_id + "(comment_num, content, username, upvotes, downvotes, time) values(" + comment_num + ",\"" + content + "\",\"" + username + "\"," + "0," + "0,"  + creation_time + ");";
	if (!sqliteQuery(COMMENTS_PATH, command, false))
		return "0";

	command = "update posts set num_of_comments=" + comment_num + " where ID=" + post_id + ";";
	if (!sqliteQuery(DB_PATH, command, false))
		return "0";
	
	command = "select num_of_comments from commented_posts_of_" + userID + " where post_ID=" + post_id + ";";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(POSTS_COMMENTED_PATH, command, true))
	{
		results_lock = false;
		return "0";
	}
	my_results = results;
	results_lock = false;
	try
	{
		if (my_results.empty() == true)
		{
			command = "insert into commented_posts_of_" + userID + "(post_ID, num_of_comments) values(" + post_id + ",1" + ");";
			if (!sqliteQuery(POSTS_COMMENTED_PATH, command, false))
				return false;
		}
		else
		{
			int num_of_comments = stoi(my_results.find("num_of_comments")->second.back());
			string num_of_comments_str = to_string(num_of_comments + 1);
			command = "update commented_posts_of_" + userID + " set num_of_comments=" + num_of_comments_str + " where post_ID = " + post_id + ";";
			if (!sqliteQuery(POSTS_COMMENTED_PATH, command, false))
				return false;
		}
		pair<string, string> comment;
		comment.first = creation_time;
		comment.second = padd_zeroes(post_id, 8);
		comments.insert(comment);
		comment_num = padd_zeroes(comment_num, 8);
		return comment_num;
	}
	catch (...)
	{
		return "0";
	}
}

//returns a lot of information about a post with ID = post_id, or "0" upon failure.
string DataBase::postInfo(string post_id, string userID)
{
	unordered_map<string, vector<string>> my_post_results;
	unordered_map<string, vector<string>> my_comments_results;
	unordered_map<string, vector<string>> my_users_posts_info_results;
	unordered_map<string, vector<string>> votes_results;
	string return_data, num_of_comments_str, content, content_len_str, categoryID, post_creator_name, post_creator_name_len, post_writing_time, passed_time_for_post;
	int i;
	string command = "select content, num_of_comments, category, username, time from posts where ID = " + post_id + ";";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(DB_PATH, command, true))
	{
		results_lock = false;
		return "0";
	}
	my_post_results = results;
	results_lock = false;
	if (my_post_results.empty())
		return "0";
	try
	{
		num_of_comments_str = to_string(stoi(my_post_results.find("num_of_comments")->second.back()));
		content = my_post_results.find("content")->second.back();
		content_len_str = to_string(content.length());
		content_len_str = padd_zeroes(content_len_str, 3);
		categoryID = my_post_results.find("category")->second.back();
		categoryID = padd_zeroes(categoryID, 2);
		post_creator_name = my_post_results.find("username")->second.back();
		post_creator_name_len = to_string(post_creator_name.length());
		post_creator_name_len = padd_zeroes(post_creator_name_len, 2);
		post_writing_time = my_post_results.find("time")->second.back();
		passed_time_for_post = to_string(time_now() - stoll(post_writing_time));
		passed_time_for_post = padd_zeroes(passed_time_for_post, 13);
	}
	catch (...)
	{
		return "0";
	}
	command = "select post_ID from read_later_posts_of_" + userID + ";";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(READ_LATER_POSTS, command, true))
	{
		results_lock = false;
		return "0";
	}
	my_users_posts_info_results = results;
	results_lock = false;
	string found = "1"; // 0 - marked, 1 - not marked.
	if (my_users_posts_info_results.empty() == false)
	{
		for (i = 0; i < (signed)my_users_posts_info_results.find("post_ID")->second.size(); i++)
		{
			try
			{
				if (stoi(my_users_posts_info_results.find("post_ID")->second[i]) == stoi(post_id))
				{
					found = "0";
					break;
				}
			}
			catch (...)
			{}
		}
	}
	//return_data = "0" + content_len_str + content + post_creator_name_len + post_creator_name + passed_time_for_post + categoryID + found + num_of_comments_str;
	
	command = "select content, username, upvotes, downvotes, time from comments_of_post_" + to_string(stoi(post_id)) + " where comment_num > 0 and comment_num <= " + (stoi(num_of_comments_str) < 25 ? num_of_comments_str : "25") + ";";
	//command = "select content, username, upvotes, downvotes, time from comments_of_post_one where comment_num > 0 and comment_num < " + (stoi(num_of_comments_str) < 25 ? num_of_comments_str : "25") + ";";
	//command = "SELECT * FROM sqlite_master;";
	while (results_lock);
	results_lock = true;
	sqliteQuery(COMMENTS_PATH, command, true);
	my_comments_results = results;
	results_lock = false;
	string help_str = "";
	int num_of_comments = stoi(num_of_comments_str);
	for (i = 0; i < stoi(num_of_comments_str) && i < 25; i++)
	{
		try
		{
			string i_str = to_string(i + 1);
			string comment_num = padd_zeroes(i_str, 8);
			string comment_content = my_comments_results.find("content")->second[i];
			string comment_length_str = to_string(comment_content.length());
			comment_length_str = padd_zeroes(comment_length_str, 3);
			string writer_name = my_comments_results.find("username")->second[i];
			string writer_name_length_str = to_string(writer_name.length());
			writer_name_length_str = padd_zeroes(writer_name_length_str, 2);
			string upvotes = my_comments_results.find("upvotes")->second[i];
			upvotes = padd_zeroes(upvotes, 7);
			string downvotes = my_comments_results.find("downvotes")->second[i];
			downvotes = padd_zeroes(downvotes, 7);
			string comment_creation_time = my_comments_results.find("time")->second[i];
			string passed_time = to_string(time_now() - stoll(comment_creation_time));
			passed_time = padd_zeroes(passed_time, 13);
			command = "select up_or_down from votes_of_" + userID + " where post_ID=" + post_id + " and comment_num=" + to_string(i + 1);
			while (results_lock);
			results_lock = true;
			if(!sqliteQuery(VOTES_PATH, command, true))
			{
				results_lock = false;
				num_of_comments--;
				continue;
			}
			votes_results = results;
			results_lock = false;
			string personalVote;
			if (votes_results.empty() == true)
				personalVote = "0";
			else if (votes_results.find("up_or_down")->second.back() == "up")
				personalVote = "1";
			else if (votes_results.find("up_or_down")->second.back() == "down")
				personalVote = "2";
			help_str += comment_num + writer_name_length_str + writer_name + passed_time + comment_length_str + comment_content + upvotes + downvotes + personalVote;
		}
		catch (...)
		{
			num_of_comments--;
		}
	}
	if (num_of_comments > 25)
		num_of_comments = 25;
	if (num_of_comments == 0)
		help_str = "";
	num_of_comments_str = padd_zeroes(to_string(num_of_comments), 2);
	return_data = "0" + content_len_str + content + post_creator_name_len + post_creator_name + passed_time_for_post + categoryID + found + num_of_comments_str + help_str;
	return return_data;
}

//returns comments on a post with ID = post_id, or "1" upon failure.
string DataBase::updateCommentsOnPost(string post_id, int numOfCurrComments, string userID)
{
	unordered_map<string, vector<string>> my_results;
	unordered_map<string, vector<string>> votes_results;
	string returnStr = "", help_str = "";
	string command = "select * from comments_of_post_" + to_string(stoi(post_id)) + " where comment_num > " + to_string(numOfCurrComments) + " and comment_num < " + to_string(numOfCurrComments + 25);
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(COMMENTS_PATH, command, true))
	{
		results_lock = false;
		return "1";
	}
	my_results = results;
	results_lock = false;
	if (my_results.empty())
		return "000";
	else
		returnStr += "0";
	int numOfComments = my_results.find("comment_num")->second.size(); //I could use any filed, not just comment_num.
	string numOfComentsStr = to_string(numOfComments);
	int i;
	for (i = 0; i < stoi(numOfComentsStr); i++)
	{
		try
		{
			string comment_num = my_results.find("comment_num")->second[i];
			string writerName = my_results.find("username")->second[i];
			string time_since_creation = padd_zeroes(to_string(time_now() - stoll(my_results.find("time")->second[i])), 13);
			string content = my_results.find("content")->second[i];
			string upvotes = padd_zeroes(my_results.find("upvotes")->second[i], 7);
			string downvotes = padd_zeroes(my_results.find("downvotes")->second[i], 7);
			command = "select up_or_down from votes_of_" + userID + " where post_ID=" + post_id + " and comment_num=" + comment_num;
			while (results_lock);
			results_lock = true;
			if (!sqliteQuery(VOTES_PATH, command, true))
			{
				results_lock = false;
				numOfComments--;
				continue;
			}
			votes_results = results;
			results_lock = false;
			string personalVote;
			if (votes_results.empty() == true)
				personalVote = "0";
			else if (votes_results.find("up_or_down")->second.back() == "up")
				personalVote = "1";
			else if (votes_results.find("up_or_down")->second.back() == "down")
				personalVote = "2";
			help_str += padd_zeroes(comment_num, 8) + padd_zeroes(to_string(writerName.length()), 2) + writerName + time_since_creation + padd_zeroes(to_string(content.length()), 3) + content + upvotes + downvotes + personalVote;
		}
		catch (...)
		{
			numOfComments--;
		}
	}
	if (numOfComments == 0)
		return "1";
	returnStr += padd_zeroes(to_string(numOfComments), 2) + help_str;
	return returnStr;
}

//returns whether the user is allowed to view the post according to his profile.
bool DataBase::checkRequirementsMatch(string post_id, string user_id)
{
	unordered_map<string, vector<string>> my_results;
	string command = "select name, date_of_birth, hobbies, occupation, interests from users where ID=" + user_id + ";";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(DB_PATH, command, true))
		return false;
	my_results = results;
	results_lock = false;
	string username, creatorName;
	string hobbies = "00", occupation = "00", interests = "00";
	string required_hobbies = "00", required_occupation = "00", required_interests = "00";
	int age = -1, required_minAge = 0, required_maxAge = 0;
	if (my_results.empty())
		return "000";
	if (my_results.find("name") == my_results.end())
		return false;
	username = my_results.find("name")->second.back();
	if (my_results.find("date_of_birth") != my_results.end())
		age = calcAge(my_results.find("date_of_birth")->second.back());
	if (my_results.find("hobbies") != my_results.end())
		hobbies = my_results.find("hobbies")->second.back();
	if (my_results.find("occupation") != my_results.end())
		occupation = my_results.find("occupation")->second.back();
	if (my_results.find("inetersts") != my_results.end())
		interests = my_results.find("interests")->second.back();
	command = "select username, min_age, max_age, hobbies, occupation, interests from posts where ID=" + post_id + ";";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(DB_PATH, command, true))
		return false;
	my_results = results;
	results_lock = false;
	if (my_results.find("username") == my_results.end())
		return false;
	creatorName = my_results.find("username")->second.back();
	if (my_results.find("interests") != my_results.end())
		required_interests = my_results.find("interests")->second.back();
	if (my_results.find("occupation") != my_results.end())
		required_occupation = my_results.find("occupation")->second.back();
	if (my_results.find("hobbies") != my_results.end())
		required_hobbies = my_results.find("hobbies")->second.back();
	if (my_results.find("max_age") != my_results.end())
		required_maxAge = stoi(my_results.find("max_age")->second.back());
	if (my_results.find("min_age") != my_results.end())
		required_minAge = stoi(my_results.find("min_age")->second.back());
	
	if (((required_interests == interests || required_interests == "00") && (required_occupation == occupation || required_occupation == "00") && (required_hobbies == hobbies || required_hobbies == "00") && ((age > required_minAge && age < required_maxAge) || (required_minAge == 0 && required_maxAge == 0))) || username == creatorName) //checking if the user has all the requirements.
		true;
	else
		return false;
	
}

//returns some information about a post with ID = post_id, or an empty string upon failure.
string DataBase::shortenedPostInfo(string post_id)
{
	unordered_map<string, vector<string>> my_post_results;
	string return_data;
	string command = "select content, username, category, time from posts where ID = " + post_id + ";";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(DB_PATH, command, true))
	{
		results_lock = false;
		return "";
	}
	my_post_results = results;
	results_lock = false;
	if (my_post_results.empty())
	{
		return ""; //not supposed to happen
	}
	try
	{
		string content = my_post_results.find("content")->second.back();
		string content_len_str = to_string(content.length());
		content_len_str = padd_zeroes(content_len_str, 3);
		string post_creator_name = my_post_results.find("username")->second.back();
		string post_creator_name_len = to_string(post_creator_name.length());
		post_creator_name_len = padd_zeroes(post_creator_name_len, 2);
		string category = my_post_results.find("category")->second.back();
		category = padd_zeroes(category, 2);
		string post_writing_time = my_post_results.find("time")->second.back();
		string passed_time_for_post = to_string(time_now() - stoll(post_writing_time));
		passed_time_for_post = padd_zeroes(passed_time_for_post, 13);
		return_data = content_len_str + content + post_creator_name_len + post_creator_name + category + passed_time_for_post;
	}
	catch (...)
	{
		return "";
	}
	return return_data;
}

//returns some information about a profile with ID = userID, or an empty string upon failure.
string DataBase::shortenedProfileInfo(string userID)
{
	unordered_map<string, vector<string>> my_results;
	string return_data;
	try
	{
		string command = "select name from users where ID = " + to_string(stoi(userID)) + ";";
		while (results_lock);
		results_lock = true;
		if (!sqliteQuery(DB_PATH, command, true))
		{
			results_lock = false;
			return "";
		}
		my_results = results;
		results_lock = false;
		if (my_results.empty())
		{
			return ""; //not supposed to happen
		}
		string username = my_results.find("name")->second.back();
		string usernameLen = padd_zeroes(to_string(username.length()), 2);
		ifstream pic;
		pic.open((PICS_LINK + string("profile_images\\profile_pic_of_") + to_string(stoi(userID)) + string(".jpg")).c_str());
		if (pic.is_open()) //if there is a picture.
		{
			string pic_link = PICS_LINK + string("profile_images\\profile_pic_of_") + to_string(stoi(userID)) + string(".jpg");
			string pic_link_len = padd_zeroes(to_string(pic_link.length()), 2);
			pic.close();
			return_data = pic_link_len + pic_link;
		}
		else
		{
			return_data = "0";
		}
		return_data += usernameLen + username;
		return return_data;
	}
	catch (...)
	{
		return "";
	}
}

//deletes the comments that were written more than 1 hour ago from the comments list.
void DataBase::updateComments()
{
	int i, numOfCommentsToDel, millis_in_an_hour = 60*60*1000;
	vector<unordered_map<string, string>::iterator> indexes_to_delete;
	unordered_map<string, string>::iterator it;
	long long currTime = time_now();
	while (comments_list_lock);
	comments_list_lock = true;
	try
	{
		for (it = comments.begin(); it != comments.end(); it++)
		{
			if (currTime - stoi(it->first) > millis_in_an_hour)
				indexes_to_delete.push_back(it);
		}
		numOfCommentsToDel = indexes_to_delete.size();
		for (i = 0; i < numOfCommentsToDel; i++)
		{
			comments.erase(indexes_to_delete[i]);
		}
	}
	catch (...)
	{}
	comments_list_lock = false;
}

//updates the list of newest posts.
void DataBase::updateNewestPosts()
{
	unordered_map<string, vector<string>> my_results, votes_results;
	string command = "select ID, num_of_comments, content, username, time, category, min_age, max_age, hobbies, occupation, interests from posts;";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(DB_PATH, command, true))
	{
		results_lock = false;
		return;
	}
	my_results = results;
	results_lock = false;
	int i, num_of_posts = stoi(getLastItem("ID", "posts", DB_PATH));
	if (num_of_posts == 0)
		return;
	vector<pair<string, string>> posts_list;
	pair<string, string> help_pair;
	try
	{
		if (posts_list.size() < num_of_posts)
		{
			for (i = posts_list.size(); i < num_of_posts; i++)
				posts_list.push_back(make_pair("0", "0"));
		}
		for (i = num_of_posts - 1; i >= 0; i--)
		{
			string ID = my_results.find("ID")->second[i];
			ID = padd_zeroes(ID, 8);
			string num_of_comments = my_results.find("num_of_comments")->second[i];
			num_of_comments = padd_zeroes(num_of_comments, 7);
			string content = my_results.find("content")->second[i];
			string content_len = to_string(content.length());
			content_len = padd_zeroes(content_len, 3);
			string writer_name = my_results.find("username")->second[i];
			string writer_name_len = to_string(writer_name.length());
			writer_name_len = padd_zeroes(writer_name_len, 2);
			string creation_time = my_results.find("time")->second[i];
			string passed_time = to_string(time_now() - stoll(creation_time));
			passed_time = padd_zeroes(passed_time, 13);
			string min_age = my_results.find("min_age")->second[i];
			min_age = padd_zeroes(min_age, 2);
			string max_age = my_results.find("max_age")->second[i];
			max_age = padd_zeroes(max_age, 2);
			string hobbies = my_results.find("hobbies")->second[i];
			hobbies = padd_zeroes(hobbies, 2);
			string occupation = my_results.find("occupation")->second[i];
			occupation = padd_zeroes(occupation, 2);
			string interests = my_results.find("interests")->second[i];
			interests = padd_zeroes(interests, 2);
			help_pair.first = ID + num_of_comments + content_len + content + writer_name_len + writer_name + passed_time + min_age + max_age + hobbies + occupation + interests;
			help_pair.second = my_results.find("category")->second[i];
			posts_list[num_of_posts - 1 - i] = help_pair;
		}
		while (newest_posts_list_lock);
		newest_posts_list_lock = true;
		newest_posts_list = posts_list;
		newest_posts_list_lock = false;
	}
	catch (...)
	{
		return;
	}
}

//updates the list of hottest posts.
void DataBase::updateHottestPosts()
{
	while (hottest_posts_list_lock);
	hottest_posts_list_lock = true;
	hottest_posts_list.clear();
	hottest_posts_list_lock = false;
	if (comments.size() == 0)
	{
		return;
	}
	int i, help;
	unordered_map<string, vector<string>> my_results, votes_results;
	unordered_map <string, int> help_map; //<post id, num_of_appearences>
	unordered_map<string, string>::iterator it;
	unordered_map<string, int>::iterator help_it;
	vector<string> post_ids;
	unordered_map<string, vector<string>>::iterator helpIt;
	unordered_map<string, string> my_comments = comments;
	try
	{
		//This loop enters into help_map every post that was commented in the last hour and the number of comments it had in the last hour into help_map.
		for (it = my_comments.begin(); it != my_comments.end(); it++)
		{
			help_it = help_map.find(it->second);
			if (help_it != help_map.end()) // if found.
				help_it->second++;
			else
			{
				pair <string, int> help_pair;
				help_pair.first = it->second;
				help_pair.second = 1;
				help_map.insert(help_pair);
			}
		}
		//this loop enters the values in help_map into post_ids in such an order that the first one is the one with the most appearences and the last is the one with the least appearences.
		while (help_map.empty() == false)
		{
			pair<string, int> maxVal = findMax(help_map);
			//if (helpFindstrInVector(existing_posts_ids, maxVal.first) == -1) //if it is equal to -1 it means isn't there
			post_ids.push_back(maxVal.first);
			help_it = help_map.find(maxVal.first);
			help_map.erase(help_it);
		}
		if (post_ids.size() != 0)
		{
			//the next line and the loop create the SQL query.
			string command = "select ID, content, num_of_comments, category, username, time, hobbies, occupation, interests, min_age, max_age from posts where ";
			for (i = 0; i < (signed)post_ids.size(); i++)
			{
				command += "ID=" + post_ids[i];
				if (i != post_ids.size() - 1)
					command += " or ";
				else
					command += ";";
			}

			while (results_lock);
			results_lock = true;
			if (!sqliteQuery(DB_PATH, command, true))
			{
				results_lock = false;
				return;
			}
			my_results = results;
			results_lock = false;
			string num_of_posts;
			if (my_results.empty() == true)
				num_of_posts = "0";
			else
				num_of_posts = to_string(my_results.find("ID")->second.size()); //I could choose any field, not only ID.
			num_of_posts = padd_zeroes(num_of_posts, 2);
			string help_msg = "";
			vector<int> post_ids_int = strVec_to_intVec(post_ids);
			pair<string, string> help_pair;
			int num_of_posts_sent = 0;
			//this loop gets the information about posts and inserts it into hottest_posts_list.
			for (i = 0; i < stoi(num_of_posts); i++)
			{
				help_msg = "";
				//this line helps match the index of a post in my_results to the index of a post in post_ids_int.
				int index = helpFindintInVector(my_results.find("ID")->second, post_ids_int[i]);
				if (index != -1)
				{
					num_of_posts_sent++;
					string ID = to_string(post_ids_int[i]);
					ID = padd_zeroes(ID, 8);
					string num_of_comments = my_results.find("num_of_comments")->second[index];
					num_of_comments = padd_zeroes(num_of_comments, 7);
					string content = my_results.find("content")->second[index];
					string content_len = to_string(content.length());
					content_len = padd_zeroes(content_len, 3);
					string writer_name = my_results.find("username")->second[index];
					string writer_name_len = to_string(writer_name.length());
					writer_name_len = padd_zeroes(writer_name_len, 2);
					string creation_time = my_results.find("time")->second[index];
					string passed_time = to_string(time_now() - stoll(creation_time));
					passed_time = padd_zeroes(passed_time, 13);
					string min_age = my_results.find("min_age")->second[index];
					min_age = padd_zeroes(min_age, 2);
					string max_age = my_results.find("max_age")->second[index];
					max_age = padd_zeroes(max_age, 2);
					string hobbies = my_results.find("hobbies")->second[index];
					hobbies = padd_zeroes(hobbies, 2);
					string occupation = my_results.find("occupation")->second[index];
					occupation = padd_zeroes(occupation, 2);
					string interests = my_results.find("interests")->second[index];
					interests = padd_zeroes(interests, 2);
					help_msg += ID + num_of_comments + content_len + content + writer_name_len + writer_name + passed_time + min_age + max_age + hobbies + occupation + interests;
					help_pair.first = help_msg;
					help_pair.second = my_results.find("category")->second[index];
					while (hottest_posts_list_lock);
					hottest_posts_list_lock = true;
					hottest_posts_list.push_back(help_pair);
					hottest_posts_list_lock = false;
				}
			}
		}
	}
	catch (...)
	{

		return;
	}
}

//returns the newest posts.
string DataBase::getNewestPosts(string category, string num_of_current_posts, string userID)
{
	while (newest_posts_list_lock);
	newest_posts_list_lock = true;
	vector<pair<string, string>> the_list = newest_posts_list;
	newest_posts_list_lock = false;
	if (the_list.empty())
		return "000";
	unordered_map<string, vector<string>> my_results;
	bool finished = false;
	int num_of_posts = 0;
	int i, count = 0;
	string msg = "0";
	string help_str = "";
	string command = "select date_of_birth, hobbies, occupation, interests from users where ID=" + userID + ";";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(DB_PATH, command, true))
		return "000";
	my_results = results;
	results_lock = false;
	string hobbies = "00", occupation = "00", interests = "00";
	string required_hobbies, required_occupation, required_interests;
	int age = -1, required_minAge, required_maxAge;
	if (my_results.empty())
		return "000";
	if (my_results.find("date_of_birth") != my_results.end())
		age = calcAge(my_results.find("date_of_birth")->second.back());
	if (my_results.find("hobbies") != my_results.end())
		hobbies = my_results.find("hobbies")->second.back();
	if (my_results.find("occupation") != my_results.end())
		occupation = my_results.find("occupation")->second.back();
	if (my_results.find("inetersts") != my_results.end())
		interests = my_results.find("interests")->second.back();
	command = " select post_ID from created_posts_of_" + userID + ";";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(POSTS_CREATED_PATH, command, true))
		return "000";
	my_results = results;
	results_lock = false;
	vector<string> users_created_posts = my_results.find("post_ID")->second;
	try
	{
		for (i = 0; num_of_posts < 12 && i < the_list.size(); i++)
		{
			int size = the_list[i].first.size();
			required_interests = the_list[i].first.substr(size - 2, 2);
			required_occupation = the_list[i].first.substr(size - 2 - 2, 2);
			required_hobbies = the_list[i].first.substr(size - 2 - 2 - 2, 2);
			required_maxAge = stoi(the_list[i].first.substr(size - 2 - 2 - 2 - 2, 2));
			required_minAge = stoi(the_list[i].first.substr(size - 2 - 2 - 2 - 2 - 2, 2));
			if (category == "00")
			{
				if (((required_interests == interests || required_interests == "00") && (required_occupation == occupation || required_occupation == "00") && (required_hobbies == hobbies || required_hobbies == "00") && ((age > required_minAge && age < required_maxAge) || (required_minAge == 0 && required_maxAge == 0))) || (helpFindstrInVector(users_created_posts, to_string(stoi(the_list[i].first.substr(0, 8)))) == -1 ? false : true))
				{
					if (count == stoi(num_of_current_posts))
					{
						help_str += the_list[i].first.substr(0, size - 10) + the_list[i].second;
						num_of_posts++;
					}
					else
						count++;
				}
			}
			else
			{
				if (((required_interests == interests || required_interests == "00") && (required_occupation == occupation || required_occupation == "00") && (required_hobbies == hobbies || required_hobbies == "00") && ((age > required_minAge && age < required_maxAge) || (required_minAge == 0 && required_maxAge == 0))) || (helpFindstrInVector(users_created_posts, to_string(stoi(the_list[i].first.substr(0, 8)))) == -1 ? false : true))
				{
					if (the_list[i].second == category)
					{
						if (count == stoi(num_of_current_posts))
						{
							help_str += the_list[i].first.substr(0, size - 10) + the_list[i].second;
							num_of_posts++;
						}
						else
							count++;
					}
				}
			}

		}
		msg += padd_zeroes(to_string(num_of_posts), 2);
		msg += help_str;
		return msg;
	}
	catch (...)
	{
		return "";
	}
}

//returns the hottest posts.
string DataBase::getHottestPosts(string category, string num_of_current_posts, string userID)
{
	while (hottest_posts_list_lock);
	hottest_posts_list_lock = true;
	vector<pair<string, string>> the_list = hottest_posts_list;
	hottest_posts_list_lock = false;
	if (the_list.empty())
		return "000";
	unordered_map<string, vector<string>> my_results;
	bool finished = false;
	int num_of_posts = 0;
	int i, count = 0;
	string msg = "0";
	string help_str = "";
	string command = "select date_of_birth, hobbies, occupation, interests from users where ID=" + userID + ";";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(DB_PATH, command, true))
		return "000";
	my_results = results;
	results_lock = false;
	string hobbies = "00", occupation = "00", interests = "00";
	string required_hobbies, required_occupation, required_interests;
	int age = -1, required_minAge, required_maxAge;
	if (my_results.empty())
		return "000";
	if (my_results.find("date_of_birth") != my_results.end())
		age = calcAge(my_results.find("date_of_birth")->second.back());
	if (my_results.find("hobbies") != my_results.end())
		hobbies = my_results.find("hobbies")->second.back();
	if (my_results.find("occupation") != my_results.end())
		occupation = my_results.find("occupation")->second.back();
	if (my_results.find("inetersts") != my_results.end())
		interests = my_results.find("interests")->second.back();
	command = " select post_ID from created_posts_of_" + userID + ";";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(POSTS_CREATED_PATH, command, true))
		return "000";
	my_results = results;
	results_lock = false;
	vector<string> users_created_posts = my_results.find("post_ID")->second;
	try
	{
		for (i = 0; num_of_posts < 12 && i < the_list.size(); i++)
		{
			int size = the_list[i].first.size();
			required_interests = the_list[i].first.substr(size - 2, 2);
			required_occupation = the_list[i].first.substr(size - 2 - 2, 2);
			required_hobbies = the_list[i].first.substr(size - 2 - 2 - 2, 2);
			required_maxAge = stoi(the_list[i].first.substr(size - 2 - 2 - 2 - 2, 2));
			required_minAge = stoi(the_list[i].first.substr(size - 2 - 2 - 2 - 2 - 2, 2));
			if (category == "00")
			{
				if (((required_interests == interests || required_interests == "00") && (required_occupation == occupation || required_occupation == "00") && (required_hobbies == hobbies || required_hobbies == "00") && ((age > required_minAge && age < required_maxAge) || (required_minAge == 0 && required_maxAge == 0))) || helpFindstrInVector(users_created_posts, to_string(stoi(the_list[i].first.substr(0, 8)))))
				{
					if (count == stoi(num_of_current_posts))
					{
						help_str += the_list[i].first.substr(0, size - 10) + the_list[i].second;
						num_of_posts++;
					}
					else
						count++;
				}
			}
			else
			{
				if (((required_interests == interests || required_interests == "00") && (required_occupation == occupation || required_occupation == "00") && (required_hobbies == hobbies || required_hobbies == "00") && ((age > required_minAge && age < required_maxAge) || (required_minAge == 0 && required_maxAge == 0))) || helpFindstrInVector(users_created_posts, to_string(stoi(the_list[i].first.substr(0, 8)))))
				{
					if (the_list[i].second == category)
					{
						if (count == stoi(num_of_current_posts))
						{
							help_str += the_list[i].first.substr(0, size - 10) + the_list[i].second;
							num_of_posts++;
						}
						else
							count++;
					}
				}
			}
		}
		msg += padd_zeroes(to_string(num_of_posts), 2);
		msg += help_str;
		return msg;
	}
	catch (...)
	{
		return "";
	}
}

//updates the votes of a post. returns true upon success.
bool DataBase::updateVotes(string post_id, string comment_num, int up_or_down, string vote_size, string userID)
{
	//if vote_size = 2 I need to refer to both of the fields.
	unordered_map<string, vector<string>> my_results;
	try
	{
		if (vote_size == "2")
		{
			string command = "select upvotes, downvotes from comments_of_post_" + to_string(stoi(post_id)) + " where comment_num=" + to_string(stoi(comment_num)) + ";";
			while (results_lock);
			results_lock = true;
			if (!sqliteQuery(COMMENTS_PATH, command, true))
			{
				results_lock = false;
				return false;
			}
			my_results = results;
			results_lock = false;
			string upvotes = to_string(stoi(my_results.find("upvotes")->second.back()) + up_or_down);
			string downvotes = to_string(stoi(my_results.find("downvotes")->second.back()) - up_or_down);
			if (stoi(upvotes) < 0)
				upvotes = "0";
			if (stoi(downvotes) < 0)
				downvotes = "0";
			command = "update comments_of_post_" + to_string(stoi(post_id)) + " set upvotes = " + upvotes + ", downvotes = " + downvotes + " where comment_num = " + to_string(stoi(comment_num)) + "; ";
			if (!sqliteQuery(COMMENTS_PATH, command, false))
				return false;

			command = "insert into votes_of_" + userID + "(up_or_down, post_id, comment_num) values(\"" + (up_or_down == 1 ? "up\"" : "down\"") + "," + post_id + "," + comment_num + ");";
			if (!sqliteQuery(VOTES_PATH, command, false))
				return false;
		}
		else if (vote_size == "1")
		{
			if (up_or_down == 1)
			{
				string command = "select upvotes from comments_of_post_" + to_string(stoi(post_id)) + " where comment_num=" + to_string(stoi(comment_num)) + ";";
				while (results_lock);
				results_lock = true;
				if (!sqliteQuery(COMMENTS_PATH, command, true))
				{
					results_lock = false;
					return false;
				}
				my_results = results;
				results_lock = false;
				string upvotes = to_string(stoi(my_results.find("upvotes")->second.back()) + 1);
				command = "update comments_of_post_" + to_string(stoi(post_id)) + " set upvotes = " + upvotes + " where comment_num = " + to_string(stoi(comment_num)) + "; ";
				if (!sqliteQuery(COMMENTS_PATH, command, false))
					return false;
				command = "insert into votes_of_" + userID + "(up_or_down, post_id, comment_num) values(\"up\"," + post_id + "," + comment_num + ");";
				if (!sqliteQuery(VOTES_PATH, command, false))
					return false;
			}
			else if (up_or_down == -1)
			{
				string command = "select downvotes from comments_of_post_" + to_string(stoi(post_id)) + " where comment_num=" + to_string(stoi(comment_num)) + ";";
				while (results_lock);
				results_lock = true;
				if (!sqliteQuery(COMMENTS_PATH, command, true))
				{
					results_lock = false;
					return false;
				}
				my_results = results;
				results_lock = false;
				string downvotes = to_string(stoi(my_results.find("downvotes")->second.back()) + 1);
				command = "update comments_of_post_" + to_string(stoi(post_id)) + " set downvotes = " + downvotes + " where comment_num = " + to_string(stoi(comment_num)) + "; ";
				if (!sqliteQuery(COMMENTS_PATH, command, false))
					return false;
				command = "insert into votes_of_" + userID + "(up_or_down, post_id, comment_num) values(\"down\"," + post_id + "," + comment_num + ");";
				if (!sqliteQuery(VOTES_PATH, command, false))
					return false;
			}
			else
				cout << "WTF?" << endl;
		}
		else if (vote_size == "0")
		{
			if (up_or_down == 1)
			{
				string command = "select upvotes from comments_of_post_" + to_string(stoi(post_id)) + " where comment_num=" + to_string(stoi(comment_num)) + ";";
				while (results_lock);
				results_lock = true;
				if (!sqliteQuery(COMMENTS_PATH, command, true))
				{
					results_lock = false;
					return false;
				}
				my_results = results;
				results_lock = false;
				string upvotes = to_string(stoi(my_results.find("upvotes")->second.back()) - 1);
				if (stoi(upvotes) < 0)
					upvotes = "0";
				command = "update comments_of_post_" + to_string(stoi(post_id)) + " set upvotes = " + upvotes + " where comment_num = " + to_string(stoi(comment_num)) + "; ";
				if (!sqliteQuery(COMMENTS_PATH, command, false))
					return false;
				command = "delete from votes_of_" + userID + " where post_id=" + post_id + " and comment_num=" + comment_num + ";"; // (up_or_down, post_id, comment_num) values(\"up\"," + post_id + "," + comment_num + ");";
				if (!sqliteQuery(VOTES_PATH, command, false))
					return false;
			}

			else if (up_or_down == -1)
			{
				string command = "select downvotes from comments_of_post_" + to_string(stoi(post_id)) + " where comment_num=" + to_string(stoi(comment_num)) + ";";
				while (results_lock);
				results_lock = true;
				if (!sqliteQuery(COMMENTS_PATH, command, true))
				{
					results_lock = false;
					return false;
				}
				my_results = results;
				results_lock = false;
				string downvotes = to_string(stoi(my_results.find("downvotes")->second.back()) - 1);
				if (stoi(downvotes) < 0)
					downvotes = "0";
				command = "update comments_of_post_" + to_string(stoi(post_id)) + " set downvotes = " + downvotes + " where comment_num = " + to_string(stoi(comment_num)) + "; ";
				if (!sqliteQuery(COMMENTS_PATH, command, false))
					return false;
				command = "delete from votes_of_" + userID + " where post_id=" + post_id + " and comment_num=" + comment_num + ";"; // (up_or_down, post_id, comment_num) values(\"up\"," + post_id + "," + comment_num + ");";
				if (!sqliteQuery(VOTES_PATH, command, false))
					return false;
			}
			else
				cout << "WTF???" << endl;
		}
		else
			cout << "WTF???" << endl;
	}
	catch (...)
	{
		return false;
	}
	return true;
}

//returns the saved posts.
string DataBase::getSavedPosts(string type, string num_of_current_posts, string lastID, string userID)
{
	try
	{
		if (type == "my posts")
		{
			string highest_ID, lowest_ID, command = "";
			unordered_map<string, vector<string>> my_results1, my_results2;
			if (stoi(num_of_current_posts) == 0)
			{
				highest_ID = getLastItem("ID", "created_posts_of_" + userID, POSTS_CREATED_PATH);
				if (stoi(highest_ID) == 0)
					return "1";
				lowest_ID = to_string(stoi(highest_ID) - 10);
				if (stoi(lowest_ID) < 0)
					lowest_ID = "0";
			}
			else
			{
				command += "select ID from created_posts_of_" + userID + " where post_ID=" + lastID;
				while (results_lock);
				results_lock = true;
				if (!sqliteQuery(POSTS_CREATED_PATH, command, true))
				{
					results_lock = false;
					return "1";
				}
				my_results1 = results;
				results_lock = false;

				if (my_results1.empty() == false)
				{
					string ID = my_results1.find("ID")->second.back();
					highest_ID = to_string(stoi(ID) - 1);
					lowest_ID = to_string(stoi(highest_ID) - 10);
					if (stoi(lowest_ID) < 0)
						lowest_ID = "0";
				}
				else
					return "000";
			}
			string return_msg = "0";
			command = "select post_ID from created_posts_of_" + userID + " where ID<=" + highest_ID + " and ID>" + lowest_ID + " limit 10";
			while (results_lock);
			results_lock = true;
			if (!sqliteQuery(POSTS_CREATED_PATH, command, true))
			{
				results_lock = false;
				return "1";
			}
			my_results1 = results;
			results_lock = false;
			if (my_results1.empty() == true)
				return "000";
			command = "select ID, content, username, num_of_comments, category, time from posts where ";
			return_msg += padd_zeroes(to_string(my_results1.find("post_ID")->second.size()), 2);
			int i, k;
			for (i = my_results1.find("post_ID")->second.size() - 1; i >= 0; i--)
			{
				command += "ID=" + my_results1.find("post_ID")->second[i];
				if (i != 0)
					command += " or ";
			}
			while (results_lock);
			results_lock = true;
			if (!sqliteQuery(DB_PATH, command, true))
			{
				results_lock = false;
				return "1";
			}
			my_results2 = results;
			results_lock = false;
			for (i = my_results1.find("post_ID")->second.size() - 1; i >= 0; i--)
			{
				string wanted_ID = my_results1.find("post_ID")->second[i];
				string content, content_len, username, username_len, num_of_comments, creation_time, time, category;
				return_msg += padd_zeroes(wanted_ID, 8);
				for (k = 0; k < (signed)my_results2.find("ID")->second.size(); k++)
				{
					if (my_results2.find("ID")->second[k] == wanted_ID)
					{
						content = my_results2.find("content")->second[k];
						content_len = to_string(content.length());
						username = my_results2.find("username")->second[k];
						username_len = to_string(username.length());
						num_of_comments = my_results2.find("num_of_comments")->second[k];
						creation_time = my_results2.find("time")->second[k];
						time = to_string(time_now() - stoll(creation_time));
						category = my_results2.find("category")->second[k];

						content_len = padd_zeroes(content_len, 3);
						username_len = padd_zeroes(username_len, 2);
						time = padd_zeroes(time, 13);
						num_of_comments = padd_zeroes(num_of_comments, 7);
						category = padd_zeroes(category, 2);
						break;
					}
				}
				return_msg += num_of_comments + content_len + content + username_len + username + time + category;
			}


			return return_msg;
		}

		else if (type == "commented posts")
		{
			string highest_ID, lowest_ID, command = "";
			unordered_map<string, vector<string>> my_results1, my_results2;
			if (stoi(num_of_current_posts) == 0)
			{
				highest_ID = getLastItem("ID", "commented_posts_of_" + userID, POSTS_COMMENTED_PATH);
				if (stoi(highest_ID) == 0)
					return "1";
				lowest_ID = to_string(stoi(highest_ID) - 10);
				if (stoi(lowest_ID) < 0)
					lowest_ID = "0";
			}
			else
			{
				command += "select ID from commented_posts_of_" + userID + " where post_ID=" + lastID;
				while (results_lock);
				results_lock = true;
				if (!sqliteQuery(POSTS_COMMENTED_PATH, command, true))
				{
					results_lock = false;
					return "1";
				}
				my_results1 = results;
				results_lock = false;
				if (my_results1.empty() == false)
				{
					string ID = my_results1.find("ID")->second.back();
					highest_ID = to_string(stoi(ID) - 1);
					lowest_ID = to_string(stoi(highest_ID) - 10);
					if (stoi(lowest_ID) < 0)
						lowest_ID = "0";
				}
				else
					return "000";
			}
			string return_msg = "0";
			command = "select post_ID from commented_posts_of_" + userID + " where ID<=" + highest_ID + " and ID>" + lowest_ID + " limit 10";
			while (results_lock);
			results_lock = true;
			if (!sqliteQuery(POSTS_COMMENTED_PATH, command, true))
			{
				results_lock = false;
				return "1";
			}
			my_results1 = results;
			results_lock = false;
			command = "select ID, content, username, num_of_comments, time, category from posts where ";
			if (my_results1.empty() == true)
				return "000";
			return_msg += padd_zeroes(to_string(my_results1.find("post_ID")->second.size()), 2);
			int i, k;
			for (i = my_results1.find("post_ID")->second.size() - 1; i >= 0; i--)
			{
				command += "ID=" + my_results1.find("post_ID")->second[i];
				if (i != 0)
					command += " or ";
			}
			while (results_lock);
			results_lock = true;
			if (!sqliteQuery(DB_PATH, command, true))
			{
				results_lock = false;
				return "1";
			}
			my_results2 = results;
			results_lock = false;
			for (i = my_results1.find("post_ID")->second.size() - 1; i >= 0; i--)
			{
				string wanted_ID = my_results1.find("post_ID")->second[i];
				string content, content_len, username, username_len, num_of_comments, creation_time, time, category;
				return_msg += padd_zeroes(wanted_ID, 8);
				for (k = 0; k < (signed)my_results2.find("ID")->second.size(); k++)
				{
					if (my_results2.find("ID")->second[k] == wanted_ID)
					{
						content = my_results2.find("content")->second[k];
						content_len = to_string(content.length());
						username = my_results2.find("username")->second[k];
						username_len = to_string(username.length());
						num_of_comments = my_results2.find("num_of_comments")->second[k];
						creation_time = my_results2.find("time")->second[k];
						time = to_string(time_now() - stoll(creation_time));
						category = my_results2.find("category")->second[k];

						content_len = padd_zeroes(content_len, 3);
						username_len = padd_zeroes(username_len, 2);
						time = padd_zeroes(time, 13);
						num_of_comments = padd_zeroes(num_of_comments, 7);
						category = padd_zeroes(category, 2);
						break;
					}
				}
				return_msg += num_of_comments + content_len + content + username_len + username + time + category;
			}

			return return_msg;
		}

		else if (type == "posts to read later")
		{
			string highest_ID, lowest_ID, command = "";
			unordered_map<string, vector<string>> my_results1, my_results2;
			if (stoi(num_of_current_posts) == 0)
			{
				highest_ID = getLastItem("ID", "read_later_posts_of_" + userID, READ_LATER_POSTS);
				if (stoi(highest_ID) == 0)
					return "1";
				lowest_ID = to_string(stoi(highest_ID) - 10);
				if (stoi(lowest_ID) < 0)
					lowest_ID = "0";
			}
			else
			{
				command += "select ID from read_later_posts_of_" + userID + " where post_ID=" + lastID;
				while (results_lock);
				results_lock = true;
				if (!sqliteQuery(READ_LATER_POSTS, command, true))
				{
					results_lock = false;
					return "1";
				}
				my_results1 = results;
				results_lock = false;
				if (my_results1.empty() == false)
				{
					string ID = my_results1.find("ID")->second.back();
					highest_ID = to_string(stoi(ID) - 1);
					lowest_ID = to_string(stoi(highest_ID) - 10);
					if (stoi(lowest_ID) < 0)
						lowest_ID = "0";
				}
				else
					return "000";
			}
			string return_msg = "0";
			command = "select post_ID from read_later_posts_of_" + userID + " where ID<=" + highest_ID + " and ID>" + lowest_ID + " limit 10";
			while (results_lock);
			results_lock = true;
			if (!sqliteQuery(READ_LATER_POSTS, command, true))
			{
				results_lock = false;
				return "1";
			}
			my_results1 = results;
			results_lock = false;
			command = "select ID, content, username, num_of_comments, time, category from posts where ";
			if (my_results1.empty() == true)
				return "000";
			return_msg += padd_zeroes(to_string(my_results1.find("post_ID")->second.size()), 2);
			int i, k;
			for (i = my_results1.find("post_ID")->second.size() - 1; i >= 0; i--)
			{
				command += "ID=" + my_results1.find("post_ID")->second[i];
				if (i != 0)
					command += " or ";
			}
			while (results_lock);
			results_lock = true;
			if (!sqliteQuery(DB_PATH, command, true))
			{
				results_lock = false;
				return "1";
			}
			my_results2 = results;
			results_lock = false;
			for (i = my_results1.find("post_ID")->second.size() - 1; i >= 0; i--)
			{
				string wanted_ID = my_results1.find("post_ID")->second[i];
				string content, content_len, username, username_len, num_of_comments, creation_time, time, category;
				return_msg += padd_zeroes(wanted_ID, 8);
				for (k = 0; k < (signed)my_results2.find("ID")->second.size(); k++)
				{
					if (my_results2.find("ID")->second[k] == wanted_ID)
					{
						content = my_results2.find("content")->second[k];
						content_len = to_string(content.length());
						username = my_results2.find("username")->second[k];
						username_len = to_string(username.length());
						num_of_comments = my_results2.find("num_of_comments")->second[k];
						creation_time = my_results2.find("time")->second[k];
						time = to_string(time_now() - stoll(creation_time));
						category = my_results2.find("category")->second[k];

						content_len = padd_zeroes(content_len, 3);
						username_len = padd_zeroes(username_len, 2);
						time = padd_zeroes(time, 13);
						num_of_comments = padd_zeroes(num_of_comments, 7);
						category = padd_zeroes(category, 2);
						break;
					}
				}
				return_msg += num_of_comments + content_len + content + username_len + username + time + category;
			}

			return return_msg;
		}
		else
		{
			cout << "DataBase::getSavedPosts - not in protocol" << endl;
			return "1";
		}
	}
	catch (...)
	{
		return "1";
	}
}

//deletes all of the saved posts. returns true upon success.
bool DataBase::deleteAllPosts(string userID)
{
	string command = "delete from read_later_posts_of_" + userID + ";";
	if (!sqliteQuery(READ_LATER_POSTS, command, false))
		return false;
	return true;
}

//deletes some of the saved posts. returns true upon success.
bool DataBase::deletePosts(string userID, int numOfPosts, vector<string> IDs)
{
	int i;
	string command = "delete from read_later_posts_of_" + userID + " where ";
	try
	{
		for (i = 0; i < numOfPosts; i++)
		{
			if (i != 0)
				command += " or ";
			command += "post_ID=" + IDs[i];
		}
		command += ";";
		if (!sqliteQuery(READ_LATER_POSTS, command, false))
			return false;
	}
	catch (...)
	{
		return false;
	}
	return true;
}

//adds a posts to the read later list of a user. returns true upon success.
bool DataBase::addPostToReadLater(string userID, string postID)
{
	string command = "insert into read_later_posts_of_" + userID + "(post_ID) values(" + postID + ");";
	if (!sqliteQuery(READ_LATER_POSTS, command, false))
		return false;
	return true;
}

//returns a post that matches the paremeters. returns "1" upon failure.
string DataBase::postsSearch(string title, string categoryID, long long timeRange, int numOfCurrPosts)
{
	//TODO: First get all the posts that fit the category and time range. Then compare their titles with the requested title and see if there are matching chracters.
	unordered_map<string, vector<string>> my_results1, my_results2;
	string earliest_time;
	if (timeRange == 0)
		earliest_time = "0";
	else
		earliest_time = to_string(time_now() - timeRange);
	string command;
	if (categoryID != "00")
		command = "select ID, content, username, time, num_of_comments from posts where category=\"" + categoryID + "\" and time>" + earliest_time + ";";
	else
		command = "select ID, content, username, time, num_of_comments from posts where time>" + earliest_time + ";";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(DB_PATH, command, true))
	{
		results_lock = false;
		return "1";
	}
	my_results1 = results;
	results_lock = false;
	string help_return_str = "";
	if (my_results1.empty())
	{
		return "000";
	}
	try
	{
		int i, num_of_posts = 0, size = my_results1.find("ID")->second.size();
		for (i = 0; i < size; i++)
		{
			if (my_results1.find("content")->second[i].find(title) != -1)
			{
				num_of_posts++;
				string ID = padd_zeroes(my_results1.find("ID")->second[i], 8);
				string contentLen = padd_zeroes(to_string(my_results1.find("content")->second[i].length()), 3);
				string content = padd_zeroes(my_results1.find("content")->second[i], stoi(contentLen));
				string usernameLen = padd_zeroes(to_string(my_results1.find("username")->second[i].length()), 2);
				string username = my_results1.find("username")->second[i];
				string creation_time = my_results1.find("time")->second[i];
				string passed_time = padd_zeroes(to_string(time_now() - stoll(creation_time)), 13);
				string num_of_comments = padd_zeroes(my_results1.find("num_of_comments")->second[i], 7);
				help_return_str += ID + num_of_comments + contentLen + content + usernameLen + username + passed_time + categoryID;
			}
		}
		string return_str = "0" + padd_zeroes(to_string(num_of_posts), 2) + help_return_str;
		return return_str;
	}
	catch (...)
	{
		return "1";
	}
}

//Updates a user's status. Returns true upon success.
bool DataBase::updateStatus(string userID, string newStatus)
{
	string command = "update users set status=\"" + newStatus + "\" where ID=" + userID + ";";
	if (!sqliteQuery(DB_PATH, command, false))
		return false;
	return true;
}

//Updates a user's personal info. Returns true upon success.
bool DataBase::updatePersonalInfo(string userID, string birthDay, string birthMonth, string birthYear, string hobbies, string occupation, string interests)
{
	string date_of_birth = birthDay + "/" + birthMonth + "/" + birthYear;
	string command = "update users set date_of_birth=\"" + date_of_birth + "\", hobbies=\"" + hobbies + "\", occupation=\"" + occupation + "\", interests=\"" + interests + "\" where ID=" + userID + ";";
	if (!sqliteQuery(DB_PATH, command, false))
		return false;
	return true;
}

//Returns info about profile. Returns "1" upon failure.
string DataBase::getProfile(string username, string usernameLen)
{
	string status = "", status_length = "000", birth_date, birthDay = "00", birthMonth = "00", birthYear = "0000", hobbies = "00", occupation = "00", interests = "00", pic_link = "", size = "00000000";
	ifstream profile_pic;
	unordered_map<string, vector<string>> my_results1;
	string command = "select ID, status, date_of_birth, hobbies, occupation, interests from users where name=\"" + username + "\"";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(DB_PATH, command, true))
	{
		results_lock = false;
		return "1";
	}
	my_results1 = results;
	results_lock = false;
	try
	{
		if (my_results1.empty())
			return "1";
		string ID = padd_zeroes(my_results1.find("ID")->second.back(), 8);
		if (my_results1.find("status") != my_results1.end())
		{
			status = my_results1.find("status")->second.back();
			status_length = padd_zeroes(to_string(status.length()), 3);
		}
		if (my_results1.find("date_of_birth") != my_results1.end())
		{
			birth_date = my_results1.find("date_of_birth")->second.back();
			if (birth_date != "0")
			{
				birthDay = birth_date.substr(0, 2);
				birthMonth = birth_date.substr(2 + 1, 2);
				birthYear = birth_date.substr(2 + 1 + 2 + 1, 4);
			}
		}
		if (my_results1.find("hobbies") != my_results1.end())
			hobbies = padd_zeroes(my_results1.find("hobbies")->second.back(), 2);
		if (my_results1.find("occupation") != my_results1.end())
			occupation = padd_zeroes(my_results1.find("occupation")->second.back(), 2);
		if (my_results1.find("interests") != my_results1.end())
			interests = padd_zeroes(my_results1.find("interests")->second.back(), 2);
		pic_link = PICS_LINK + string("profile_images\\profile_pic_of_") + to_string(stoi(ID)) + string(".jpg");//my_results1.find("picture_link")->second.back();
		profile_pic.open(pic_link, ios::in | ios::binary | ios::ate);
		if (profile_pic.is_open() == false)
		{
			cout << "DataBase::getProfile - Profile picture of " << username << " could not be opened" << endl;
			return "0" + ID + usernameLen + username + status_length + status + birthDay + birthMonth + birthYear + hobbies + occupation + interests + "00000000";
		}
		size = padd_zeroes(to_string(profile_pic.tellg()), 8);
		profile_pic.close();
		string return_msg = "0" + ID + usernameLen + username + status_length + status + birthDay + birthMonth + birthYear + hobbies + occupation + interests + size;
		return return_msg;
	}
	catch (...)
	{
		return "1";
	}
}

//Returns a pointer to a file (fstream). Returns NULL upon failure.
ifstream* DataBase::getProfilePic(string username, string usernameLen)
{
	string userID = getIDByName(username);
	string pic_link = PICS_LINK + string("profile_images\\profile_pic_of_") + userID + string(".jpg");
	ifstream* profile_pic = new ifstream;
	profile_pic->open(pic_link.c_str(), ios::binary | ios::in);
	if (profile_pic->is_open() == false)
	{
		cout << "Profile picture of " << username << " could not be opened" << endl;
		return NULL;
	}
	else
		return profile_pic;
}

//Updates personal data. Return ok ('0') or not ('1') or there's someone else with this email ('2').
char DataBase::updatePersonalData(string userID, string oldData, string newData, string type)
{
	unordered_map<string, vector<string>> my_results1;
	string command;
	if (type == "password")
	{
		command = "select password from users where ID = \"" + userID + "\";";
		while (results_lock);
		results_lock = true;
		if (!sqliteQuery(DB_PATH, command, true))
		{
			results_lock = false;
			return '1';
		}
		my_results1 = results;
		results_lock = false;
		if (my_results1.find("password") == my_results1.end()) //not supposed to ever happen.
		{
			cout << "no such user with ID = " + userID;
			return '1';
		}
		else if (my_results1.find("password")->second.back() != oldData) //if oldData doesn't match the data in the DB
			return '1';
		else //if everything is ok
		{
			command = "update users set password = \"" + newData + "\" where ID = \"" + userID + "\";";
			if (!sqliteQuery(DB_PATH, command, false))
				return '1';
			return '0';
		}
	}
	else if (type == "email")
	{
		command = "select email from users where ID = \"" + userID + "\";";
		while (results_lock);
		results_lock = true;
		if (!sqliteQuery(DB_PATH, command, true))
		{
			results_lock = false;
			return '1';
		}
		my_results1 = results;
		results_lock = false;
		if (my_results1.find("email") == my_results1.end()) //not supposed to ever happen.
		{
			cout << "no such user with ID = " + userID;
			return '1';
		}
		else if (my_results1.find(type)->second.back() != oldData) //if oldData doesn't match the data in the DB
			return '1';
		
		command = "select ID from users where email = \"" + newData + "\";";
		while (results_lock);
		results_lock = true;
		if (!sqliteQuery(DB_PATH, command, true))
			return '1';
		my_results1 = results;
		results_lock = false;
		if (my_results1.find("ID") != my_results1.end()) //if there's someone else with this email (the new email)
			return '2';
		else //if everything is ok
		{
			command = "update users set email = \"" + newData + "\" where ID = \"" + userID + "\";";
			sqliteQuery(DB_PATH, command, false);
			return '0';
		}
	}
	else
	{
		return '1';
	}
	
}

//Blocks or unblocks a user (depends on his current situation). Returns '0' if it blocked someone or '1' if it unblocked someone. returns '2' upon failure.
string DataBase::blockOrUnblock(string userID, string name)
{
	unordered_map<string, vector<string>> my_results1;
	string ID_of_blocked = getIDByName(name);
	if (stoi(ID_of_blocked) == 0)
		return "2";
	string returnVal;
	string command = "select ID from blocks_of_" + userID + " where ID_of_blocked_user=" + ID_of_blocked + ";";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(BLOCKS, command, true))
	{
		results_lock = false;
		return "2";
	}
	my_results1 = results;
	results_lock = false;
	if (my_results1.empty()) //this means it's not blocked and needs to be blocked
	{
		command = "insert into blocks_of_" + userID + "(ID_of_blocked_user) values(" + ID_of_blocked + ");";
		if (!sqliteQuery(BLOCKS, command, false))
			return "2";
		returnVal = "0";
	}

	else //this means it's blocked and needs to be unblocked
	{
		command = "delete from blocks_of_" + userID + " where ID_of_blocked_user = " + ID_of_blocked + ";";
		if (!sqliteQuery(BLOCKS, command, false))
			return "2";
		returnVal = "1";
	}
	return returnVal;
}

//Returns the users that match the paremeters. Returns "1" upon failure.
string DataBase::userSearch(int numOfCurrUsers, string requested_name, int lowestAge, int highestAge, string occupation, string hobbies, string interests)
{
	int i, count = numOfCurrUsers, numOfSent = 0, currAge;
	unordered_map<string, vector<string>> my_results;
	string command, returnStr, helpStr = "", currName, currDate;
	bool noAgeLimit = false, addedParameter = false;
	if (lowestAge == 0 && highestAge == 0)
		noAgeLimit = true;
	if (noAgeLimit == false)
		command = "select name, date_of_birth from users";
	else
		command = "select name from users";
	if (occupation != "00" || hobbies != "00" || interests != "00")
		command += " where ";
	if (occupation != "00")
	{
		command += "occupation = \"" + occupation + "\"";
		addedParameter = true;
	}
	if (hobbies != "00")
	{ 
		if (addedParameter)
			command += " and ";
		command += "hobbies = \"" + hobbies + "\"";
		addedParameter = true;
	}
	if (interests != "00")
	{
		if (addedParameter == true)
			command += " and ";
		command += "interests = \"" + interests + "\"";
	}
	command += ";";
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(DB_PATH, command, true))
		return "1";
	my_results = results;
	results_lock = false;
	if (my_results.empty())
	{
		return "1";
	}
	try
	{
		if (noAgeLimit)
		{
			for (i = 0; i < my_results.find("name")->second.size() && numOfSent < 10; i++)
			{
				currName = to_lower_case(my_results.find("name")->second[i]);
				if (strstr(currName.c_str(), requested_name.c_str()) != NULL || requested_name == "")
				{
					if (count == 0)
					{
						numOfSent++;
						helpStr += padd_zeroes(to_string(currName.length()), 2);
						helpStr += my_results.find("name")->second[i];
					}
					else
						count--;
				}
			}
			if (numOfSent == 0)
				return "1";
			else
			{
				returnStr = "0" + padd_zeroes(to_string(numOfSent), 2) + helpStr;
				return returnStr;
			}
		}
		else
		{
			for (i = 0; i < my_results.find("name")->second.size() && numOfSent < 10; i++)
			{
				currName = to_lower_case(my_results.find("name")->second[i]);
				currDate = my_results.find("date_of_birth")->second[i];
				currAge = calcAge(currDate);
				if ((strstr(currName.c_str(), requested_name.c_str()) != NULL || requested_name == "") && currAge >= lowestAge && currAge <= highestAge)
				{
					if (count == 0)
					{
						numOfSent++;
						helpStr += padd_zeroes(to_string(currName.length()), 2);
						helpStr += my_results.find("name")->second[i];
					}
					else
						count--;
				}
			}
			if (numOfSent == 0)
				return "1";
			else
			{
				returnStr = "0" + padd_zeroes(to_string(numOfSent), 2) + helpStr;
				return returnStr;
			}
		}
	}
	catch (...)
	{
		return "1";
	}
}

//Returns whether the allgedly blocked user is blocked.
bool DataBase::checkIfBlocked(string userID, string allegedlyBlockedID)
{
	unordered_map<string, vector<string>> my_results;
	string command = "select ID_of_blocked_user from blocks_of_" + userID + " where ID_of_blocked_user = " + allegedlyBlockedID;
	while (results_lock);
	results_lock = true;
	if (!sqliteQuery(BLOCKS, command, true))
	{
		results_lock = false;
		return true;
	}
	my_results = results;
	results_lock = false;
	if (my_results.empty())
		return false;
	else
		return true;
}

void DataBase::help_initializaton()
{
	chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
	long long time_now_ms = ms.count();
	pair<string, string> help;
	help.first = to_string(time_now_ms);
	help.second = "00000004";
	comments.insert(help);
	help.first = to_string(time_now_ms + 1000);
	help.second = "00000005";
	comments.insert(help);
	help.first = to_string(time_now_ms + 2000);
	help.second = "00000002";
	comments.insert(help);
	help.first = to_string(time_now_ms + 3000);
	help.second = "00000009";
	comments.insert(help);
	help.first = to_string(time_now_ms + 4000);
	help.second = "00000001";
	comments.insert(help);
	help.first = to_string(time_now_ms + 5000);
	help.second = "00000008";
	comments.insert(help);
	help.first = to_string(time_now_ms + 6000);
	help.second = "00000003";
	comments.insert(help);
	help.first = to_string(time_now_ms + 7000);
	help.second = "00000007";
	comments.insert(help);
	help.first = to_string(time_now_ms + 8000);
	help.second = "00000006";
	comments.insert(help);
}

//Executes the query.
bool DataBase::sqliteQuery(string path, string query, bool callback_f)
{
	char *zErrMsg = 0;
	bool* lock;
	sqlite3* db;
	if (path == DB_PATH)
	{
		lock = &main_db_locked;
	}
	else if (path == COMMENTS_PATH)
	{
		lock = &comments_db_locked;
	}
	else if (path == MESSAGGES_PATH)
	{
		lock = &messages_db_locked;
	}
	else if (path == VOTES_PATH)
	{
		lock = &votes_lock;
	}
	/*else if (path == USERS_POSTS_INFO_PATH)
	{
		lock = &users_posts_info_lock;
	}*/
	else if (path == POSTS_COMMENTED_PATH)
	{
		lock = &posts_commented_lock;
	}
	else if (path == POSTS_CREATED_PATH)
	{
		lock = &created_posts_lock;
	}
	else if (path == READ_LATER_POSTS)
	{
		lock = &read_later_posts_lock;
	}
	else if (path == BLOCKS)
	{
		lock = &blocks_lock;
	}
	else
	{
		cout << "DataBase::sqliteQuery - WTF??" << endl;
		return false;
	}
	try
	{
		while (*lock);
		*lock = true;
		if (sqlite3_open(path.c_str(), &db) != SQLITE_OK)
		{
			cout << endl << "-------------------------------" << endl;
			cout << "sqlite3_open failed." << endl;
			cout << "-------------------------------" << endl;
			sqlite3_close(db);
			return false;
		}

		sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
		if (callback_f)
		{
			results.clear();
			if (sqlite3_exec(db, query.c_str(), callback, NULL, &zErrMsg) != SQLITE_OK)
			{
				cout << endl << "-------------------------------" << endl;
				cout << "DataBase::sqliteQuery - " << sqlite3_errmsg(db) << endl;
				cout << "-------------------------------" << endl;
				sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);
				*lock = false;
				sqlite3_close(db);
				return false;
			}
		}
		else
		{
			if (sqlite3_exec(db, query.c_str(), NULL, NULL, &zErrMsg) != SQLITE_OK)
			{
				cout << endl << "-------------------------------" << endl;
				cout << "DataBase::sqliteQuery - " << sqlite3_errmsg(db) << endl;
				cout << "-------------------------------" << endl;
				sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);
				*lock = false;
				sqlite3_close(db);
				return false;
			}
		}


		sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);
		*lock = false;
		sqlite3_close(db);
		return true; //if this line is reached it means the function succeeded.
	}
	catch (...)
	{
		*lock = false;
		sqlite3_close(db);
		return false;
	}
}