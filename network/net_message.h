// msgpack ֱ���ܸ�ҵ���ʹ�õ�
#pragma once
#include "../common/basic/basicTypes.h"

/*
Э������ʹ��google protobuf
message NetMessage{
	uint16 uProtocol = 1;	// Э���
	uint8 type = 2;			// ��Ϣ�����ͣ�1=���������ȥ�İ�����2 = ���ذ��������İ���
	optional bytes content = 3;	// ��Ϣ�ֽ�
}
*/

#pragma pack(push,1) //�����ֽڶ���
struct _stNetMessage 
{
	uint32 m_op;		//Э���
	uint32 m_mask;		//mask
	uint16 m_size;		//���ݴ�С
	char m_buffer[1024];//��������
	
	_stNetMessage()
		:m_op(0),
		m_mask(0), 
		m_size(0){
		//m_proto.Clear();
		memset(m_buffer, 0, sizeof(m_buffer));
	}

	inline void	reset(uint32 apply_key, char* buffer, uint16 size){
		m_op = apply_key;
		//m_proto.SerializeToArray(buffer, size);		
		memcpy(m_buffer,buffer,size);
		m_size = size;
	}
};
#pragma pack(pop)
