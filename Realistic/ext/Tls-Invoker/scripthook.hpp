#include <Includes.h>
#include <singeton.hpp>


using namespace std;
class Hooks : public Singleton<Hooks>
{
private:
    void Thread();
public:

    void Initialize();
    void UnHook();
};

inline auto g_tls_hook = new Hooks;