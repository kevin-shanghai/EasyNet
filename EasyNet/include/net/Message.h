#ifndef MESSAGE_H__
#define MESSAGE_H__

#include <string>
#include "Common/NetBaseLight/CommAdaptor/mcsf_commadaptor_message.h"

namespace Net
{

    static const int kNameLength = Mcsf::rt::kNameLength;
    //
    //typedef struct MSGHEAD_
    //{
    //    int32_t CRC;
    //    int32_t CommandId;
    //    int32_t MessageLength;
    //    char sReceiver[kNameLength];
    //    char sSender[kNameLength];

    //    MSGHEAD_(void)
    //    {
    //        MessageLength = 0;
    //        CRC = -1;
    //        CommandId = 0;
    //        (void)memset(sReceiver, 0, kNameLength);
    //        (void)memset(sSender, 0, kNameLength);
    //    }
    //}MSGHEAD;

    //typedef struct MSGPACKAGE_
    //{
    //    MSGHEAD headpack;
    //    std::string strpack;
    //    MSGPACKAGE_(void)
    //    {
    //        strpack = "";
    //    }
    //    MSGPACKAGE_(const MSGPACKAGE_& rhs)
    //    {
    //        memcpy(&this->headpack, &rhs.headpack, sizeof(MSGHEAD));
    //        strpack = rhs.strpack;
    //    }

    //    MSGPACKAGE_& operator =(MSGPACKAGE_ rhs)
    //    {
    //        memcpy(&this->headpack, &rhs.headpack, sizeof(MSGHEAD));
    //        strpack = rhs.strpack;
    //        return *this;
    //    }

    //    void Reset()
    //    {
    //        memset(&headpack, 0, sizeof(MSGHEAD));
    //        strpack.clear();
    //    }
    //}MSGPACKAGE, *PMSGPACKAGE;

    static const int kMessageHeaderLength = sizeof(Mcsf::rt::MSGHEAD);
    static const int kRegisterClientCommandId = Mcsf::rt::kRegisterClientCommandId;
    typedef Mcsf::rt::MSGPACKAGE Message;
}

#endif
