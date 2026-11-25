
#include "../include/banking.h"

#include <stdexcept>
#include <utility>

// BankAccount

BankAccount::BankAccount() 
    : id(++counter), owner("Unknown"), balance(0.0) {
}

BankAccount::BankAccount(const std::string& name, const double amount) 
    : id(++counter), owner(name) {
    if (amount < 0.0) {
        throw std::invalid_argument("Negative initial balance");
    }
    balance = amount;
}

BankAccount::BankAccount(BankAccount&& other) 
    : id(++counter), owner(std::move(other.owner)), balance(std::exchange(other.balance, 0.0)) {
}

BankAccount& BankAccount::operator=(BankAccount&& rhs) {
    if (this != &rhs) {
        owner = std::move(rhs.owner);
        balance = std::exchange(rhs.balance, 0.0);
    }

    return *this;
}

BankAccount::~BankAccount() {
    --counter;
}

void BankAccount::deposit(const double amount) {
    if (amount <= 0.0) {
        throw std::invalid_argument("Deposit must be positive");
    }

    balance += amount;
}

void BankAccount::withdraw(const double amount) {
    if (amount <= 0.0) {
        throw std::invalid_argument("Withdrawal must be positive");
    }

    if (amount > balance) {
        throw std::runtime_error("Insufficient funds");
    }

    balance -= amount;
}

size_t BankAccount::get_id() const {
    return id;
}

double BankAccount::get_balance() const {
    return balance;
}

bool BankAccount::operator==(const BankAccount& rhs) const {
    return id == rhs.id;
}

bool BankAccount::operator<(const BankAccount& rhs) const {
    if (balance != rhs.balance) {
        return balance < rhs.balance;
    }

    return id < rhs.id;
}

std::ostream& operator<<(std::ostream& os, const BankAccount& account) {
    os << "ID: " << account.id << ", Owner: " << account.owner << ", Balance: " << account.balance;
    return os;
}

// CreditAccount

CreditAccount::CreditAccount()
    : BankAccount(), credit_limit(0.0) {}

CreditAccount::CreditAccount(const std::string& name,
                             const double amount)
    : BankAccount(name, amount), credit_limit(0.0) {}

CreditAccount::CreditAccount(CreditAccount&& other)
    : BankAccount(std::move(other)),
      credit_limit(std::exchange(other.credit_limit, 0.0)) {}

CreditAccount& CreditAccount::operator=(CreditAccount&& rhs) {
    if (this != &rhs) {
        BankAccount::operator=(std::move(rhs));

        credit_limit = std::exchange(rhs.credit_limit, 0.0);
    }

    return *this;
}

void CreditAccount::withdraw(const double amount) {
    if (amount <= 0.0) {
        throw std::invalid_argument("Withdrawal must be positive");
    }

    if (amount > balance + credit_limit) {
        throw std::runtime_error("Credit limit exceeded");
    }

    balance -= amount;
}

void CreditAccount::set_limit(const double amount) {
    if (amount <= 0.0) {
        throw std::invalid_argument("Limit increase must be positive");
    }

    credit_limit += amount;
}


// DebitAccount

void DebitAccount::withdraw(const double amount) {
    BankAccount::withdraw(amount);
}

void DebitAccount::reverse_happiness() {
    happy = !happy;
}

bool DebitAccount::is_happy() const {
    return happy;
}


// DebitJunior

DebitJunior::DebitJunior(DebitAccount*& p) : parent(p) {
    balance = parent->get_balance();
    owner = "child of user with ID " + std::to_string(parent->get_id());
}

DebitJunior::DebitJunior(DebitJunior&& other) 
    : DebitAccount(std::move(other)), parent(std::move(other.parent)) {
}
DebitJunior& DebitJunior::operator=(DebitJunior&& rhs) {
    if (this != &rhs) {
        DebitAccount::operator=(std::move(rhs));

        parent = std::move(rhs.parent);
    }   
    return *this;
}

void DebitJunior::withdraw(const double amount) {
    if (parent == nullptr || !parent->is_happy()) {
        throw std::runtime_error("Parent account is unavailable");
    }

    DebitAccount::withdraw(amount);
}
