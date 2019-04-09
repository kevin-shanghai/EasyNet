#ifndef HTTP_CONTEXT_H__
#define HTTP_CONTEXT_H__
#include "EasyNet/include/net/HttpRequest.h"


namespace Net
{
    class ChannelBuffer;
    class HttpParser
    {
    public:
        enum HttpRequestParseState
        {
            kExpectRequestLine,
            kExpectHeaders,
            kExpectBody,
            kGotAll,
        };

        HttpParser() : state_(kExpectRequestLine)
        {
        }


        bool expectRequestLine() const
        { return state_ == kExpectRequestLine; }

        bool expectHeaders() const
        { return state_ == kExpectHeaders; }

        bool expectBody() const
        { return state_ == kExpectBody; }

        bool gotAllHeaders() const
        { return state_ == kGotAll; }

        void receiveRequestLine()
        { state_ = kExpectHeaders; }

        void receiveHeaders()
        { state_ = kGotAll; }  // FIXME

        void reset()
        {
            state_ = kExpectRequestLine;
            HttpRequest temp;
            request_.swap(temp);
        }

        const HttpRequest& request() const
        { return request_; }

        HttpRequest& request() { return request_; }

        static bool parseRequestLine(const char* begin, const char* end, HttpParser* context);
        static bool parseRequest(ChannelBuffer* buf, HttpParser* context);

    private:
        HttpRequestParseState state_;
        HttpRequest request_;
    };
}




#endif