#ifndef QTMEMORYPOOL_H
#define QTMEMORYPOOL_H

#include <atomic>

#include <QtGlobal>

/*!
 * \brief The spin_lock class
 *
 * Simple Spin Lock implementation.
 *
 */
class QtSpinLock
{
public:
    inline void lock() {
        while(lck.test_and_set(std::memory_order_acquire))
        {}
    }

    inline void unlock() {
        lck.clear(std::memory_order_release);
    }

private:
    std::atomic_flag lck = ATOMIC_FLAG_INIT;
};




/*!
 * \brief The QtMemoryPool class
 *
 *  The QtMemoryPool is used for basic free list logic
 *
 * \tparam
 */
template<class T, class _Mutex = void>
class QtMemoryPool
{
    Q_STATIC_ASSERT_X(sizeof(T) >= sizeof(void*),
                  "size of T must be greater of equal than size of pointer");
public:
    QtMemoryPool()
        : _Head(0)
    {	// construct with empty list
    }

    inline ~QtMemoryPool() {
        mtx.lock();
        T* _Ptr = pop();
        for(;_Ptr != Q_NULLPTR; _Ptr = pop()) {
            delete _Ptr;
        }
        mtx.unlock();
    }

    inline void push(T *_Ptr)
    {	// push onto free list
        mtx.lock();
        ((Node*)_Ptr)->_Next = _Head;
        _Head = (Node*)_Ptr;
        mtx.unlock();
    }

    inline T *pop()
    {	// pop node from free list
        mtx.lock();
        void *_Ptr = _Head;
        if (_Ptr != 0)
        {	// relink
            _Head = _Head->_Next;
        }
        mtx.unlock();
        return static_cast<T*>(_Ptr);
    }

private:
    struct Node
    {	// list node
        Node *_Next;
    };
    Node *_Head;
    _Mutex mtx;
};


template<class T>
class QtMemoryPool<T, void>
{
    Q_STATIC_ASSERT_X(sizeof(T) >= sizeof(void*),
                  "size of T must be greater of equal than size of pointer");
public:
    QtMemoryPool()
        : _Head(0)
    {	// construct with empty list
    }

    inline ~QtMemoryPool() {
        T* _Ptr = pop();
        for(;_Ptr != Q_NULLPTR; _Ptr = pop()) {
            delete _Ptr;
        }
    }

    inline void push(T *_Ptr)
    {	// push onto free list
        ((Node*)_Ptr)->_Next = _Head;
        _Head = (Node*)_Ptr;
    }

    inline T *pop()
    {	// pop node from free list
        void *_Ptr = _Head;
        if (_Ptr != 0)
        {	// relink
            _Head = _Head->_Next;
        }
        return static_cast<T*>(_Ptr);
    }

private:
    struct Node
    {	// list node
        Node *_Next;
    };
    Node *_Head;
};


#endif // QTMEMORYPOOL_H
