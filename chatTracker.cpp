// ChatTracker.cpp

// This is a correct but inefficient implementation of
// the ChatTracker functionality.

#include <iostream>
#include "ChatTracker.h"
#include <string>
#include <vector>
#include <functional>
#include <stack>
using namespace std;

// This class does the real work of the implementation.

class hashtable_stringtoinfo {
private:
    int m_buckets;
    struct Node {
        Node(const std::string& name, int indices)
            :name(name), left(nullptr), right(nullptr) {
            index.push_back(indices);
        }

        vector<int> index;
        string name;
        Node* left, * right;
    };
    struct bucket {
        Node* headptr;
        bool used;
    };
    vector<bucket> hashtable;
public:
    hashtable_stringtoinfo(int buckets);
    ~hashtable_stringtoinfo();
    vector<int>* search(string name);
    bool searcher(string name);
    void insert(string name, int indices);
    void remove(string name);
    unsigned int mapfunc(string name);
    void freetree(Node* root);
};

class ChatTrackerImpl
{
public:
    ChatTrackerImpl(int maxBuckets);
    void join(string user, string chat);
    int terminate(string chat);
    int contribute(string user);
    int leave(string user, string chat);
    int leave(string user);
private:
    struct Info
    {
        Info(string u, string c) : user(u), chat(c), count(0) {}
        string user;
        string chat;
        int count;
    };

    struct Tableofinfo 
    {
        vector<Info> m_info;
        hashtable_stringtoinfo usersearch;
        hashtable_stringtoinfo chatsearch;
        Tableofinfo(const int& buckets)
            :usersearch(buckets), chatsearch(buckets),m_info() {}

    };
    struct TableofLeavers {
        vector<Info> m_usersWhoLeft;
        hashtable_stringtoinfo usersearch;
        hashtable_stringtoinfo chatsearch;
        TableofLeavers(const int& buckets)
            :usersearch(buckets), chatsearch(buckets),m_usersWhoLeft() {}
    };

    Tableofinfo stayers;
    TableofLeavers leavers;
};


hashtable_stringtoinfo::hashtable_stringtoinfo(int buckets) 
    :m_buckets(buckets),hashtable(buckets)
{}


void hashtable_stringtoinfo::freetree(Node* root) {
    if (root == nullptr)
        return;
    freetree(root->left);
    freetree(root->right);
    delete root;
}
hashtable_stringtoinfo::~hashtable_stringtoinfo() {
    for (int i = 0; i < m_buckets; i++) {
        if (!hashtable[i].used)
            continue;
        else {
            freetree(hashtable[i].headptr); //possible memory leak
        }
    }
}

unsigned int hashtable_stringtoinfo::mapfunc(string name) {
    std::hash<std::string> str_hash;
    unsigned int hashValue = str_hash(name);
    hashValue = hashValue % m_buckets;
    return hashValue;

}

bool hashtable_stringtoinfo::searcher(string name) {
    unsigned int bucket = mapfunc(name);
    if (hashtable[bucket].used) {

        Node* p = hashtable[bucket].headptr;
        while (p != nullptr) {
            if (name == p->name)
                return true;
            if (name < p->name)
                p = p->left;
            else if (name > p->name)
                p = p->right;
        }
        return false;
    }
    else {
        return false;
    }
}

vector<int>* hashtable_stringtoinfo::search(string name) {
    unsigned int bucket = mapfunc(name);
    Node* p = hashtable[bucket].headptr;
    if (searcher(name)) {
        while (p != nullptr) {
            if (name == p->name)
                return (&p->index);
            if (name < p->name)
                p = p->left;
            else if (name > p->name)
                p = p->right;
        }
    }
    return nullptr;
}

void hashtable_stringtoinfo::insert(string name, int indices) {
    unsigned int bucket = mapfunc(name);
    if (!hashtable[bucket].used || hashtable[bucket].headptr==nullptr) {
        hashtable[bucket].headptr = new Node(name, indices);
        hashtable[bucket].used = true;
    }
    else {
        Node* cur = hashtable[bucket].headptr;
        for (;;) {
            if (cur->name == name) {
                vector<int>* save = search(name);
                save->push_back(indices);
                return;
            }
            else if (name < cur->name) {
                if (cur->left == nullptr) {
                    cur->left = new Node(name, indices);
                    return;
                }
                cur = cur->left;
            }
            else if (name > cur->name) {
                if (cur->right == nullptr) {
                    cur->right = new Node(name, indices);
                    return;
                }
                cur = cur->right;
            }

        }
    }
}

void hashtable_stringtoinfo::remove(string name) {
    unsigned int bucket = mapfunc(name);
    if (!hashtable[bucket].used)
        return;
    else {
        Node* p = hashtable[bucket].headptr;
        Node* parent = hashtable[bucket].headptr;
        while (p != nullptr) {
            if (name == p->name) {
                if (p == hashtable[bucket].headptr) {
                    hashtable[bucket].headptr = nullptr;
                    delete p;
                    return;
                }
                else if (p->left == nullptr && p->right == nullptr) {
                    parent->left = nullptr;
                    parent->right = nullptr;
                    delete p;
                    return;
                }
                else if (p->left != nullptr && p->right != nullptr) {
                    Node* a = p;
                    Node* a_parent = p;
                    string b;
                    while (a->right != nullptr) {
                        a_parent = a;
                        a = a->right;
                        b = a->name;
                        if (a->left == nullptr && a->right == nullptr) {
                            a_parent->left = nullptr;
                            a_parent->right = nullptr;
                            delete a;
                            
                        }
                        else {
                            if (a->left != nullptr && a->right == nullptr) {
                                if (a_parent->left!=nullptr && a_parent->left == a)
                                    a_parent->left = a->left;
                                else if (a_parent->right != nullptr && a_parent->right == a)
                                    a_parent->right = a->left;
                                delete a;
                            
                            }
                            else if (a->right != nullptr && a->left == nullptr) {
                                if (a_parent->left!=nullptr && a_parent->left == a)
                                    a_parent->left = a->right;
                                else if (a_parent->right != nullptr && a_parent->right == a)
                                    a_parent->right = a->right;
                                delete a;
                               
                            }
                        }

                    }
                    p->name = b;
                    return;
                }
                else {
                    if (p->left != nullptr && p->right == nullptr) {
                        if (parent->left != nullptr && parent->left == p)
                            parent->left = p->left;
                        else if (parent->right != nullptr && parent->right == p)
                            parent->right = p->left;
                        delete p;
                        return;
                    }
                    else if (p->right != nullptr && p->left == nullptr) {
                        if (parent->left != nullptr && parent->left == p)
                            parent->left = p->right;
                        else if (parent->right != nullptr && parent->right == p)
                            parent->right = p->right;
                        delete p;
                        return;
                    }
                }
            }
            else if (name < p->name) {
                parent = p;
                p = p->left;
            }
                
            else if (name > p->name) {
                parent = p;
                p = p->right;
            }
        }
    }
}

ChatTrackerImpl::ChatTrackerImpl(int maxBuckets )
    :stayers(maxBuckets), leavers(maxBuckets) 
{}

void ChatTrackerImpl::join(string user, string chat)
{
    if (stayers.usersearch.searcher(user)) {
        vector<int>* userhold = stayers.usersearch.search(user);
        vector<int>* chathold = stayers.chatsearch.search(chat);
        for (vector<int>::iterator p = userhold->begin(); p!=userhold->end();p++) {
            if (stayers.m_info.at(*p).chat == chat) {
                Info info = stayers.m_info.at(*p );
                for (vector<int>::iterator s = chathold->begin(); s != chathold->end(); s++) {
                    if (*p == *s) {
                        chathold->erase(s);
                        break;
                    }
                }
                userhold->erase(p);
                stayers.m_info.push_back(info);
                int b = stayers.m_info.size() - 1;
                stayers.chatsearch.insert(chat, b);
                stayers.usersearch.insert(user, b);
                return;
            }

        }
    }
    stayers.m_info.push_back(Info(user, chat));
    int a = stayers.m_info.size() - 1;
    stayers.chatsearch.insert(chat, a);
    stayers.usersearch.insert(user, a);
}

int ChatTrackerImpl::terminate(string chat)
{
    int total = 0;
    if (stayers.chatsearch.searcher(chat) || leavers.chatsearch.searcher(chat)) {
        if (stayers.chatsearch.searcher(chat)) {
            vector<int>* chathold = stayers.chatsearch.search(chat);
            for (vector<int>::iterator p = (*chathold).begin(); p != (*chathold).end(); p++) {
                total += stayers.m_info.at(*p).count;
                if (stayers.usersearch.searcher(stayers.m_info.at(*p).user)) {
                    vector<int>* findthem = stayers.usersearch.search(stayers.m_info.at(*p).user);
                    for (vector<int>::iterator w = (*findthem).begin(); w != (*findthem).end(); w++) {
                        if (*p == *w) {
                            (*findthem).erase(w);
                            break;
                        }
                    }
                }
            }
            stayers.chatsearch.remove(chat);
        }
        if (leavers.chatsearch.searcher(chat)) {
            vector<int>* chathold = leavers.chatsearch.search(chat);
            for (vector<int>::iterator p = (*chathold).begin(); p != (*chathold).end(); p++) {
                total += leavers.m_usersWhoLeft.at(*p).count;
                if (leavers.usersearch.searcher(leavers.m_usersWhoLeft.at(*p).user)) {
                    vector<int>* findthem = leavers.usersearch.search(leavers.m_usersWhoLeft.at(*p).user);
                    for (vector<int>::iterator w = (*findthem).begin(); w != (*findthem).end(); w++) {
                        if (*p == *w) {
                            (*findthem).erase(w);
                            break;
                        }
                    }
                }
            }
            leavers.chatsearch.remove(chat);
        }
        return total;
    }
    else {
        return 0;
    }
}

int ChatTrackerImpl::contribute(string user)
{
    if (stayers.usersearch.searcher(user)) {
        vector<int>* occurrence = stayers.usersearch.search(user);
        if (occurrence->size() > 0) {
            vector<Info>::iterator d = stayers.m_info.begin() + occurrence->at(occurrence->size()-1);
            d->count++;
            return d->count;
        }
        return 0;
    }
    return 0;
}

int ChatTrackerImpl::leave(string user, string chat)
{
    if (stayers.chatsearch.searcher(chat) && stayers.usersearch.searcher(user)) {
        vector<int>* sc = stayers.chatsearch.search(chat);
        vector<int>* su = stayers.usersearch.search(user);
        int count(0),state(0),a(0);
        for (vector<int>::iterator led = sc->begin(); led != sc->end(); led++) {
            if (stayers.m_info.at(*led).user == user) {
                count = stayers.m_info.at(*led).count;
                a = *led;
                leavers.m_usersWhoLeft.push_back(stayers.m_info.at(*led));
                sc->erase(led);
                stayers.m_info.at(a).count = 0;
                state = 1;
                break;
                
            }
        }
        if (state == 1) {
            for (vector<int>::iterator les = su->begin(); les != su->end(); les++) {
                if (stayers.m_info.at(*les).chat == chat) {
                    su->erase(les);
                    break;
                }
            }

            int a = leavers.m_usersWhoLeft.size()-1;
            leavers.chatsearch.insert(chat, a);
            leavers.usersearch.insert(user, a);

            return count;
        }
        else {
         return -1;
        }
    }
        return -1;
}

int ChatTrackerImpl::leave(string user)
{
    if (stayers.usersearch.searcher(user)) {
        vector<int>* ref = stayers.usersearch.search(user);
        if (ref->size() > 0) {
            vector<int>* fer = stayers.chatsearch.search(stayers.m_info.at(ref->back()).chat);
            int count = stayers.m_info.at(ref->back()).count;
            leavers.m_usersWhoLeft.push_back(stayers.m_info.at(ref->back()));
            int a = leavers.m_usersWhoLeft.size() - 1;
            leavers.chatsearch.insert(stayers.m_info.at(ref->back()).chat, a);
            leavers.usersearch.insert(user, a);
            for (vector<int>::iterator d = fer->end(); d > fer->begin();) {
                d--;
                if (*d == ref->back()) {
                    fer->erase(d);
                    break;
                }
            }
            ref->pop_back();
            return count;
        }
        else {
            return -1;
        }
    }
    return -1;
}

//*********** ChatTracker functions **************

// These functions simply delegate to ChatTrackerImpl's functions.
// You probably don't want to change any of this code.

ChatTracker::ChatTracker(int maxBuckets)
{
    m_impl = new ChatTrackerImpl(maxBuckets);
}

ChatTracker::~ChatTracker()
{
    delete m_impl;
}

void ChatTracker::join(string user, string chat)
{
    m_impl->join(user, chat);
}

int ChatTracker::terminate(string chat)
{
    return m_impl->terminate(chat);
}

int ChatTracker::contribute(string user)
{
    return m_impl->contribute(user);
}

int ChatTracker::leave(string user, string chat)
{
    return m_impl->leave(user, chat);
}

int ChatTracker::leave(string user)
{
    return m_impl->leave(user);
}

