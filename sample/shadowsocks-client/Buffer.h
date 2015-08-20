/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/16

****************************************************************************/

#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <functional>
#include <QObject>

class Buffer
{
    class BufferData
    {
    public:
        explicit BufferData()
        {
            readerIndex_ = 0;
            writerIndex_ = 0;
        }

        explicit BufferData(const char* data, size_t len)
        {
            readerIndex_ = 0;
            writerIndex_ = 0;
            append(data, len);
        }

        size_t writableBytes() const
        {
            return buffer_.size() - writerIndex_;
        }

        size_t readableBytes() const
        {
            return writerIndex_ - readerIndex_;
        }

        void ensureWritableBytes(size_t len)
        {
            if (writableBytes() < len)
            {
                makeSpace(len);
            }
            Q_ASSERT(writableBytes() >= len);
        }

        char* beginWrite()
        {
            return begin() + writerIndex_;
        }

        const char* beginWrite() const
        {
            return begin() + writerIndex_;
        }

        void hasWritten(size_t len)
        {
            Q_ASSERT(len <= writableBytes());
            writerIndex_ += len;
        }

        void unwrite(size_t len)
        {
            Q_ASSERT(len <= readableBytes());
            writerIndex_ -= len;
        }

        void append(const char* /*restrict*/ data, size_t len)
        {
            ensureWritableBytes(len);
            std::copy(data, data+len, beginWrite());
            hasWritten(len);
        }

        const char* peek()
        {
            return begin();
        }

        template<typename T>
        T peek()
        {
            static_assert(std::is_integral<T>::value, "integral required.");
            T* p = reinterpret_cast<T*>(begin());
            return *p;
        }

        template<typename T>
        T read()
        {
            static_assert(std::is_integral<T>::value, "integral required.");
            T* p = reinterpret_cast<T*>(begin());
            retrieve(sizeof T);
            return *p;
        }

        void retrieve(size_t len)
        {
            if (len < readableBytes())
            {
                readerIndex_ += len;
            }
            else
            {
                retrieveAll();
            }
        }

        void retrieveAll()
        {
            readerIndex_ = 0;
            writerIndex_ = 0;
        }

        QString retrieveAsString(size_t len)
        {
            Q_ASSERT(len <= readableBytes());
            QString result = QString::fromUtf8(peek(), len);
            retrieve(len);
            return result;
        }

        QString retrieveAsString()
        {
            return retrieveAsString(readableBytes());
        }


    private:
        char* begin()
        {
            return &*buffer_.begin();
        }

        const char* begin() const
        {
            return &*buffer_.begin();
        }

        void makeSpace(size_t len)
        {
            if (writableBytes()< len)
            {
                // FIXME: move readable data
                buffer_.resize(writerIndex_+len);
            }
            else
            {
                // move readable data to the front, make space inside buffer
                size_t readable = readableBytes();
                std::copy(begin()+readerIndex_,
                          begin()+writerIndex_,
                          begin());
                readerIndex_ = 0;
                writerIndex_ = readerIndex_ + readable;
            }
        }
    private:
        std::vector<char> buffer_;
        size_t readerIndex_;
        size_t writerIndex_;

    };

    typedef std::shared_ptr<BufferData> BufferDataPtr;
public:
    explicit Buffer():
        m_ptr(new BufferData)
    {

    }

    Buffer(const QByteArray& data)
    {
        qDebug()<<"Buffer QByteArray";
        m_ptr = std::make_shared<BufferData>(data.constData(), data.size());
    }
    Buffer(const QString& string)
    {
        qDebug()<<"Buffer QString";
        QByteArray data = string.toUtf8();
        m_ptr = std::make_shared<BufferData>(data.constData(), data.size());
    }
    Buffer(const char *ch, int size = -1)
    {
        qDebug()<<"Buffer char*";
        if (size<0)
        {
            size = strlen(ch);
        }
        m_ptr = std::make_shared<BufferData>(ch, size);
    }
    Buffer(const Buffer& b):
        m_ptr(b.m_ptr)
    {
        //qDebug()<<"Buffer Copy";
    }
    Buffer(Buffer&& b)
    {
        m_ptr.swap(b.m_ptr);
        //qDebug()<<"Buffer Copy RightValue";
    }

    BufferData& data()
    {
        return *m_ptr.get();
    }

    BufferData* const operator ->() const
    {
        return m_ptr.get();
    }

private:
    BufferDataPtr m_ptr;
};



#endif // BUFFER_H
