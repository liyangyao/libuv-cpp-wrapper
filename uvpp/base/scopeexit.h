/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/6/26

****************************************************************************/

#ifndef UVPP_SCOPEEXIT_H
#define UVPP_SCOPEEXIT_H

#include <functional>

class ScopeGuard
{
public:
    explicit ScopeGuard(std::function<void()> onExitScope)
        : m_onExitScope(onExitScope), m_dismissed(false)
    { }

    ~ScopeGuard()
    {
        if(!m_dismissed)
        {
            m_onExitScope();
        }
    }

    void Dismiss()
    {
        m_dismissed = true;
    }

private:
    std::function<void()> m_onExitScope;
    bool m_dismissed;

private: // noncopyable
    ScopeGuard(ScopeGuard const&);
    ScopeGuard& operator=(ScopeGuard const&);
};

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)
#define ON_SCOPE_EXIT(callback) ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)

#endif // SCOPEEXIT_H
