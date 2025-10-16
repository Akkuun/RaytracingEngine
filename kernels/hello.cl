__kernel void hello(__global int* out)
{
    int a = 3;
    int b = 3;
    out[0] = a + b;
}