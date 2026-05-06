#pragma once
#include <vector>
#include "miniz.h"
class checksumparent{
    protected:
    checksumparent()=default;
    public:
    virtual std::vector<unsigned char> bufferdump(){
        return {};
    }
    inline unsigned long long getchecksum(){
        std::vector<unsigned char> buffer=bufferdump();
        return mz_crc32(0,buffer.data(),buffer.size());
    }
    inline unsigned long long getchecksum(const std::vector<unsigned char>& buffer)const{
        return mz_crc32(0,buffer.data(),buffer.size());
    }
    inline bool verifychecksum(unsigned long long checksum){
        return getchecksum()==checksum;
    }
    inline bool verifychecksum(unsigned long long checksum,const std::vector<unsigned char>& buffer)const{
        return mz_crc32(0,buffer.data(),buffer.size())==checksum;
    }
    //static inline bool verifychecksum(const std::vector<unsigned char>::const_iterator awal,
    //    const std::vector<unsigned char>::const_iterator akhir,unsigned long long checksum){
    //    return mz_crc32(0,&(*awal),(size_t)(akhir-awal))==checksum;
    //}
    static inline bool verifychecksum(
        std::vector<unsigned char>::const_iterator awal,
        std::vector<unsigned char>::const_iterator akhir,
        unsigned long long checksum
    ){
        if (awal == akhir)
            return checksum == 0;

        return mz_crc32(
            0,
            std::addressof(*awal),
            static_cast<size_t>(std::distance(awal, akhir))
        ) == checksum;
}
};
class CheckSumForStatic{
    private:
    unsigned long long checksum;
    public:
    CheckSumForStatic(const std::vector<unsigned char>& buffer){
        checksum=mz_crc32(0,buffer.data(),buffer.size());
    }
    CheckSumForStatic(unsigned long long checksum_in):checksum(checksum_in){}
    bool verifychecksum(unsigned long long checksum_in){
        return checksum_in==checksum;
    }
    inline unsigned long long getchecksum()const{
        return checksum;
    }
    bool verifychecksum(const std::vector<unsigned char>& buffer){
        return mz_crc32(0,buffer.data(),buffer.size())==checksum;
    }
};