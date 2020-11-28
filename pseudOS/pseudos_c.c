
void test_sd_write()
{
    sd_read_sector(10);
    volatile int* p = 0x60000000;
    int i = 0;
    for (i = 0; i < 512; i++)
        p[i] = 0;
    volatile int* wb = 0x60001008;
    *wb = 1;
    volatile int* ready = 0x60002010;
    while (!(*ready));
    return;
}
