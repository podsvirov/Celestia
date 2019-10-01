
#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <string>
#include <celutil/util.h>
#include <celutil/utf8.h>

class AstroObject;
class PlanetarySystem;

class Name
{
protected:
    std::shared_ptr<std::string> m_ptr { nullptr };
//     size_t m_hash { 0 };
//     void makeHash();
    const static std::string m_empty;
public:
    Name(const char *s) {
        if (s == nullptr)
            m_ptr = std::make_shared<std::string>(m_empty);
        else
            m_ptr = std::make_shared<std::string>(s);
    }
    Name(const std::string& s) { m_ptr = std::make_shared<std::string>(s); }
    Name() : m_ptr { nullptr } {}
    Name(const Name& n)
    {
        m_ptr = n.ptr();
//         m_hash = n.m_hash;
    }
    const std::shared_ptr<std::string>& ptr() const { return m_ptr; }
//     size_t hash() const { return m_hash; }
    bool empty() const { return null() ? true : m_ptr->empty(); }
    const std::string& str() const { return null() ? m_empty : *m_ptr; }
    Name & operator=(const Name &n);
    Name & operator=(const std::string &n) { m_ptr = std::make_shared<std::string>(n); return *this; }
    Name & operator=(const char *n) { m_ptr = std::make_shared<std::string>(n); return *this; }
    operator const std::string() const { return str(); }
    bool null() const { return !m_ptr; }
    size_t length() const { return null() ? 0 : m_ptr->length(); }
    const char *c_str() const { return str().c_str(); }
    static const Name getEmpty() { return m_empty; }
};

bool inline operator==(const Name& n1, const Name& n2)
{
    return n1.ptr() == n2.ptr() || n1.str() == n2.str();
}

bool inline operator!=(const Name& n1, const Name& n2)
{
    return !(n1 == n2);
}

bool inline operator<(const Name &n1, const Name &n2)
{
    return n1.str() < n2.str();
}

bool inline operator>(const Name &n1, const Name &n2)
{
    return n1.str() > n2.str();
}

struct NameGreaterThanPredicate
{
    bool operator()(const Name &n1, const Name &n2)
    {
        return n1.ptr() < n2.ptr();
    }
};
/*
struct NameHash
{
    size_t operator()(const Name& h) const { return h.hash(); }
};

struct NameComp
{
    size_t operator()(const Name & n1, const Name &n2) const { return n1.hash() == n2.hash(); }
};
*/

class NameInfo
{
    Name m_canonical;
    Name m_localized;
    Name m_domain;
    AstroObject *m_object { nullptr };
    PlanetarySystem *m_system { nullptr };
    NameInfo() = default;
 public:
    typedef std::shared_ptr<NameInfo> SharedPtr;
    typedef std::shared_ptr<const NameInfo> SharedConstPtr;
    NameInfo(const std::string& val, const Name domain, AstroObject *o, PlanetarySystem *p = nullptr, bool greek = true)
    {
        std::string _val = greek ? ReplaceGreekLetterAbbr(val) : val;
        m_canonical = _val;
        m_domain = domain;
        m_object = o;
        m_system = p;
    }
    NameInfo(const Name& val, const Name domain,  AstroObject *o, PlanetarySystem *p = nullptr)
    {
        m_canonical = val;
        m_domain = domain;
        m_object = o;
        m_system = p;
    }
    NameInfo(const NameInfo &other)
    {
        m_canonical = other.m_canonical;
        m_localized = other.m_localized;
        m_domain = other.m_domain;
        m_object = other.m_object;
        m_system = other.m_system;
    }
    bool hasLocalized() const;
    const Name getCanon() const;
    const Name getLocalized(bool = true) const;
    void translate();
    const AstroObject *getObject() const { return m_object; }
    AstroObject* getObject() { return m_object; }
//     PlanetarySystem *getSystem() { return m_system; }
    PlanetarySystem *getSystem() const { return m_system; }
    static const SharedConstPtr nullPtr;
    static NameInfo::SharedConstPtr createShared(const Name, const Name, AstroObject *, PlanetarySystem * = nullptr);
    static NameInfo::SharedConstPtr createShared(const std::string&, const Name, AstroObject *, PlanetarySystem * = nullptr, bool greek = true);
    static void runTranslation();
    static void stopTranslation();
 protected:
    std::recursive_mutex m_mutex;
    static std::queue<SharedPtr> m_trQueue;
    static std::mutex m_trquMutex;
    static std::condition_variable m_trquNotifier;
    static std::thread m_trThread;
    static void pushForTr(NameInfo::SharedPtr);
    static NameInfo::SharedPtr popForTr();
    static void trThread();
};

bool inline operator==(const NameInfo &n1, const NameInfo &n2)
{
    return n1.getCanon().str() == n2.getCanon().str() && n1.getSystem() == n2.getSystem();
}

bool inline operator!=(const NameInfo &n1, const NameInfo &n2)
{
    return n1.getCanon().str() != n2.getCanon().str() || n1.getSystem() != n2.getSystem();
}

bool inline operator<(const NameInfo &n1, const NameInfo &n2)
{
    return n1.getCanon().str() < n2.getCanon().str() || n1.getSystem() < n2.getSystem();
}

bool inline operator>(const NameInfo &n1, const NameInfo &n2)
{
    return n1.getCanon().str() > n2.getCanon().str() || n1.getSystem() > n2.getSystem();
}

typedef std::set<NameInfo> NameInfoSet;
typedef std::set<NameInfo::SharedPtr> SharedNameInfoSet;
typedef std::set<NameInfo::SharedConstPtr> SharedConstNameInfoSet;
typedef std::set<Name> NameSet;
typedef std::map<Name, NameInfo> NameMap;
typedef std::map<Name, NameInfo::SharedConstPtr> SharedNameMap;
