#pragma once
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>

// This table is used for admission records whose values do not own their own key.
// Dead keys are removed when the next operation accesses the table.
template<class Key, class Value> class ConditionalWeakTable {
    using WeakKey = std::weak_ptr<typename Key::element_type>;
    std::map<WeakKey, Value, std::owner_less<WeakKey>> entries_;
    std::mutex mutex_;
    void prune() { for (auto it = entries_.begin(); it != entries_.end();) { if (it->first.expired()) it = entries_.erase(it); else ++it; } }
public:
    void Add(const Key& key, const Value& value) {
        if (!key || !value) throw std::invalid_argument("ConditionalWeakTable requires non-null key and value");
        std::lock_guard lock(mutex_); prune();
        if (!entries_.emplace(WeakKey(key), value).second) throw std::invalid_argument("Duplicate key");
    }
    bool TryGetValue(const Key& key, Value& result) {
        if (!key) throw std::invalid_argument("ConditionalWeakTable requires non-null key");
        std::lock_guard lock(mutex_); prune();
        auto it = entries_.find(WeakKey(key));
        if (it == entries_.end()) { result = {}; return false; }
        result = it->second; return true;
    }
    bool Remove(const Key& key) {
        if (!key) throw std::invalid_argument("ConditionalWeakTable requires non-null key");
        std::lock_guard lock(mutex_); prune(); return entries_.erase(WeakKey(key)) != 0;
    }
};
// Membership-only weak set for detecting repeated issuance for an operand.
template<class Key> class WeakIdentitySet {
    using WeakKey = std::weak_ptr<typename Key::element_type>;
    std::map<WeakKey, bool, std::owner_less<WeakKey>> entries_;
    std::mutex mutex_;
    void prune() { for (auto it = entries_.begin(); it != entries_.end();) { if (it->first.expired()) it = entries_.erase(it); else ++it; } }
public:
    void Add(const Key& key) {
        if (!key) throw std::invalid_argument("WeakIdentitySet requires a non-null key");
        std::lock_guard lock(mutex_); prune();
        if (!entries_.emplace(WeakKey(key), true).second) throw std::invalid_argument("Duplicate key");
    }
    bool Contains(const Key& key) {
        if (!key) throw std::invalid_argument("WeakIdentitySet requires a non-null key");
        std::lock_guard lock(mutex_); prune(); return entries_.find(WeakKey(key)) != entries_.end();
    }
};