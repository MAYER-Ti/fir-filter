__kernel void FirFilter(__global const short* input,
                        __global float* output,
                        __constant const float* taps)
{
    const short gid = (short)get_global_id(0); //get id item
    const uint gsize = get_global_size(0);
    const uint lsize = get_local_size(0);
//    const uint localInputSize = (gsize-gid);
//    __local float mass[128];
    float sum = input[gid] * taps[0];
    const uint grid = get_group_id(0);

    for(int i = 1; i < lsize; i++){
        if((gid+i) > gsize){
            return;
        }
        sum += (input[gid+i] * taps[i]);
    }
    output[gid] = sum;
}
