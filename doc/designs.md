#### IRQ design

interface: **interrupt**, **int_istimer**, **int_reply**

peripheral units(GPIO, UART, ...) and **timer.v**: single-clock high pulse `irq`

**interrupt_unit.v**: gather `irq`s, send **interrupt** to **cpu**, request-reply, assert high until `int_reply` is high, turn low has 1 cycle latency. 

timer or external judged by **int_istimer**, which asserts/deasserts same as **interrupt**



#### Bus design

**d[31:0]**

**a[31:0]**

**spo[31:0]**

**rd**

**we**

**ready**

**grant**

**mmapper.v**: combinatorial mapper, map judging **a**, need to keep **a** to receive **ready** in time. 

peripheral units: should deassert **ready** right after **rd** or **we** if result won't be ready at next clock posedge -- no latency

**arbitration.v**: handle multiple R/W -- transparent to **cpu** and **gpu** and **dbu** -- still use **ready** to indicate status. **rd** or **we** should remain high until **grant** get high