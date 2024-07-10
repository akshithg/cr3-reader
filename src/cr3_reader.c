#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/smp.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("gee");
MODULE_DESCRIPTION("A module to periodically read CR3 register and process info of current process on all CPUs, excluding swapper process");

static struct timer_list my_timer;

void read_cr3_on_cpu(void *info)
{
    unsigned long cr3;
    struct mm_struct *mm;
    int cpu;

    if (current->pid == 0) {
        // Skip the swapper process
        return;
    }

    cpu = smp_processor_id();

    // Use inline assembly to read the CR3 register
    asm volatile("mov %%cr3, %0" : "=r" (cr3));

    //printk(KERN_INFO "[cr3reader] CPU %d: CR3 value for current process: %lx\n", cpu, cr3);

    mm = current->mm;
    if (mm) {
        unsigned long pgd = (unsigned long)mm->pgd;
        //printk(KERN_INFO "[cr3reader] CPU %d: PGD (CR3 equivalent) for current process: %lx\n", cpu, pgd);

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
}

void read_cr3_and_process_info(struct timer_list *t)
{
    // Run the read_cr3_on_cpu function on all CPUs
    on_each_cpu(read_cr3_on_cpu, NULL, 1);

    // Re-arm the timer to fire again in 2 seconds
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));
}

static int __init cr3_read_init(void)
{
    printk(KERN_INFO "[cr3reader] CR3 reader module loaded\n");

    // Initialize the timer
    timer_setup(&my_timer, read_cr3_and_process_info, 0);

    // Schedule the first execution of the timer
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));

    return 0;
}

static void __exit cr3_read_exit(void)
{
    // Remove the timer if it's still active
    del_timer(&my_timer);
    printk(KERN_INFO "[cr3reader] CR3 reader module unloaded\n");
}

module_init(cr3_read_init);
module_exit(cr3_read_exit);
