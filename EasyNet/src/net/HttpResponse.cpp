#include "EasyNet/include/net/HttpResponse.h"
#include "EasyNet/include/net/ChannelBuffer.h"
#include "EasyNet/include/base/PlatformDefine.h"
#include <sstream>



using namespace Net;

void HttpResponse::appendToBuffer(ChannelBuffer* output) const
{
   
     std::stringstream ss;
     ss << "HTTP/1.1 " << statusCode_ << " ";
    output->Append(ss.str());
    output->Append(statusMessage_);
    output->Append("\r\n");

    if (closeConnection_)
    {
        output->Append("Connection: close\r\n");
    }
    else
    {
        ss.clear();
        ss.str("");
        std::cout << ss.str() << std::endl;
        ss << "Content-Length: " << body_.size() << "\r\n";
        output->Append(ss.str());
        output->Append("Connection: Keep-Alive\r\n");
    }

    for (auto it = headers_.begin();
        it != headers_.end();
        ++it)
    {
        output->Append(it->first);
        output->Append(": ");
        output->Append(it->second);
        output->Append("\r\n");
    }

    output->Append("\r\n");
    output->Append(body_);
}
