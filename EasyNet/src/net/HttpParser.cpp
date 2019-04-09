#include "EasyNet/include/base/TimeStamp.h"
#include "EasyNet/include/net/HttpParser.h"
#include "EasyNet/include/net/ChannelBuffer.h"


using namespace Net;

bool HttpParser::parseRequestLine(const char* begin, const char* end, HttpParser* context)
{
    bool succeed = false;
    const char* start = begin;
    const char* space = std::find(start, end, ' ');
    HttpRequest& request = context->request();
    if (space != end && request.SetMethod(start, space))
    {
        start = space+1;
        space = std::find(start, end, ' ');
        if (space != end)
        {
            const char* question = std::find(start, space, '?');
            if (question != space)
            {
                request.setPath(start, question);
                request.setQuery(question, space);
            }
            else
            {
                request.setPath(start, space);
            }
            start = space+1;
            succeed = end-start == 8 && std::equal(start, end-1, "HTTP/1.");
            if (succeed)
            {
                if (*(end-1) == '1')
                {
                    request.SetVersion(HttpRequest::kHttp11);
                }
                else if (*(end-1) == '0')
                {
                    request.SetVersion(HttpRequest::kHttp10);
                }
                else
                {
                    succeed = false;
                }
            }
        }
    }
    return succeed;
}

bool HttpParser::parseRequest(ChannelBuffer* buf, HttpParser* context)
{
    bool ok = true;
    bool hasMore = true;
    while (hasMore)
    {
        if (context->expectRequestLine())
        {
            const char* crlf = buf->FindCRLF();
            if (crlf)
            {
                ok = parseRequestLine(buf->GetReadPointer(), crlf, context);
                if (ok)
                {          
                    Timestamp receiveTime = Timestamp::Now();
                    context->request().setReceiveTime(receiveTime);
                    buf->Retrieve(crlf + 2 - buf->GetReadPointer());
                    context->receiveRequestLine();
                }
                else
                {
                    hasMore = false;
                }
            }
            else
            {
                hasMore = false;
            }
        }
        else if (context->expectHeaders())
        {
            const char* crlf = buf->FindCRLF();
            if (crlf)
            {
                const char* colon = std::find(buf->GetReadPointer(), crlf, ':');
                if (colon != crlf)
                {
                    context->request().addHeader(buf->GetReadPointer(), colon, crlf);
                }
                else
                {
                    // empty line, end of header
                    context->receiveHeaders();
                    hasMore = !context->gotAllHeaders();
                }
                buf->Retrieve(crlf+2 - buf->GetReadPointer());
            }
            else
            {
                hasMore = false;
            }
        }
        else if (context->expectBody())
        {
            
        }
    }
    return ok;
}