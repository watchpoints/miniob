class Cstring
{
 private:
    char* m_ptr;
    int m_len;
public:

Cstring(char* ptr)
{

}


Cstring();
{

}

// 拷贝构造
//
Cstring &Cstring (const Cstring & other)
 {  


     m_ptr =new(nothrow)char[other.size()];
     if(m_ptr ==nullptr)
     {    

         return *this;
     }

     memcpy(m_ptr,other.m_ptr,other.m_length);

     return *this;

 }

Cstring&  Cstring( Cstring && other)
{
    this->m_ptr =other.m_ptr;
    this->m_len =other.m_len;

    other.m_ptr =nullptr;
    other.m_len =0;
    return  *this;

 }
// =
 Cstring& operaotr =(const Cstring & other)
 {

 }

 Cstring& operaotr =( Cstring && other)
 {

 }

~Cstring();
{

}

}