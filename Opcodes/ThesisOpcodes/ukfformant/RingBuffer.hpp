//
//  RingBuffer.hpp
//  CsoundLib64
//
//  Created by Edward Costello on 09/08/2018.
//

#ifndef RingBuffer_hpp
#define RingBuffer_hpp

#include <atomic>
#include <cstring>
#include <stdlib.h>

template <typename T>
class RingBuffer {
public:
    RingBuffer();
    RingBuffer( size_t count );
    RingBuffer( RingBuffer &&other );
    ~RingBuffer();
    void resize( size_t count );
    void clear();
    size_t getSize() const;
    size_t getAvailableWrite() const;
    size_t getAvailableRead() const;
    bool write( const T *array, size_t count );
    bool read( T *array, size_t count );
    bool peak( T *array, size_t count );
    bool consume(size_t count );

    

private:
    size_t getAvailableWrite( size_t writeIndex, size_t readIndex ) const;
    size_t getAvailableRead( size_t writeIndex, size_t readIndex ) const;

    T *mData;
    size_t mAllocatedSize;
    std::atomic<size_t> mWriteIndex, mReadIndex;
};



#endif /* RingBuffer_hpp */
