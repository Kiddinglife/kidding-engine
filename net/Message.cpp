#include "Message.h"
#include "Packet.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

Messages* Messages::staticMsgsPtr = NULL;
//std::vector<Messages*>* gPtrMsgsPtrContainer = NULL;
std::vector<Messages*> gPtrMsgsPtrContainer;
FixedMessages* gPtrFixedMsgs = NULL;

/**
 * @TO-DO
 */
bool Messages::initializeWatcher()
{
	return false;
}

/**
 * @Brief
 *
 */
Message*  Messages::add_msg(const std::string& ihName, MessageArgs* args,
	ACE_INT8 msgType, Message* msg)
{
	if( msgID_ == 1 )
	{
		//ACE_DEBUG(( LM_DEBUG, "Messages::@1::add()::starts....\n" ));
	}

	/// 初始化msg的id
	FixedMessages::MSGInfo* msgInfo = ACE_Singleton<FixedMessages, ACE_Null_Mutex>::instance()->isFixed(ihName);

	if( msgInfo == NULL )
	{
		//ACE_DEBUG(( LM_DEBUG, "Messages::@1::msgInfo == NULL\n" ));
		while( ACE_Singleton<FixedMessages, ACE_Null_Mutex>::instance()->isFixed(msgID_) )
		{
			msgID_++;
		}
		msg->msgID_ = msgID_++;
	} else
	{
		//ACE_DEBUG(( LM_DEBUG, "Messages::@2::msgInfo != NULL\n" ));
		msg->msgID_ = msgInfo->msgid;
	}

	/// 初始化msg各个参数值
	msg->name_ = ihName;
	msg->pMsgArgs_ = args;
	msg->msgType_ = msgType;
	msg->exposed_ = false;
	msg->pMsgs_ = this;
	msg->onAdded2Msgs();

	///将该msg添加到msgs中去
	msgs_[msg->msgID_] = msg;

	/// if it is fixed-len msg, 
	if( msgType == NETWORK_VARIABLE_MESSAGE )
	{
		ACE_DEBUG(( LM_DEBUG,
			"%M::Messages :: add(%d) :: name = %s, msgID = %d, msgType_ = Variable.\n",
			msgs_.size(), ihName.c_str(), msg->msgID_ ));
	} else
	{
		msg->msgArgsBytesCount_ = args->args_bytes_count();
		if( msg->category() == MESSAGE_CATEGORY_ENTITY )
		{
			msg->msgArgsBytesCount_ += ENTITY_ID_SIZE;
		}

		ACE_DEBUG(( LM_DEBUG,
			"%M::Messages :: add(%d) :: name = %s, msgID = %d, msgArgsCount_ = Fixed(%d).\n",
			msgs_.size(), msg->name_.c_str(), msg->msgID_, msg->msgArgsBytesCount_ ));
	}

	/// return the added msg
	return msg;
}

/**
 * @Brief
 * 该方法根据消息id找到对应的消息
 * \n
 * @param MessageID msgID
 * @ret 消息指针
 */
Message* Messages::find(MessageID msgID)
{
	MessageMap::iterator iter = msgs_.find(msgID);
	return  iter != msgs_.end() ? iter->second : NULL;
}

/**
 * @Brief
 * 该方法安全释放全局指针gPtrFixedMsgs和gPtrMsgsPtrContainer
 */
void Messages::finalise(void)
{
	SAFE_RELEASE(gPtrFixedMsgs);
	//SAFE_RELEASE(gPtrMsgsPtrContainer);
}
/////////////////////////////////////// For Test Use Globals ///////////////////////////////
//Messages g_msgs;
//void inport_msgs()
//{
//	g_msgs.add_msg("currhandler1", new msgarg, NETWORK_FIXED_MESSAGE, new Message);
//	g_msgs.add_msg("currhandler2", new msgarg_variable, NETWORK_VARIABLE_MESSAGE, new Message);
//	g_msgs.add_msg("currhandler3", new msgarg, NETWORK_FIXED_MESSAGE, new Message);
//	g_msgs.add_msg("currhandler4", new msgarg_variable, NETWORK_VARIABLE_MESSAGE, new Message);
//}
//////////////////////////////////////////////////////////////////////////////////////////////
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL