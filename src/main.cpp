#include <stdio.h>

#include "Nrl/Utils.hpp"
#include "Nrl/Option.hpp"

class Dummy {
public:
    [[nodiscard]] static Dummy New(const char* name) {
        printf("Dummy %s created\n", name);
        return Dummy(name);
    }

    ~Dummy(void) {
        if (m_Name) {
            printf("Dummy %s destroyed\n", m_Name);
            m_Name = nullptr;
        }
    }

    Dummy(const Dummy& other)
    : m_Name(other.m_Name) {
        printf("Dummy %s copied\n", m_Name);
    }

    Dummy& operator=(const Dummy& other) {
        if (this == &other)
            return *this;

        printf("Dummy %s copied into %s\n", other.m_Name, m_Name);
        m_Name = other.m_Name;

        return *this;
    }

    Dummy(Dummy&& other) noexcept
    : m_Name(Nrl::Exchange(other.m_Name, nullptr)) {
        printf("Dummy %s moved\n", m_Name);
    }

    Dummy& operator=(Dummy&& other) noexcept {
        if (this == &other)
            return *this;

        printf("Dummy %s moved into %s\n", other.m_Name, m_Name);
        m_Name = Nrl::Exchange(other.m_Name, nullptr);

        return *this;
    }

    void greet(void) const {
        printf("%s says hello!\n", m_Name);
    }

    [[nodiscard]] constexpr static Dummy _None(void) { return Dummy(nullptr); }
    [[nodiscard]] constexpr bool _is_some(void) const { return m_Name; }
private:
    Dummy(const char* name) : m_Name(name) {}
private:
    const char* m_Name;
};

int main(int argc, char* argv[]) {
    printf("=====================\n");
    {
        auto d1 = Nrl::SomeWith<Dummy>(Dummy::New, "Jack");

        d1.ref().unwrap()->greet();

        if (d1.is_some()) {
            Dummy u = d1.unwrap();
        }

        auto d2 = Nrl::None<Dummy>();
        if (d2.is_some()) {
            Dummy u = d2.unwrap();
        }

        d2 = Nrl::Move(d2);
    }
    printf("=====================\n");
}
