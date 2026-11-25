#pragma once

#include <string>
#include <iostream>

class BankAccount {
protected: // !
    inline static size_t counter = 0;
    const size_t id;
    std::string owner;
    double balance;

public:
    // конструкторы, деструктор и операторы copy/move
    BankAccount();
    BankAccount(const std::string& name, const double amount);
    BankAccount(const BankAccount& other) = delete;
    BankAccount(BankAccount&& other);
    BankAccount& operator=(const BankAccount& rhs) = delete;
    BankAccount& operator=(BankAccount&& rhs);
    virtual ~BankAccount(); // !

    // методы
    void deposit(const double amount);
    size_t get_id() const;
    double get_balance() const;
    virtual void withdraw(double amount);

    // операторы прочие
    bool operator==(const BankAccount& rhs) const;
    bool operator<(const BankAccount& rhs) const;
    friend std::ostream& operator<<(std::ostream& os, const BankAccount& account); // !
};


class CreditAccount final : public BankAccount {
protected:
    double credit_limit;

public:
    CreditAccount();
    CreditAccount(const std::string& name, const double amount);
    CreditAccount(CreditAccount&& other);
    CreditAccount& operator=(CreditAccount&& rhs);

    virtual void withdraw(double amount) override;
    void set_limit(double amount);
};


class DebitAccount : public BankAccount {
protected:
    bool happy = true;

public:
    using BankAccount::BankAccount; // !

    void withdraw(double amount) override;
    void reverse_happiness();
    bool is_happy() const;
};


class DebitJunior final : public DebitAccount {
public:
    DebitAccount* parent = nullptr;

    DebitJunior() = delete;
    DebitJunior(DebitAccount*& p);
    DebitJunior(DebitJunior&& other);
    DebitJunior& operator=(DebitJunior&& rhs);

    void withdraw(double amount) override;
};