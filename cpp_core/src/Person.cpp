#include "Person.h"
#include "Validation.h"

#include <sstream>

using namespace std;
Person::Person()
    : id(""), name(""), age(0), contact(""), address("") {}

Person::Person(const string& id, const string& name, int age,
               const string& contact, const string& address)
    : id(Validation::sanitizeField(id)),
      name(Validation::sanitizeField(name)),
      age(age),
      contact(Validation::sanitizeField(contact)),
      address(Validation::sanitizeField(address)) {}

Person::~Person() {}

string Person::getId() const { return id; }
string Person::getName() const { return name; }
int Person::getAge() const { return age; }
string Person::getContact() const { return contact; }
string Person::getAddress() const { return address; }

void Person::setId(const string& id) { this->id = Validation::sanitizeField(id); }
void Person::setName(const string& name) { this->name = Validation::sanitizeField(name); }
void Person::setAge(int age) { this->age = age; }
void Person::setContact(const string& contact) { this->contact = Validation::sanitizeField(contact); }
void Person::setAddress(const string& address) { this->address = Validation::sanitizeField(address); }

string Person::displayInfo() const {
    ostringstream out;
    out << "ID: " << id << ", Name: " << name << ", Age: " << age
        << ", Contact: " << contact << ", Address: " << address;
    return out.str();
}
