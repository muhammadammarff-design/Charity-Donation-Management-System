#ifndef PERSON_H
#define PERSON_H

#include <string>

class Person {
protected:
    std::string id;
    std::string name;
    int age;
    std::string contact;
    std::string address;

public:
    Person();
    Person(const std::string& id, const std::string& name, int age,
           const std::string& contact, const std::string& address);
    virtual ~Person();

    std::string getId() const;
    std::string getName() const;
    int getAge() const;
    std::string getContact() const;
    std::string getAddress() const;

    void setId(const std::string& id);
    void setName(const std::string& name);
    void setAge(int age);
    void setContact(const std::string& contact);
    void setAddress(const std::string& address);

    virtual std::string displayInfo() const;
};

#endif
