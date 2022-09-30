#include "protocols.h"
#include "../client/xibridge_client.h"

/*
 *staic data init
*/

/*
   structure to connect packet type and command schema
   command schema ia a string like this "v_p_0_d_0_0_x"
   v - version, p - packet type, 0 - 32-bit zero, d - 32-bit non-zero, x - array bytes of any length,
   l - 32-bit length + byte array of this length, b - 0 or 1 32-bit, u -any 32-bit number
 */
cmd_schema Protocol1::_cmd_shemas[9]=
{
    { pkt1_raw, "v_p_0_d_0_0_x" },
    { pkt1_open_req, "v_p_0_d_0_0" },
    { pkt1_open_resp, "v_p_0_d_0_0_b" },
    { pkt1_close_req, "v_p_0_d_0_0" },
    { pkt1_close_resp, "v_p_0_d_0_0_b" },
    { pkt1_enum_req, "v_p_0_0_0_0_0" },
    { pkt1_enum_resp, "v_p_0_u_x" },
    { pkt1_error_ntf, "v_p_0_d_0_0" },
    { pkt1_raw, nullptr }
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
    { pkt3_ver_req, "v_p_0_0_0_0_0_0" },
    { pkt3_ver_resp, "v_p_0_0_0_0_0_0_u" },
    { pkt3_cmd_req, "v_p_0_I_0_0_l_u" },
    { pkt3_cmd_resp, "v_p_0_I_0_0_l" },
    { pkt3_open_req, "v_p_0_I_0_0" },
    { pkt3_open_resp, "v_p_0_I_0_0_b" },
    { pkt3_close_req, "v_p_0_I_0_0" },
    { pkt3_close_resp, "v_p_0_I_0_0_b" },
    { pkt3_enum_req, "v_p_0_0_0_0_0_0" },
    { pkt3_enum_resp, "v_p_0_0_0_0_0_0_x" },
    { pkt3_error_resp, "v_p_0_I_0_0_u" },
    { pkt3_error_resp, nullptr }
};

const int Protocol2::URPC_CID_SIZE = 4;

const cmd_schema &cmd_schema::get_schema(uint32_t pckt, 
                                         const cmd_schema *_ss)
{
    int i;
    for (i = 0; _ss[i].schema != nullptr; i++)
    {
        if (_ss[i].pkt_type == pckt) break; 
    }
    return _ss[i];
}

int cmd_schema::get_plain_command_length() const
{
    std::string sc(schema);
    int l = 0;
    for (auto ch : sc)
    {
        if (ch == '_') continue;
        switch (ch)
        {
        case 'v':
        case '0':
        case 'p':
        case 'd':
        case 'b':
        case 'u':
            l += (int)sizeof(uint32_t);
            break;
        case 'I':
            l += (int) (sizeof(uint32_t)* 3);
            break;

        default:
            return -1; // non plain command
            break;
        }
    }
    return l;
}

bvector cmd_schema::gen_plain_command(uint32_t proto, 
                                      const DevId &dev, 
                                      uint32_t zero_one, 
                                      uint32_t some) const
{
    int length = get_plain_command_length();
    if (length == -1) return bvector();
    std::string sc(schema);
    MBuf mbuf(length);
    for (auto ch : sc)
    {
        if (ch == '_') continue;
        switch (ch)
        {
        case 'v':
            mbuf << Hex32(proto);
            break;
        case  '0':
            mbuf << Hex32((uint32_t)0);
            break;
        case 'p':
            mbuf << Hex32(pkt_type);
            break;
        case 'd':
            mbuf << Hex32(dev.id());
            break;
        case 'b':
            mbuf << Hex32(zero_one);
            break;
        case 'u':
            mbuf << Hex32(some);
            break;
        case 'I':
            mbuf << HexIDev3(&dev);
            break;
        }
    }
    return mbuf.to_vector();
}

bool cmd_schema::is_match(const uint8_t *data, 
                          int len, 
                          uint32_t proto, 
                          const DevId & dev_num) const
{
    MBuf mbuf(data, len);
    Hex32 hex32; HexIDev3 hdev3;
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
            if (hex32 != dev_num.id()) return false;
            break;
        case 'x':
            if (mbuf.restOfSize(-1) == SIZE_MAX) return false;
            break;
        case 'l':
            mbuf >> hex32;
            mbuf.mseek((int)hex32);
            break;
        case 'b':
            mbuf >> hex32;
            if ((uint32_t)hex32 != 0 && (uint32_t)hex32 != 1) return false;
            break;
        case 'u':
            mbuf >> hex32;
            break;
        case 'I':
            mbuf >> hdev3;
            break;
        default:
            return false;
        }
    }
    return !mbuf.wasBroken();
}

uint32_t AProtocol::get_pckt_of_cmd(const bvector& cmd)
{
    Hex32 x, p;
    MBuf b(cmd.data(), cmd.size());
    b >> x >> p;
    return p;
}

bool AProtocol::get_data_from_bindy_callback(MBuf& cmd,
                                             bvector &res_data,
                                             bvector& data,
                                             uint32_t &pckt_type)
{
    res_data.clear(); data.clear(); _res_err = 0;
    Hex32 skip_prt, skip_tout, sr, pckt, serial; HexIDev3 hdev;
    cmd >> skip_prt >> pckt >> skip_tout;
    if (is_device_id_extended() == false)
    {
        cmd >> serial;
    }
    else
    {
        cmd >> hdev;
    }
    
    if (cmd.wasBroken())
    {
        *_perror = ERR_PCKT_FMT;
        return false;
    }

    if (!get_spec_data(cmd, res_data, data, pckt))
        return false;

    pckt_type = pckt;
    return true;
}

bvector Protocol1::create_cmd_request(const DevId &devid, 
                                      uint32_t /*tmout*/, 
                                      const bvector *data, 
                                      uint32_t /*resp_length*/)
{
    return create_client_request(pkt1_raw, devid, 0, data);
}

bvector Protocol2::create_cmd_request(const DevId &devid, 
                                      uint32_t /*tmout*/, 
                                      const bvector *data, 
                                      uint32_t resp_length)
{
    bvector data_and_length;
    auto data_cbeg = data -> cbegin();
    if (data != nullptr && data->size() >= (size_t)URPC_CID_SIZE)
    {
        data_and_length.insert(data_and_length.end(), data_cbeg, data_cbeg + URPC_CID_SIZE );
        add_uint32_2_bvector(data_and_length, resp_length - (uint32_t)(sizeof(uint32_t))); // minus answer code length according to Protocol2
        data_and_length.insert(data_and_length.end(), data_cbeg + URPC_CID_SIZE, data->cend());
    }
    return create_client_request(pkt2_cmd_req, devid, 0,  &data_and_length);
}

bvector Protocol3::create_cmd_request(const DevId &devid, 
                                      uint32_t /*tmout*/, 
                                      const bvector *data, 
                                      uint32_t resp_length)
{
    bvector data_and_length;
    add_uint32_2_bvector(data_and_length, data == nullptr ? 0: (uint32_t)data -> size());
    if (data != nullptr) data_and_length.insert(data_and_length.end(), data -> cbegin(), data -> cend());
    add_uint32_2_bvector(data_and_length, resp_length);
    return create_client_request(pkt3_cmd_req, devid, 0, &data_and_length);
}

bool Protocol1::get_spec_data(MBuf&  mbuf,
    bvector &/*res_data*/,
    bvector &data,
    uint32_t pckt)
{
    // 16 bytes has already read from mbuf
    Hex32 count,  r;
    Hex32 devnum(0, true);
    size_t len;
    _res_err = 0;
    if (_is_server)
    {
        switch (pckt)
        {
        case pkt1_raw:

            mbuf.mseek(8 + 4); //according to protocol
            len = mbuf.restOfSize(-1);
            if (mbuf.wasBroken() || len == SIZE_MAX)
            {
                *_perror = ERR_PCKT_FMT;
                return false;
            }
            data.assign(mbuf.cur_data(), mbuf.cur_data() + len);
            return true;

        case pkt1_open_req:
            return true;

        case pkt1_close_req:
            return true;

        case pkt1_enum_req:
            return true;
        default:
            *_perror = ERR_PCKT_INV;
            return false;

        }
    }
    else
    {
        switch (pckt)
        {
        case pkt1_error_ntf:

            *_perror = ERR_DEVICE_LOST;
            return false;
        case pkt1_raw:
            mbuf.mseek(8);
            len = mbuf.restOfSize(-1);
            if (mbuf.wasBroken() || len == SIZE_MAX)
            {
                *_perror = ERR_PCKT_FMT;
                return false;
            }
            data = mbuf.to_vector(true);
            return true;
        case pkt1_open_resp:
        case pkt1_close_resp:

            mbuf.mseek(8);
            len = mbuf.restOfSize(-1);
            if (len != sizeof (uint32_t))
            {
                *_perror = ERR_PCKT_FMT;
                return false;
            }
            mbuf >> r;
            _res_err = (uint32_t)r;
            return true;

        case pkt1_enum_resp:
            mbuf.mseek(-4);
            mbuf >> count;
            _res_err = (uint32_t)count;
            for (int i = 0; i < (int)count; i++)
            {
                mbuf >> devnum;
                // according to protocol1
                DevId devid(devnum);
                add_dev_id_bvector_net_order(data, devid.to_xibridge_device_t());
                // according to protocol1
                mbuf.mseek(180 - sizeof(uint32_t));
            }
            // according to protocol desc - the length of               
            if (mbuf.wasBroken())
                *_perror = ERR_PCKT_FMT;
            return !mbuf.wasBroken();
        default:
            *_perror = ERR_PCKT_INV;
            return false;
        }
    }
}

bool Protocol2::get_spec_data(MBuf&  mbuf,
                              bvector &/*res_data*/,
                              bvector &data,
                              uint32_t pckt)
{
    // 16 bytes or 16 bytes + 8 (extended identifier) has already read from mbuf
    size_t len;
    Hex32 r;
    _res_err = 0;
    if (_is_server)
    {
        switch (pckt)
        {
        case pkt2_cmd_req:

            mbuf.mseek(8);
            //mbuf >> r; get data as is!!!
            len = mbuf.restOfSize(-1);
            if (mbuf.wasBroken() || len == SIZE_MAX)
            {
                *_perror = ERR_PCKT_FMT;
                return false;
            }
            data.assign(mbuf.cur_data(), mbuf.cur_data() + len);
            return true;
        case pkt2_open_req:
            return true;
        case pkt2_close_req:
            return true;
        default:
            *_perror = ERR_PCKT_INV;
            return false;
        }
    }
    else
    {
        switch (pckt)
        {
        case pkt2_cmd_resp:

            mbuf.mseek(8);
            len = mbuf.restOfSize(-1);
            if (mbuf.wasBroken() || len == SIZE_MAX)
            {
                *_perror = ERR_PCKT_FMT;
                return false;
            }
            data = mbuf.to_vector(true);
            return true;
        case pkt2_open_resp:
        case pkt2_close_resp:
            mbuf.mseek(8);
            len = mbuf.restOfSize(-1);
            if (len != sizeof (uint32_t))
            {
                *_perror = ERR_PCKT_FMT;
                return false;
            }
            mbuf >> r;
            _res_err = r;
            return true;
        default:
            *_perror = ERR_PCKT_INV;
            return false;
        }
    }
}

bool Protocol3::get_spec_data(MBuf&  mbuf,
                              bvector &/*res_data*/,
                              bvector &data,
                              uint32_t pckt)
{
    // 16 bytes has already read from mbuf
    Hex32 size, r;
    HexIDev3 dev;
    size_t len;
    _res_err = 0;
    if (_is_server)
    {
        switch (pckt)
        {
        case pkt3_cmd_req:
            mbuf.mseek(8);
            mbuf >> size;
            len = mbuf.restOfSize(-1);
            if (mbuf.wasBroken() || len != (size_t)size)
            {
                *_perror = ERR_PCKT_FMT;
                return false;
            }
            data = mbuf.to_vector(true);
            return true;
        case pkt3_open_req:
        case pkt3_close_req:
        case pkt3_enum_req:
        case pkt3_ver_req:
            return true;
        default:
            *_perror = ERR_PCKT_INV;
            return false;
        }
    }
    else
    {
        switch (pckt)
        {
        case pkt3_cmd_resp:
            mbuf >> size;
            len = mbuf.restOfSize(-1);
            if (mbuf.wasBroken() || len != (size_t)size)
            {
                *_perror = ERR_PCKT_FMT;
                return false;
            }
            data = mbuf.to_vector(true);
            return true;
        case pkt3_open_resp:
        case pkt3_close_resp:
        case pkt3_ver_resp:
        case pkt3_error_resp:
            mbuf.mseek(8);
            len = mbuf.restOfSize(-1);
            if (len != sizeof (uint32_t))
            {
                *_perror = ERR_PCKT_FMT;
                return false;
            }
            mbuf >> r;
            _res_err = r;
            return true;
        case pkt3_enum_resp:
            mbuf.mseek(8);
            mbuf >> size;
            _res_err = (uint32_t)size;
            for (int i = 0; i < (int)size; i++)
            {
                mbuf >> dev;
                // according to protocol3
                DevId devid = dev.toDevId();
                add_dev_id_bvector_net_order(data, devid.to_xibridge_device_t());
            }

            // according to protocol desc - the length of               
            if (mbuf.wasBroken()) *_perror = ERR_PCKT_FMT;
            return !mbuf.wasBroken();

        default:
            *_perror = ERR_PCKT_FMT;
            return false;
        }
    }
}

bvector Protocol1::create_client_request(uint32_t pckt, 
                                         uint32_t serial, 
                                         uint32_t /*tmout*/, 
                                         const bvector *data)
{
    MBuf mbuf(64 + (data == nullptr ? 0 : (int)data->size()));
    mbuf << Hex32(version()) << Hex32((uint32_t)pckt) << Hex32((uint32_t)0x0) << Hex32(serial) << Hex32((uint32_t)0x0) << Hex32((uint32_t)0x0);
    if (pckt == pkt1_raw)
    {
        if (data != nullptr) mbuf.memwrite(data->data(), (int)data->size());
    }
    else if (pckt == pkt1_enum_req)
        mbuf << Hex32((uint32_t)0x00);
    return mbuf.to_vector();
}

bvector Protocol2::create_client_request(uint32_t pckt, 
                                         uint32_t serial, 
                                         uint32_t /*tmout*/, 
                                         const bvector * data)
{
    MBuf mbuf(64 + (data == nullptr ? 0 : (int)data->size()));
    mbuf << Hex32(version()) << Hex32((uint32_t)pckt) <<Hex32((uint32_t)0x0) << Hex32(serial) << Hex32((uint32_t)0x0) << Hex32((uint32_t)0x0);
    if (pckt == pkt2_cmd_req)
    {
       if (data != nullptr) mbuf.memwrite(data->data(), (int)data->size());
    }
    return mbuf.to_vector();
}

bvector Protocol3::create_client_request(uint32_t pckt, 
                                         const DevId &devid, 
                                         uint32_t tmout, 
                                         const bvector * data)
{
    MBuf mbuf(64 + (data == nullptr ? 0 : (int)data -> size()));
    mbuf << Hex32(version()) << Hex32(pckt) << Hex32(tmout);
    HexIDev3 _idev(&devid);
    if (pckt == pkt3_close_req || pckt == pkt3_open_req || pckt == pkt3_cmd_req)
    {

        mbuf << _idev << Hex32((uint32_t)0x0) << Hex32((uint32_t)0x0);
        if (pckt == pkt3_cmd_req)
        {
            if (data != nullptr) mbuf.memwrite(data->data(), (int)data->size());
        }
    }
    else // enumerate and version req
    {
        mbuf << Hex32((uint32_t)0x00) << Hex32((uint32_t)0x00) << Hex32((uint32_t)0x00) << Hex32((uint32_t)0x00) << Hex32((uint32_t)0x00);
    }
    return mbuf.to_vector();
}

bool Protocol1::translate_response(uint32_t /*pckt*/, 
                                   const bvector& res_data)
{
     return  _res_err == 1;
}

bool Protocol2::translate_response(uint32_t pckt, 
                                   const bvector& res_data)
{
    return  _res_err == 1;
}

bool Protocol3::translate_response(uint32_t pckt, 
                                   const bvector& res_data)
{
    if (pckt == pkt3_error_resp)
    {
        *_perror = ERR_DEVICE_LOST;
        // to do error codes !!!
        return false;
    }
    return _res_err == 1;
}

AProtocol *create_appropriate_protocol(uint32_t version_number, 
                                       uint32_t * perror)
{
    AProtocol * p = nullptr;
    switch (version_number)
    {
    case 1:
        p = new Protocol1(perror, false);
        break;
    case 2:
        p = new Protocol2(perror, false);
        break;
    case 3:
        p = new Protocol3(perror, false);
        break;
    }
    return p;
}