void uart_putchar(int x)
{
    return;
}
void printstring(int* addr)
{
    while(*addr != 0) {
        uart_putchar(*addr);
        addr = addr + 4;
    }
    return;
}
