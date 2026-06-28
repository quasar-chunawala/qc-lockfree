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

            _mm_pause();                
        }

        while(true){
            for(int i{0}; i < iterations[2]; ++i){
                if(try_lock())
                    return;

                _mm_pause();                    
                _mm_pause();
                _mm_pause();
                _mm_pause();
                _mm_pause();
                _mm_pause();
                _mm_pause();
                _mm_pause();
                _mm_pause();
                _mm_pause();
            }
        }
    }

    private:
    std::atomic<bool> flag{false};
};