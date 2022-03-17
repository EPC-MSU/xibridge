#include "Protocols.h"
#include "utils.h"
#include <string.h> 

// staic data init
cmd_schema Protocol1::_cmd_shemas[9]=
{
{ pkt1_raw, "v_p_0_d_0_0_x" },
{ pkt1_open_req, "v_p_0_d_0_0" },
{ pkt1_open_resp, "v_p_0_d_0_0_b" },
{ pkt1_close_req, "v_p_0_d_0_0" },
{ pkt1_close_resp, "v_p_0_d_0_0_b" },
{ pkt1_enum_req, "v_p_0_0_0_0_0"},
{ pkt1_enum_resp, "v_p_0_u_x" },
{ pkt1_error_ntf , "v_p_0_d_0_0"},
{ pkt1_raw, nullptr}
};

cmd_schema  Protocol2 ::_cmd_shemas[7] =
{
{ pkt2_cmd_req, "v_p_0_d_0_0_x" },
								 { pkt2_cmd_resp, "v_p_0_d_0_0_x" },
								 { pkt2_open_req, "v_p_0_d_0_0" },
								 { pkt2_open_resp, "v_p_0_d_0_0_b" },
								 { pkt2_close_req, "v_p_0_d_0_0" },
								 { pkt2_close_resp, "v_p_0_d_0_0_b" },
								 { pkt2_cmd_req, nullptr }
};

cmd_schema Protocol3::_cmd_shemas[12] =
{
{ pkt3_ver_req, "v_p_u_0_0_0" },
{ pkt3_ver_resp, "v_p_u_0_0_0_d" },
{ pkt3_cmd_req, "v_p_u_d_0_0_l_u" },
{ pkt3_cmd_resp, "v_p_u_d_0_0_l" },
{ pkt3_open_req, "v_p_u_d_0_0" },
{ pkt3_open_resp, "v_p_u_d_0_0_b" },
{ pkt3_close_req, "v_p_u_d_0_0" },
{ pkt3_close_resp, "v_p_u_d_0_0_b" },
{ pkt3_enum_req, "v_p_u_0_0_0" },
{ pkt3_enum_resp, "v_p_u_0_0_0_x" },
{ pkt3_error_resp, "v_p_u_d_0_0_u" },
{ pkt3_error_resp, nullptr }
};


const cmd_schema &cmd_schema::get_schema(uint32 pckt, const cmd_schema *_ss)
{
	int i;
	for (i = 0; _ss[i].schema != nullptr; i++)
	{
		if (_ss[i].pkt_type == pckt) break; 
	}
	return _ss[i];
}

// structure to connect packet type and command schema
//  command schema ia a string like this "v_p_0_d_0_0_x"
//  v - version, p - packet type, 0 - 32-bit zero, d - 32-bit non-zero, x - array bytes of any length,
//   l - 32-bit length + byte array of thislength, b - 0 or 1 32-bit, u -any 32-bit numver
bool cmd_schema::is_match(const uint8 *data, int len, uint32 proto, uint32 dev_num) const
{
	MBuf mbuf(data, len);
	Hex32 hex32;
	std::string sc(schema);
	for (auto ch: sc)
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
			if (mbuf.restOfSize(-1) < 0) return false;
			break;
		case 'l':
			mbuf >> hex32;
			mbuf.mseek((int)hex32);
			break;
		case 'b':
			mbuf >> hex32;
			if ((uint32)hex32 != 0 && (uint32)hex32 != 1) return false;
			break;
		case 'u':
			mbuf >> hex32;
			break;
		default:
			return false;
		}
	}
	return !mbuf.wasBroken();
}


bool AProtocol::get_data_from_bindy_callback(MBuf& cmd,
	bvector &green_data,
	bvector& grey_data,
	uint32 &pckt_type,
	uint32 &serial)
{
	grey_data.clear(); green_data.clear();
    Hex32 skip_prt, skip_tout, sr, pckt;
	cmd >> skip_prt >> pckt >> skip_tout >> sr;
	
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

bvector AProtocol::create_cmd_request(uint32 serial, uint32 tmout, const bvector *data , uint32 resp_length)
{
	bvector data_and_length;
	if (data != nullptr) data_and_length = *data;
	add_uint32_2_bvector(data_and_length, resp_length);
	return create_cmd_req_proxy(serial, tmout, data_and_length);
}

bvector Protocol3::create_cmd_request(uint32 serial, uint32 tmout, const bvector *data, uint32 resp_length)
{
	bvector data_and_length;
	add_uint32_2_bvector(data_and_length, data == nullptr ? 0: data -> size());
	if (data != nullptr) data_and_length.insert(data_and_length.end(), data -> cbegin(), data -> cend());
	add_uint32_2_bvector(data_and_length, resp_length);
	return create_cmd_req_proxy(serial, tmout, data_and_length);
}

bool Protocol1::get_spec_data(MBuf&  mbuf,
	bvector &green_data,
	bvector &grey_data,
	uint32 pckt)
{
	// 16 bytes has already read from mbuf
	Hex32 count, devnum;

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
						 grey_data = mbuf.to_vector(true);
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
						 green_data = mbuf.to_vector(true);
					     return true;
		}
		case pkt1_enum_resp:
		{
                         mbuf.mseek(-4);
						 mbuf >> count;
						 grey_data = mbuf.to_vector(true);  //all data as is is grey
						 MBuf gr_buf(sizeof(uint32) * count);
						 {
							 for (int i = 0; i < count; i++)
							 {
								 mbuf >> devnum;
								 gr_buf << Hex32(devnum, true); // another order of bytes
								 mbuf.mseek(180 - 4);
							 }

							 _is_inv_pcktfmt = true;
							 return false;
						 }
						 grey_data = mbuf.to_vector(true);
						 green_data = gr_buf.to_vector();
					     return !mbuf.wasBroken() && !gr_buf.wasBroken();
					    
		}
		
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
						 grey_data = mbuf.to_vector(true);
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
								green_data = mbuf.to_vector(true);
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
							 grey_data = mbuf.to_vector(true);

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
							  grey_data = mbuf.to_vector(true);
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
							  green_data = mbuf.to_vector(true);
							  return true;

		}
		case pkt3_enum_resp:
		{

							   mbuf.mseek(8);
							   mbuf >> size;
							   green_data.assign(mbuf.cur_data(), mbuf.cur_data() + size * sizeof(uint32));
							   mbuf >> size;
							   if (mbuf.restOfSize(-1) != (int)size) return false;
							   grey_data = mbuf.to_vector(true);
							   return true;

		}
		default:
			_is_inv_pckt = true;
			return false;
		}
	}
}

bvector Protocol1::create_client_request(uint32 pckt, uint32 serial, uint32 /*tmout*/, const bvector *data)
{
	MBuf mbuf(64 + (data == nullptr ? 0 : data->size()));
	mbuf << Hex32(version()) << Hex32((uint32)pckt) << Hex32((uint32)0x0) << Hex32(serial) << Hex32((uint32)0x0) << Hex32((uint32)0x0);
	if (pckt == pkt1_raw)
	{
		if (data != nullptr) mbuf.memwrite(data->data(), data->size());
	}
	else if (pckt == pkt1_enum_req)
		mbuf << Hex32((uint32)0x00);
	return mbuf.to_vector();
}

bvector Protocol2::create_client_request(uint32 pckt, uint32 serial, uint32 /*tmout*/, const bvector * data)
{
	MBuf mbuf(64 + (data == nullptr ? 0 : data->size()));
	mbuf << Hex32(version()) << Hex32((uint32)pckt) <<Hex32((uint32)0x0) << Hex32(serial) << Hex32((uint32)0x0) << Hex32((uint32)0x0);
	if (pckt == pkt2_cmd_req)
	{
		if (data != nullptr) mbuf.memwrite(data->data(), data->size());
	}
	return mbuf.to_vector();
}

bvector Protocol3::create_client_request(uint32 pckt, uint32 serial, uint32 tmout, const bvector * data)
{
	MBuf mbuf(64 + (data == nullptr ? 0 : data -> size()));
	mbuf << Hex32(version()) << Hex32(pckt) << Hex32(tmout);
	uint32 _serial = 0;
	if (pckt == pkt3_close_req || pckt == pkt3_open_req || pckt == pkt3_cmd_req)
		_serial = serial;
	mbuf << Hex32(serial) << Hex32((uint32)0x0) << Hex32((uint32)0x0);
	if (pckt == pkt3_cmd_req)
	{
		if (data != nullptr) mbuf.memwrite(data->data(), data->size());
	}
	return mbuf.to_vector();
}

bool Protocol1::translate_response(uint32 pckt, const bvector& green)
{
	if (pckt == pkt1_error_ntf)
	{
		_is_device_broken = true;
		return false;
	}
	if (green.size() > 0) return green[0] != 0;
	return  true;
}

bool Protocol2::translate_response(uint32 pckt, const bvector& green)
{
    if (green.size() > 3 && pckt == pkt2_open_resp) return green[3] != 0;
	return  true;
}

bool Protocol3::translate_response(uint32 pckt, const bvector& green)
{
	if (pckt == pkt3_error_resp)
	{
		_is_device_broken = true;
		// to do error codes !!!
		return false;
	}
	if (green.size() > 0 && pckt == pkt3_open_resp) return true;
	return  false;
}

AProtocol *create_appropriate_protocol(int version_number)
{
	AProtocol * p = nullptr;
	switch (version_number)
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