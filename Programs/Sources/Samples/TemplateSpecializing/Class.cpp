#include "Template.h"

void Class<int>::Print() const
{
    printf("Type: %ld (int)\n", sizeof(int));
}
