__kernel void hello(__global int* out)
{
    int a = 2;
    int b = 3;
    out[0] = a + b;
}