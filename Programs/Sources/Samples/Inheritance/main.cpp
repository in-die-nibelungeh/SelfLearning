
#include <new>
#include <stdio.h>
#include <stdlib.h>

class VectorBase
{
     friend class Vector;
public:
     VectorBase(int n)
     : m_AddressAligned(NULL), m_Length(n)
     {
         printf("%s\n", __func__);
     }

     VectorBase(void* ptr, int n)
     : m_AddressAligned(ptr), m_Length(n)
     {
         printf("%s\n", __func__);
     }

     ~VectorBase()
     {
         printf("%s\n", __func__);
         printf("Aligned=%p\n", m_AddressAligned);
     }


     inline int GetLength(void) const { return m_Length; }

private:
     void* m_AddressAligned;
     int m_Length;
};

class Vector : public VectorBase
{
public:
     Vector(int n)
     : VectorBase(n)
     {
         printf("%s\n", __func__);
         const int align = 16;
         m_AddressBase = new double[n+align-1];
         unsigned char* ptr = reinterpret_cast<unsigned char*>(m_AddressBase);
         while ( (reinterpret_cast<int>(ptr) % align) != 0) ptr++;
         m_AddressAligned = ptr;
         printf("Base=%p\n", m_AddressBase);
         printf("Aligned=%p\n", m_AddressAligned);
     }
     ~Vector()
     {
         printf("%s\n", __func__);
         printf("Base=%p\n", m_AddressBase);
         delete[] m_AddressBase;
     }
private:
     double* m_AddressBase;
};

void PrintLength(VectorBase& v)
{
     printf("Length=%d\n", v.GetLength());
}

int main(void)
{
     VectorBase base(4);
     Vector vector(2);
     PrintLength(base);
     PrintLength(vector);

     return 0;
}
