#ifndef PERSON_H
#define PERSON_H

#include <string>

using namespace std;

class Person {
protected:
    string id;
    string name;
    int age;
    string contact;
    string address;

public:
    Person();
    Person(const string& id, const string& name, int age,
           const string& contact, const string& address);
    virtual ~Person();

    string getId() const;
    string getName() const;
    int getAge() const;
    string getContact() const;
    string getAddress() const;

    void setId(const string& id);
    void setName(const string& name);
    void setAge(int age);
    void setContact(const string& contact);
    void setAddress(const string& address);

    virtual string displayInfo() const;
};

#endif
