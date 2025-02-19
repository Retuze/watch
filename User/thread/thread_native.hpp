#ifndef THREAD_NATIVE_HPP
#define THREAD_NATIVE_HPP

#include <..\rt-thread\include\rtthread.h>
#include <..\rt-thread\bsp\rtconfig.h>
#include <functional>

extern "C" {
    static inline  void* __thread_create(void (*entry)(void*), void* arg, const char* name, 
                         rt_uint32_t stack_size, rt_uint8_t priority) {
        return rt_thread_create(name, entry, arg, stack_size, priority, 10);
    }
    static inline void __thread_join(void* thread) {
        rt_thread_startup((rt_thread_t)thread);
    }
    static inline void __thread_delete(void* thread) {
        rt_thread_delete((rt_thread_t)thread);
    }
}

namespace std {

class thread {
private:
    void* m_handle;
    
    template<typename F>
    static void thread_func(void* arg) {
        F* f = static_cast<F*>(arg);
        (*f)();
        delete f;
    }

public:
    thread() : m_handle(nullptr) {}
    
    template<typename Function, typename... Args>
    explicit thread(Function&& f, Args&&... args) {
        m_handle = new std::thread(std::forward<Function>(f), 
                                  std::forward<Args>(args)...);
    }
    
    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;
    
    thread(thread&& other) noexcept {
        m_handle = other.m_handle;
        other.m_handle = nullptr;
    }
    
    thread& operator=(thread&& other) noexcept {
        if (m_handle) {
            join();
        }
        m_handle = other.m_handle;
        other.m_handle = nullptr;
        return *this;
    }
    
    ~thread() {
        if (joinable()) {
            rt_kprintf("Error: thread was not joined\n");
        }
    }
    
    bool joinable() const noexcept {
        return m_handle != nullptr;
    }
    
    void join() {
        if (!joinable()) {
            rt_kprintf("Error: cannot join non-joinable thread\n");
            return;
        }
        __thread_join(m_handle);
        __thread_delete(m_handle);
        m_handle = nullptr;
    }
    
    void detach() {
        if (!joinable()) {
            rt_kprintf("Error: cannot detach non-joinable thread\n");
            return;
        }
        m_handle = nullptr;
    }
};

} // namespace std

#endif // THREAD_NATIVE_HPP
