#include "Protocols.h"
#include "utils.h"
#include <string.h> 

// staic data init
cmd_schema Protocol1::_cmd_shemas[9]=
{
{ pkt1_raw, "v_p_0_" },
{ pkt1_open_req, "v_p_0_"},
{ pkt1_open_resp, "v_p_0_"},
{ pkt1_close_req, "v_p_0" },
{ pkt1_close_resp, "v_p_0_" },
{ pkt1_enum_req, "v_p_0_"},
{ pkt1_enum_resp, "v_p_0_" },
{ pkt1_error_ntf , "v_p_"},
{ pkt1_raw, nullptr}
};

cmd_schema  Protocol2 ::_cmd_shemas[7] =
								 {
								 { pkt2_cmd_req, "v_p_0_" },
								 { pkt2_cmd_resp, "v_p_0_" },
								 { pkt2_open_req, "v_p_0_" },
								 { pkt2_open_resp, "v_p_0_" },
								 { pkt2_close_req, "v_p_0" },
								 { pkt2_close_resp, "v_p_0_" },
								 { pkt2_cmd_req, nullptr }
};

cmd_schema Protocol3::_cmd_shemas[7] =
{
{ pkt3_cmd_req, "v_p_0_" },
{ pkt3_cmd_resp, "v_p_0_" },
{ pkt3_open_req, "v_p_0_" },
{ pkt3_open_resp, "v_p_0_" },
{ pkt3_close_req, "v_p_0" },
{ pkt3_close_resp, "v_p_0_" },
{ pkt3_error_resp, nullptr }
};



//  v - version, p - packet type, 0 - 32-bit zero, d - 32-bit non-zero, x - array bytes of any length,
//  b - 32 - bit length + byte array of thislength
bool cmd_schema::is_match(const uint8 *data, int len, uint32 proto, uint32 dev_num)
{
	MBuf mbuf(data, len);
	Hex32 hex32;

	for (auto ch: schema )
	{
		if (ch == '_') continue;
		switch (ch)
		{
		case 'v':
			mbuf >> hex32;
			if (hex32 != proto)
				return false;
			break;
		case  '0':
			mbuf >> hex32;
			if (hex32 != 0) return false;
			break;
		case 'p':
			mbuf >> hex32;
			if (pkt_type != hex32) return false;
			break;
		case 'd':
			mbuf >> hex32;
			if (hex32 != dev_num) return false;
			break;
		case 'x':
			if (mbuf.restOfSize(mbuf.realSize()) <= 0) return false;
			break;
		case 'b':
			mbuf >> hex32;
			if (mbuf.restOfSize(mbuf.realSize()) != (int)hex32) return false;
			break;
		default:
			return false;
		}
		return !mbuf.wasBroken();
	}
}


bool AProtocol::get_data_from_bindy_callback(MBuf& cmd,
	bvector &green_data,
	bvector& grey_data,
	uint32 &pckt_type,
	uint32 &serial)
{
	grey_data.clear(); green_data.clear();
    Hex32 skip_prt, skip_tout, sr, pckt;
	cmd >> skip_prt >> skip_tout >> sr >> pckt;
	
	if (cmd.wasBroken())
	{
		_is_inv_pcktfmt = true;  
		return false;
	}

	if (!get_spec_data(cmd, green_data, grey_data, pckt))
		return false;
	serial = sr;
	pckt_type = pckt;

	return true;
}

bool Protocol1::get_spec_data(MBuf&  mbuf,
	bvector &green_data,
	bvector &grey_data,
	uint32 pckt)
{
	// 16 bytes has already read from mbuf

	if (_is_server)
	{
		switch (pckt)
		{
		case pkt1_raw:
		{
						 mbuf.mseek(8);
						 int len = mbuf.restOfSize(-1);
						 if (mbuf.wasBroken() || len == -1)
						 {
							 _is_inv_pcktfmt = true;
							 return false;
						 }
						 grey_data.assign(mbuf.cur_data(), mbuf.cur_data()+len);
						
						 return true;
		}
		case pkt1_open_req:
		{
						  return true;
		}
		case pkt1_close_req:
		{
						  return true;
		}
		case pkt1_enum_req:
			              return true;
		default:
			_is_inv_pckt = true;
			return false;
		}
	}
	else
	{
		switch (pckt)
		{
		case pkt1_raw:
		{
						 mbuf.mseek(8);
						 int len = mbuf.restOfSize(-1);
						 if (mbuf.wasBroken() || len == -1)
						 {
							 _is_inv_pcktfmt = true;
							 return false;
						 }
						 return true;
		}
		case pkt1_open_resp:
		case pkt1_close_resp:
		{              
						 mbuf.mseek(8);
						 int len = mbuf.restOfSize(-1);
						 if (len != sizeof (uint32))
						 {
							 _is_inv_pcktfmt = true;
							 return false;
						 }
						 green_data.assign(mbuf.cur_data(), mbuf.cur_data() + len);
					     return true;
		}
		case pkt1_enum_resp:
		{
                         mbuf.mseek(8);
						 int len = mbuf.restOfSize(-1);
						 if (len != sizeof (uint32))
						 {
							 _is_inv_pcktfmt = true;
							 return false;
						 }
						 grey_data.assign(mbuf.cur_data(), mbuf.cur_data() + len);
					     return true;
					    
		}
		case pkt1_enum_req:
			             return true;
		default:
			_is_inv_pckt = true;
			return false;
		}
	}
}

bool Protocol2::get_spec_data(MBuf&  mbuf,
	bvector &green_data,
	bvector &grey_data,
	uint32 pckt)
{
	// 16 bytes has already read from mbuf

	if (_is_server)
	{
		switch (pckt)
		{
		case pkt2_cmd_req:
		{
						 mbuf.mseek(8);
						 int len = mbuf.restOfSize(-1);
						 if (mbuf.wasBroken() || len == -1)
						 {
							 _is_inv_pcktfmt = true;
							 return false;
						 }
						 grey_data.assign(mbuf.cur_data(), mbuf.cur_data() + len);

						 return true;
		}
		case pkt2_open_req:
		{
							  return true;
		}
		case pkt2_close_req:
		{
							   return true;
		}
		default:
			_is_inv_pckt = true;
			return false;
		}
	}
	else
	{
		switch (pckt)
		{
		case pkt2_cmd_resp:
		{
						 mbuf.mseek(8);
						 int len = mbuf.restOfSize(-1);
						 if (mbuf.wasBroken() || len == -1)
						 {
							 _is_inv_pcktfmt = true;
							 return false;
						 }
						 return true;
		}
		case pkt2_open_resp:
		case pkt2_close_resp:
		{
								mbuf.mseek(8);
								int len = mbuf.restOfSize(-1);
								if (len != sizeof (uint32))
								{
									_is_inv_pcktfmt = true;
									return false;
								}
								green_data.assign(mbuf.cur_data(), mbuf.cur_data() + len);
								return true;
		}
		
		default:
			_is_inv_pckt = true;
			return false;
		}
	}
}

bool Protocol3::get_spec_data(MBuf&  mbuf,
	bvector &green_data,
	bvector &grey_data,
	uint32 pckt)
{
	// 16 bytes has already read from mbuf
	Hex32 size;
	if (_is_server)
	{
		switch (pckt)
		{
		case pkt3_cmd_req:
		{
							 mbuf.mseek(8);
							 mbuf >> size;
							 int len = mbuf.restOfSize(-1);
							 if (mbuf.wasBroken() || len != (int)size)
							 {
								 _is_inv_pcktfmt = true;
								 return false;
							 }
							 grey_data.assign(mbuf.cur_data(), mbuf.cur_data() + len);

							 return true;
		}
		case pkt3_open_req:
		case pkt3_close_req:
		case pkt3_enum_req:
		case pkt3_ver_req:
							 return true;
			                  
		default:
			_is_inv_pckt = true;
			return false;
		}
	}
	else
	{
		switch (pckt)
		{
		case pkt3_cmd_resp:
		{
							  mbuf >> size;
							  int len = mbuf.restOfSize(-1);
							  if (mbuf.wasBroken() || len != (int)size)
							  {
								  _is_inv_pcktfmt = true;
								  return false;
							  }
							  grey_data.assign(mbuf.cur_data(), mbuf.cur_data() + len);
		}
		case pkt3_open_resp:
		case pkt3_close_resp:
		case pkt3_ver_resp:
		case pkt3_error_resp:
		{

							  mbuf.mseek(8);
							  int len = mbuf.restOfSize(-1);
							  if (len != sizeof (uint32))
							  {
								  _is_inv_pcktfmt = true;
								  return false;
							  }
							  green_data.assign(mbuf.cur_data(), mbuf.cur_data() + len);
							  return true;

		}
		case pkt3_enum_resp:
		{

							   mbuf.mseek(8);
							   int len = mbuf.restOfSize(-1);
							   if (len != sizeof (uint32))
							   {
								   _is_inv_pcktfmt = true;
								   return false;
							   }
							   green_data.assign(mbuf.cur_data(), mbuf.cur_data() + len);
							   return true;

		}
		default:
			_is_inv_pckt = true;
			return false;
		}
	}
}

bvector Protocol1::create_client_request(uint32 pckt, uint32 /*tmout*/, uint32 serial, const bvector *data)
{
	bvector ret;
	MBuf mbuf(64 + (data == nullptr) ? 0 : data->size());
	mbuf << Hex32(version()) << Hex32((uint32)pckt) << Hex32((uint32)0x0) << Hex32(serial) << Hex32((uint32)0x0) << Hex32((uint32)0x0); return mbuf.to_vector();
	if (pckt == pkt1_raw)
	{
		if (data != nullptr) mbuf.memwrite(data->data(), data->size());
	}
	return mbuf.to_vector();
}

bvector Protocol2::create_client_request(uint32 pckt, uint32 serial, uint32 /*tmout*/, const bvector * data)
{
	bvector ret;
	MBuf mbuf(64 + (data == nullptr) ? 0 : data->size());
	mbuf << Hex32(version()) << Hex32((uint32)pckt) <<Hex32((uint32)0x0) << Hex32(serial) << Hex32((uint32)0x0) << Hex32((uint32)0x0);
	if (pckt == pkt2_cmd_req)
	{
		if (data != nullptr) mbuf.memwrite(data->data(), data->size());
	}
	return mbuf.to_vector();
}

bvector Protocol3::create_client_request(uint32 pckt,  uint32 serial, uint32 tmout, const bvector * data)
{
	bvector ret;
	MBuf mbuf(64 + (data == nullptr) ? 0 : data -> size());
	mbuf << Hex32(version()) << Hex32(pckt) << Hex32(tmout);
	uint32 _serial = 0;
	if (pckt == pkt3_close_req || pckt == pkt3_open_req || pckt == pkt3_cmd_req)
		_serial = serial;

	mbuf << Hex32(serial) << Hex32((uint32)0x0) << Hex32((uint32)0x0);
	if (pckt == pkt3_cmd_req)
	{
		mbuf << Hex32((data == nullptr) ? 0 : data->size());
		if (data != nullptr) mbuf.memwrite(data->data(), data->size());
	}
	return mbuf.to_vector();
}

AProtocol *create_appropriate_protocol(uint32 version)
{
	AProtocol * p = nullptr;
	switch (version)
	{
	case 1:
		p = new Protocol1(false);
		break;
	case 2:
		p = new Protocol2(false);
		break;
	case 3:
		p = new Protocol3(false);
		break;
	}
	return p;
}