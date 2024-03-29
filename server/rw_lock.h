#ifndef RW_LOCK_H
#define RW_LOCK_H
#include <mutex>

/*
 * A write-preference Read-Write lock - taken from https://github.com/bo-yang/read_write_lock
 */
class ReadWriteLock {
public:
    ReadWriteLock() {
        _nread = _nread_waiters = 0;
        _nwrite = _nwrite_waiters = 0;
    }

    void read_lock() {
        std::unique_lock<std::mutex> lck(_mtx);
        if (_nwrite || _nwrite_waiters) {
            _nread_waiters++;
            while (_nwrite || _nwrite_waiters)
                _rcond.wait(lck); // calls lck.unlock() inherently, lck.lock() is called after notified.
            _nread_waiters--;
        }
        _nread++;
    }

    void read_unlock() {
        std::unique_lock<std::mutex> lck(_mtx);
        _nread--;
        if (_nwrite_waiters)
            _wcond.notify_one();
    }

    void write_lock() {
        std::unique_lock<std::mutex> lck(_mtx);
        if (_nread || _nwrite) {
            _nwrite_waiters++;
            while (_nread || _nwrite)
                _wcond.wait(lck);
            _nwrite_waiters--;
        }
        _nwrite++;
    }

    void write_unlock() {
        std::unique_lock<std::mutex> lck(_mtx);
        _nwrite--;
        if (_nwrite_waiters) // write-preference
            _wcond.notify_one();
        else if (_nread_waiters)
            _rcond.notify_all();
    }
    // added to let to know if write operation has been requested at this moment
    bool is_write_lock_requested()
    {
        std::unique_lock<std::mutex> lck(_mtx);
        return _nwrite != 0 || _nwrite_waiters != 0;
    }

private:
    std::mutex _mtx;
    std::condition_variable _rcond;
    std::condition_variable _wcond;
    uint32_t _nread, _nread_waiters;
    uint32_t _nwrite, _nwrite_waiters;
};
#endif