#ifndef PROTOBUF_DISPATCHER_H__
#define PROTOBUF_DISPATCHER_H__
#include <map>
#include "EasyNet/include/net/ICallBackDefine.h"
#include "google/protobuf/message.h"
#include <iostream>
  


namespace Net
{
    typedef Function<void(const ProtobufMessagePtr&) > ProtobufMsgCallback;
    class MsgCallBack
    {
    public:
        virtual void OnMessage(const ProtobufMessagePtr& msg) = 0;
    };


    template <class T>
    class ProtobufMsgCallBackT : public MsgCallBack
    {
    public:
        
        typedef Function<void( const Shared_ptr<T>& )> CallBackT;
        ProtobufMsgCallBackT(const CallBackT& cb): cb_(cb)
        {

        }

        virtual void OnMessage(const ProtobufMessagePtr& msg)
        {
            T* temp = dynamic_cast<T*>(msg.get());
            assert (temp);
            // do not do this, this will influence the reference counter//
            //Shared_ptr<T> concreateMsgPtr(temp);
            Shared_ptr<T> concreateMsgPtr =  Static_Pointer_Cast<T>(msg);
            if (msg)
            {
                cb_(concreateMsgPtr);
            }
        
        }

    private:
         CallBackT cb_;

    };

    class ProtobufMessageDispatcher
    {
    public:
        typedef std::map<const google::protobuf::Descriptor*,  Shared_ptr<MsgCallBack> > CallbackMap;

        
        template <class T>
        void RegisterCallback(const typename ProtobufMsgCallBackT<T>::CallBackT& cb)
        {
            assert (protoBufMsgCallbackMap_.find(T::descriptor()) == protoBufMsgCallbackMap_.end());
            Shared_ptr<ProtobufMsgCallBackT<T> > temp(new ProtobufMsgCallBackT<T>( cb ));
            protoBufMsgCallbackMap_[T::descriptor()] = temp;
        }

        void onProtobufMessage(const ProtobufMessagePtr& message) const
        {
            auto it = protoBufMsgCallbackMap_.find(message->GetDescriptor());
            if (it != protoBufMsgCallbackMap_.end())
            {
                const Shared_ptr<MsgCallBack>& cb = it->second;
                cb->OnMessage(message);
            }
            else
            {
                std::cout << "message type:"  << std::endl;;
            }
        }


    private:
        CallbackMap protoBufMsgCallbackMap_;

    };
}




#endif