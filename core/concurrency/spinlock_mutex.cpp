#include <array>
#include <thread>
#include <atomic>
#include <emmintrin.h>

struct spinlock_mutex{
    bool try_lock() noexcept{
        bool isUnlocked = false;
        return !flag.compare_exchange_strong(isUnlocked, true, std::memory_order_acquire);
    }

    void unlock() noexcept{
        flag.store(false, std::memory_order_release);
    }

    void pause_cpu(){
        #if defined(__x86_64) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
            _mm_pause();
        #elif defined(__arch64__) || defined((__arm__))
            __asm__ volatile("yield");
        #endif
    }

    // Exponential backoff strategy
    void lock() noexcept{
        constexpr std::array iterations{ 5, 10, 3000 };

        for(auto i{0uz}; i < iterations[0]; ++i){
            if(try_lock())
                return;                
        }

        for(int i{0}; i < iterations[1]; ++i){
            if(try_lock())
                return;

            
            pause_cpu();                
        }

        while(true){
            for(int i{0}; i < iterations[2]; ++i){
                if(try_lock())
                    return;

                pause_cpu();                    
                pause_cpu();
                pause_cpu();
                pause_cpu();
                pause_cpu();
                pause_cpu();
                pause_cpu();
                pause_cpu();
                pause_cpu();
                pause_cpu();
            }
        }
    }

    private:
    std::atomic<bool> flag{false};
};