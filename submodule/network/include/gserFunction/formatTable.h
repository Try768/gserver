#ifndef aruma
#define aruma
#include <unordered_map>
#include <string>

#define cuct constexpr unsigned char
#define cust constexpr unsigned short
constexpr unsigned int maxPacketLenght =1024;
struct player{
    unsigned long long id;
    
    int timeout;
    std::string name;
    std::string ip;
    std::string port;
};
struct dstack{
    std::vector<unsigned char> buffer;
    std::string ip;
    std::string port;
};
namespace id_table{
    namespace client{
        cuct cek_koneksi=0x00;
        cuct pralogin=0x01;
        cuct login =0x02;
        cuct keluar=0x05;
        cuct req=0x06;
        cuct kirim=0x09;
        cuct gagal=0x0a;
    }
    namespace server{
        cuct pralogin=0x01;
        cuct login =0x02;
        cuct cek_koneksi=0x00;
        cuct tendang=0x03;
        cuct dataBesar=0x07;
        cuct req=0x06;
        cuct kirim=0x09;
    }
}
namespace idIsiTable
{
    namespace client{
        namespace req
        {
            cust playerls=0x0005;
            cust playernear=0x0007;
            cust playerinfo=0x0008;
            cust map=0x0001;
        } // namespace req
        namespace kirim
        {
            cust input=0x0003;
            cust interact=0x0004;
        } // namespace kirim
        
        
    }
    namespace server
    {
        cust playerinfo=0x0008;
        cust playerlslen=0x0006;
        cust playerls=0x0005;
        cust chunklen=0x0002;
        cust mapPerChunk=0x0001;
        
    } // namespace server
    
} // namespace name

#endif