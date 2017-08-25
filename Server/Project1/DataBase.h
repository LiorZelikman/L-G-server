#pragma once

#include <unordered_map>
#include <vector>
#include "helper.h"
#include "sqlite3.h"
#include <time.h>
#include <chrono>
#include <algorithm>
#include <Windows.h>
#include <fstream>
#include <cstdio>
#include <string>
#include <thread>

//PC
#define DB_PATH "C:\\Users\\User\\Desktop\\Project_DG_LZ\\Project\\Server\\Databases\\server.db"
#define COMMENTS_PATH "C:\\Users\\User\\Desktop\\Project_DG_LZ\\Project\\Server\\Databases\\comments_on_posts.db"
#define MESSAGGES_PATH "C:\\Users\\User\\Desktop\\Project_DG_LZ\\Project\\Server\\Databases\\messages_of_users.db"
#define VOTES_PATH "C:\\Users\\User\\Desktop\\Project_DG_LZ\\Project\\Server\\Databases\\votes.db"
#define POSTS_COMMENTED_PATH "C:\\Users\\User\\Desktop\\Project_DG_LZ\\Project\\Server\\Databases\\commented_posts.db"
#define POSTS_CREATED_PATH "C:\\Users\\User\\Desktop\\Project_DG_LZ\\Project\\Server\\Databases\\created_posts.db"
#define READ_LATER_POSTS "C:\\Users\\User\\Desktop\\Project_DG_LZ\\Project\\Server\\Databases\\read_later_posts.db"
#define BLOCKS "C:\\Users\\User\\Desktop\\Project_DG_LZ\\Project\\Server\\Databases\\blocks_list_of_users.db"
//laptop
/*
#define DB_PATH "C:\\Users\\User\\Desktop\\magshimim\\12th_grade\\Project_DG_LZ\\Project\\Server\\Databases\\server.db"
#define COMMENTS_PATH "C:\\Users\\User\\Desktop\\magshimim\\12th_grade\\Project_DG_LZ\\Project\\Server\\Databases\\comments_on_posts.db"
#define MESSAGGES_PATH "C:\\Users\\User\\Desktop\\magshimim\\12th_grade\\Project_DG_LZ\\Project\\Server\\Databases\\messages_of_users.db"
#define VOTES_PATH "C:\\Users\\User\\Desktop\\magshimim\\12th_grade\\Project_DG_LZ\\Project\\Server\\Databases\\votes.db"
#define POSTS_COMMENTED_PATH "C:\\Users\\User\\Desktop\\magshimim\\12th_grade\\Project_DG_LZ\\Project\\Server\\Databases\\commented_posts.db"
#define POSTS_CREATED_PATH "C:\\Users\\User\\Desktop\\magshimim\\12th_grade\\Project_DG_LZ\\Project\\Server\\Databases\\created_posts.db"
#define READ_LATER_POSTS "C:\\Users\\User\\Desktop\\magshimim\\12th_grade\\Project_DG_LZ\\Project\\Server\\Databases\\read_later_posts.db"
#define BLOCKS "C:\\Users\\User\\Desktop\\magshimim\\12th_grade\\Project_DG_LZ\\Project\\Server\\Databases\\blocks_list_of_users.db"
*/
//No t0uchie V
/*
#define DB_PATH "C:\\Users\\User\\Desktop\\GoMaN\\Magshimim\\12\\Project_DG_LZ\\Project\\Server\\Databases\\server.db"
#define COMMENTS_PATH "C:\\Users\\User\\Desktop\\GoMaN\\Magshimim\\12\\Project_DG_LZ\\Project\\Server\\Databases\\comments_on_posts.db"
#define MESSAGGES_PATH "C:\\Users\\User\\Desktop\\GoMaN\\Magshimim\\12\\Project_DG_LZ\\Project\\Server\\Databases\\messages_of_users.db"
#define VOTES_PATH "C:\\Users\\User\\Desktop\\GoMaN\\Magshimim\\12\\Project_DG_LZ\\Project\\Server\\Databases\\votes.db"
#define POSTS_COMMENTED_PATH "C:\\Users\\User\\Desktop\\GoMaN\\Magshimim\\12\\Project_DG_LZ\\Project\\Server\\Databases\\commented_posts.db"
#define POSTS_CREATED_PATH "C:\\Users\\User\\Desktop\\GoMaN\\Magshimim\\12\\Project_DG_LZ\\Project\\Server\\Databases\\created_posts.db"
#define READ_LATER_POSTS "C:\\Users\\User\\Desktop\\GoMaN\\Magshimim\\12\\Project_DG_LZ\\Project\\Server\\Databases\\read_later_posts.db"
*/

#define PICS_LINK "C:\\Users\\User\\Desktop\\Project_DG_LZ\\Project\\Server\\"
//#define BUFF_SIZE 2048 //0.5MB

static unordered_map<string, vector<string>> results;

class DataBase
{
public:
	//functions:
	DataBase();//ctor
	~DataBase();//dtor
	bool isNameAndPassMatch(string user, string pass); //returns true if the password of user matches to the password in the DB.
	string getIDByName(string name); //return the ID of the user whose username is user. returns "0" upon failure.
	bool isUserAlreadyExist(string name); //returns true if there's a user whose name is equal to the paremeter.
	bool isEmailAlreadyExist(string email); //returns true if there's a user whose email matches to the paremeter.
	bool insertNewUser(string name, string password, string email); //return success or fail.
	bool IsNameAndEmailMatch(string name, string email); //returns true if the email of user matches to the email in the DB.
	bool updatePassword(string name, string new_password); //return success or fail.
	bool insertNewMessage(string message, string type, string reciever_id, string sneder_id, string pic_link, string time, string action_type); //action type is either "sender", "reciever", or "both". returns true upon success and false upon failue.
	string getLastItem(string column, string table, string path);//return "0" upon failure.
	bool insertNewPost(string username, string content, string category, string userID, string min_age, string max_age, string hobbies, string occupation, string interests); //returns true upon success, false upon failure.
	string insertNewComment(string post_id, string content, string username, string userID); //returns the comment number or "0" upon failure.
	string postInfo(string post_id, string userID); //returns a lot of information about a post with ID = post_id, or "0" upon failure.
	string updateCommentsOnPost(string post_id, int numOfCurrComments, string userID); //returns comments on a post with ID = post_id, or "1" upon failure.
	bool checkRequirementsMatch(string post_id, string user_id); //returns whether the user is allowed to view the post according to his profile.
	string shortenedPostInfo(string post_id); //returns some information about a post with ID = post_id, or an empty string upon failure.
	string shortenedProfileInfo(string userID); //returns some information about a profile with ID = userID, or an empty string upon failure.
	void updateComments(); //deletes the comments that were written more than 1 hour ago from the comments list.
	void updateNewestPosts(); //updates the list of newest posts.
	void updateHottestPosts(); //updates the list of hottest posts.
	string getNewestPosts(string category, string num_of_current_posts, string userID); //returns the newest posts.
	string getHottestPosts(string category, string num_of_current_posts, string userID); //returns the hottest posts.
	string getSavedPosts(string type, string num_of_current_posts, string lastID, string userID); //returns the saved posts.
	bool deleteAllPosts(string userID); //deletes all of the saved posts. returns true upon success.
	bool deletePosts(string userID, int numOfPosts, vector<string> IDs); //deletes some of the saved posts. returns true upon success.
	bool addPostToReadLater(string userID, string postID); //adds a posts to the read later list of a user. returns true upon success.
	bool updateVotes(string post_id, string comment_num, int up_or_down, string vote_size, string userID); //updates the votes of a post. returns true upon success.
	string postsSearch(string title, string categoryID, long long timeRange, int numOfCurrPosts); //returns a post that matches the paremeters. returns "1" upon failure.
	bool updateStatus(string userID, string newStatus); //return true upon success.
	bool updatePersonalInfo(string userID, string birthDay, string birthMonth, string birthYear, string hobbies, string occupation, string interests); //returns true upon success.
	string getProfile(string username, string usernameLen); //returns info about profile. returns "1" upon failure.
	ifstream* getProfilePic(string username, string usernameLen); //returns a pointer to a file (fstream). returns NULL upon failure.
	char updatePersonalData(string userID, string oldData, string newData, string type); //updates personal data. return ok ('0') or not ('1') or there's someone else with this email ('2').
	string blockOrUnblock(string userID, string name); //blocks or unblocks a user (depends on his current situation). return '0' if it blocked someone or '1' if it unblocked someone. returns '2' upon failure.
	bool checkIfBlocked(string userID, string allegedlyBlockedID);
	string userSearch(int numOfCurrUsers, string requested_name, int lowestAge, int highestAge, string occupation, string hobbies, string interests); //returns the users that match the paremeters. returns "1" upon failure.
	bool sqliteQuery(string path, string query, bool callback_f); //Executes the query.
	void help_initializaton();
	void background_posts_updater()
	{
		while (true)
		{
			updateNewestPosts();		
			updateHottestPosts();
			this_thread::sleep_for(chrono::seconds(30));
		}
	}
	void background_comments_updater()
	{
		while (true)
		{
			this_thread::sleep_for(chrono::hours(1));
			updateComments();
		}
	}
private:
	bool main_db_locked = false, comments_db_locked = false, messages_db_locked = false, results_lock = false, votes_lock = false, posts_commented_lock = false, created_posts_lock = false, read_later_posts_lock = false, blocks_lock = false, newest_posts_list_lock = false, hottest_posts_list_lock = false, comments_list_lock = false;
	unordered_map<string, string> comments; //<time, post_id>
	vector<pair<string, string>> newest_posts_list; //<post info, category>
	vector<pair<string, string>> hottest_posts_list; //<post info, category>
	int static callback(void* notUsed, int argc, char** argv, char** azCol)
	{
		int i;
		try
		{
			for (i = 0; i < argc; i++)
			{
				if (argv[i] != NULL)
				{
					if (results.find(azCol[i]) != results.end())
					{
						auto it = results.find(azCol[i]);
						if (it != results.end())
						{
							it->second.push_back(argv[i]);
						}
					}
					else
					{
						pair<string, vector<string>> p;
						p.first = azCol[i];
						p.second.push_back(argv[i]);
						results.insert(p);
					}
				}
			}
			return 0;
		}
		catch (...)
		{
			return -1;
		}
	}
	string padd_zeroes(string str, int final_size)
	{
		string help = str;
		str = "";
		while ((signed)(str.length() + help.length()) < final_size)
			str += "0";
		str += help;
		return str;
	}
	long long time_now()
	{
		chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
		long long time_now_ms = ms.count();
		return time_now_ms;
	}
	int helpFindstrInVector(vector<string> vec, string str)
	{
		int i;
		for (i = 0; i < (signed)vec.size(); i++)
		{
			if (vec[i] == str)
				return i;
		}
		return -1;
	}
	int helpFindintInVector(vector<string> vec, int val)
	{
		int i;
		for (i = 0; i < (signed)vec.size(); i++)
		{
			if (stoi(vec[i]) == val)
				return i;
		}
		return -1;
	}
	vector<int> strVec_to_intVec(vector<string> vec)
	{
		int i;
		vector<int> return_vec;
		for (i = 0; i < (signed)vec.size(); i++)
			return_vec.push_back(stoi(vec[i]));
		return return_vec;
	}
	pair<string, int> findMax(unordered_map <string, int> de_map)
	{
		int maxVal = -1;
		string id;
		unordered_map <string, int>::iterator it = de_map.begin();
		for (; it != de_map.end(); it++)
		{
			if (it->second > maxVal)
			{
				maxVal = it->second;
				id = it->first;
			}
		}
		pair <string, int> returnVal;
		returnVal.first = id;
		returnVal.second = maxVal;
		return returnVal;
	}
	bool sortHelper(int i, int j) { return (i>j); }
	int calcAge(string dateOfBirth)
	{
		try
		{
			int passedYears;
			int day = stoi(dateOfBirth.substr(0, 2));
			int month = stoi(dateOfBirth.substr(3, 2));
			int year;
			if (dateOfBirth.length() == 10)
				year = stoi(dateOfBirth.substr(6, 4));
			else if (dateOfBirth.length() == 8)
				year = stoi(dateOfBirth.substr(6, 2)) + 1900;
			else
				return -1;
			time_t t = time_now() / 1000;   // get time now
			struct tm * now = localtime(&t);
			/*cout << (now->tm_year + 1900) << '-'
				<< (now->tm_mon + 1) << '-'
				<< now->tm_mday
				<< endl;
				*/
			passedYears = (now->tm_year + 1900) - year;
			if (now->tm_mon+1 < month)
				passedYears--;
			else if (now->tm_mon == month && now->tm_mday < day)
				passedYears--;
			return passedYears;
		}
		catch (...)
		{
			return -1;
		}
	}
	string to_lower_case(string str)
	{
		int i, size = str.length();
		for (i = 0; i < size; i++)
		{
			if (str[i] >= 65 && str[i] <= 90)
				str[i] += 32;
		}
		return str;
	}
};