#pragma once

class i_net_message
{
public:
	int get_group()
	{
		return memory::call_v_function<int(__thiscall*)(void*)>(this, 8)(this);
	}

	const char* get_name()
	{
		return memory::call_v_function<const char* (__thiscall*)(void*)>(this, 9)(this);
	}
};

//https://media.discordapp.net/attachments/1194221664339234816/1211674968878747728/image.png?ex=65ef0f40&is=65dc9a40&hm=1fbef90240540692e40b12e5eb45d662fb482cfbadc59dea1c18da9685a8ba3a&=&format=webp&quality=lossless
class NET_SetConVar : public i_net_message
{
public:
	char pad[24];

	NET_SetConVar(const char* name, const char* value)
	{
		using NET_SetConVar_t = NET_SetConVar* (__fastcall*)(void*, const char*, const char*);

		static NET_SetConVar_t NET_SetConVar = (NET_SetConVar_t)memory::relative_to_absolute((uintptr_t)memory::pattern_scanner(xorstr("engine.dll"), xorstr("E8 ? ? ? ? 45 33 C9 48 8D 54 24 ? 45 33 C0 48 8D 0D ? ? ? ?")), 1, 6);

		if (!NET_SetConVar)
			throw;

		NET_SetConVar(this, name, value);
	}
};

class c_net_channel
{
public:
	char pad[8];
	int unk;
	int out_sequence_nr;
	int in_sequence_nr;
	int out_sequence_nr_ack;
	int out_reliable_state;
	int in_reliable_state;
	int choked_packets;

public:
	const char* get_address()
	{
		return memory::call_v_function<const char* (__thiscall*)(void*)>(this, 1)(this);
	}

	float get_latency(int flow)
	{
		return memory::call_v_function<float(__thiscall*)(void*, int)>(this, 9)(this, flow);
	}

	bool send_net_msg(i_net_message& msg, bool force_reliable = false, bool voice = false)
	{
		return memory::call_v_function<bool(__thiscall*)(void*, i_net_message&, bool, bool)>(this, 40)(this, msg, force_reliable, voice);
	}

	void transmit(bool only_reliable = false)
	{
		memory::call_v_function<void(__thiscall*)(void*, bool)>(this, 47)(this, only_reliable);
	}
};

#pragma once
struct bf_write
{
	uint8_t* m_pData;
	int      m_nDataBytes;
	int      m_nDataBits;
	int      m_iCurBit;
	bool     m_bOverflow;
	bool     m_bAssertOnOverflow;
	char     pad[2];

	void start_writing(void* data, int bytes)
	{
		m_pData = (uint8_t*)data;
		m_nDataBytes = bytes;
		m_nDataBits = bytes * 8;
		m_iCurBit = 0;
		m_bOverflow = false;
		m_bAssertOnOverflow = true;
	}
};

struct bf_read
{
	uint8_t* m_pData;
	int      m_nDataBytes;
	int      m_nDataBits;
	int      m_iCurBit;
	bool     m_bOverflow;
	bool     m_bAssertOnOverflow;
	char     pad[2];
};

class c_clc_move : public i_net_message
{
public:
	void* vtable;
	bool     m_bReliable;
	char     pad0[7];
	void* m_NetChannel;
	void* m_pMsgHandler;
	int      m_nBackupCommands;    // +0x20
	int      m_nNewCommands;       // +0x24
	char     pad1[0x38];           // +0x28..0x5F
	int      m_nLength;            // +0x60
	char     pad2[4];              // +0x64
	bf_write m_DataOut;            // +0x68

	static inline void** s_vtbl = nullptr;

	void construct()
	{
		*reinterpret_cast<void***>(this) = s_vtbl;
		m_bReliable = false;
		m_NetChannel = nullptr;
		m_pMsgHandler = nullptr;
		m_nBackupCommands = 0;
		m_nNewCommands = 0;
		m_nLength = 0;
		memset(&m_DataOut, 0, sizeof(m_DataOut));
	}

	static void capture_vtbl(i_net_message& live_msg)
	{
		if (!s_vtbl)
			s_vtbl = *reinterpret_cast<void***>(&live_msg);
	}
};