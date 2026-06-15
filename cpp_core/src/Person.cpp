#include "Person.h"
#include "Validation.h"

#include <sstream>

Person::Person()
    : id(""), name(""), age(0), contact(""), address("") {}

Person::Person(const std::string& id, const std::string& name, int age,
               const std::string& contact, const std::string& address)
    : id(Validation::sanitizeField(id)),
      name(Validation::sanitizeField(name)),
      age(age),
      contact(Validation::sanitizeField(contact)),
      address(Validation::sanitizeField(address)) {}

Person::~Person() {}

std::string Person::getId() const { return id; }
std::string Person::getName() const { return name; }
int Person::getAge() const { return age; }
std::string Person::getContact() const { return contact; }
std::string Person::getAddress() const { return address; }

void Person::setId(const std::string& id) { this->id = Validation::sanitizeField(id); }
void Person::setName(const std::string& name) { this->name = Validation::sanitizeField(name); }
void Person::setAge(int age) { this->age = age; }
void Person::setContact(const std::string& contact) { this->contact = Validation::sanitizeField(contact); }
void Person::setAddress(const std::string& address) { this->address = Validation::sanitizeField(address); }

std::string Person::displayInfo() const {
    std::ostringstream out;
    out << "ID: " << id << ", Name: " << name << ", Age: " << age
        << ", Contact: " << contact << ", Address: " << address;
    return out.str();
}
