#include <linux/kernel.h>

#include <linux/module.h>

#include <linux/kprobes.h>

#include <linux/kallsyms.h>

#include <linux/sched.h>


extern int npages; /* Defined in Listing 21.2 */

/* Per-probe structure */
static struct kprobe bandaid;

/* Pre Handler: Invoked before running probed instruction */
int
bandaid_pre(struct kprobe *p, struct pt_regs *regs)
{
    if (npages > 10) npages = 10;
    return 0;
}

/* Post Handler: Invoked after running probed instruction */
void
bandaid_post(struct kprobe *p, struct pt_regs *regs,
             unsigned long flags)
{
    /* Nothing to do */
}

/* Fault Handler: Invoked if the pre/post-handlers
   encounter a fault */
int
bandaid_fault(struct kprobe *p, struct pt_regs *regs,
                  int trapnr)
{
    return 0;
}

int
init_module(void)
{
    int retval;

    /* Fill the kprobe structure */
    bandaid.pre_handler = bandaid_pre;
    bandaid.post_handler = bandaid_post;
    bandaid.fault_handler = bandaid_fault;

    /* Arrive at the target address as explained */
    bandaid.addr = (kprobe_opcode_t*)
        kallsyms_lookup_name("memwalkd") + 0xaa;
    if (!bandaid.addr) {
        printk("Bad Probe Point\n");
        return -1;
    }

    /* Register the kprobe */
    if ((retval = register_kprobe(&bandaid)) < 0) {
        printk("register_kprobe error, return value=%d\n",
               retval);
        return -1;
    }

    return 0;
}

void
module_cleanup(void)
{
    unregister_kprobe(&bandaid);
}

MODULE_LICENSE("GPL"); /* You can't link the Kprobes API
                          unless your user module is GPL'ed */
