#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("gee");
MODULE_DESCRIPTION("A module to read CR3 register and process info whenever a new process is executed");

static struct kprobe kp_execve;
static struct kprobe kp_execveat;

static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
    unsigned long cr3;
    struct mm_struct *mm;
    int cpu;

    if (current->pid == 0) {
        // Skip the swapper process
        return 0;
    }

    cpu = smp_processor_id();

    // Use inline assembly to read the CR3 register
    asm volatile("mov %%cr3, %0" : "=r" (cr3));

    printk(KERN_INFO "[cr3reader] CPU %d: CR3 value for current process: %lx\n", cpu, cr3);

    mm = current->mm;
    if (mm) {
        // Get the executable file path
        if (mm->exe_file) {
            char buffer[256];
            struct path exe_path_struct = mm->exe_file->f_path;
            char *path_buffer = d_path(&exe_path_struct, buffer, sizeof(buffer));
            printk(KERN_INFO "[cr3reader] cpu: %d cr3: %lx exe: %s\n", cpu, cr3, path_buffer);
        }
    }
    else {
        printk(KERN_INFO "[cr3reader] cpu: %d cr3: %lx", cpu, cr3);
    }

    return 0;
}

static int __init cr3_read_init(void)
{
    int ret;
    printk(KERN_INFO "[cr3reader] CR3 reader module loaded\n");

    kp_execve.pre_handler = handler_pre;
    kp_execve.symbol_name = "do_execve";

    ret = register_kprobe(&kp_execve);
    if (ret < 0) {
        printk(KERN_ERR "[cr3reader] register_kprobe for do_execve failed, returned %d\n", ret);
        return ret;
    }

    kp_execveat.pre_handler = handler_pre;
    kp_execveat.symbol_name = "do_execveat";

    ret = register_kprobe(&kp_execveat);
    if (ret < 0) {
        printk(KERN_ERR "[cr3reader] register_kprobe for do_execveat failed, returned %d\n", ret);
        unregister_kprobe(&kp_execve);
        return ret;
    }

    printk(KERN_INFO "[cr3reader] Kprobes registered\n");
    return 0;
}

static void __exit cr3_read_exit(void)
{
    unregister_kprobe(&kp_execve);
    unregister_kprobe(&kp_execveat);
    printk(KERN_INFO "[cr3reader] Kprobes unregistered\n");
    printk(KERN_INFO "[cr3reader] CR3 reader module unloaded\n");
}

module_init(cr3_read_init);
