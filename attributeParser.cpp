#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <map>
#include <fstream>
using namespace std;

void show_tokens(vector<string> tokens)
{
    for (int i = 0; i < tokens.size(); i++)
    {
        cout << tokens[i] << endl;
    }
}

struct attribute
{
    string name;
    string value;
    int idx;
    struct attribute *prev_tag;
    struct attribute *next_tag;
    attribute(string name, string value, attribute *prev_tag, attribute *next_tag, int idx) : name(name), value(value), prev_tag(prev_tag), next_tag(next_tag), idx(idx){};
};

void show_map_mp_idx(map<int, string> &mp_idx)
{
    int i = 0;
    while (mp_idx.find(i) != mp_idx.end())
    {
        cout << "tag close: " << mp_idx[i++] << endl;
    }
}

void show_map_mp_tags(map<string, struct attribute *> &mp_tags, string tag)
{
    if (mp_tags.find(tag) != mp_tags.end())
    {
        cout << "tag name: " << mp_tags[tag]->name << endl
             << "tag value: " << mp_tags[tag]->value << endl;
        struct attribute *temp_at = mp_tags[tag];
        while (temp_at->next_tag)
        {
            temp_at = temp_at->next_tag;
            cout << "tag name: " << temp_at->name << endl
                 << "tag value: " << temp_at->value << endl;
        }
    }
    else
    {
        cout << "no opening tag by name: " << tag << endl;
    }
}

int process_token_mp(vector<string> file_token, map<string, struct attribute *> &mp_tags, map<int, string> &mp_idx, int idx)
{

    if (file_token.size() == 0)
    {
        cout << "no tokens" << endl;
        return -1;
    }
    else if (file_token.size() >= 4)
    {
        //opening tag
        struct attribute *head = new attribute(file_token[1], file_token[3], NULL, NULL, idx);
        struct attribute *tail = head;

        for (size_t i = 4; i < file_token.size(); i = i + 3)
        {
            struct attribute *at = new attribute(file_token[i], file_token[i + 2], head, NULL, idx);
            head->next_tag = at;
            head = head->next_tag;
        }
        mp_tags[file_token[0]] = tail;
        mp_idx[idx] = file_token[0];
    }
    else if (file_token.size() == 1)
    {
        //closing tag
        mp_idx[idx] = file_token[0];
    }
    else
    {
        cout << "sysntax error: wrong opening tag" << endl;
        return -1;
    }
    return 0;
}

int attribute_parser(vector<string> file, map<string, struct attribute *> &mp_tags, map<int, string> &mp_idx)
{
    string delimeter1 = "<";
    string delimeter2 = " ";
    string delimeter3 = ">";
    string delimeter4 = "</";
    for (int i = 0; i < file.size(); i++)
    {
        size_t pos;
        //check for the closing tag
        //cout << "line is " << file[i];
        pos = file[i].find(delimeter4);
        if (pos != 0)
        {
            vector<string> file_token;
            //this is the opening tag
            //check for the opening brackets
            pos = file[i].find(delimeter1);
            //cout << "pos " << pos << endl;
            if (pos == 0)
            {
                file[i].erase(0, pos + delimeter1.length());
                //check for the spaces and collect the elements
                while ((pos = file[i].find(delimeter2)) != string::npos)
                {
                    //cout << "found the tag" << endl;
                    string st;
                    st = file[i].substr(0, pos);
                    file_token.push_back(st);
                    file[i].erase(0, pos + delimeter2.length());
                }
                //check for the closing bracket
                pos = file[i].find(delimeter3);
                if (pos != string::npos)
                {
                    // found the closing brackets
                    string st;
                    st = file[i].substr(0, pos);
                    file_token.push_back(st);
                    file[i].erase(0, pos + delimeter3.length());
                    if (-1 == process_token_mp(file_token, mp_tags, mp_idx, i))
                        return -1;
                }
                else
                {
                    cout << "syntax error: could not find the closing braces" << endl;
                    return -1;
                }
            }
            else
            {
                cout << "syntax error: could not find the opening braces" << endl;
                return -1;
            }
            //show_tokens(file_token);
        }
        else
        {
            //this is the closing tag
            vector<string> file_token;
            //cout << "found the closing tag" << endl;
            file[i].erase(0, pos + delimeter4.length());
            if ((pos = file[i].find(delimeter3)) != string::npos)
            {
                string st = file[i].substr(0, pos);
                file_token.push_back(st);
                file[i].erase(0, pos + delimeter3.length());
                if (-1 == process_token_mp(file_token, mp_tags, mp_idx, i))
                    return -1;
            }
            else
            {
                //the syntax error
                cout << "syntax error: closing tag" << endl;
                return -1;
            }
            //show_tokens(file_token);
        }
    }
    //process the tokens now
    //show_map_mp_tags(mp_tags, "tag");
    //show_map_mp_tags(mp_tags, "tag1");
    //show_map_mp_tags(mp_tags, "tag3");
    //show_map_mp_idx(mp_idx);
    return 0;
}

void run_query(vector<string> queries_tokens, map<string, struct attribute *> mp_tags, map<int, string> mp_idx)
{
    bool query_check = false;
    map< string, vector<string> > nested_tags;
    //show_map_mp_idx(mp_idx);
    vector <string> q_str; 
    string temp;
    for(int  i = 0; i < mp_idx.size(); i++){
        int pos = 0;
        if((pos = temp.find(mp_idx[i])) == string::npos){
            if(temp.length() == 0){
                temp += mp_idx[i];
            }
            else{
                temp += "." + mp_idx[i];
            }
        }
        else{
            q_str.push_back(temp);
            temp = temp.substr(0, pos -1);
            if(temp == mp_idx[i]){
                temp.erase(0,temp.length());
            }
        }
    }
    // construct the query
    string query;
    for(int i = 0; i < queries_tokens.size() - 1; i++){
        if(i == 0)
        query += queries_tokens[i];
        else query += "." + queries_tokens[i];
    }
    //cout << "constructed query: " << query << endl;

    for(int i = 0; i < q_str.size() ; i++){
        //cout << "q: " << q_str[i] << endl;
        if(q_str[i] == query){
            query_check = true;
            break;
        }
    }
            

    //finally after traversing the command process the query
    if (query_check == true)
    {
        //cout << "all tags are aligned and query is correct" <<endl;
        //cout << queries_tokens[queries_tokens.size() -1 ] << endl;
        struct attribute *temp_at = mp_tags[queries_tokens[queries_tokens.size() - 2]];
        while (temp_at != NULL)
        {
            if (!queries_tokens[queries_tokens.size() - 1].compare(temp_at->name))
            {
                //remove the " "
                string str = temp_at->value;
                str.erase(0, 1);
                str.erase(str.length() - 1, 1);
                cout << str << endl;
                return;
            }
            temp_at = temp_at->next_tag;
        }
        cout << "Not Found!" << endl;
    }
    else
        cout << "Not Found!" << endl;
    return;
}

int parse_queries(vector<string> queries, map<string, struct attribute *> mp_tags, map<int, string> mp_idx)
{
    for (int i = 0; i < queries.size(); i++)
    {
        string delimeter1 = ".";
        string delimeter2 = "~";

        vector<string> queries_tokens;
        size_t pos = 0;
        while ((pos = queries[i].find(delimeter1)) != string::npos)
        {
            //cout << "delimeter1 found hence it is nested query" << endl;
            string t;
            t = queries[i].substr(0, pos);
            queries_tokens.push_back(t);
            queries[i].erase(0, pos + delimeter1.length());
        }

        if ((pos = queries[i].find(delimeter2)) != string::npos)
        {
            //cout << "delimeter2 found hence the query is complete" << endl;
            string t;
            t = queries[i].substr(0, pos);
            queries_tokens.push_back(t);
            queries[i].erase(0, pos + delimeter2.length());
        }
        else
        {
            cout << "query is incomplete" << endl;
            return -1;
        }
        queries_tokens.push_back(queries[i]);
        //show the results
        //show_tokens(queries_tokens);
        run_query(queries_tokens, mp_tags, mp_idx);
        //queries_tokens.pop_back();
        //queries_tokens.pop_back();
        
    }
    return 0;
}

int main()
{
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    int N, Q;
    cin >> N;
    cin >> Q;
    vector<string> file;
    vector<string> queries;
    ifstream inFile;
    //inFile.open("sample.txt");
    cin.ignore();
    for (int i = 0; i < N; i++)
    {
        string s;
        getline(cin, s);
        //getline(cin, s);
        file.push_back(s);
    }
    map<string, struct attribute *> mp_tags;
    map<int, string> mp_idx;

    if (-1 == attribute_parser(file, mp_tags, mp_idx))
        return -1;
    //cout << "entr query" << endl;
    //cin.ignore();
    for (int i = 0; i < Q; i++)
    {
        string s;
        getline(cin, s);
        queries.push_back(s);
    }
    if (-1 == parse_queries(queries, mp_tags, mp_idx))
        return -1;
    return 0;
}
