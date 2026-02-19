#pragma once

#include "system_stats.h"
#include "user.h"

#include <nlohmann/json.hpp>

#include <optional>
#include <string>
#include <unordered_map>


namespace api {

class HostApiManager {
    using jsonNode = std::unordered_map<std::string, nlohmann::json>;

    template <bool OnlyValid = true, class Op, class... Args>
        requires std::invocable<Op, HostApiManager*, Args...>
    HostApiManager& defer(Op&& operation, Args&&... args);

public:
    HostApiManager(
        std::string host,
        std::string username,
        std::string password);

    HostApiManager& run();

    HostApiManager& clear();

    bool isCompleted();

    HostApiManager& init();

    HostApiManager& loadUsers();

    HostApiManager& addUser(const user::User& user);

    HostApiManager& saveUsers();

    template <class Callable>
        requires std::invocable<Callable, user::User&>
    HostApiManager& visitUser(const std::string& name, Callable visitor);

    template <class Callable>
        requires std::invocable<Callable, user::User&>
    HostApiManager& visitUsers(Callable visitor);

    const user::User& getUser(const std::string& name);

    SystemStats getSystemStats() const;

private:
    bool getToken();

    bool loadUsersImpl();

    bool updateUser(const std::string& name, const user::User& user);

    bool checkToken();

    const std::string host_;
    const std::string username_;
    const std::string password_;

    std::optional<std::string> token_;
    std::unordered_map<std::string, user::User> users_;

    bool valid_;

    std::function<void()> deferredOp_;
    std::shared_ptr<HostApiManager> self;
};


template <bool OnlyValid, class Op, class... Args>
    requires std::invocable<Op, HostApiManager*, Args...>
HostApiManager& HostApiManager::defer(Op&& operation, Args&&... args)
{
    auto weakThis = std::weak_ptr(self);

    deferredOp_ =
        [weakThis,
            prevOp    = std::move(deferredOp_),
            args      = std::make_tuple(std::forward<Args>(args)...),
            operation = std::forward<Op>(operation)]() mutable
    {
        if (prevOp)
            prevOp();

        auto lockedThis = weakThis.lock();
        if (!lockedThis || OnlyValid && !lockedThis->valid_)
            return;

        std::apply(
            [&](auto&&... args)
            { operation(lockedThis.get(), std::forward<decltype(args)>(args)...); },
            std::move(args));
    };

    return *this;
}

template <class Callable>
    requires std::invocable<Callable, user::User&>
HostApiManager& HostApiManager::visitUser(const std::string& name, Callable visitor)
{
    return defer(
        [](HostApiManager* self, const std::string& name, Callable visitor)
        {
            if (self->users_.contains(name))
                visitor(self->users_[name]);
            else
                self->valid_ = false;
        }, name, visitor);
}

template <class Callable>
    requires std::invocable<Callable, user::User&>
HostApiManager& HostApiManager::visitUsers(Callable visitor)
{
    return defer(
        [](HostApiManager* self, Callable visitor)
        {
            for (auto& [_, user] : self->users_) {
                visitor(user);
            }
        },
        visitor);
}

std::unique_ptr<HostApiManager> createHostApiManager();

} // namespace api
