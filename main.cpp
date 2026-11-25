#include "include/banking.h"
#include "include/deque.h"

#include <iostream>
#include <vector>
#include <memory>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

void deque_test() {
    std::cout << "Deque Test" << '\n';
    Deque deque;
    deque.PushBack(1);
    std::cout << deque.Size() << '\n';

}

struct AccountComparator {
    using AccountPtr = std::unique_ptr<BankAccount>;
    bool operator()(const AccountPtr& lhs,
                    const AccountPtr& rhs) const {
        return *lhs < *rhs;
    }
};

void bank_test() {
    std::cout << "Bank Test\n" << "\nVector\n";
    using AccountPtr = std::unique_ptr<BankAccount>;

    // vector
    auto parent = std::make_unique<DebitAccount>("papa", 1234.00);
    auto creditor = std::make_unique<CreditAccount>("mama", 4200.00);

    DebitAccount* parent_ptr = parent.get();

    std::vector<AccountPtr> accounts;

    accounts.push_back(std::move(parent));
    accounts.push_back(std::move(creditor));
    accounts.push_back(std::make_unique<DebitJunior>(parent_ptr));
    for (const auto& account : accounts) {
        std::cout << *account << '\n';
    }
    std::cout << '\n';
    accounts[2]->withdraw(1000.0);
    for (const auto& account : accounts) {
        std::cout << *account << '\n';
    }
    std::cout << "\nSet\n";
    // set
    std::set<AccountPtr, AccountComparator> account_set;
    account_set.insert(std::make_unique<DebitAccount>("1st", 123.4));
    account_set.insert(std::make_unique<CreditAccount>("2nd", 12.34));
    account_set.insert(std::make_unique<DebitAccount>("3rd", 1.234));

    for (const auto& account : account_set) {
        std::cout << *account << '\n';
    }

    auto it = account_set.begin();

    if (it != account_set.end()) {
        std::cout << "Lowest balance: " << (*it)->get_balance() << '\n';
    }
}

int main() {
    // deque_test();
    std::cout << "\n";
    bank_test();
    return 0;
}